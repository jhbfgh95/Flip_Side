// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "W_ShopPlayerCardSlotContainer.generated.h"

/**
 * 
 */
class UVerticalBox;
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

	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	
private:

	UPROPERTY()
	TObjectPtr<UShopCardWSubsystem> CardSubsystem;
	UPROPERTY()
	TObjectPtr<UUnlockGISubsystem> UnlockSubsystem;
	UPROPERTY()
	TObjectPtr<UDataManagerSubsystem> DataManager;
protected:
	UPROPERTY()
	TArray<TObjectPtr<UW_ShopPlayerCardSlot>> PlayerCardSlots;

	TArray<TObjectPtr<UW_ShopPlayerSelectedCardSlot>> SelectedPlayerCardSlots;


	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UVerticalBox> PlayerSelectSlotBox;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UVerticalBox> SlotBox;

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess))
	TSubclassOf<UUserWidget> PlayerCardSlotWidget;

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess))
	TSubclassOf<UUserWidget> PlayerSelectedCardSlotWidget;

protected:
	int32 UsingSelectSlotCount =0;
protected:
	UFUNCTION()
	void UpdatePlayerCard(int32 CardID);

	UFUNCTION()
	void UpdatePlayerSelectCard();

	UFUNCTION()
	void AddPlayerCardSlot(FCardData AddCardData);

	UFUNCTION()
	void AddPlayerSelectCardSlot(FCardData AddCardData);
};
