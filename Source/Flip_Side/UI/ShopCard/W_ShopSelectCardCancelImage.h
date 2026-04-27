// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "W_ShopSelectCardCancelImage.generated.h"

/**
 * 
 */
UCLASS()
class FLIP_SIDE_API UW_ShopSelectCardCancelImage : public UUserWidget
{
	GENERATED_BODY()
	


	virtual void NativeConstruct() override;

	class UShopCardWSubsystem* ShopCardSubsystem;

	UPROPERTY(meta = (BindWidget))
	class UImage* CancelImage;

	
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry,const FPointerEvent& InMouseEvent) override;

};
