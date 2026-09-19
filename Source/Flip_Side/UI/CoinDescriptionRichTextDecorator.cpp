#include "UI/CoinDescriptionRichTextDecorator.h"
#include "UI/CoinDescriptionSectionWidget.h"
#include "Components/RichTextBlock.h"
#include "Misc/Base64.h"

namespace CoinDescriptionRichTextPrivate
{
	class FInlineDecorator : public FRichTextDecorator
	{
	public:
		explicit FInlineDecorator(URichTextBlock* InOwner) : FRichTextDecorator(InOwner),
			Section(InOwner ? InOwner->GetTypedOuter<UCoinDescriptionSectionWidget>() : nullptr) {}
		virtual bool Supports(const FTextRunParseResults& Run, const FString& Text) const override
		{
			return (Run.Name == TEXT("coin") && Run.MetaData.Contains(TEXT("key"))) ||
				(Run.Name == TEXT("coinexpr") && Run.MetaData.Contains(TEXT("expr")));
		}
	protected:
		virtual TSharedPtr<SWidget> CreateDecoratorWidget(const FTextRunInfo& Run, const FTextBlockStyle& Style) const override
		{
			if (!Section.IsValid()) return nullptr;
			if (const FString* Encoded = Run.MetaData.Find(TEXT("expr")))
			{
				FString Expression;
				return FBase64::Decode(*Encoded, Expression) ? Section->CreateExpressionDisplay(Expression, Style) : nullptr;
			}
			const FString* Key = Run.MetaData.Find(TEXT("key"));
			return Key ? Section->CreateInlineDisplay(FName(**Key), Style) : nullptr;
		}
	private:
		TWeakObjectPtr<UCoinDescriptionSectionWidget> Section;
	};
}
TSharedPtr<ITextDecorator> UCoinDescriptionRichTextDecorator::CreateDecorator(URichTextBlock* InOwner)
{
	return MakeShared<CoinDescriptionRichTextPrivate::FInlineDecorator>(InOwner);
}
