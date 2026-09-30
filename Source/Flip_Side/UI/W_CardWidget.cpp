// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/W_CardWidget.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Components/RichTextBlock.h"
#include "Engine/GameInstance.h"
#include "Engine/Texture2D.h"
#include "Subsystem/DataManagerSubsystem.h"
#include "UI/ItemDescriptionFormatter.h"
#include "UI/ItemDescriptionRichTextDecorator.h"
#include "Widgets/SBoxPanel.h"
#include "Widgets/Images/SImage.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Text/STextBlock.h"
#include "Materials/MaterialInstanceDynamic.h"

namespace
{
    const FName WCardIconParameterName(TEXT("Weapon_Icon"));
    const FName WCardColorParameterName(TEXT("Weapon_Color"));
    const FLinearColor WCardIconColor(1.0f, 0.823529f, 1.0f, 1.0f); // FFD2FFFF
}


void UW_CardWidget::NativeConstruct()
{
    Super::NativeConstruct();


}

void UW_CardWidget::InitCard(FCardData CardData)
{
    if (IsValid(CardIconImage) && IsValid(CardData.Icon))
    {
        // CardIconImage의 BP Brush에 지정한 공용 UI 머티리얼을 유지하고 파라미터만 갱신합니다.
        CardIconMaterialInstance = CardIconImage->GetDynamicMaterial();
        if (IsValid(CardIconMaterialInstance))
        {
            CardIconMaterialInstance->SetTextureParameterValue(WCardIconParameterName, CardData.Icon);
            CardIconMaterialInstance->SetVectorParameterValue(WCardColorParameterName, WCardIconColor);
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("[CardInfo] GetDynamicMaterial failed. CardIconImage Brush에 공용 UI 머티리얼을 지정하세요."));
        }
    }

    if (IsValid(CardTitle))
    {
        CardTitle->SetText(FText::FromString(CardData.CardName));
    }
    
    if (IsValid(CardDescription))
    {
        KeywordDefinitions.Reset();
        UGameInstance* Instance = GetGameInstance();
        UDataManagerSubsystem* Manager = IsValid(Instance) ? Instance->GetSubsystem<UDataManagerSubsystem>() : nullptr;
        TArray<FKeywordDefinitionData> Definitions;
        if (IsValid(Manager)) Manager->GetAllEnabledKeywordDefinitions(Definitions);
        for (const FKeywordDefinitionData& Definition : Definitions)
        {
            KeywordDefinitions.Add(Definition.KeywordCode, Definition);
        }
        CardDescription->SetDecorators({ UItemDescriptionRichTextDecorator::StaticClass() });
        // 카드에는 별도 키워드 헤더를 만들지 않고 원래 행 위치에 표시합니다.
        FString Description = CardData.Card_Description.Replace(TEXT("\\n"), TEXT("\n"));
        for (const FString& Label : { FString(TEXT("공격")), FString(TEXT("적중")), FString(TEXT("즉시")),
            FString(TEXT("KW:Attack")), FString(TEXT("KW:Hit")), FString(TEXT("KW:Instant")) })
        {
            const FName Code = FItemDescriptionFormatter::FindKeyword(Label);
            Description = Description.Replace(*(TEXT("[") + Label + TEXT("]")),
                *FString::Printf(TEXT("<itemkw code=\"%s\"/>"), *Code.ToString()));
        }
        CardDescription->SetText(FText::FromString(Description));
        CardDescription->RefreshTextLayout();
    }
}

TSharedPtr<SWidget> UW_CardWidget::CreateKeywordDisplay(FName Code, const FTextBlockStyle& Style)
{
    const FKeywordDefinitionData* Definition = KeywordDefinitions.Find(Code);
    FString Label = Code == TEXT("Attack") ? TEXT("공격") : Code == TEXT("Hit") ? TEXT("적중") : TEXT("즉시");
    if (Definition && !Definition->DisplayName.IsEmpty()) Label = Definition->DisplayName.ToString();
    UTexture2D* Icon = Definition ? Definition->Icon.Get() : nullptr;
    TSharedRef<SHorizontalBox> Row = SNew(SHorizontalBox).Visibility(EVisibility::HitTestInvisible);
    if (IsValid(Icon))
    {
        TSharedRef<FSlateBrush> Brush = MakeShared<FSlateBrush>();
        Brush->SetResourceObject(Icon);
        Brush->DrawAs = ESlateBrushDrawType::Image;
        const FVector2D Size(FMath::Max(1.0, KeywordIconSize.X), FMath::Max(1.0, KeywordIconSize.Y));
        Brush->ImageSize = Size;
        Row->AddSlot().AutoWidth().VAlign(VAlign_Center)
            [SNew(SBox).WidthOverride(Size.X).HeightOverride(Size.Y)
                [SNew(SImage).Image_Lambda([Brush]() -> const FSlateBrush* { return &Brush.Get(); })]];
    }
    Row->AddSlot().AutoWidth().VAlign(VAlign_Center)
        .Padding(IsValid(Icon) ? FMath::Max(0.0f, KeywordIconNameSpacing) : 0.0f, 0.0f, 0.0f, 0.0f)
        [SNew(STextBlock).Text(FText::FromString(Label)).Font(Style.Font)
            .ColorAndOpacity(Definition ? FSlateColor(Definition->UIColor) : Style.ColorAndOpacity)];
    return Row;
}
