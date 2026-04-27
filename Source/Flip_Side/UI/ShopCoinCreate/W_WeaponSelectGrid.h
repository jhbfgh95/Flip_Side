// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DataTypes/FlipSide_Enum.h"
#include "W_WeaponSelectGrid.generated.h"

/**
 * 
 */
class UW_SelectWeaponButton;
UCLASS()
class FLIP_SIDE_API UW_WeaponSelectGrid : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	virtual void NativeConstruct() override;
    virtual void NativeDestruct() override;

private:
    class UUnlockGISubsystem* UnlockSubSystem;

private:

    TArray<class UW_SelectWeaponButton*> SelectWeaponButtons;

    UPROPERTY(meta = (BindWidget))
    class UUniformGridPanel* ButtonGrid;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* DescripText;
private:
    
	UPROPERTY(meta = (BindWidgetAnim), Transient)
    UWidgetAnimation* OpenWeaponGridAnim;
    
	UPROPERTY(meta = (BindWidgetAnim), Transient)
    UWidgetAnimation* CloseWeaponGridAnim;


private:
    void AddButton(int32 ID);
    UFUNCTION()
    void OnWeaponUnlockAdaptor(EWeaponClass WeaponClass,int32 ID);
    UFUNCTION()
    void ShowDescripText();
    UFUNCTION()
    void HideDescripText();

public:
    UFUNCTION()
    void InitWeaponGrid();

    UPROPERTY(EditAnywhere)
    TSubclassOf<class UW_SelectWeaponButton> WeaponSelectButton;

    UPROPERTY(EditAnywhere)
    int32 ColumnCount;

    UPROPERTY(EditAnywhere, Category = "Info")
	EWeaponClass GridWeaponClass;


    void OpenWeaponGrid();
    void CloseWeaponGrid();

    void InitPanelAnimation();
};
