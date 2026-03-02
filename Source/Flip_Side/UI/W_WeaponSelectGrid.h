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

private:
    class UUnlockGISubsystem* UnlockSubSystem;

    class UShopWeaponDataWSubsystem* ShopWeaponSubsystem;
private:

    TArray<class UW_SelectWeaponButton*> SelectWeaponButtons;

    int32 ButtonNum = 9;

    int32 CurrentPage = 0;

    UPROPERTY(meta = (BindWidget))
    class UButton* NextPageButton;

    UPROPERTY(meta = (BindWidget))
    class UButton* PreviousPageButton;

    UPROPERTY(meta = (BindWidget))
    UW_SelectWeaponButton* SelectWeaponButton1;
    UPROPERTY(meta = (BindWidget))
    UW_SelectWeaponButton* SelectWeaponButton2;
    UPROPERTY(meta = (BindWidget))
    UW_SelectWeaponButton* SelectWeaponButton3;
    UPROPERTY(meta = (BindWidget))
    UW_SelectWeaponButton* SelectWeaponButton4;
    UPROPERTY(meta = (BindWidget))
    UW_SelectWeaponButton* SelectWeaponButton5;
    UPROPERTY(meta = (BindWidget))
    UW_SelectWeaponButton* SelectWeaponButton6;
    UPROPERTY(meta = (BindWidget))
    UW_SelectWeaponButton* SelectWeaponButton7;
    UPROPERTY(meta = (BindWidget))
    UW_SelectWeaponButton* SelectWeaponButton8;
    UPROPERTY(meta = (BindWidget))
    UW_SelectWeaponButton* SelectWeaponButton9;
private:
    
	UPROPERTY(meta = (BindWidgetAnim), Transient)
    UWidgetAnimation* OpenWeaponGridAnim;
    
	UPROPERTY(meta = (BindWidgetAnim), Transient)
    UWidgetAnimation* CloseWeaponGridAnim;
public:
    UFUNCTION()
    void InitWeaponGrid(EWeaponClass WeaponClass,int32 Index);

    UPROPERTY(EditAnywhere)
    TSubclassOf<class UW_SelectWeaponButton> WeaponSelectButton;

    UPROPERTY(EditAnywhere)
    int32 ColumnCount;

    UPROPERTY(EditAnywhere, Category = "Info")
	EWeaponClass GridWeaponClass;

    UFUNCTION()
	void SetNextPageGrid();

	UFUNCTION()
	void SetPreviousPageGrid();

	void SetPageButton();

    void OpenWeaponGrid();
    void CloseWeaponGrid();

    void InitPanelAnimation();
};
