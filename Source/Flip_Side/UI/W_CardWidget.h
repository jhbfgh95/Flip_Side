// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DataTypes/CardTypes.h"
#include "W_CardWidget.generated.h"

/**
 * 
 */


UCLASS()
class FLIP_SIDE_API UW_CardWidget : public UUserWidget
{
	GENERATED_BODY()
	
	protected:
	virtual void NativeConstruct() override;

	private:
	UPROPERTY(meta = (BindWidget))
	class UBorder* CardImage;
	
	UPROPERTY(meta = (BindWidget))
	class UImage* CardIconImage;
	
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* CardTitle;
	
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* CardDescription;

	public:
	//void SetCardIcon(UImage* Image);
	//void InitCard(UTexture2D* IconImage, FString Title,FString Description);
	void InitCard(FCardData CardData);
};
