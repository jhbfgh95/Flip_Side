// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DataTypes/WeaponDataTypes.h"
#include "W_ShopCoinManage.generated.h"

/**
 * 
 */
UCLASS()
class FLIP_SIDE_API UW_ShopCoinManage : public UUserWidget
{
	GENERATED_BODY()

private:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

private:
	class UDataManagerSubsystem* DataSubsystem;
	class UShopCoinWSubsystem* CoinSubsystem;
	

private:
	UPROPERTY(meta = (BindWidget))
	class UW_WeaponDescription* WeaponDes;

private:
	UFUNCTION()
	void SetDesText(FFaceData FaceData);

	UFUNCTION()
	void CoinFlipAdaptor(bool IsCoinFront);

	UFUNCTION()
	void CoinSlotChangeAdaptor();
	UFUNCTION()
	void SetWeaponAdaptor(int32 WeaponID);

	UFUNCTION()
	void SetDes(int32 WeaponID);

	
	UFUNCTION()
	void ShowDesPanel(int32 WeaponID);
	UFUNCTION()
	void HideDesPanel();
};
