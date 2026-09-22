#include "UI/BattleCoinInfoWidget.h"

#include "Components/Button.h"
#include "Components/ContentWidget.h"
#include "Components/Image.h"
#include "Components/PanelWidget.h"
#include "Components/RichTextBlock.h"
#include "Components/TextBlock.h"
#include "Subsystem/DataManagerSubsystem.h"
#include "Engine/GameInstance.h"
#include "Engine/Texture2D.h"
#include "Components/UniformGridPanel.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "UI/BattleBuffIconWidget.h"
#include "UI/CoinDescriptionBookmarkWidget.h"
#include "UI/CoinDescriptionSectionWidget.h"
#include "UI/KeywordDescriptionWidget.h"

void UBattleCoinInfoWidget::NativeConstruct()
{
	Super::NativeConstruct();
	// 명시적으로 바인딩한 스탯 이미지만 DB에서 갱신합니다. 하위 위젯 탐색은 하지 않습니다.
	UDataManagerSubsystem* IconDB = IsValid(GetGameInstance()) ? GetGameInstance()->GetSubsystem<UDataManagerSubsystem>() : nullptr;
	if (IsValid(IconDB))
	{
		UImage* StatImages[] = { AttackPowerIcon, WeaponPowerIcon, CountIcon, HealthIcon, ShieldIcon };
		const FName StatCodes[] = { TEXT("STAT:AttackPower"), TEXT("STAT:WeaponPower"), TEXT("STAT:Count"), TEXT("STAT:Health"), TEXT("STAT:Shield") };
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
	if (IsValid(WeaponIcon)) WeaponMaterial = WeaponIcon->GetDynamicMaterial();
	if (IsValid(OppositeFaceButton))
		OppositeFaceButton->OnClicked.AddUniqueDynamic(this, &UBattleCoinInfoWidget::HandleOppositeFaceClicked);
	if (IsValid(DetailedDescriptionToggleButton))
		DetailedDescriptionToggleButton->OnClicked.AddUniqueDynamic(this, &UBattleCoinInfoWidget::ToggleDetailedDescriptions);
	// 키워드 사전은 슬롯 팝업과 동일하게 BP에서 배치한 위젯을 재사용합니다.
	if (IsValid(MainKeywordDescriptionContainer))
		if (UKeywordDescriptionWidget* Widget = Cast<UKeywordDescriptionWidget>(MainKeywordDescriptionContainer->GetContent()))
			Widget->SetKeywordGroup(EKeywordDescriptionGroup::Main);
	if (IsValid(AdditionalKeywordDescriptionContainer))
		if (UKeywordDescriptionWidget* Widget = Cast<UKeywordDescriptionWidget>(AdditionalKeywordDescriptionContainer->GetContent()))
			Widget->SetKeywordGroup(EKeywordDescriptionGroup::Additional);
	ClearBattleCoinInfo();
}

void UBattleCoinInfoWidget::NativeDestruct()
{
	if (IsValid(OppositeFaceButton))
		OppositeFaceButton->OnClicked.RemoveDynamic(this, &UBattleCoinInfoWidget::HandleOppositeFaceClicked);
	if (IsValid(DetailedDescriptionToggleButton))
		DetailedDescriptionToggleButton->OnClicked.RemoveDynamic(this, &UBattleCoinInfoWidget::ToggleDetailedDescriptions);
	ClearBattleCoinInfo();
	Super::NativeDestruct();
}

void UBattleCoinInfoWidget::SetBattleCoinInfo(const FBattleCoinInfoViewData& InData)
{
	if (InData.CoinInstanceID == INDEX_NONE)
	{
		ClearBattleCoinInfo();
		return;
	}
	const bool bTargetChanged = CurrentData.CoinInstanceID != InData.CoinInstanceID;
	CurrentData = InData;
	if (bTargetChanged)
	{
		bShowingOppositeFace = false;
		bDetailed = false;
	}
	if (IsValid(CoinInfoContent)) CoinInfoContent->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	if (IsValid(EmptyInfoText)) EmptyInfoText->SetVisibility(ESlateVisibility::Collapsed);
	if (IsValid(CoinCurrentHPText)) CoinCurrentHPText->SetText(FText::AsNumber(FMath::Max(0, InData.CurrentHP)));
	if (IsValid(CoinMaxHPText)) CoinMaxHPText->SetText(FText::AsNumber(FMath::Max(0, InData.MaxHP)));
	if (IsValid(ShieldText))
	{
		ShieldText->SetText(FText::AsNumber(FMath::Max(0, InData.Shield)));
		ShieldText->SetVisibility(InData.Shield > 0 ? ESlateVisibility::HitTestInvisible : ESlateVisibility::Collapsed);
	}
	RefreshFace(bTargetChanged);
	RefreshStatusEffects(InData.StatusEffects);
	// 자식 버튼과 ScrollBox의 hit-test를 막지 않습니다.
	SetVisibility(ESlateVisibility::SelfHitTestInvisible);
}

void UBattleCoinInfoWidget::ClearBattleCoinInfo()
{
	CurrentData = FBattleCoinInfoViewData();
	bShowingOppositeFace = false;
	bDetailed = false;
	DisplayedWeaponID = INDEX_NONE;
	if (IsValid(CoinInfoContent)) CoinInfoContent->SetVisibility(ESlateVisibility::Collapsed);
	if (IsValid(EmptyInfoText))
	{
		EmptyInfoText->SetText(NSLOCTEXT("BattleCoinInfo", "Empty", "코인에 마우스를 올려 정보를 확인하세요."));
		EmptyInfoText->SetVisibility(ESlateVisibility::HitTestInvisible);
	}
	for (UTextBlock* Text : {WeaponNameText.Get(), CoinCurrentHPText.Get(), CoinMaxHPText.Get(), ShieldText.Get(), DisplayedFaceText.Get()})
		if (IsValid(Text)) Text->SetText(FText::GetEmpty());
	for (URichTextBlock* Text : {AttackPointText.Get(), WeaponPointText.Get(), WeaponCountText.Get()})
		if (IsValid(Text)) Text->SetText(FText::GetEmpty());
	if (IsValid(WeaponIcon)) WeaponIcon->SetVisibility(ESlateVisibility::Hidden);
	if (IsValid(OppositeFaceButton)) OppositeFaceButton->SetIsEnabled(false);
	RefreshDescriptions({}, true);
	RefreshStatusEffects({});
	RefreshDetailedDescriptions();
}

void UBattleCoinInfoWidget::HandleOppositeFaceClicked()
{
	if (CurrentData.CoinInstanceID == INDEX_NONE) return;
	bShowingOppositeFace = !bShowingOppositeFace;
	// 화면의 선택만 변경합니다. Actor/StatusComponent의 실제 면은 변경하지 않습니다.
	RefreshFace(true);
}

void UBattleCoinInfoWidget::RefreshFace(bool bResetSelection)
{
	const bool bUpperIsBack = CurrentData.UpperFace == EFaceState::Back;
	const bool bDisplayBack = bUpperIsBack != bShowingOppositeFace;
	const FBattleWeaponFaceInfoViewData& Face = bDisplayBack ? CurrentData.BackFace : CurrentData.FrontFace;
	if (IsValid(WeaponIcon))
	{
		if (!IsValid(WeaponMaterial)) WeaponMaterial = WeaponIcon->GetDynamicMaterial();
		if (IsValid(WeaponMaterial))
		{
			WeaponMaterial->SetTextureParameterValue(TEXT("Weapon_Icon"), Face.WeaponIcon);
			WeaponMaterial->SetVectorParameterValue(TEXT("Weapon_Color"), Face.WeaponColor);
		}
		else WeaponIcon->SetBrushFromTexture(Face.WeaponIcon, false);
		WeaponIcon->SetVisibility(IsValid(Face.WeaponIcon) ? ESlateVisibility::HitTestInvisible : ESlateVisibility::Hidden);
	}
	if (IsValid(WeaponNameText)) WeaponNameText->SetText(Face.WeaponName);
	if (IsValid(AttackPointText)) AttackPointText->SetText(FormatStatText(TEXT("공격력"), Face.BaseStats.AttackPoint, Face.FinalStats.AttackPoint));
	if (IsValid(WeaponPointText)) WeaponPointText->SetText(FormatStatText(TEXT("무기력"), Face.BaseStats.WeaponPoint, Face.FinalStats.WeaponPoint));
	if (IsValid(WeaponCountText)) WeaponCountText->SetText(FormatStatText(TEXT("횟수"), Face.BaseStats.WeaponCnt, Face.FinalStats.WeaponCnt));
	if (IsValid(DisplayedFaceText)) DisplayedFaceText->SetText(bShowingOppositeFace
		? NSLOCTEXT("BattleCoinInfo", "Opposite", "뒷면") : NSLOCTEXT("BattleCoinInfo", "Upper", "앞면"));
	if (IsValid(OppositeFaceButton)) OppositeFaceButton->SetIsEnabled(true);
	RefreshDescriptions(Face.Description.Sections, bResetSelection || DisplayedWeaponID != Face.WeaponID);
	DisplayedWeaponID = Face.WeaponID;
	RefreshDetailedDescriptions();
}

void UBattleCoinInfoWidget::RefreshDescriptions(const TArray<FCoinDescriptionSectionData>& Sections, bool bResetSelection)
{
	const bool bRebuild = bResetSelection || DescriptionData.Num() != Sections.Num();
	DescriptionData = Sections;
	DescriptionData.StableSort([](const FCoinDescriptionSectionData& A, const FCoinDescriptionSectionData& B)
	{
		return static_cast<uint8>(A.MainKeyword) < static_cast<uint8>(B.MainKeyword);
	});
	if (bRebuild)
	{
		for (UCoinDescriptionBookmarkWidget* Bookmark : Bookmarks)
			if (IsValid(Bookmark)) Bookmark->OnBookmarkClicked.RemoveAll(this);
		Bookmarks.Reset();
		if (IsValid(BookmarkContainer)) BookmarkContainer->ClearChildren();
		SelectedSection = INDEX_NONE;
	}
	if (DescriptionData.IsEmpty())
	{
		if (IsValid(DescriptionWidget))
		{
			DescriptionWidget->SetSectionData(FCoinDescriptionSectionData());
			DescriptionWidget->SetVisibility(ESlateVisibility::Collapsed);
		}
		return;
	}
	if (!IsValid(DescriptionContainer)) return;
	if (!IsValid(DescriptionWidget) && DescriptionSectionWidgetClass && !DescriptionSectionWidgetClass->HasAnyClassFlags(CLASS_Abstract))
		DescriptionWidget = CreateWidget<UCoinDescriptionSectionWidget>(this, DescriptionSectionWidgetClass);
	if (!IsValid(DescriptionWidget)) return;
	if (DescriptionWidget->GetParent() != DescriptionContainer)
	{
		DescriptionWidget->RemoveFromParent();
		DescriptionContainer->SetContent(DescriptionWidget);
	}
	if (bRebuild)
	{
		for (int32 Index = 0; Index < DescriptionData.Num(); ++Index)
		{
			UCoinDescriptionBookmarkWidget* Bookmark = nullptr;
			if (IsValid(BookmarkContainer) && DescriptionBookmarkWidgetClass &&
				!DescriptionBookmarkWidgetClass->HasAnyClassFlags(CLASS_Abstract) && !DescriptionData[Index].MainKeywordCode.IsNone())
			{
				Bookmark = CreateWidget<UCoinDescriptionBookmarkWidget>(this, DescriptionBookmarkWidgetClass);
				if (IsValid(Bookmark))
				{
					BookmarkContainer->AddChild(Bookmark);
					Bookmark->SetBookmarkData(DescriptionData[Index]);
					Bookmark->InitializeBookmark(true, Index);
					Bookmark->OnBookmarkClicked.AddUObject(this, &UBattleCoinInfoWidget::HandleBookmarkClicked);
				}
			}
			Bookmarks.Add(Bookmark);
		}
	}
	// 스탯만 바뀌면 같은 설명 위젯/책갈피를 유지하고 표시 수치만 갱신합니다.
	SelectDescription(DescriptionData.IsValidIndex(SelectedSection) ? SelectedSection : 0);
}

void UBattleCoinInfoWidget::HandleBookmarkClicked(bool bFrontFace, int32 Index)
{
	SelectDescription(Index);
}

void UBattleCoinInfoWidget::SelectDescription(int32 Index)
{
	if (!DescriptionData.IsValidIndex(Index) || !IsValid(DescriptionWidget)) return;
	SelectedSection = Index;
	DescriptionWidget->SetSectionData(DescriptionData[Index]);
	DescriptionWidget->SetDetailed(bDetailed);
	DescriptionWidget->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	for (int32 BookmarkIndex = 0; BookmarkIndex < Bookmarks.Num(); ++BookmarkIndex)
		if (IsValid(Bookmarks[BookmarkIndex])) Bookmarks[BookmarkIndex]->SetBookmarkActive(BookmarkIndex == Index);
}

void UBattleCoinInfoWidget::ToggleDetailedDescriptions()
{
	bDetailed = !bDetailed;
	RefreshDetailedDescriptions();
}

void UBattleCoinInfoWidget::RefreshDetailedDescriptions()
{
	if (IsValid(DescriptionWidget)) DescriptionWidget->SetDetailed(bDetailed);
	if (IsValid(DetailedDescriptionToggleText)) DetailedDescriptionToggleText->SetText(bDetailed
		? NSLOCTEXT("CoinDescription", "DetailOn", "상세 표시: 켜짐")
		: NSLOCTEXT("CoinDescription", "DetailOff", "상세 표시: 꺼짐"));
}

void UBattleCoinInfoWidget::RefreshStatusEffects(const TArray<FBattleStatusEffectViewData>& Effects)
{
	if (!IsValid(StatusEffectGrid)) return;
	// 기존 자식을 재사용해 스탯 이벤트마다 ScrollBox와 레이아웃이 초기화되지 않게 합니다.
	while (StatusIcons.Num() > Effects.Num())
	{
		UBattleBuffIconWidget* Icon = StatusIcons.Pop();
		if (IsValid(Icon)) { Icon->ClearBuffData(); Icon->RemoveFromParent(); }
	}
	while (StatusIcons.Num() < Effects.Num())
	{
		if (!BattleBuffIconWidgetClass || BattleBuffIconWidgetClass->HasAnyClassFlags(CLASS_Abstract)) break;
		UBattleBuffIconWidget* Icon = CreateWidget<UBattleBuffIconWidget>(this, BattleBuffIconWidgetClass);
		if (!IsValid(Icon)) break;
		const int32 Index = StatusIcons.Num();
		StatusEffectGrid->AddChildToUniformGrid(Icon, Index / 5, Index % 5);
		StatusIcons.Add(Icon);
	}
	for (int32 Index = 0; Index < StatusIcons.Num(); ++Index)
		if (IsValid(StatusIcons[Index])) StatusIcons[Index]->SetBuffData(Effects[Index]);
	StatusEffectGrid->SetVisibility(Effects.IsEmpty() ? ESlateVisibility::Collapsed : ESlateVisibility::SelfHitTestInvisible);
}

FText UBattleCoinInfoWidget::FormatStatText(const TCHAR* Label, int32 BaseValue, int32 FinalValue)
{
	const int32 Difference = FinalValue - BaseValue;
	if (Difference == 0) return FText::FromString(FString::Printf(TEXT("%s %d"), Label, FinalValue));
	return FText::FromString(FString::Printf(TEXT("%s %d <%s>(%+d)</>"),
		Label, FinalValue, Difference > 0 ? TEXT("Green") : TEXT("Red"), Difference));
}
