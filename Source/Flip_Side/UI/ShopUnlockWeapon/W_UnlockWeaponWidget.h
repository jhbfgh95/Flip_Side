// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "WeaponDataTypes.h"
#include "DataTypes/WeaponDataTypes.h"
#include "W_UnlockWeaponWidget.generated.h"

/**
 * 
 */
UCLASS()
class FLIP_SIDE_API UW_UnlockWeaponWidget : public UUserWidget
{
	GENERATED_BODY()
	
private:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
private:
	class UDataManagerSubsystem* DataSubsystem;
private:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UW_UnlockWeaponSlotContainer> UnlockWeaponSlotContainer;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UW_SelectedUnlockWeapon> SelectedUnlockWeapon;

	UPROPERTY(meta =(BindWidget))
	TObjectPtr<class UW_UnlockSelectWeaponButton> UnlockButton;

	UPROPERTY(meta = (BindWidget))
	class UW_WeaponDescription* WeaponDes;

	UPROPERTY(meta = (BindWidget))
	class UW_PriceWidget* WeaponPriceText;

public:
	UW_UnlockWeaponSlotContainer* GetUnlockWeaponSlotContainer() const;
	UW_SelectedUnlockWeapon* GetSelectedUnlockWeapon() const;
	UW_UnlockSelectWeaponButton* GetUnlockButton() const;
	UW_PriceWidget* GetWeaponPriceWidget() const;
	UW_WeaponDescription* GetWeaponDescription() const;
};
