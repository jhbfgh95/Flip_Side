// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "BossDataTypes.h"
#include "W_BossHP.generated.h"

/**
 * 
 */
UCLASS()
class FLIP_SIDE_API UW_BossHP : public UUserWidget
{
	GENERATED_BODY()
	
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

private:
	UPROPERTY(VisibleAnywhere, Category = "HP")
	int32 MaxHp = 0;
	UPROPERTY(VisibleAnywhere,  Category = "HP")
	int32 CurrentHp = 0;

	UPROPERTY(VisibleAnywhere,  Category = "Shield")
	int32 MaxShield = 0;
	UPROPERTY(VisibleAnywhere,  Category = "Shield")
	int32 CurrentShield = 0;

	int32 CurrentStage = -1;

	UPROPERTY(EditAnywhere, Category = "BossHP|Animation", meta = (AllowPrivateAccess))
	float ProgressBarInterpSpeed = 5.f;

	float DisplayHpPercent = 0.f;
	float TargetHpPercent = 0.f;
	float DisplayShieldPercent = 0.f;
	float TargetShieldPercent = 0.f;

	bool bIsHpBarInitialized = false;
	bool bIsShieldBarInitialized = false;
	bool bDisplayingGroggyAsShield = false;

private:
	UPROPERTY(meta = (BindWidget))
    class UTextBlock* HpText;
	
	UPROPERTY(meta = (BindWidget))
    class UTextBlock* ShieldText;

	UPROPERTY(meta = (BindWidgetOptional))
	class UTextBlock* BossNameText;

	UPROPERTY(meta = (BindWidgetOptional))
	class UTextBlock* HpTotalText;

	UPROPERTY(meta = (BindWidgetOptional))
	class UTextBlock* ShieldTotalText;

	UPROPERTY(meta = (BindWidgetOptional))
	class UProgressBar* HPProgressBar;

	UPROPERTY(meta = (BindWidgetOptional))
	class UProgressBar* ShieldProgressBar;

	// 새 틀(금색 프레임) 전용 요소입니다. 없으면 이전 레이아웃처럼 숫자만 갱신합니다.
	// 눈금이 들어가는 판. 바와 같은 자리에 두면 코드가 최대치에 맞춰 세로 눈금선을 채웁니다.
	UPROPERTY(meta = (BindWidgetOptional))
	class UCanvasPanel* HpTickPanel;

	UPROPERTY(meta = (BindWidgetOptional))
	class UCanvasPanel* ShieldTickPanel;

	// 숫자를 감싼 판. 게이지 끝을 따라 움직이며, 빈 칸이 좁으면 게이지 안쪽에 들어갑니다.
	UPROPERTY(meta = (BindWidgetOptional))
	class UWidget* HpLabelBox;

	UPROPERTY(meta = (BindWidgetOptional))
	class UWidget* ShieldLabelBox;

	// 둥근 창에 들어가는 보스 문양. 보스 코인과 같은 boss_icon_path 그림을 씁니다.
	UPROPERTY(meta = (BindWidgetOptional))
	class UImage* BossEmblemImage;

	// 눈금 한 칸의 값은 1, 2, 5, 10, 20, 25, 50... 중에서 칸 수가 MaxTickSegments 이하가 되는 가장 작은 값입니다.
	UPROPERTY(EditAnywhere, Category = "BossHP|Ticks", meta = (AllowPrivateAccess, ClampMin = "2"))
	int32 MaxTickSegments = 12;

	UPROPERTY(EditAnywhere, Category = "BossHP|Ticks", meta = (AllowPrivateAccess))
	FLinearColor TickColor = FLinearColor(0.f, 0.f, 0.f, 0.6f);

	UPROPERTY(EditAnywhere, Category = "BossHP|Ticks", meta = (AllowPrivateAccess, ClampMin = "1"))
	float TickWidth = 2.f;

	UPROPERTY(EditAnywhere, Category = "BossHP|Label", meta = (AllowPrivateAccess))
	float LabelMargin = 6.f;

	UPROPERTY(EditAnywhere, Category = "BossHP|Label", meta = (AllowPrivateAccess))
	float LabelFollowSpeed = 12.f;

	// 숫자 판의 어두운 바탕을 숨깁니다. 바탕이 게이지 끝을 가렸다가 비켜나면 게이지가 다시 차는 것처럼 보이기 때문입니다.
	UPROPERTY(EditAnywhere, Category = "BossHP|Label", meta = (AllowPrivateAccess))
	bool bHideLabelBackground = true;

