// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DataTypes/CardTypes.h"
#include "W_ShopPlayerCardSlotContainer.generated.h"

/**
 * 
 */
class UVerticalBox;
class UTextBlock;
class UW_ShopCardSlot;
class UShopCardWSubsystem;
class UUnlockGISubsystem;
class UDataManagerSubsystem;
class UW_ShopPlayerCardSlot;
class UW_ShopPlayerSelectedCardSlot;
UCLASS()
class FLIP_SIDE_API UW_ShopPlayerCardSlotContainer : public UUserWidget
{
	GENERATED_BODY()

protected:
	UPROPERTY()
	TArray<TObjectPtr<UW_ShopPlayerCardSlot>> PlayerCardSlots;

	TArray<TObjectPtr<UW_ShopPlayerSelectedCardSlot>> SelectedPlayerCardSlots;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UVerticalBox> PlayerSelectSlotBox;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UVerticalBox> SlotBox;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> SelectCardCountText;

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess))
	TSubclassOf<UUserWidget> PlayerCardSlotWidget;

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess))
	TSubclassOf<UUserWidget> PlayerSelectedCardSlotWidget;

public:
	void InitWidget(const TArray<FCardData> UnlockCardData);

	void SetSelectCardCountText(int32 SelectCardCount);
	
	TArray<UW_ShopPlayerCardSlot*> GetShopPlayerCardSlots();
	
	TArray<UW_ShopPlayerSelectedCardSlot*> GetShopPlayerSelectedCardSlots();
protected:
	int32 UsingSelectSlotCount =0;

public:

	UW_ShopPlayerCardSlot* AddPlayerCardSlot(FCardData AddCardData);

	void AddPlayerSelectCardSlot(FCardData AddCardData, UW_ShopPlayerCardSlot* ConnectedSlot);

	void RemovePlayerSelectCardSlot(int32 RemoveIndex);

	int32 GetPlayerSelectedCardIndex(int32 CardID);
	
	const UW_ShopPlayerSelectedCardSlot* GetSelectedCardSlot(int32 Index);
};
