// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DataTypes/CoinDataTypes.h"
#include "BattleBuffIconWidget.generated.h"

/** 상태효과 아이콘, 적용 출처 테두리, xN 중첩 수를 표시하는 공용 위젯입니다. */
UCLASS()
class FLIP_SIDE_API UBattleBuffIconWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

	void SetBuffData(const FBattleStatusEffectViewData& InData);
	void ClearBuffData();

protected:
	// Brush에는 Buff_Icon/Buff_SourceBorder/Buff_SourceColor 파라미터가 있는 UI 머테리얼을 지정합니다.
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<class UImage> BuffIconImage;

	// BP에서 정적인 x Text와 StackCountText를 함께 넣고 배치할 컨테이너입니다.
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<class UWidget> StackCountContainer;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<class UTextBlock> StackCountText;

	UPROPERTY(EditDefaultsOnly, Category = "Battle Buff Icon|Coin")
	TObjectPtr<class UTexture2D> CoinBorderTexture;

	UPROPERTY(EditDefaultsOnly, Category = "Battle Buff Icon|Coin")
	FLinearColor CoinSourceColor = FLinearColor::White;

	UPROPERTY(EditDefaultsOnly, Category = "Battle Buff Icon|Item")
	TObjectPtr<class UTexture2D> ItemBorderTexture;

	UPROPERTY(EditDefaultsOnly, Category = "Battle Buff Icon|Item")
	FLinearColor ItemSourceColor = FLinearColor::White;

	// TODO: Boss 주체 버프가 기획·DB에 추가되면 BossBorderTexture를 실제 조회 분기와 연결합니다.
	UPROPERTY(EditDefaultsOnly, Category = "Battle Buff Icon|Boss")
	TObjectPtr<class UTexture2D> BossBorderTexture;

	UPROPERTY(EditDefaultsOnly, Category = "Battle Buff Icon|Boss")
	FLinearColor BossSourceColor = FLinearColor::White;

private:
	bool ResolveSourceStyle(
		EStatusEffectSourceType SourceType,
		class UTexture2D*& OutBorderTexture,
		FLinearColor& OutSourceColor
	) const;

	UPROPERTY(Transient)
	TObjectPtr<class UMaterialInstanceDynamic> BuffIconMaterial;
};
