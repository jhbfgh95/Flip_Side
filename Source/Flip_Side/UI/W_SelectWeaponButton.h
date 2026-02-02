// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DataTypes/FlipSide_Enum.h"
#include "W_SelectWeaponButton.generated.h"

/**
 * 
 */
UCLASS()
class FLIP_SIDE_API UW_SelectWeaponButton : public UUserWidget
{
	GENERATED_BODY()
	

private:
	class UCoinCreateWSubsystem* CoinCreateWSubSystem;
	
protected:
	virtual void NativeConstruct() override;

public:

	//아이콘
	UPROPERTY(EditAnywhere, Category = "Info")
	int32 WeaponID;

	UPROPERTY(EditAnywhere, Category = "Info")
	EWeaponClass WeaponClass;

	UPROPERTY(EditAnywhere, Category = "Info")
	FString WeaponName;

	//아이디
    UPROPERTY(meta = (BindWidget))
    class UButton* WeaponButton;

	
    UPROPERTY(meta = (BindWidget))
    class UTextBlock* WeaponNameBlock;

	UPROPERTY(EditAnywhere, Category="Button")
	class UTexture2D* WeapoTexture;



	//함수
	//class UW_CoinCreateWidget* CoinCreateWidget;

	//코인 면 설정
	UFUNCTION()
	void SelectWeapon();
	
	void InitButton();


	//찾는 버튼인지 판단
	bool IsFindWeapon(FText FindText);
};
