// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DataTypes/FlipSide_Enum.h"
#include "DataTypes/WeaponDataTypes.h"
#include "W_UnlockWeaponButton.generated.h"

/**
 * 
 */
UCLASS()
class FLIP_SIDE_API UW_UnlockWeaponButton : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	
	virtual void NativeOnInitialized() override;
	virtual void NativeDestruct() override;

private:

	class UShopUnlockWeaponWSubsystem* UnlockWeaponSubsystem;
	class UUnlockGISubsystem* UnlockGISubsystem;
	class UDataManagerSubsystem* DataManager;
protected:

	UPROPERTY(meta = (BindWidget))
	class UImage* LockImage;

	UPROPERTY(meta = (BindWidget))
	class UImage* WeaponImage;
	
	UPROPERTY(meta = (BindWidget))
	class UButton* WeaponButton;

	FFaceData WeaponData;

    UPROPERTY(meta = (BindWidget))
    class UTextBlock* WeaponNameBlock;

	UPROPERTY(EditAnywhere, Category = "Info")
	FString WeaponName;

	UPROPERTY(EditAnywhere, Category="Button")
	class UTexture2D* WeaponTexture;

	
private:
	
	UPROPERTY(meta = (BindWidgetAnim), Transient)
    UWidgetAnimation* FadeAnim;

public:
	UFUNCTION()
	void UnlockWeapon(EWeaponClass SettingWeaponClass, int32 ID);

	void InitButton(int32 ID);

	UFUNCTION()
	void ClickWeaponButton();
};
