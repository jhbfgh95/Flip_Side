// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DataTypes/FlipSide_Enum.h"
#include "DataTypes/WeaponDataTypes.h"
#include "W_UnlockWeaponGrid.generated.h"

/**
 * 
 */
UCLASS()
class FLIP_SIDE_API UW_UnlockWeaponGrid : public UUserWidget
{
	GENERATED_BODY()
	

private:

    class UShopUnlockWeaponWSubsystem* UnlockWeaponSubsystem;
	class UUnlockGISubsystem* UnlockSubSystem;

private:
	virtual void NativeConstruct() override;
    virtual void NativeDestruct() override;
private:

	TArray<class UW_UnlockWeaponButton*> UnlockButtons;

    UPROPERTY(meta = (BindWidget))
    class UUniformGridPanel* ButtonGrid;


private:
    void InitWeaponGrid();

    void AddButton(int32 WeaponID);
public:

    UPROPERTY(EditAnywhere)
    TSubclassOf<class UW_UnlockWeaponButton> WeaponUnlockButton;

	UPROPERTY(EditAnywhere)
	EWeaponClass GridWeaponClass;
	
	UPROPERTY(EditAnywhere)
	int32 ColumnCount;

};
