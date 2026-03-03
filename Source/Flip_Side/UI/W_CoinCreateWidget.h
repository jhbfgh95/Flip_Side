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

	


public:

	//설명 위젯 //추후 설명창 위젯으로 변경
    UPROPERTY(meta = (BindWidget))
    class UTextBlock* explainText;
//////////////////
	//코인 무기설정 요청 함수
	void SetCreateCoinWepon(int32 WeponID);
	//코인 정보 업데이트 하는 함수
	//매개변수로 코인 상태를 받아야함
	void UpdateCoinState(struct FCoinTypeStructure UpdateCoinInfo);
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
	class UButton* ChangeCoinSideButton;
	
    UPROPERTY(meta = (BindWidget))
	class UButton* TestClass;
    UPROPERTY(meta = (BindWidget))
	class UButton* TestClass2;
    UPROPERTY(meta = (BindWidget))
	class UButton* TestClass3;
	
	UFUNCTION()
	void SetDeal();
	UFUNCTION()
	void SetTank();
	UFUNCTION()
	void SetUtil();


    UPROPERTY(meta = (BindWidget))
    class UButton* FinishButton;

	class UW_WeaponSelectGrid* CurrentOpenGrid;

private:
	UFUNCTION()
	void ChangeCoinSide();

	UFUNCTION()
	void SetClassGrid(EWeaponClass weaponClass);

	UFUNCTION()
	void FinishCreate();
};
