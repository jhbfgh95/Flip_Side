// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/ShopItemExplainPannel.h"

#include "Subsystem/ShopItemWSubSystem.h"


void AShopItemExplainPannel::BeginPlay()
{
    Super::BeginPlay();
    ShopItemSubsystem =  GetWorld()->GetSubsystem<UShopItemWSubsystem>();

	if(ShopItemSubsystem)
	{
		//ShopItemSubsystem->OnSelectedCoinUpdate.AddDynamic(this, &AShopCoinExplainPannel::GetExplainByID);
        ShopItemSubsystem->OnItemHovered.AddDynamic(this, &AShopItemExplainPannel::GetExplainByID);
        ShopItemSubsystem->OnItemUnHovered.AddDynamic(this, &AShopItemExplainPannel::ResetPannel);

	}

}

void AShopItemExplainPannel::GetExplainByID(int32 ID)
{
    FString text = FString::Printf(TEXT("Score: %d"), ID);
    UpdateExplanationText(text);
}


void AShopItemExplainPannel::ResetPannel()
{
    FString text = FString::Printf(TEXT("Select Item!"));
    UpdateExplanationText(text);
}