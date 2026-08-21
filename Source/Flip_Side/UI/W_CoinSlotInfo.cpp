#include "UI/W_CoinSlotInfo.h"

#include "Components/Image.h"
#include "Components/RichTextBlock.h"
#include "Components/TextBlock.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "UI/WeaponDescriptionRichTextDecorator.h"

namespace
{
	FString ResolveDisplayName(const FString& TokenType, const FString& TokenCode)
	{
		if (TokenType == TEXT("KW"))
		{
			if (TokenCode == TEXT("Attack")) return TEXT("공격");
			if (TokenCode == TEXT("Hit")) return TEXT("적중");
			if (TokenCode == TEXT("Miss")) return TEXT("미적중");
			if (TokenCode == TEXT("Mobility")) return TEXT("기동");
			if (TokenCode == TEXT("Continuous")) return TEXT("연속");
			if (TokenCode == TEXT("Absorb")) return TEXT("흡수");
			if (TokenCode == TEXT("Strike")) return TEXT("타격");
		}
		else if (TokenType == TEXT("STAT"))
		{
			if (TokenCode == TEXT("AttackPower")) return TEXT("공격력");
			if (TokenCode == TEXT("WeaponPower")) return TEXT("무기력");
			if (TokenCode == TEXT("Count")) return TEXT("횟수");
			if (TokenCode == TEXT("AttackRange")) return TEXT("공격 사거리");
			if (TokenCode == TEXT("AbilityRange")) return TEXT("능력 사거리");
		}
		else if (TokenType == TEXT("BUFF"))
		{
			if (TokenCode == TEXT("Absorb")) return TEXT("흡수 버프");
			if (TokenCode == TEXT("Strike")) return TEXT("타격 버프");
		}

		return TokenCode;
	}

	bool IsNumericStat(const FString& TokenCode)
	{
		return TokenCode == TEXT("AttackPower")
			|| TokenCode == TEXT("WeaponPower")
			|| TokenCode == TEXT("Count");
	}

	int32 ResolveStatValue(const FWeaponDescriptionDisplayData& DisplayData, const FString& TokenCode)
	{
		if (TokenCode == TEXT("AttackPower")) return DisplayData.AttackPower;
		if (TokenCode == TEXT("WeaponPower")) return DisplayData.WeaponPower;
		if (TokenCode == TEXT("Count")) return DisplayData.Count;
		return 0;
	}

	bool IsKeywordHeaderLine(const FString& Line)
	{
		FString TrimmedLine = Line;
		TrimmedLine.TrimStartAndEndInline();
		if (!TrimmedLine.StartsWith(TEXT("[KW:")))
		{
			return false;
		}

		FString PlainText;
		int32 Cursor = 0;
		while (Cursor < TrimmedLine.Len())
		{
			const int32 TokenStart = TrimmedLine.Find(TEXT("["), ESearchCase::CaseSensitive, ESearchDir::FromStart, Cursor);
			if (TokenStart == INDEX_NONE)
			{
				PlainText += TrimmedLine.Mid(Cursor);
				break;
			}

			PlainText += TrimmedLine.Mid(Cursor, TokenStart - Cursor);
			const int32 TokenEnd = TrimmedLine.Find(TEXT("]"), ESearchCase::CaseSensitive, ESearchDir::FromStart, TokenStart + 1);
			if (TokenEnd == INDEX_NONE)
			{
				PlainText += TrimmedLine.Mid(TokenStart);
				break;
			}
			Cursor = TokenEnd + 1;
		}

		PlainText.ReplaceInline(TEXT(":"), TEXT(""));
		PlainText.TrimStartAndEndInline();
		return PlainText.IsEmpty();
	}
}

void UW_CoinSlotInfo::NativeConstruct()
{
	Super::NativeConstruct();

	if (IsValid(HoveredFrontWeaponIcon))
	{
		FrontDynamicMaterial = HoveredFrontWeaponIcon->GetDynamicMaterial();
	}
	if (IsValid(HoveredBackWeaponIcon))
	{
		BackDynamicMaterial = HoveredBackWeaponIcon->GetDynamicMaterial();
	}

	ConfigureDescriptionRichText(HoveredFrontWeaponDes);
	ConfigureDescriptionRichText(HoveredBackWeaponDes);
}