	// 바탕 대신 숫자에 두르는 테두리 굵기입니다. 눈금선과 게이지 위에서도 글자가 읽히게 합니다. 0이면 테두리가 없습니다.
	UPROPERTY(EditAnywhere, Category = "BossHP|Label", meta = (AllowPrivateAccess, ClampMin = "0"))
	int32 LabelOutlineSize = 3;

	UPROPERTY(EditAnywhere, Category = "BossHP|Label", meta = (AllowPrivateAccess))
	FLinearColor LabelOutlineColor = FLinearColor(0.07f, 0.03f, 0.02f, 1.f);

	// 문양 색입니다. 문양 그림(검은 바탕에 흰 실루엣)은 MI_WCoinIcon 머티리얼이 바탕을 지우고 이 색으로 칠합니다.
	UPROPERTY(EditAnywhere, Category = "BossHP|Emblem", meta = (AllowPrivateAccess))
	FLinearColor EmblemTint = FLinearColor(0.95f, 0.88f, 0.70f, 1.f);

	UPROPERTY()
	TObjectPtr<class UMaterialInstanceDynamic> EmblemMaterialInstance;

	// 보스 초상화(실시간 카메라)입니다. 보스의 BossPortraitComponent가 찍은 화면을 이 머티리얼로 둥근 창에 그립니다. 비우면 문양만 씁니다.
	UPROPERTY(EditAnywhere, Category = "BossHP|Portrait", meta = (AllowPrivateAccess))
	TObjectPtr<class UMaterialInterface> PortraitMaterial;

	// 초상화 밝기입니다. 카메라가 찍은 그대로는 어두울 수 있어 머티리얼의 Exposure 값으로 넘깁니다.
	UPROPERTY(EditAnywhere, Category = "BossHP|Portrait", meta = (AllowPrivateAccess, ClampMin = "0.1"))
	float PortraitExposure = 1.f;

	// 위젯에 처음 들어 있던 문양 머티리얼(MI_WCoinIcon)입니다. 초상화에서 문양으로 되돌릴 때 씁니다.
	UPROPERTY()
	TObjectPtr<class UMaterialInterface> EmblemMaterial;

	// 지금 둥근 창에 묶여 있는 그림입니다. HUD 데이터가 올 때마다 같은지 비교해서 바뀔 때만 다시 묶습니다.
	UPROPERTY()
	TObjectPtr<class UTextureRenderTarget2D> BoundPortraitTexture;

	UPROPERTY()
	TObjectPtr<class UTexture2D> BoundEmblemTexture;

	float HpLabelX = -1.f;
	float ShieldLabelX = -1.f;
	int32 HpTickMax = -1;
	int32 ShieldTickMax = -1;

public:

	void SetCurrnetStage(const int32 StageNum) { CurrentStage = StageNum;}
	void SetBossHUDData(const FBossHUDData& InData);
	void InitBossHp(int32 SetMaxHp);
	void InitBossShield(int32 SetMaxShield);
	void SetBossName(const FString& SetBossName);

	void ChangeMaxHp(int32 AddMaxHp);
	void ChangeCurrentHp(int32 AddHpValue);

	void ChangeMaxShield(int32 AddMaxShield);
	void ChangeCurrentShield(int32 AddShieldValue);

	void InitGroggyAsShield(int32 SetMaxGroggy);
	void UpdateGroggyAsShield(int32 NewCurrentGroggy);
	int32 GetCurrentShield() const { return CurrentShield; }

private:
	void RefreshHpBar();
	void RefreshShieldBar();
	void UpdateProgressBars(float InDeltaTime);
	void SnapHpBarToTarget();
	void SnapShieldBarToTarget();
	float GetHpPercent() const;
	float GetShieldPercent() const;

	// 새 틀 전용: 눈금, 따라가는 숫자 판, 보스 문양
	void RebuildTicks(class UCanvasPanel* TickPanel, int32 MaxValue);
	void UpdateLabelPosition(class UWidget* LabelBox, const class UProgressBar* Bar, float Percent, float& InOutLabelX, float InDeltaTime);
	// 숫자 판 모양: 바탕을 숨기고 글자에 테두리를 넣습니다.
	void ApplyLabelStyle();
	// 둥근 창: HUD 데이터에 초상화가 있으면 초상화를, 아니면 문양을 그립니다.
	void UpdateBossWindow(const FBossHUDData& InData);
	void ApplyBossPortrait(class UTextureRenderTarget2D* Portrait);
	void ApplyBossEmblem(class UTexture2D* Emblem);
	static int32 ChooseTickStep(int32 MaxValue, int32 MaxSegments);

};
