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
#include "Components/WrapBox.h"
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
	if (IsValid(WeaponIcon)) WeaponMaterial = WeaponIcon->GetDynamicMaterial();
	// 기존 Brush의 UI 머테리얼을 사용하며, 텍스처/색상/장식은 BP가 결정합니다.
	if (IsValid(HPFillImage))
	{
		HPFillMaterial = HPFillImage->GetDynamicMaterial();
		if (!IsValid(HPFillMaterial)) UE_LOG(LogTemp, Warning, TEXT("[BattleCoinInfo] HPFillImage Brush에 FillAmount 파라미터를 가진 UI 머테리얼이 필요합니다."));
	}
	if (IsValid(ShieldFillImage))
	{
		ShieldFillMaterial = ShieldFillImage->GetDynamicMaterial();
		if (!IsValid(ShieldFillMaterial)) UE_LOG(LogTemp, Warning, TEXT("[BattleCoinInfo] ShieldFillImage Brush에 FillAmount 파라미터를 가진 UI 머테리얼이 필요합니다."));
	}
	if (IsValid(OppositeFaceButton))
		OppositeFaceButton->OnClicked.AddUniqueDynamic(this, &UBattleCoinInfoWidget::HandleOppositeFaceClicked);
	if (IsValid(BackToReadyButton))
		BackToReadyButton->OnClicked.AddUniqueDynamic(this, &UBattleCoinInfoWidget::HandleBackToReadyClicked);
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
	if (IsValid(BackToReadyButton))
		BackToReadyButton->OnClicked.RemoveDynamic(this, &UBattleCoinInfoWidget::HandleBackToReadyClicked);
	if (IsValid(OppositeFaceButton))
		OppositeFaceButton->OnClicked.RemoveDynamic(this, &UBattleCoinInfoWidget::HandleOppositeFaceClicked);
	if (IsValid(DetailedDescriptionToggleButton))
		DetailedDescriptionToggleButton->OnClicked.RemoveDynamic(this, &UBattleCoinInfoWidget::ToggleDetailedDescriptions);
	ClearBattleCoinInfo();
	Super::NativeDestruct();
}

void UBattleCoinInfoWidget::HandleBackToReadyClicked()
{
	OnBackToReadyRequested.Broadcast();
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
	if (IsValid(CoinCurrentHPText))
	{
		CoinCurrentHPText->SetText(FText::AsNumber(FMath::Max(0, InData.CurrentHP)));
		const float HPRatio = InData.MaxHP > 0
			? FMath::Clamp(static_cast<float>(InData.CurrentHP) / InData.MaxHP, 0.0f, 1.0f) : 0.0f;
		const FLinearColor& TextColor = HPRatio <= 0.4f ? CurrentHPLowColor
			: (HPRatio <= 0.7f ? CurrentHPMediumColor : CurrentHPHighColor);
		CoinCurrentHPText->SetColorAndOpacity(FSlateColor(TextColor));
	}
	if (IsValid(CoinMaxHPText)) CoinMaxHPText->SetText(FText::AsNumber(FMath::Max(0, InData.MaxHP)));
	if (IsValid(ShieldText) && IsValid(ShieldIcon))
	{
		ShieldText->SetText(FText::AsNumber(FMath::Max(0, InData.Shield)));
		ShieldText->SetVisibility(InData.Shield > 0 ? ESlateVisibility::HitTestInvisible : ESlateVisibility::Hidden);
		ShieldIcon->SetVisibility(InData.Shield > 0 ? ESlateVisibility::HitTestInvisible : ESlateVisibility::Hidden);
	}
	RefreshFace(bTargetChanged);
	RefreshVitalGauges();
	RefreshStatusEffects(InData.StatusEffects);
	// 자식 버튼과 ScrollBox의 hit-test를 막지 않습니다.
	SetVisibility(ESlateVisibility::SelfHitTestInvisible);
}

