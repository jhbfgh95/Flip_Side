#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "BossDataTypes.h"
#include "W_ShopBossProgressSlot.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnShopBossProgressSlotOpenFinished);

/** 상점의 보스 클리어 진행도에서 보스 하나를 표시하는 슬롯입니다. */
UCLASS()
class FLIP_SIDE_API UW_ShopBossProgressSlot : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Boss Progress")
	void InitializeProgressSlot(const FBossDisplayData& InBossData, bool bInCleared);

	UFUNCTION(BlueprintCallable, Category = "Boss Progress")
	void OpenSlot();

protected:
	virtual void NativeConstruct() override;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<class UImage> BossImage;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<class UTextBlock> BossNameText;

	UPROPERTY(meta = (BindWidgetAnim), Transient)
	TObjectPtr<class UWidgetAnimation> OpenSlotAnimation;

	UPROPERTY(BlueprintReadOnly, Category = "Boss Progress")
	FBossDisplayData BossData;

	UPROPERTY(BlueprintReadOnly, Category = "Boss Progress")
	bool bCleared = false;

public:
	// OpenSlotAnimation 재생이 끝난 시점에 호출됩니다.
	UPROPERTY(BlueprintAssignable, Category = "Boss Progress|Events")
	FOnShopBossProgressSlotOpenFinished OnOpenSlotAnimationFinished;

private:
	UFUNCTION()
	void HandleOpenSlotAnimationFinished();
};
