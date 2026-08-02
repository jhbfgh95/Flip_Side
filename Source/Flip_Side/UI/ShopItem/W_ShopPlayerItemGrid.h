// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "W_ShopPlayerItemGrid.generated.h"

/**
 * 
 */
class UVerticalBox;
UCLASS()
class FLIP_SIDE_API UW_ShopPlayerItemGrid : public UUserWidget
{
	GENERATED_BODY()
	
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	
private:
	class UShopItemWSubsystem* ShopItemSubsystem;

protected:

	TArray<class UW_ShopPlayerItemSlot*> PlayerItemSlots;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UVerticalBox> SlotBox;

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess))
	TSubclassOf<UUserWidget> PlayerItemSlot;

private:

	UFUNCTION()
	void ShowDescrip(FItemData ItemInfo);
	
	UFUNCTION()
	void HideDescrip();

	

};
