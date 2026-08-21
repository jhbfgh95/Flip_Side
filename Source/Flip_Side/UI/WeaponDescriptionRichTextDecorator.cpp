#include "UI/WeaponDescriptionRichTextDecorator.h"

#include "Components/RichTextBlock.h"
#include "Engine/Texture2D.h"
#include "Styling/CoreStyle.h"
#include "Widgets/Images/SImage.h"
#include "Widgets/Layout/SBorder.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/SBoxPanel.h"
#include "Widgets/Text/STextBlock.h"

namespace
{
	const FName AttackPowerCode(TEXT("AttackPower"));
	const FName WeaponPowerCode(TEXT("WeaponPower"));
	const FName CountCode(TEXT("Count"));

	FName ReadCode(const FTextRunInfo& RunInfo)
	{
		const FString* Code = RunInfo.MetaData.Find(TEXT("code"));
		return Code ? FName(**Code) : NAME_None;
	}

	class FWeaponDescriptionTextDecorator final : public FRichTextDecorator
	{
	public:
		FWeaponDescriptionTextDecorator(
			URichTextBlock* InOwner,
			UWeaponDescriptionRichTextDecorator* InDecorator)
			: FRichTextDecorator(InOwner)
			, Decorator(InDecorator)
		{
		}

		virtual bool Supports(const FTextRunParseResults& RunParseResult, const FString& Text) const override
		{
			return RunParseResult.Name == TEXT("kw_header")
				|| RunParseResult.Name == TEXT("kw_ref")
				|| RunParseResult.Name == TEXT("stat")
				|| RunParseResult.Name == TEXT("stat_ref")
				|| RunParseResult.Name == TEXT("buff_ref")
				|| RunParseResult.Name == TEXT("value_ref");
		}

	protected:
		virtual TSharedPtr<SWidget> CreateDecoratorWidget(
			const FTextRunInfo& RunInfo,
			const FTextBlockStyle& DefaultTextStyle) const override
		{
			if (!Decorator)
			{
				return nullptr;
			}

			const FName Code = ReadCode(RunInfo);
			if (RunInfo.Name == TEXT("kw_header"))
			{
				return SNew(SBorder)
					.BorderImage(FCoreStyle::Get().GetBrush(TEXT("WhiteBrush")))
					.BorderBackgroundColor(Decorator->ResolveTokenColor(Code))
					.Padding(FMargin(4.0f, 1.0f))
					[
						SNew(STextBlock)
						.Text(RunInfo.Content)
						.Font(DefaultTextStyle.Font)
						.ColorAndOpacity(FLinearColor::White)
					];
			}

			if (RunInfo.Name != TEXT("stat"))
			{
				return nullptr;
			}

			const FString* ValueString = RunInfo.MetaData.Find(TEXT("value"));
			const FString* DetailedString = RunInfo.MetaData.Find(TEXT("detailed"));
			const bool bDetailed = DetailedString && *DetailedString == TEXT("1");
			const FString DisplayName = Decorator->ResolveTokenDisplayName(Code).ToString();
			const FString StatText = bDetailed
				? FString::Printf(TEXT("[%s] : %s"), *DisplayName, ValueString ? **ValueString : TEXT("0"))
				: (ValueString ? *ValueString : TEXT("0"));

			TSharedRef<SHorizontalBox> StatBox = SNew(SHorizontalBox);
			if (const FSlateBrush* IconBrush = Decorator->FindStatIcon(Code))
			{
				const float IconSize = FMath::Max(1.0f, static_cast<float>(DefaultTextStyle.Font.Size));
				StatBox->AddSlot()
				.AutoWidth()
				.VAlign(VAlign_Center)
				.Padding(0.0f, 0.0f, 2.0f, 0.0f)
				[
					SNew(SBox)
					.WidthOverride(IconSize)
					.HeightOverride(IconSize)
					[
						SNew(SImage).Image(IconBrush)
					]
				];
			}

			StatBox->AddSlot()
			.AutoWidth()
			.VAlign(VAlign_Center)
			[
				SNew(STextBlock)
				.Text(FText::FromString(StatText))
				.Font(DefaultTextStyle.Font)
				.ColorAndOpacity(Decorator->ResolveTokenColor(Code))
			];

			return StatBox;
		}

		virtual void CreateDecoratorText(
			const FTextRunInfo& RunInfo,
			FTextBlockStyle& InOutTextStyle,
			FString& InOutString) const override
		{
			InOutString += RunInfo.Content.ToString();
			if (Decorator)
			{
				InOutTextStyle.SetColorAndOpacity(Decorator->ResolveTokenColor(ReadCode(RunInfo)));
			}
		}

	private:
		UWeaponDescriptionRichTextDecorator* Decorator = nullptr;
	};
}

TSharedPtr<ITextDecorator> UWeaponDescriptionRichTextDecorator::CreateDecorator(URichTextBlock* InOwner)
{
	return MakeShared<FWeaponDescriptionTextDecorator>(InOwner, this);
}

