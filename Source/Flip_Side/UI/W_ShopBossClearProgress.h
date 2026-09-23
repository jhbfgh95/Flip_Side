#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "BossDataTypes.h"
#include "W_ShopBossClearProgress.generated.h"

/** 상점에서 보스별 클리어 진행도를 구성하는 위젯입니다. */
UCLASS()
class FLIP_SIDE_API UW_ShopBossClearProgress : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Boss Progress")
	void SetBossProgress(const TArray<FBossDisplayData>& InBossDataList, int32 CurrentStage);

	UFUNCTION(BlueprintCallable, Category = "Boss Progress")
	void HandleBackgroundWidget();

	UFUNCTION()
	void HandleOpenSlotAnimationFinished();

protected:
	virtual void NativeConstruct() override;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<class UUniformGridPanel> BossProgressBox;

	// WBP에서 투명한 버튼을 이 이름으로 만들어 배치하면 진행도 창을 닫을 수 있습니다.
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<class UButton> BackgroundButton;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss Progress")
	TSubclassOf<class UW_ShopBossProgressSlot> BossProgressSlotClass;

	UPROPERTY(Transient)
	TArray<TObjectPtr<class UW_ShopBossProgressSlot>> ProgressSlots;

	bool IsCheckNextBoss = false;
	bool IsPlaiyingOpenAnim = false;

	int32 CurrentStageLevel = -1;
};