void UBattleCoinInfoWidget::ClearBattleCoinInfo()
{
	CurrentData = FBattleCoinInfoViewData();
	RefreshVitalGauges();
	bShowingOppositeFace = false;
	bDetailed = false;
	DisplayedWeaponID = INDEX_NONE;
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

void UBattleCoinInfoWidget::RefreshVitalGauges()
{
	const bool bHasCoin = CurrentData.CoinInstanceID != INDEX_NONE;
	const float HPFill = CurrentData.MaxHP > 0
		? FMath::Clamp(static_cast<float>(CurrentData.CurrentHP) / CurrentData.MaxHP, 0.0f, 1.0f) : 0.0f;
	// 기준량 없는 이전 데이터도 0 나눗셈 없이 표시합니다. 정상 데이터는 획득 시 기준량을 사용합니다.
	const int32 ShieldCapacity = FMath::Max(CurrentData.ShieldGaugeCapacity, CurrentData.Shield);
	const float ShieldFill = ShieldCapacity > 0
		? FMath::Clamp(static_cast<float>(CurrentData.Shield) / ShieldCapacity, 0.0f, 1.0f) : 0.0f;
	if (IsValid(HPFillMaterial)) HPFillMaterial->SetScalarParameterValue(TEXT("FillAmount"), bHasCoin ? HPFill : 0.0f);
	if (IsValid(ShieldFillMaterial)) ShieldFillMaterial->SetScalarParameterValue(TEXT("FillAmount"), bHasCoin ? ShieldFill : 0.0f);
	if (IsValid(HPFillImage)) HPFillImage->SetVisibility(bHasCoin && IsValid(HPFillMaterial)
		? ESlateVisibility::HitTestInvisible : ESlateVisibility::Hidden);
	// Hidden은 Overlay 크기를 유지합니다. 감소한 쉴드의 투명 영역으로 뒤의 HP가 드러납니다.
	if (IsValid(ShieldFillImage)) ShieldFillImage->SetVisibility(bHasCoin && CurrentData.Shield > 0 && IsValid(ShieldFillMaterial)
		? ESlateVisibility::HitTestInvisible : ESlateVisibility::Hidden);
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
	// 전투 윗면이 아니라 DB에 저장된 Front/Back을 표시합니다.
	if (IsValid(DisplayedFaceText)) DisplayedFaceText->SetText(bDisplayBack
		? NSLOCTEXT("BattleCoinInfo", "StoredBack", "뒷면") : NSLOCTEXT("BattleCoinInfo", "StoredFront", "앞면"));
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
	// 설명 본문을 배치하지 않은 화면에서도 책갈피는 독립적으로 생성합니다.
	if (IsValid(DescriptionContainer))
	{
		if (!IsValid(DescriptionWidget) && DescriptionSectionWidgetClass && !DescriptionSectionWidgetClass->HasAnyClassFlags(CLASS_Abstract))
			DescriptionWidget = CreateWidget<UCoinDescriptionSectionWidget>(this, DescriptionSectionWidgetClass);
		if (IsValid(DescriptionWidget) && DescriptionWidget->GetParent() != DescriptionContainer)
		{
			DescriptionWidget->RemoveFromParent();
			DescriptionContainer->SetContent(DescriptionWidget);
		}
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
	if (!DescriptionData.IsValidIndex(Index)) return;
	SelectedSection = Index;
	if (IsValid(DescriptionWidget))
	{
		DescriptionWidget->SetSectionData(DescriptionData[Index]);
		DescriptionWidget->SetDetailed(bDetailed);
		DescriptionWidget->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	}
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
	// 상세 버튼, 면 전환, 실시간 스탯 갱신 모두 같은 표시 상태를 사용합니다.
	if (CurrentData.CoinInstanceID != INDEX_NONE)
	{
		const bool bDisplayBack = (CurrentData.UpperFace == EFaceState::Back) != bShowingOppositeFace;
		const FBattleWeaponFaceInfoViewData& Face = bDisplayBack ? CurrentData.BackFace : CurrentData.FrontFace;
		if (IsValid(AttackPointText)) AttackPointText->SetText(FormatStatText(Face.BaseStats.AttackPoint, Face.FinalStats.AttackPoint, bDetailed));
		if (IsValid(WeaponPointText)) WeaponPointText->SetText(FormatStatText(Face.BaseStats.WeaponPoint, Face.FinalStats.WeaponPoint, bDetailed));
		if (IsValid(WeaponCountText)) WeaponCountText->SetText(FormatStatText(Face.BaseStats.WeaponCnt, Face.FinalStats.WeaponCnt, bDetailed));
	}
	if (IsValid(DescriptionWidget)) DescriptionWidget->SetDetailed(bDetailed);
	if (IsValid(DetailedDescriptionToggleText)) DetailedDescriptionToggleText->SetText(bDetailed
		? NSLOCTEXT("CoinDescription", "DetailOn", "상세 표시: 켜짐")
		: NSLOCTEXT("CoinDescription", "DetailOff", "상세 표시: 꺼짐"));
}

void UBattleCoinInfoWidget::RefreshStatusEffects(const TArray<FBattleStatusEffectViewData>& Effects)
{
	// 원래 순서를 유지하면서 표시 목록만 분리합니다. 상태 적용 로직에는 관여하지 않습니다.
	TArray<FBattleStatusEffectViewData> Buffs;
	TArray<FBattleStatusEffectViewData> Debuffs;
	for (const FBattleStatusEffectViewData& Effect : Effects)
	{
		(Effect.Polarity == EStatusPolarity::Debuff ? Debuffs : Buffs).Add(Effect);
	}
	auto RefreshContainer = [this](UWrapBox* Container,
		TArray<TObjectPtr<UBattleBuffIconWidget>>& Icons, const TArray<FBattleStatusEffectViewData>& Entries)
	{
		if (!IsValid(Container)) return;
		// 목록별 자식을 재사용해 수치 갱신 시 스크롤/레이아웃 초기화를 줄입니다.
		while (Icons.Num() > Entries.Num())
		{
			UBattleBuffIconWidget* Icon = Icons.Pop();
			if (IsValid(Icon)) { Icon->ClearBuffData(); Icon->RemoveFromParent(); }
		}
		while (Icons.Num() < Entries.Num())
		{
			if (!BattleBuffIconWidgetClass || BattleBuffIconWidgetClass->HasAnyClassFlags(CLASS_Abstract)) break;
			UBattleBuffIconWidget* Icon = CreateWidget<UBattleBuffIconWidget>(this, BattleBuffIconWidgetClass);
			if (!IsValid(Icon)) break;
			// 행/열을 고정하지 않습니다. WrapBox의 너비와 자식의 Desired Size로 배치합니다.
			Container->AddChildToWrapBox(Icon);
			Icons.Add(Icon);
		}
		for (int32 Index = 0; Index < Icons.Num(); ++Index)
			if (IsValid(Icons[Index])) Icons[Index]->SetBuffData(Entries[Index]);
		Container->SetVisibility(Entries.IsEmpty() ? ESlateVisibility::Collapsed : ESlateVisibility::SelfHitTestInvisible);
	};
	RefreshContainer(BuffContainer, BuffIcons, Buffs);
	RefreshContainer(DebuffContainer, DebuffIcons, Debuffs);
}

FText UBattleCoinInfoWidget::FormatStatText(int32 BaseValue, int32 FinalValue, bool bShowDetails)
{
	const int32 Difference = FinalValue - BaseValue;
	if (Difference == 0) return FText::AsNumber(FinalValue);
	const TCHAR* Style = Difference > 0 ? TEXT("Green") : TEXT("Red");
	// 기본은 최종값 전체를, 상세는 순변화량만 기존 RichText 스타일로 강조합니다.
	if (!bShowDetails)
		return FText::FromString(FString::Printf(TEXT("<%s>%d</>"), Style, FinalValue));
	return FText::FromString(FString::Printf(TEXT("%d <%s>(%+d)</>"), FinalValue, Style, Difference));
}
