// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DataTypes/FlipSide_Enum.h"
#include "W_UnlockWeaponGrid.generated.h"

/**
 * 
 */
UCLASS()
class FLIP_SIDE_API UW_UnlockWeaponGrid : public UUserWidget
{
	GENERATED_BODY()
	

private:
	class UShopWeaponDataWSubsystem* WeaponDataSubSystem;
	class UUnlockGISubsystem* UnlockSubSystem;
private:
	virtual void NativeConstruct() override;

private:

    UPROPERTY(meta = (BindWidget))
    class UW_UnlockWeaponButton* UnlockButton1;
    UPROPERTY(meta = (BindWidget))
    class UW_UnlockWeaponButton* UnlockButton2;
    UPROPERTY(meta = (BindWidget))
    class UW_UnlockWeaponButton* UnlockButton3;
    UPROPERTY(meta = (BindWidget))
    class UW_UnlockWeaponButton* UnlockButton4;
    UPROPERTY(meta = (BindWidget))
    class UW_UnlockWeaponButton* UnlockButton5;
    UPROPERTY(meta = (BindWidget))
    class UW_UnlockWeaponButton* UnlockButton6;
    UPROPERTY(meta = (BindWidget))
    class UW_UnlockWeaponButton* UnlockButton7;
    UPROPERTY(meta = (BindWidget))
    class UW_UnlockWeaponButton* UnlockButton8;
    UPROPERTY(meta = (BindWidget))
    class UW_UnlockWeaponButton* UnlockButton9;
private:
    UPROPERTY(meta = (BindWidget))
    class UButton* NextPageButton;

    UPROPERTY(meta = (BindWidget))
    class UButton* PreviousPageButton;

	TArray<class UW_UnlockWeaponButton*> UnlockButtons;

	int32 CurrentPage = 0;


private:

    UPROPERTY(meta = (BindWidgetAnim), Transient)
    UWidgetAnimation* OpenPanelAnim;

    UPROPERTY(meta = (BindWidgetAnim), Transient)
    UWidgetAnimation* ClosePanelAnim;

public:

    UPROPERTY(EditAnywhere)
    TSubclassOf<class UW_UnlockWeaponButton> WeaponUnlockButton;

	UPROPERTY(EditAnywhere)
	EWeaponClass GridWeaponClass;
	
	UPROPERTY(EditAnywhere)
	int32 ColumnCount;

	UFUNCTION()
	void SetNextPageGrid();

	UFUNCTION()
	void SetPreviousPageGrid();

	void SetPageButton();


    void OpenUnlockPanel();
    
    void CloseUnlockPanel();

    void InitPanelAnimation();
};
