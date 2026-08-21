// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DataTypes/CoinDataTypes.h"
#include "W_BattleCoinInfo.generated.h"

/** ReadyCoinSlot과 필드 CoinActor가 공유하는 양면 코인 정보 팝업입니다. */
UCLASS()
class FLIP_SIDE_API UW_BattleCoinInfo : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

	void SetBattleCoinInfo(const FBattleCoinInfoViewData& InData);
	void ClearBattleCoinInfo();
	void SetAdditionalBuffsVisible(bool bVisible);

protected:
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<class UImage> FrontWeaponIcon;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<class UTextBlock> FrontWeaponNameText;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<class URichTextBlock> FrontAttackPointText;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<class URichTextBlock> FrontWeaponPointText;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<class URichTextBlock> FrontWeaponCountText;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<class UTextBlock> FrontWeaponDescriptionText;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<class UImage> BackWeaponIcon;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<class UTextBlock> BackWeaponNameText;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<class URichTextBlock> BackAttackPointText;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<class URichTextBlock> BackWeaponPointText;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<class URichTextBlock> BackWeaponCountText;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<class UTextBlock> BackWeaponDescriptionText;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<class UTextBlock> CoinCurrentHPText;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<class UTextBlock> CoinMaxHPText;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<class UTextBlock> ShieldText;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<class UBattleBuffIconWidget> BuffIcon1;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<class UBattleBuffIconWidget> BuffIcon2;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<class UBattleBuffIconWidget> BuffIcon3;

	// 서로 다른 상태효과가 3종을 넘으면 Shift를 누르는 동안 이 패널의 동적 위젯을 표시합니다.
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<class UPanelWidget> AdditionalBuffPanel;

	UPROPERTY(EditDefaultsOnly, Category = "Battle Coin Info|Buff")
	TSubclassOf<class UBattleBuffIconWidget> BattleBuffIconWidgetClass;

private:
	void CacheFixedBuffIcons();
	void SetWeaponFaceData(
		const FBattleWeaponFaceInfoViewData& FaceData,
		class UImage* WeaponIcon,
		TObjectPtr<class UMaterialInstanceDynamic>& WeaponMaterial,
		class UTextBlock* WeaponNameText,
		class URichTextBlock* AttackPointText,
		class URichTextBlock* WeaponPointText,
		class URichTextBlock* WeaponCountText,
		class UTextBlock* WeaponDescriptionText
	);
	void RebuildBuffIcons(const TArray<FBattleStatusEffectViewData>& StatusEffects);
	static FText FormatStatText(const TCHAR* Label, int32 BaseValue, int32 FinalValue);

	UPROPERTY(Transient)
	TObjectPtr<class UMaterialInstanceDynamic> FrontWeaponMaterial;

	UPROPERTY(Transient)
	TObjectPtr<class UMaterialInstanceDynamic> BackWeaponMaterial;

	UPROPERTY(Transient)
	TArray<TObjectPtr<class UBattleBuffIconWidget>> FixedBuffIcons;

	bool bAdditionalBuffsRequested = false;
	bool bHasAdditionalBuffs = false;
};
