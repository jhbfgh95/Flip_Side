// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/W_ShopMainWidget.h"
#include "W_Tuto_ShopMainWidget.generated.h"

/**
 * 
 */
UCLASS()
class FLIP_SIDE_API UW_Tuto_ShopMainWidget : public UW_ShopMainWidget
{
	GENERATED_BODY()
		
virtual void NativeConstruct() override;
	virtual void ClickBattleStartButton() override;
	class AGameMode_ShopTuto* TutoGameMode;
};