void UWeaponDescriptionRichTextDecorator::ConfigureStatPresentation(
	UTexture2D* InAttackPowerIcon,
	UTexture2D* InWeaponPowerIcon,
	UTexture2D* InCountIcon,
	const FLinearColor& InAttackPowerColor,
	const FLinearColor& InWeaponPowerColor,
	const FLinearColor& InCountColor)
{
	AttackPowerIcon = InAttackPowerIcon;
	WeaponPowerIcon = InWeaponPowerIcon;
	CountIcon = InCountIcon;
	AttackPowerColor = InAttackPowerColor;
	WeaponPowerColor = InWeaponPowerColor;
	CountColor = InCountColor;

	SetBrushTexture(AttackPowerBrush, AttackPowerIcon);
	SetBrushTexture(WeaponPowerBrush, WeaponPowerIcon);
	SetBrushTexture(CountBrush, CountIcon);
}

const FSlateBrush* UWeaponDescriptionRichTextDecorator::FindStatIcon(FName StatCode) const
{
	if (StatCode == AttackPowerCode && IsValid(AttackPowerIcon))
	{
		return &AttackPowerBrush;
	}
	if (StatCode == WeaponPowerCode && IsValid(WeaponPowerIcon))
	{
		return &WeaponPowerBrush;
	}
	if (StatCode == CountCode && IsValid(CountIcon))
	{
		return &CountBrush;
	}
	return nullptr;
}

FLinearColor UWeaponDescriptionRichTextDecorator::ResolveTokenColor(FName TokenCode) const
{
	if (TokenCode == AttackPowerCode)
	{
		return AttackPowerColor;
	}
	if (TokenCode == WeaponPowerCode)
	{
		return WeaponPowerColor;
	}
	if (TokenCode == CountCode)
	{
		return CountColor;
	}

	// TODO(DB_KEYWORD_RECONNECT): keyword_definition.ui_color_rgba 일괄 조회 결과로 교체합니다.
	if (TokenCode == TEXT("Attack")) return FLinearColor(0.86f, 0.24f, 0.22f, 1.0f);
	if (TokenCode == TEXT("Hit")) return FLinearColor(0.95f, 0.54f, 0.16f, 1.0f);
	if (TokenCode == TEXT("Miss")) return FLinearColor(0.48f, 0.52f, 0.58f, 1.0f);
	if (TokenCode == TEXT("Mobility")) return FLinearColor(0.18f, 0.66f, 0.92f, 1.0f);
	if (TokenCode == TEXT("Continuous")) return FLinearColor(0.56f, 0.34f, 0.88f, 1.0f);
	if (TokenCode == TEXT("Absorb")) return FLinearColor(0.20f, 0.72f, 0.55f, 1.0f);
	if (TokenCode == TEXT("Strike")) return FLinearColor(0.92f, 0.38f, 0.50f, 1.0f);
	if (TokenCode == TEXT("AttackRange")) return FLinearColor(0.95f, 0.44f, 0.28f, 1.0f);
	if (TokenCode == TEXT("AbilityRange")) return FLinearColor(0.34f, 0.78f, 0.92f, 1.0f);
	return FLinearColor::White;
}

FText UWeaponDescriptionRichTextDecorator::ResolveTokenDisplayName(FName TokenCode) const
{
	if (TokenCode == TEXT("Attack")) return FText::FromString(TEXT("공격"));
	if (TokenCode == TEXT("Hit")) return FText::FromString(TEXT("적중"));
	if (TokenCode == TEXT("Miss")) return FText::FromString(TEXT("미적중"));
	if (TokenCode == TEXT("Mobility")) return FText::FromString(TEXT("기동"));
	if (TokenCode == TEXT("Continuous")) return FText::FromString(TEXT("연속"));
	if (TokenCode == TEXT("Absorb")) return FText::FromString(TEXT("흡수"));
	if (TokenCode == TEXT("Strike")) return FText::FromString(TEXT("타격"));
	if (TokenCode == AttackPowerCode) return FText::FromString(TEXT("공격력"));
	if (TokenCode == WeaponPowerCode) return FText::FromString(TEXT("무기력"));
	if (TokenCode == CountCode) return FText::FromString(TEXT("횟수"));
	if (TokenCode == TEXT("AttackRange")) return FText::FromString(TEXT("공격 사거리"));
	if (TokenCode == TEXT("AbilityRange")) return FText::FromString(TEXT("능력 사거리"));
	return FText::FromName(TokenCode);
}

void UWeaponDescriptionRichTextDecorator::SetBrushTexture(FSlateBrush& Brush, UTexture2D* Texture)
{
	Brush.DrawAs = ESlateBrushDrawType::Image;
	Brush.ImageSize = FVector2D(32.0f, 32.0f);
	Brush.SetResourceObject(Texture);
}
