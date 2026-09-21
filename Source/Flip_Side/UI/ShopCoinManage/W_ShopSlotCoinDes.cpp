// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/ShopCoinManage/W_ShopSlotCoinDes.h"
#include "Components/TextBlock.h"
#include "Components/PanelWidget.h"
#include "Components/Button.h"
#include "Components/WidgetSwitcher.h"
#include "DataTypes/WeaponDataTypes.h"
#include "DataTypes/KeywordDataTypes.h"
#include "UI/CoinDescriptionFormatter.h"
#include "UI/CoinDescriptionSectionWidget.h"
#include "UI/CoinDescriptionBookmarkWidget.h"
#include "Engine/World.h"
#include "TimerManager.h"

void UW_ShopSlotCoinDes::NativeConstruct()
{
    Super::NativeConstruct();

    if (IsValid(AbilityButton)) AbilityButton->OnClicked.AddUniqueDynamic(this, &ThisClass::HandleAbilityButtonClicked);
    if (IsValid(RangeButton)) RangeButton->OnClicked.AddUniqueDynamic(this, &ThisClass::HandleRangeButtonClicked);

    if (IsValid(AbilityButton) && IsValid(RangeButton))
    {
        AbilityButtonDefaultStyle = AbilityButton->GetStyle();
        RangeButtonDefaultStyle = RangeButton->GetStyle();
        bHasButtonDefaultStyles = true;
    }

    SetActivePage(0);
    SetExplainTextEmpty();
}

void UW_ShopSlotCoinDes::SetExplainTextEmpty()
{
    if (UWorld* World = GetWorld()) World->GetTimerManager().ClearTimer(HideTimer);
    ClearBookmarks();
    Sections.Reset();
    SelectedSectionIndex = INDEX_NONE;
    bDescriptionHovered = false;
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

    SetVisibility(ESlateVisibility::Collapsed);
}

void UW_ShopSlotCoinDes::SetWeaponDescription(const FFaceData& Weapon, const TArray<FKeywordDefinitionData>& Keywords)
{
    SetExplainTextEmpty();
    if (WeaponNameText) WeaponNameText->SetText(FText::FromString(Weapon.WeaponName));
    const FWeaponNumericStats Stats{Weapon.AttackPoint, Weapon.BehaviorPoint, Weapon.Count};
    Sections = FCoinDescriptionFormatter::Parse(Weapon.KOR_DES, Keywords, Stats).Sections;
    if (!IsValid(DescriptionSection))
    {
        UE_LOG(LogTemp, Warning, TEXT("W_WeaponDescription: Add a CoinDescriptionSectionWidget named DescriptionSection to the WBP."));
        return;
    }
    RebuildBookmarks();
    if (!Sections.IsEmpty()) SelectDescription(0);
    // The parent must receive mouse enter/leave while its bookmark buttons remain clickable.
    SetVisibility(ESlateVisibility::Visible);
}

void UW_ShopSlotCoinDes::ClearBookmarks()
{
    for (UCoinDescriptionBookmarkWidget* Bookmark : Bookmarks)
    {
        if (!IsValid(Bookmark)) continue;
        Bookmark->OnBookmarkClicked.RemoveAll(this);
        Bookmark->RemoveFromParent();
    }
    Bookmarks.Reset();
}

void UW_ShopSlotCoinDes::RebuildBookmarks()
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

void UW_ShopSlotCoinDes::SelectDescription(int32 Index)
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

void UW_ShopSlotCoinDes::HandleBookmarkClicked(bool bFrontFace, int32 Index)
{
    SelectDescription(Index);
}

void UW_ShopSlotCoinDes::SetDetailed(bool bInDetailed)
{
    bDetailed = bInDetailed;
    if (IsValid(DescriptionSection)) DescriptionSection->SetDetailed(bDetailed);
}

void UW_ShopSlotCoinDes::SetActivePage(int32 PageIndex)
{
    if (IsValid(ContentSwitcher))
    {
        ContentSwitcher->SetActiveWidgetIndex(PageIndex);
    }

    RefreshPageButtonStyles(PageIndex);
}

void UW_ShopSlotCoinDes::RefreshPageButtonStyles(int32 ActivePageIndex)
{
    if (!bHasButtonDefaultStyles) return;

    SetPageButtonSelected(AbilityButton, AbilityButtonDefaultStyle, ActivePageIndex == 0);
    SetPageButtonSelected(RangeButton, RangeButtonDefaultStyle, ActivePageIndex == 1);
}

void UW_ShopSlotCoinDes::SetPageButtonSelected(UButton* Button, const FButtonStyle& DefaultStyle, bool bSelected)
{
    if (!IsValid(Button)) return;

    FButtonStyle Style = DefaultStyle;
    FLinearColor NormalTint = Style.Normal.TintColor.GetSpecifiedColor();
    NormalTint.A = bSelected ? 1.0f : 0.75f;
    Style.Normal.TintColor = FSlateColor(NormalTint);
    Button->SetStyle(Style);
}

void UW_ShopSlotCoinDes::HandleAbilityButtonClicked()
{
    SetActivePage(0);
}

void UW_ShopSlotCoinDes::HandleRangeButtonClicked()
{
    SetActivePage(1);
}

void UW_ShopSlotCoinDes::RequestHide()
{
    if (UWorld* World = GetWorld())
        World->GetTimerManager().SetTimer(HideTimer, this, &ThisClass::HideIfNotHovered, 0.2f, false);
}

void UW_ShopSlotCoinDes::HideIfNotHovered()
{
    if (!bDescriptionHovered) SetExplainTextEmpty();
    OnHideFinished.Broadcast();
}

void UW_ShopSlotCoinDes::NativeOnMouseEnter(const FGeometry& Geometry, const FPointerEvent& Event)
{
    Super::NativeOnMouseEnter(Geometry, Event);
    bDescriptionHovered = true;
    if (UWorld* World = GetWorld()) World->GetTimerManager().ClearTimer(HideTimer);
    OnDescriptionHoverChanged.Broadcast(true);
}

void UW_ShopSlotCoinDes::NativeOnMouseLeave(const FPointerEvent& Event)
{
    Super::NativeOnMouseLeave(Event);
    bDescriptionHovered = false;
    OnDescriptionHoverChanged.Broadcast(false);
    RequestHide();
}

void UW_ShopSlotCoinDes::NativeDestruct()
{
    if (IsValid(AbilityButton)) AbilityButton->OnClicked.RemoveDynamic(this, &ThisClass::HandleAbilityButtonClicked);
    if (IsValid(RangeButton)) RangeButton->OnClicked.RemoveDynamic(this, &ThisClass::HandleRangeButtonClicked);
    SetExplainTextEmpty();
    Super::NativeDestruct();
}


