#include "UI/CoinDescriptionExpression.h"
#include "UI/CoinDescriptionFormatter.h"

namespace CoinExpressionPrivate
{
	struct FValue
	{
		double Number = 0.0;
		bool bKnown = true;
	};

	class FParser
	{
	public:
		FParser(const FString& InText, const FCoinDescriptionSectionData& InSection)
			: Text(InText.Replace(TEXT("×"), TEXT("*")).Replace(TEXT("÷"), TEXT("/"))
				.Replace(TEXT("−"), TEXT("-")).Replace(TEXT("–"), TEXT("-"))), Section(InSection) {}

		FCoinExpressionResult Run()
		{
			if (Text.Len() > 4096) { Fail(TEXT("Expression too long")); return Result; }
			const FValue Value = Sum(0);
			SkipSpace();
			if (Offset != Text.Len()) Fail(TEXT("Unexpected character or operator"));
			if (Result.UniqueKeys.IsEmpty()) Fail(TEXT("Stat expression requires a stat or buff"));
			Result.bValid = Result.Error.IsEmpty();
			Result.bHasUnknownValue = !Value.bKnown;
			if (Result.bValid && Value.bKnown)
			{
				Result.Value = Value.Number;
				// 중간 반올림/Clamp 금지. 최종 계산에만 반올림과 하한 0을 적용합니다.
				Result.DisplayValue = FMath::RoundToDouble(FMath::Max(0.0, Value.Number));
			}
			return Result;
		}
	private:
		void SkipSpace() { while (Offset < Text.Len() && FChar::IsWhitespace(Text[Offset])) ++Offset; }
		void Fail(const TCHAR* Reason) { if (Result.Error.IsEmpty()) Result.Error = Reason; }
		bool Accept(TCHAR C)
		{
			SkipSpace();
			if (!Result.Error.IsEmpty() || Offset >= Text.Len() || Text[Offset] != C) return false;
			++Offset;
			AddPart(FString::Chr(C));
			return true;
		}
		void AddPart(const FString& Literal, FName Key = NAME_None)
		{
			if (Result.Parts.Num() >= 256) { Fail(TEXT("Too many expression terms")); return; }
			Result.Parts.Add({ Key, Literal });
			if (!Key.IsNone()) Result.UniqueKeys.AddUnique(Key);
		}
		FValue Apply(FValue Left, FValue Right, TCHAR Op)
		{
			if (Op == TEXT('/') && Right.bKnown && Right.Number == 0.0)
			{
				Fail(TEXT("Division by zero"));
				return {};
			}
			if (!Left.bKnown || !Right.bKnown) return { 0.0, false };
			double Value = 0.0;
			switch (Op)
			{
			case TEXT('+'): Value = Left.Number + Right.Number; break;
			case TEXT('-'): Value = Left.Number - Right.Number; break;
			case TEXT('*'): Value = Left.Number * Right.Number; break;
			case TEXT('/'): Value = Left.Number / Right.Number; break;
			}
			if (!FMath::IsFinite(Value)) Fail(TEXT("Non-finite expression result"));
			return { Value, true };
		}
		FValue Sum(int32 Depth)
		{
			FValue Left = Product(Depth);
			while (Result.Error.IsEmpty())
			{
				if (Accept(TEXT('+'))) Left = Apply(Left, Product(Depth), TEXT('+'));
				else if (Accept(TEXT('-'))) Left = Apply(Left, Product(Depth), TEXT('-'));
				else break;
			}
			return Left;
		}
		FValue Product(int32 Depth)
		{
			FValue Left = Unary(Depth);
			while (Result.Error.IsEmpty())
			{
				if (Accept(TEXT('*'))) Left = Apply(Left, Unary(Depth), TEXT('*'));
				else if (Accept(TEXT('/'))) Left = Apply(Left, Unary(Depth), TEXT('/'));
				else break;
			}
			return Left;
		}
		FValue Unary(int32 Depth)
		{
			if (!Result.Error.IsEmpty()) return {};
			if (Depth > 32) { Fail(TEXT("Expression nesting too deep")); return {}; }
			if (Accept(TEXT('+'))) return Unary(Depth + 1);
			if (Accept(TEXT('-')))
			{
				FValue Value = Unary(Depth + 1);
				Value.Number = -Value.Number;
				return Value;
			}
			if (Accept(TEXT('(')))
			{
				FValue Value = Sum(Depth + 1);
				if (!Accept(TEXT(')'))) Fail(TEXT("Missing closing parenthesis"));
				return Value;
			}
			SkipSpace();
			if (Offset < Text.Len() && Text[Offset] == TEXT('['))
			{
				const int32 End = Text.Find(TEXT("]"), ESearchCase::CaseSensitive, ESearchDir::FromStart, Offset + 1);
				if (End == INDEX_NONE) { Fail(TEXT("Missing closing stat bracket")); return {}; }
				const FString KeyText = Text.Mid(Offset + 1, End - Offset - 1);
				Offset = End + 1;
				if (KeyText.StartsWith(TEXT("VALUE:"))) return Number(KeyText.RightChop(6));
				const FName Key(*KeyText);
				FCoinDescriptionTokenData Data;
				if (!FCoinDescriptionFormatter::ResolveToken(Key, Section, Data) ||
					(!Data.bHasValue && KeyText != TEXT("BUFF:Absorb") && KeyText != TEXT("BUFF:Strike")))
				{
					Fail(TEXT("Unsupported stat in expression"));
					return {};
				}
				AddPart(FString(), Key);
				// 버프값 미정은 0이 아닙니다. 수식 전체를 기호식으로 남깁니다.
				return { static_cast<double>(Data.Value), Data.bHasValue };
			}
			const int32 Start = Offset;
			while (Offset < Text.Len() && (FChar::IsDigit(Text[Offset]) || Text[Offset] == TEXT('.'))) ++Offset;
			return Number(Text.Mid(Start, Offset - Start));
		}
		FValue Number(const FString& Literal)
		{
			int32 Dots = 0, Digits = 0;
			for (TCHAR C : Literal)
			{
				if (C == TEXT('.')) ++Dots;
				else if (FChar::IsDigit(C)) ++Digits;
				else { Fail(TEXT("Invalid numeric literal")); return {}; }
			}
			if (Digits == 0 || Dots > 1) { Fail(TEXT("Expected number or stat")); return {}; }
			const double Value = FCString::Atod(*Literal);
			if (!FMath::IsFinite(Value)) Fail(TEXT("Non-finite number"));
			AddPart(Literal);
			return { Value, true };
		}
		FString Text;
		const FCoinDescriptionSectionData& Section;
		int32 Offset = 0;
		FCoinExpressionResult Result;
	};
}

FCoinExpressionResult FCoinDescriptionExpression::Evaluate(const FString& Expression, const FCoinDescriptionSectionData& Section)
{
	return CoinExpressionPrivate::FParser(Expression, Section).Run();
}
