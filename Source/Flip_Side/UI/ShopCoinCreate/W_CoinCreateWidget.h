// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DataTypes/FlipSide_Enum.h"
#include "W_CoinCreateWidget.generated.h"

/**
 * 
 */
UCLASS()
class FLIP_SIDE_API UW_CoinCreateWidget : public UUserWidget
{
	GENERATED_BODY()
private:

	//text

	class UCoinCreateWSubsystem* CoinCreateWSubSystem;
	
	class UShopCoinWSubsystem* ShopCoinSubSystem;

	class AGameMode_Shop* ShopGameMode;
protected:

	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

public:


	//제작중인 코인의 앞뒤 바꾸는 함수
	void FlipCreateCoinSide();

private:
	    
	UPROPERTY(meta = (BindWidget))
    class UW_WeaponSelectGrid* dealClassGrid;
    UPROPERTY(meta = (BindWidget))
    class UW_WeaponSelectGrid* tankClassGrid;
    UPROPERTY(meta = (BindWidget))
    class UW_WeaponSelectGrid* utilClassGrid;

    UPROPERTY(meta = (BindWidget))
    class UButton* FinishButton;

	class UW_WeaponSelectGrid* CurrentOpenGrid;

private:
	UFUNCTION()
	void SetClassGrid(EWeaponClass weaponClass);

	UFUNCTION()
	void FinishCreate();
};
