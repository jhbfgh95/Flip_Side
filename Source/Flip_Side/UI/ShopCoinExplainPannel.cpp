// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/ShopCoinExplainPannel.h"
#include "Subsystem/ShopLevel/CoinCreateWSubsystem.h"


void AShopCoinExplainPannel::BeginPlay()
{
    Super::BeginPlay();

    CoinCreateWSubSystem =  GetWorld()->GetSubsystem<UCoinCreateWSubsystem>();

	if(CoinCreateWSubSystem)
	{
		CoinCreateWSubSystem->OnSelectedCoinUpdate.AddDynamic(this, &AShopCoinExplainPannel::GetExplainByID);
        CoinCreateWSubSystem->OnSelectedCoin.AddDynamic(this, &AShopCoinExplainPannel::InitPannel);
	}
}


void AShopCoinExplainPannel::GetExplainByID(int32 ID)
{
    //Super::GetExplainByID(ID);
    FString text = FString::Printf(TEXT("Score: %d"), ID);
    UpdateExplanationText(text);
}

void AShopCoinExplainPannel::InitPannel(FCoinTypeStructure CoinInfo, EWeaponClass CoinClass)
{
    GetExplainByID(CoinInfo.FrontWeaponID);
}