void UW_CoinSlotInfo::SetCoinSlotInfo(const FBattleCoinSlotViewData& InData)
{
	CachedCoinSlotInfo = InData;
	bHasCachedCoinSlotInfo = true;

	if (IsValid(CoinSlotNumberText))
	{
		CoinSlotNumberText->SetText(FText::AsNumber(InData.SlotNumber));
	}
	if (IsValid(CoinCountText))
	{
		CoinCountText->SetText(FText::AsNumber(InData.CoinCount));
	}
	if (IsValid(CoinMaxHPText))
	{
		CoinMaxHPText->SetText(FText::AsNumber(InData.HP));
	}

	SetWeaponInfo(true, InData.FrontIcon, InData.FrontWeaponName, InData.FrontWeaponDisplay, InData.FrontWeaponColor);
	SetWeaponInfo(false, InData.BackIcon, InData.BackWeaponName, InData.BackWeaponDisplay, InData.BackWeaponColor);
}

void UW_CoinSlotInfo::SetDetailedStatsVisible(bool bVisible)
{
	if (bShowDetailedStats == bVisible)
	{
		return;
	}

	bShowDetailedStats = bVisible;
	RefreshWeaponDescriptions();
}

void UW_CoinSlotInfo::ConfigureDescriptionRichText(URichTextBlock* RichTextBlock)
{
	if (!IsValid(RichTextBlock))
	{
		return;
	}

	TArray<TSubclassOf<URichTextBlockDecorator>> DecoratorClasses;
	DecoratorClasses.Add(UWeaponDescriptionRichTextDecorator::StaticClass());
	RichTextBlock->SetDecorators(DecoratorClasses);

	UWeaponDescriptionRichTextDecorator* Decorator = Cast<UWeaponDescriptionRichTextDecorator>(
		RichTextBlock->GetDecoratorByClass(UWeaponDescriptionRichTextDecorator::StaticClass()));
	if (!IsValid(Decorator))
	{
		UE_LOG(LogTemp, Warning, TEXT("[CoinSlotInfo] 무기 설명 RichText Decorator 생성에 실패했습니다."));
		return;
	}

	Decorator->ConfigureStatPresentation(
		AttackPowerIcon,
		WeaponPowerIcon,
		CountIcon,
		AttackPowerColor,
		WeaponPowerColor,
		CountColor);
	RichTextBlock->RefreshTextLayout();
}

void UW_CoinSlotInfo::RefreshWeaponDescriptions()
{
	if (!bHasCachedCoinSlotInfo)
	{
		return;
	}

	if (IsValid(HoveredFrontWeaponDes))
	{
		HoveredFrontWeaponDes->SetText(FormatWeaponDescription(CachedCoinSlotInfo.FrontWeaponDisplay));
	}
	if (IsValid(HoveredBackWeaponDes))
	{
		HoveredBackWeaponDes->SetText(FormatWeaponDescription(CachedCoinSlotInfo.BackWeaponDisplay));
	}
}

