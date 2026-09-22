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
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	void SetBattleCoinInfo(const FBattleCoinInfoViewData& InData);
	void ClearBattleCoinInfo();
	UFUNCTION(BlueprintCallable, Category = "Coin Description")
	void ToggleDetailedDescriptions();

protected:
	// 빈 정보 페이지에서도 바깥의 레디/정보 전환 버튼은 계속 사용할 수 있습니다.
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<class UWidget> CoinInfoContent;
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<class UTextBlock> EmptyInfoText;
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
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<class UTextBlock> CoinMaxHPText;
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<class UTextBlock> ShieldText;
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<class UButton> OppositeFaceButton;
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
	// BP에서 ScrollBox 아래에 배치합니다. C++은 모든 상태를 5열로 채웁니다.
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<class UUniformGridPanel> StatusEffectGrid;
	UPROPERTY(EditDefaultsOnly, Category = "Battle Coin Info|Buff")
	TSubclassOf<class UBattleBuffIconWidget> BattleBuffIconWidgetClass;
	UPROPERTY(BlueprintReadOnly, Transient, Category = "Battle Coin Info")
	FBattleCoinInfoViewData CurrentData;
	UPROPERTY(BlueprintReadOnly, Category = "Battle Coin Info")
	bool bShowingOppositeFace = false;

private:
	UFUNCTION()
	void HandleOppositeFaceClicked();
	void RefreshFace(bool bResetSelection);
	void RefreshDescriptions(const TArray<FCoinDescriptionSectionData>& Sections, bool bResetSelection);
	void SelectDescription(int32 Index);
	void HandleBookmarkClicked(bool bFrontFace, int32 Index);
	void RefreshDetailedDescriptions();
	void RefreshStatusEffects(const TArray<FBattleStatusEffectViewData>& Effects);
	static FText FormatStatText(const TCHAR* Label, int32 BaseValue, int32 FinalValue);
	UPROPERTY(Transient)
	TObjectPtr<class UMaterialInstanceDynamic> WeaponMaterial;
	UPROPERTY(Transient)
	TObjectPtr<class UCoinDescriptionSectionWidget> DescriptionWidget;
	UPROPERTY(Transient)
	TArray<TObjectPtr<class UCoinDescriptionBookmarkWidget>> Bookmarks;
	UPROPERTY(Transient)
	TArray<FCoinDescriptionSectionData> DescriptionData;
	UPROPERTY(Transient)
	TArray<TObjectPtr<class UBattleBuffIconWidget>> StatusIcons;
	int32 SelectedSection = INDEX_NONE;
	int32 DisplayedWeaponID = INDEX_NONE;
	bool bDetailed = false;
};
