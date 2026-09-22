// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/W_WeaponDescription.h"
#include "Components/TextBlock.h"
#include "Subsystem/DataManagerSubsystem.h"
#include "Engine/GameInstance.h"
#include "Engine/Texture2D.h"
#include "Components/Image.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Components/PanelWidget.h"
#include "DataTypes/WeaponDataTypes.h"
#include "DataTypes/KeywordDataTypes.h"
#include "UI/CoinDescriptionFormatter.h"
#include "UI/CoinDescriptionSectionWidget.h"
#include "UI/CoinDescriptionBookmarkWidget.h"

void UW_WeaponDescription::NativeConstruct()
{
    Super::NativeConstruct();
	// 명시적으로 바인딩한 스탯 이미지만 DB에서 갱신합니다. 하위 위젯 탐색은 하지 않습니다.
	UDataManagerSubsystem* IconDB = IsValid(GetGameInstance()) ? GetGameInstance()->GetSubsystem<UDataManagerSubsystem>() : nullptr;
	if (IsValid(IconDB))
	{
		UImage* StatImages[] = { AttackPowerIcon, WeaponPowerIcon, CountIcon };
		const FName StatCodes[] = { TEXT("STAT:AttackPower"), TEXT("STAT:WeaponPower"), TEXT("STAT:Count") };
		for (int32 Index = 0; Index < UE_ARRAY_COUNT(StatImages); ++Index)
		{
			UImage* StatImage = StatImages[Index];
			UTexture2D* Icon = nullptr;
			if (!IsValid(StatImage) || !IconDB->TryGetUIIcon(StatCodes[Index], Icon)) continue;
			if (Cast<UMaterialInterface>(StatImage->GetBrush().GetResourceObject()))
			{
				if (UMaterialInstanceDynamic* Material = StatImage->GetDynamicMaterial())
					Material->SetTextureParameterValue(StatIconTextureParameter, Icon);
			}
			else
			{
				FSlateBrush Brush = StatImage->GetBrush();
				Brush.SetResourceObject(Icon);
				StatImage->SetBrush(Brush);
			}
		}
	}

    SetExplainTextEmpty();
}

void UW_WeaponDescription::SetExplainTextEmpty()
{
    ClearBookmarks();
    Sections.Reset();
    SelectedSectionIndex = INDEX_NONE;
    if (DescriptionSection)
    {
        DescriptionSection->SetSectionData(FCoinDescriptionSectionData{});
        DescriptionSection->SetVisibility(ESlateVisibility::Collapsed);
    }
    if (BookmarkBox) BookmarkBox->SetVisibility(ESlateVisibility::Collapsed);

    if (WeaponNameText)
    {
        WeaponNameText->SetText(FText::GetEmpty());
    }
    if (AttackPower) AttackPower->SetText(FText::GetEmpty());
    if (WeaponPower) WeaponPower->SetText(FText::GetEmpty());
    if (CountText) CountText->SetText(FText::GetEmpty());

    SetVisibility(ESlateVisibility::Collapsed);
}

void UW_WeaponDescription::SetWeaponDescription(const FFaceData& Weapon, const TArray<FKeywordDefinitionData>& Keywords)
{
    SetExplainTextEmpty();
    if (WeaponNameText) WeaponNameText->SetText(FText::FromString(Weapon.WeaponName));

    const FWeaponNumericStats Stats{Weapon.AttackPoint, Weapon.BehaviorPoint, Weapon.Count};

    if (AttackPower) AttackPower->SetText(FText::AsNumber(Stats.AttackPoint));
    if (WeaponPower) WeaponPower->SetText(FText::AsNumber(Stats.WeaponPoint));
    if (CountText) CountText->SetText(FText::AsNumber(Stats.WeaponCnt));

    Sections = FCoinDescriptionFormatter::Parse(Weapon.KOR_DES, Keywords, Stats).Sections;

    if (!IsValid(DescriptionSection))
    {
        UE_LOG(LogTemp, Warning, TEXT("W_WeaponDescription: Add a CoinDescriptionSectionWidget named DescriptionSection to the WBP."));
        return;
    }

    RebuildBookmarks();

    if (!Sections.IsEmpty()) SelectDescription(0);

    SetVisibility(ESlateVisibility::Visible);
}

void UW_WeaponDescription::ClearBookmarks()
{
    for (UCoinDescriptionBookmarkWidget* Bookmark : Bookmarks)
    {
        if (!IsValid(Bookmark)) continue;
        Bookmark->OnBookmarkClicked.RemoveAll(this);
        Bookmark->RemoveFromParent();
    }
    Bookmarks.Reset();
}

void UW_WeaponDescription::RebuildBookmarks()
{
    ClearBookmarks();
    Bookmarks.SetNum(Sections.Num());
    if (!IsValid(BookmarkBox) || !BookmarkWidgetClass)
    {
        UE_LOG(LogTemp, Warning, TEXT("W_WeaponDescription: Assign BookmarkBox and BookmarkWidgetClass in the WBP."));
        return;
    }
    bool bHasBookmarks = false;
    for (int32 Index = 0; Index < Sections.Num(); ++Index)
    {
        if (Sections[Index].MainKeywordCode.IsNone()) continue;
        UCoinDescriptionBookmarkWidget* Bookmark = CreateWidget<UCoinDescriptionBookmarkWidget>(this, BookmarkWidgetClass);
        if (!IsValid(Bookmark)) continue;
        Bookmarks[Index] = Bookmark;
        BookmarkBox->AddChild(Bookmark);
        Bookmark->SetBookmarkData(Sections[Index]);
        Bookmark->InitializeBookmark(true, Index);
        Bookmark->OnBookmarkClicked.AddUObject(this, &ThisClass::HandleBookmarkClicked);
        bHasBookmarks = true;
    }
    BookmarkBox->SetVisibility(bHasBookmarks ? ESlateVisibility::SelfHitTestInvisible : ESlateVisibility::Collapsed);
}

void UW_WeaponDescription::SelectDescription(int32 Index)
{
    if (!Sections.IsValidIndex(Index) || !IsValid(DescriptionSection)) return;
    SelectedSectionIndex = Index;
    DescriptionSection->SetSectionData(Sections[Index]);
    DescriptionSection->SetDetailed(bDetailed);
    DescriptionSection->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
    for (int32 BookmarkIndex = 0; BookmarkIndex < Bookmarks.Num(); ++BookmarkIndex)
    {
        if (IsValid(Bookmarks[BookmarkIndex])) Bookmarks[BookmarkIndex]->SetBookmarkActive(BookmarkIndex == Index);
    }
}

void UW_WeaponDescription::HandleBookmarkClicked(bool bFrontFace, int32 Index)
{
    SelectDescription(Index);
}

void UW_WeaponDescription::SetDetailed(bool bInDetailed)
{
    bDetailed = bInDetailed;
    if (IsValid(DescriptionSection)) DescriptionSection->SetDetailed(bDetailed);
}

void UW_WeaponDescription::NativeDestruct()
{
    SetExplainTextEmpty();
    Super::NativeDestruct();
}