FText UW_CoinSlotInfo::FormatWeaponDescription(const FWeaponDescriptionDisplayData& DisplayData) const
{
	FString RawDescription = DisplayData.TokenizedDescription.ToString();
	RawDescription.ReplaceInline(TEXT("\\n"), TEXT("\n"));

	TArray<FString> Lines;
	RawDescription.ParseIntoArrayLines(Lines, false);
	FString RichDescription;

	for (int32 LineIndex = 0; LineIndex < Lines.Num(); ++LineIndex)
	{
		const FString& Line = Lines[LineIndex];
		const bool bKeywordHeader = IsKeywordHeaderLine(Line);
		if (bKeywordHeader && LineIndex > 0)
		{
			const FString& PreviousLine = Lines[LineIndex - 1];
			if (!PreviousLine.TrimStartAndEnd().IsEmpty() && !IsKeywordHeaderLine(PreviousLine))
			{
				// 내용 다음에 새 키워드 헤더가 시작되면 한 줄을 더 비워 구역을 구분합니다.
				RichDescription += TEXT("\n");
			}
		}
		int32 ParseCursor = 0;

		while (ParseCursor < Line.Len())
		{
			const int32 TokenStart = Line.Find(TEXT("["), ESearchCase::CaseSensitive, ESearchDir::FromStart, ParseCursor);
			if (TokenStart == INDEX_NONE)
			{
				RichDescription += Line.Mid(ParseCursor);
				break;
			}

			RichDescription += Line.Mid(ParseCursor, TokenStart - ParseCursor);
			const int32 TokenEnd = Line.Find(TEXT("]"), ESearchCase::CaseSensitive, ESearchDir::FromStart, TokenStart + 1);
			if (TokenEnd == INDEX_NONE)
			{
				RichDescription += Line.Mid(TokenStart);
				break;
			}

			const FString TokenBody = Line.Mid(TokenStart + 1, TokenEnd - TokenStart - 1);
			FString TokenType;
			FString TokenCode;
			if (!TokenBody.Split(TEXT(":"), &TokenType, &TokenCode) || TokenCode.IsEmpty())
			{
				RichDescription += Line.Mid(TokenStart, TokenEnd - TokenStart + 1);
				ParseCursor = TokenEnd + 1;
				continue;
			}

			const FString DisplayName = ResolveDisplayName(TokenType, TokenCode);
			if (TokenType == TEXT("KW"))
			{
				const TCHAR* TagName = bKeywordHeader ? TEXT("kw_header") : TEXT("kw_ref");
				RichDescription += FString::Printf(TEXT("<%s code=\"%s\">[%s]</>"), TagName, *TokenCode, *DisplayName);
			}
			else if (TokenType == TEXT("STAT") && IsNumericStat(TokenCode))
			{
				RichDescription += FString::Printf(
					TEXT("<stat code=\"%s\" value=\"%d\" detailed=\"%d\"/>"),
					*TokenCode,
					ResolveStatValue(DisplayData, TokenCode),
					bShowDetailedStats ? 1 : 0);
			}
			else if (TokenType == TEXT("STAT"))
			{
				// TODO(DB_RANGE_PREVIEW_RECONNECT): 사거리 Spec/월드 캡처가 완성되기 전에는 수치 없이 스탯명만 표시합니다.
				RichDescription += FString::Printf(TEXT("<stat_ref code=\"%s\">[%s]</>"), *TokenCode, *DisplayName);
			}
			else if (TokenType == TEXT("BUFF"))
			{
				RichDescription += FString::Printf(TEXT("<buff_ref code=\"%s\">[%s]</>"), *TokenCode, *DisplayName);
			}
			else if (TokenType == TEXT("VALUE"))
			{
				RichDescription += FString::Printf(TEXT("<value_ref code=\"Value\">%s</>"), *TokenCode);
			}
			else
			{
				RichDescription += Line.Mid(TokenStart, TokenEnd - TokenStart + 1);
			}

			ParseCursor = TokenEnd + 1;
		}

		if (LineIndex + 1 < Lines.Num())
		{
			RichDescription += TEXT("\n");
		}
	}

	return FText::FromString(RichDescription);
}

void UW_CoinSlotInfo::SetWeaponInfo(
	bool bFrontFace,
	UTexture2D* Icon,
	const FText& WeaponName,
	const FWeaponDescriptionDisplayData& DisplayData,
	const FLinearColor& WeaponColor)
{
	UImage* WeaponIcon = bFrontFace ? HoveredFrontWeaponIcon : HoveredBackWeaponIcon;
	UTextBlock* WeaponNameText = bFrontFace ? HoveredFrontWeaponName : HoveredBackWeaponName;
	URichTextBlock* WeaponDescriptionText = bFrontFace ? HoveredFrontWeaponDes : HoveredBackWeaponDes;
	UMaterialInstanceDynamic* DynamicMaterial = bFrontFace ? FrontDynamicMaterial : BackDynamicMaterial;
	UTextBlock* AttackPowerText = bFrontFace ? FrontAttackPowerText : BackAttackPowerText;
	UTextBlock* WeaponPowerText = bFrontFace ? FrontWeaponPowerText : BackWeaponPowerText;
	UTextBlock* CountText = bFrontFace ? FrontCountText : BackCountText;

	if (IsValid(WeaponIcon) && IsValid(Icon) && IsValid(DynamicMaterial))
	{
		DynamicMaterial->SetTextureParameterValue(FName(TEXT("Weapon_Icon")), Icon);
		DynamicMaterial->SetVectorParameterValue(FName(TEXT("Weapon_Color")), WeaponColor);
	}
	if (IsValid(WeaponNameText))
	{
		WeaponNameText->SetText(WeaponName);
	}
	if (IsValid(AttackPowerText))
	{
		AttackPowerText->SetText(FText::AsNumber(DisplayData.AttackPower));
	}
	if (IsValid(WeaponPowerText))
	{
		WeaponPowerText->SetText(FText::AsNumber(DisplayData.WeaponPower));
	}
	if (IsValid(CountText))
	{
		CountText->SetText(FText::AsNumber(DisplayData.Count));
	}
	if (IsValid(WeaponDescriptionText))
	{
		WeaponDescriptionText->SetText(FormatWeaponDescription(DisplayData));
	}
}
