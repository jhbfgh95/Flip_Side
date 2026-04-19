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
	class UShopUnlockWeaponWSubsystem* UnlockWeaponSubsystem;
	class UDataManagerSubsystem* DataSubsystem;
private:
	UPROPERTY(meta =(BindWidget))
	UUserWidget* UnlockButton;

	UPROPERTY(meta = (BindWidget))
	class UW_WeaponDescription* WeaponDes;

private:

	void SetDesText(FFaceData FaceData);
	void SetDes(int32 WeaponID);

private:

	UFUNCTION()
	void SetWidgetState(EWeaponClass WeaponClass, int32 WeaponID, bool IsItemUnlock);

	UFUNCTION()
	void HideWidget();
	UFUNCTION()
	void VisibleWidget();
};
