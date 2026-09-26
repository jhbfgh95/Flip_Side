#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DataTypes/CoinDataTypes.h"
#include "BattleCoinInfoWidget.generated.h"

/** 기존 W_BattleCoinInfo를 레디 영역의 단면 정보 페이지로 이전한 클래스입니다. */
UCLASS()
class FLIP_SIDE_API UBattleCoinInfoWidget : public UUserWidget
{
	GENERATED_BODY()
protected:
	// 같은 이름의 BP Image를 DB 아이콘에 직접 연결합니다. Brush 크기와 색상은 유지합니다.
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<class UImage> AttackPowerIcon;
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<class UImage> WeaponPowerIcon;
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<class UImage> CountIcon;
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<class UImage> HealthIcon;
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<class UImage> ShieldIcon;
	UPROPERTY(EditDefaultsOnly, Category = "UI|Stat Icons")
	FName StatIconTextureParameter = TEXT("Icon");
public:
	// 버튼은 정보 위젯이 소유하고, 페이지 전환은 부모 레디 위젯에 요청합니다.
	FSimpleMulticastDelegate OnBackToReadyRequested;
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	void SetBattleCoinInfo(const FBattleCoinInfoViewData& InData);
	void ClearBattleCoinInfo();
	UFUNCTION(BlueprintCallable, Category = "Coin Description")
	void ToggleDetailedDescriptions();

protected:
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<class UImage> WeaponIcon;
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<class UTextBlock> WeaponNameText;
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<class URichTextBlock> AttackPointText;
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<class URichTextBlock> WeaponPointText;
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<class URichTextBlock> WeaponCountText;
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<class UTextBlock> CoinCurrentHPText;
	// 현재 HP 텍스트 전용 색상입니다. 최대 HP 텍스트와 원형 게이지에는 적용하지 않습니다.
	UPROPERTY(EditDefaultsOnly, Category = "Battle Coin Info|HP Text", meta = (DisplayName = "HP Color (0-40%)"))
	FLinearColor CurrentHPLowColor = FLinearColor::Red;
	UPROPERTY(EditDefaultsOnly, Category = "Battle Coin Info|HP Text", meta = (DisplayName = "HP Color (40-70%)"))
	FLinearColor CurrentHPMediumColor = FLinearColor::Yellow;
	UPROPERTY(EditDefaultsOnly, Category = "Battle Coin Info|HP Text", meta = (DisplayName = "HP Color (70-100%)"))
	FLinearColor CurrentHPHighColor = FLinearColor::White;
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<class UTextBlock> CoinMaxHPText;
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<class UTextBlock> ShieldText;
	// BP Overlay에서 HP를 먼저, Shield를 그 위에 배치합니다. 두 Brush에 UI 머테리얼을 지정합니다.
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<class UImage> HPFillImage;
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<class UImage> ShieldFillImage;
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<class UButton> OppositeFaceButton;
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<class UButton> BackToReadyButton;
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<class UTextBlock> DisplayedFaceText;
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<class UContentWidget> DescriptionContainer;
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<class UPanelWidget> BookmarkContainer;
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<class UContentWidget> MainKeywordDescriptionContainer;
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<class UContentWidget> AdditionalKeywordDescriptionContainer;
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<class UButton> DetailedDescriptionToggleButton;
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<class UTextBlock> DetailedDescriptionToggleText;
	UPROPERTY(EditDefaultsOnly, Category = "Coin Description")
	TSubclassOf<class UCoinDescriptionSectionWidget> DescriptionSectionWidgetClass;
	UPROPERTY(EditDefaultsOnly, Category = "Coin Description")
	TSubclassOf<class UCoinDescriptionBookmarkWidget> DescriptionBookmarkWidgetClass;
	// BP의 영역 너비/간격과 각 버프 위젯 SizeBox의 Min/Max 크기에 따라 자동 줄바꿈합니다.
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<class UWrapBox> BuffContainer;
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<class UWrapBox> DebuffContainer;
	UPROPERTY(EditDefaultsOnly, Category = "Battle Coin Info|Buff")
	TSubclassOf<class UBattleBuffIconWidget> BattleBuffIconWidgetClass;
	UPROPERTY(BlueprintReadOnly, Transient, Category = "Battle Coin Info")
	FBattleCoinInfoViewData CurrentData;
	UPROPERTY(BlueprintReadOnly, Category = "Battle Coin Info")
	bool bShowingOppositeFace = false;

private:
	UFUNCTION()
	void HandleBackToReadyClicked();
	void RefreshVitalGauges();
	UPROPERTY(Transient)
	TObjectPtr<class UMaterialInstanceDynamic> HPFillMaterial;
	UPROPERTY(Transient)
	TObjectPtr<class UMaterialInstanceDynamic> ShieldFillMaterial;
	UFUNCTION()
	void HandleOppositeFaceClicked();
	void RefreshFace(bool bResetSelection);
	void RefreshDescriptions(const TArray<FCoinDescriptionSectionData>& Sections, bool bResetSelection);
	void SelectDescription(int32 Index);
	void HandleBookmarkClicked(bool bFrontFace, int32 Index);
	void RefreshDetailedDescriptions();
	void RefreshStatusEffects(const TArray<FBattleStatusEffectViewData>& Effects);
	static FText FormatStatText(int32 BaseValue, int32 FinalValue, bool bShowDetails);
	UPROPERTY(Transient)
	TObjectPtr<class UMaterialInstanceDynamic> WeaponMaterial;
	UPROPERTY(Transient)
	TObjectPtr<class UCoinDescriptionSectionWidget> DescriptionWidget;
	UPROPERTY(Transient)
	TArray<TObjectPtr<class UCoinDescriptionBookmarkWidget>> Bookmarks;
	UPROPERTY(Transient)
	TArray<FCoinDescriptionSectionData> DescriptionData;
	UPROPERTY(Transient)
	TArray<TObjectPtr<class UBattleBuffIconWidget>> BuffIcons;
	UPROPERTY(Transient)
	TArray<TObjectPtr<class UBattleBuffIconWidget>> DebuffIcons;
	int32 SelectedSection = INDEX_NONE;
	int32 DisplayedWeaponID = INDEX_NONE;
	bool bDetailed = false;
};
