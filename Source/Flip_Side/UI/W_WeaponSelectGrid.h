// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DataTypes/FlipSide_Enum.h"
#include "W_WeaponSelectGrid.generated.h"

/**
 * 
 */
UCLASS()
class FLIP_SIDE_API UW_WeaponSelectGrid : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	virtual void NativeConstruct() override;

private:
    class UUnlockGISubsystem* UnlockSubSystem;

    class UShopWeaponDataWSubsystem* ShopWeaponSubsystem;
private:
    UPROPERTY(meta = (BindWidget))
    class UUniformGridPanel* weaponButtonGrid;

    int32 CurrentAddNum;
public:
    UPROPERTY(EditAnywhere)
    TSubclassOf<class UW_SelectWeaponButton> WeaponSelectButton;

    UPROPERTY(EditAnywhere)
    int32 ColumnCount;

    UPROPERTY(EditAnywhere, Category = "Info")
	EWeaponClass GridWeaponClass;

    UFUNCTION()
    void AddWeaponButton(EWeaponClass WeaponClass,int32 Index);
};
