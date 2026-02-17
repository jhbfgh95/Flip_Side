// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "W_GoToBattelLevelButton.generated.h"

/**
 * 
 */
UCLASS()
class FLIP_SIDE_API UW_GoToBattelLevelButton : public UUserWidget
{
	GENERATED_BODY()
	

private:
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"))
	class UButton* GotoBattleButton;


};
