// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/W_ShopCheckStartGame.h"

#include "Components/Button.h"
#include "Components/TextBlock.h"

void UW_ShopCheckStartGame::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	StartGameButton->OnClicked.AddDynamic(this, &ThisClass::HandleStartGameButtonClicked);
	CancelButton->OnClicked.AddDynamic(this, &ThisClass::HandleCancelButtonClicked);
}

void UW_ShopCheckStartGame::SetCheckStartGameText(bool bHasCoin)
{
	const FText Message = bHasCoin
		? NSLOCTEXT("ShopStartConfirmation", "Start", "게임을 시작하시겠습니까?")
		: NSLOCTEXT("ShopStartConfirmation", "StartWithoutCoins", "코인이 없습니다. 게임을 시작하시겠습니까?");

	CheckStartGameText->SetText(Message);
}

void UW_ShopCheckStartGame::HandleStartGameButtonClicked()
{
	OnStartGameConfirmed.Broadcast();
}

	
void UW_ShopCheckStartGame::HandleCancelButtonClicked()
{
	OnShopStartGameCancelConfirmed.Broadcast(EShopPage::Main);
}
