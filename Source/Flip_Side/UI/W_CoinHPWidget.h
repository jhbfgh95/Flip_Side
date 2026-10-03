#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "W_CoinHPWidget.generated.h"

class UComponent_Status;

// 필드 HP 공통 표시부입니다. Bar 버전과 기존 WBP의 연결을 유지합니다.
UCLASS()
class FLIP_SIDE_API UW_CoinHPWidget : public UUserWidget
{
	GENERATED_BODY()
protected:
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	virtual bool UsesSegmentedBar() const { return false; }

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UImage> HpImage;
	// HP 위에 겹칩니다. 감소한 영역은 머테리얼 Opacity 0으로 HP를 드러냅니다.
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<class UImage> ShieldImage;
	// 현재 HP, 구분자, 최대 HP를 묶는 HorizontalBox 등 HP 텍스트 영역입니다.
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<class UWidget> HPTextContainer;
	// 기존 WBP 호환을 위해 Optional입니다. Bar WBP에는 같은 이름으로 배치합니다.
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<class UTextBlock> CoinCurrentHPText;
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<class UTextBlock> CoinMaxHPText;
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<class UTextBlock> ShieldText;
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<class UImage> ShieldIcon;

	UPROPERTY(EditDefaultsOnly, Category = "Coin HP|Animation", meta = (ClampMin = "0.0"))
	float Duration = 0.5f;
	UPROPERTY(EditDefaultsOnly, Category = "Coin HP|Text")
	FLinearColor CurrentHPLowColor = FLinearColor::Red;
	UPROPERTY(EditDefaultsOnly, Category = "Coin HP|Text")
	FLinearColor CurrentHPMediumColor = FLinearColor::Yellow;
	UPROPERTY(EditDefaultsOnly, Category = "Coin HP|Text")
	FLinearColor CurrentHPHighColor = FLinearColor::White;

public:
	// Actor가 HP/MaxHP/Shield 변경 알림을 연결하며 위젯은 실제 수치를 조회합니다.
	void InitializeWithStatus(UComponent_Status* InStatus);
	void InitHpWidget(int32 MaxHpValue, int32 CurrentHpValue, int32 ShieldValue = 0);
	void ChangeMaxHp(int32 HPModifier);
	UFUNCTION()
	void ChangeCurrentHp(int32 HPModifier);
	void ChangeShield(int32 ShieldModifier);

private:
	void RefreshFromStatus();
	void RefreshTextAndMaterialSettings();
	void StartHpAnimation();
	void SetHpProgressBar(float Percentage);
	void SetShieldProgressBar(float Percentage);
	void StartShieldAnimation();
	UPROPERTY(Transient)
	TObjectPtr<class UMaterialInstanceDynamic> ShieldMID;
	int32 ShieldCapacity = 0;
	bool bIsShieldAnimating = false;
	float ShieldAnimTime = 0.0f;
	float DisplayedShieldPercent = 0.0f;
	float StartShieldPercent = 0.0f;
	float TargetShieldPercent = 0.0f;
	UPROPERTY(Transient)
	TObjectPtr<class UMaterialInstanceDynamic> MID;
	TWeakObjectPtr<UComponent_Status> StatusSource;
	int32 MaxHp = 1;
	int32 CurrentHp = 0;
	int32 CurrentShield = 0;
	bool bIsHpAnimating = false;
	float AnimTime = 0.0f;
	float DisplayedHpPercent = 0.0f;
	float StartHpPercent = 0.0f;
	float TargetHpPercent = 0.0f;
};
