#include "UI/W_CoinSlotInfo.h"

#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Subsystem/DataManagerSubsystem.h"
#include "Engine/GameInstance.h"
#include "Engine/Texture2D.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "UI/CoinDescriptionBookmarkWidget.h"
#include "Components/ContentWidget.h"
#include "Components/PanelWidget.h"
#include "InputCoreTypes.h"
#include "Components/Button.h"
#include "UI/KeywordDescriptionWidget.h"

void UW_CoinSlotInfo::NativeConstruct()
{
	Super::NativeConstruct();
	// 명시적으로 바인딩한 스탯 이미지만 DB에서 갱신합니다. 하위 위젯 탐색은 하지 않습니다.
	UDataManagerSubsystem* IconDB = IsValid(GetGameInstance()) ? GetGameInstance()->GetSubsystem<UDataManagerSubsystem>() : nullptr;
	if (IsValid(IconDB))
	{
		UImage* StatImages[] = { FrontAttackPowerIcon, FrontWeaponPowerIcon, FrontCountIcon, BackAttackPowerIcon, BackWeaponPowerIcon, BackCountIcon };
		const FName StatCodes[] = { TEXT("STAT:AttackPower"), TEXT("STAT:WeaponPower"), TEXT("STAT:Count"), TEXT("STAT:AttackPower"), TEXT("STAT:WeaponPower"), TEXT("STAT:Count") };
		for (int32 Index = 0; Index < UE_ARRAY_COUNT(StatImages); ++Index)
		{
			UImage* StatImage = StatImages[Index];
			UTexture2D* Icon = nullptr;
			if (!IsValid(StatImage) || !IconDB->TryGetUIIcon(StatCodes[Index], Icon)) continue;
			// 고정 스탯 아이콘도 설명 토큰과 같은 DB 색상을 사용합니다. 수치 텍스트는 변경하지 않습니다.
			FKeywordDefinitionData StatDefinition;
			const FName KeywordCode(*StatCodes[Index].ToString().RightChop(5)); // STAT: 접두사 제외
			if (IconDB->TryGetKeywordByCode(KeywordCode, StatDefinition))
				StatImage->SetColorAndOpacity(StatDefinition.UIColor);
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
	if (IsValid(CloseButton))
		CloseButton->OnClicked.AddUniqueDynamic(this, &UW_CoinSlotInfo::HandleCloseClicked);
	if (IsValid(DetailedDescriptionToggleButton))
		DetailedDescriptionToggleButton->OnClicked.AddUniqueDynamic(this, &UW_CoinSlotInfo::ToggleDetailedDescriptions);
	auto ConfigureKeywordWidget = [](UContentWidget* Container, TObjectPtr<UKeywordDescriptionWidget>& Widget, EKeywordDescriptionGroup Group)
	{
		// 디자이너에서 직접 배치한 위젯만 설정합니다. 빈 컨테이너의 내용과 레이아웃은 변경하지 않습니다.
		Widget = IsValid(Container) ? Cast<UKeywordDescriptionWidget>(Container->GetContent()) : nullptr;
		if (IsValid(Widget)) Widget->SetKeywordGroup(Group);
	};
	ConfigureKeywordWidget(MainKeywordDescriptionContainer, MainKeywordDescriptionWidget, EKeywordDescriptionGroup::Main);
	ConfigureKeywordWidget(AdditionalKeywordDescriptionContainer, AdditionalKeywordDescriptionWidget, EKeywordDescriptionGroup::Additional);
	RefreshDetailedDescriptions();

	if (IsValid(HoveredFrontWeaponIcon))
	{
		FrontDynamicMaterial = HoveredFrontWeaponIcon->GetDynamicMaterial();
	}
	if (IsValid(HoveredBackWeaponIcon))
	{
		BackDynamicMaterial = HoveredBackWeaponIcon->GetDynamicMaterial();
	}
}

void UW_CoinSlotInfo::NativeDestruct()
{
	if (IsValid(CloseButton))
		CloseButton->OnClicked.RemoveDynamic(this, &UW_CoinSlotInfo::HandleCloseClicked);
	if (IsValid(DetailedDescriptionToggleButton))
		DetailedDescriptionToggleButton->OnClicked.RemoveDynamic(this, &UW_CoinSlotInfo::ToggleDetailedDescriptions);
	ResetDetailedDescriptions();
	Super::NativeDestruct();
}

void UW_CoinSlotInfo::HandleCloseClicked()
{
	// HUD가 우클릭과 동일한 경로로 팝업 상태와 선택 표시를 함께 정리합니다.
	OnCloseRequested.Broadcast();
}

void UW_CoinSlotInfo::SetCoinSlotInfo(const FBattleCoinSlotViewData& InData)
{
	const bool bSlotChanged = CurrentSlotNumber != InData.SlotNumber;
	const bool bWeaponsChanged = CurrentFrontWeaponID != InData.FrontWeaponID || CurrentBackWeaponID != InData.BackWeaponID;
	CurrentSlotNumber = InData.SlotNumber;
	CurrentFrontWeaponID = InData.FrontWeaponID;
	CurrentBackWeaponID = InData.BackWeaponID;
	if (bWeaponsChanged || bSlotChanged)
	{
		const FCoinWeaponDescriptionData* FrontData = WeaponDescriptions.Find(InData.FrontWeaponID);
		const FCoinWeaponDescriptionData* BackData = WeaponDescriptions.Find(InData.BackWeaponID);
		SetFaceDescriptions(true, bUsePreviewDescriptions && FrontData ? FrontData->Sections : InData.FrontDescription.Sections);
		SetFaceDescriptions(false, bUsePreviewDescriptions && BackData ? BackData->Sections : InData.BackDescription.Sections);
		OnDescriptionWeaponsChanged(InData.FrontWeaponID, InData.BackWeaponID);
	}
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

	SetWeaponInfo(true, InData.FrontIcon, InData.FrontWeaponName, InData.FrontWeaponStats, InData.FrontWeaponColor);
	SetWeaponInfo(false, InData.BackIcon, InData.BackWeaponName, InData.BackWeaponStats, InData.BackWeaponColor);
}

void UW_CoinSlotInfo::SetFaceDescriptions(bool bFrontFace, const TArray<FCoinDescriptionSectionData>& Sections)
{
	UContentWidget* DescriptionContainer = bFrontFace ? FrontDescriptionContainer : BackDescriptionContainer;
	UPanelWidget* BookmarkContainer = bFrontFace ? FrontBookmarkContainer : BackBookmarkContainer;
	auto& DisplayWidget = bFrontFace ? FrontDescriptionWidget : BackDescriptionWidget;
	auto& Descriptions = bFrontFace ? FrontDescriptionData : BackDescriptionData;
	auto& Bookmarks = bFrontFace ? FrontBookmarks : BackBookmarks;
	for (UCoinDescriptionBookmarkWidget* Bookmark : Bookmarks)
		if (IsValid(Bookmark)) Bookmark->OnBookmarkClicked.RemoveAll(this);
	Bookmarks.Reset();
	if (IsValid(BookmarkContainer)) BookmarkContainer->ClearChildren();

	Descriptions = Sections;
	Descriptions.StableSort([](const FCoinDescriptionSectionData& A, const FCoinDescriptionSectionData& B)
	{
		return static_cast<uint8>(A.MainKeyword) < static_cast<uint8>(B.MainKeyword);
	});
	(bFrontFace ? SelectedFrontSection : SelectedBackSection) = INDEX_NONE;
	if (IsValid(DisplayWidget)) DisplayWidget->SetVisibility(ESlateVisibility::Collapsed);
	if (!IsValid(DescriptionContainer)) return;
	if (Descriptions.IsEmpty())
	{
		if (IsValid(DisplayWidget)) DisplayWidget->SetSectionData(FCoinDescriptionSectionData());
		return;
	}
	if (!IsValid(DisplayWidget))
	{
		if (!DescriptionSectionWidgetClass || DescriptionSectionWidgetClass->HasAnyClassFlags(CLASS_Abstract)) return;
		DisplayWidget = CreateWidget<UCoinDescriptionSectionWidget>(this, DescriptionSectionWidgetClass);
	}
	if (!IsValid(DisplayWidget)) return;
	if (DisplayWidget->GetParent() != DescriptionContainer)
	{
		DisplayWidget->RemoveFromParent();
		DescriptionContainer->SetContent(DisplayWidget);
	}

	for (int32 Index = 0; Index < Descriptions.Num(); ++Index)
	{
		const FCoinDescriptionSectionData& Data = Descriptions[Index];
		UCoinDescriptionBookmarkWidget* Bookmark = nullptr;
		if (IsValid(BookmarkContainer) && DescriptionBookmarkWidgetClass &&
			!DescriptionBookmarkWidgetClass->HasAnyClassFlags(CLASS_Abstract) && !Data.MainKeywordCode.IsNone())
		{
			Bookmark = CreateWidget<UCoinDescriptionBookmarkWidget>(this, DescriptionBookmarkWidgetClass);
			if (IsValid(Bookmark))
			{
				BookmarkContainer->AddChild(Bookmark);
				FCoinDescriptionSectionData BookmarkData;
				BookmarkData.MainKeyword = Data.MainKeyword;
				BookmarkData.MainKeywordCode = Data.MainKeywordCode;
				BookmarkData.MainKeywordLabel = Data.MainKeywordLabel;
				BookmarkData.KeywordLabels = Data.KeywordLabels;
				BookmarkData.AdditionalKeywordCodes = Data.AdditionalKeywordCodes;
				BookmarkData.AdditionalKeywords = Data.AdditionalKeywords;
				Bookmark->SetBookmarkData(BookmarkData);
				Bookmark->InitializeBookmark(bFrontFace, Index);
				Bookmark->OnBookmarkClicked.AddUObject(this, &UW_CoinSlotInfo::HandleBookmarkClicked);
			}
		}
		// 책갈피가 없는 구간도 인덱스 대응을 유지합니다.
		Bookmarks.Add(Bookmark);
	}
	SelectDescription(bFrontFace, 0);
}

void UW_CoinSlotInfo::ResetDescriptionSelection()
{
	SelectDescription(true, 0);
	SelectDescription(false, 0);
}

void UW_CoinSlotInfo::HandleBookmarkClicked(bool bFrontFace, int32 SectionIndex)
{
	SelectDescription(bFrontFace, SectionIndex);
}

void UW_CoinSlotInfo::SelectDescription(bool bFrontFace, int32 SectionIndex)
{
	const auto& Descriptions = bFrontFace ? FrontDescriptionData : BackDescriptionData;
	UCoinDescriptionSectionWidget* DisplayWidget = bFrontFace ? FrontDescriptionWidget : BackDescriptionWidget;
	int32& SelectedIndex = bFrontFace ? SelectedFrontSection : SelectedBackSection;
	if (!Descriptions.IsValidIndex(SectionIndex) || !IsValid(DisplayWidget)) return;
	if (SelectedIndex == SectionIndex) return;
	SelectedIndex = SectionIndex;
	DisplayWidget->SetSectionData(Descriptions[SectionIndex]);
	DisplayWidget->SetDetailed(bDetailToggleEnabled);
	DisplayWidget->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	const auto& Bookmarks = bFrontFace ? FrontBookmarks : BackBookmarks;
	for (int32 Index = 0; Index < Bookmarks.Num(); ++Index)
		if (IsValid(Bookmarks[Index])) Bookmarks[Index]->SetBookmarkActive(Index == SelectedIndex);
}

void UW_CoinSlotInfo::ToggleDetailedDescriptions()
{
	bDetailToggleEnabled = !bDetailToggleEnabled;
	RefreshDetailedDescriptions();
}

void UW_CoinSlotInfo::ResetDetailedDescriptions()
{
	bDetailToggleEnabled = false;
	RefreshDetailedDescriptions();
}

void UW_CoinSlotInfo::RefreshDetailedDescriptions()
{
	if (IsValid(DetailedDescriptionToggleText))
		DetailedDescriptionToggleText->SetText(bDetailToggleEnabled
			? NSLOCTEXT("CoinDescription", "DetailOn", "상세 표시: 켜짐")
			: NSLOCTEXT("CoinDescription", "DetailOff", "상세 표시: 꺼짐"));
	const bool bDetailed = bDetailToggleEnabled;
	// Shift는 현재 선택을 유지한 채 앞/뒷면의 표시 서식만 변경합니다.
	if (IsValid(FrontDescriptionWidget)) FrontDescriptionWidget->SetDetailed(bDetailed);
	if (IsValid(BackDescriptionWidget)) BackDescriptionWidget->SetDetailed(bDetailed);
}

FReply UW_CoinSlotInfo::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	// 설명의 빈 부분을 클릭해도 월드 조작으로 전달하지 않습니다.
	return InMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton
		? FReply::Handled() : Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
}

void UW_CoinSlotInfo::SetWeaponInfo(
	bool bFrontFace,
	UTexture2D* Icon,
	const FText& WeaponName,
	const FWeaponStatDisplayData& StatData,
	const FLinearColor& WeaponColor)
{
	UImage* WeaponIcon = bFrontFace ? HoveredFrontWeaponIcon : HoveredBackWeaponIcon;
	UTextBlock* WeaponNameText = bFrontFace ? HoveredFrontWeaponName : HoveredBackWeaponName;
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
		AttackPowerText->SetText(FText::AsNumber(StatData.AttackPower));
	}
	if (IsValid(WeaponPowerText))
	{
		WeaponPowerText->SetText(FText::AsNumber(StatData.WeaponPower));
	}
	if (IsValid(CountText))
	{
		CountText->SetText(FText::AsNumber(StatData.Count));
	}
}
