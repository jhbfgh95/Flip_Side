#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CardTypes.h"
#include "BattleCardSlotWidget.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FOnBattleCardSlotWidgetHovered, int32);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnBattleCardSlotWidgetUnhovered, int32);

UCLASS()
class FLIP_SIDE_API UBattleCardSlotWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

	void SetCardData(const FBattleCardSlotViewData& InData);
	void ClearCardData();

	FOnBattleCardSlotWidgetHovered OnBattleCardSlotHovered;
	FOnBattleCardSlotWidgetUnhovered OnBattleCardSlotUnhovered;

protected:
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<class UButton> CardSlotButton;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<class UImage> CardIcon;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<class UTextBlock> CardNameText;

private:
	UFUNCTION()
	void HandleCardSlotHovered();

	UFUNCTION()
	void HandleCardSlotUnhovered();

	int32 SlotNumber = INDEX_NONE;
	bool bOccupied = false;
};
