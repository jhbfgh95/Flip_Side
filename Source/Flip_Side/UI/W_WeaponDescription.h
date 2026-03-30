// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "W_WeaponDescription.generated.h"

/**
 * 
 */
UCLASS()
class FLIP_SIDE_API UW_WeaponDescription : public UUserWidget
{
	GENERATED_BODY()

	private:
	UPROPERTY(meta= (BindWidget))
	class UTextBlock* WeaponNameText;
	UPROPERTY(meta= (BindWidget))
	class URichTextBlock* WeaponText;

	public:
	void SetExplainText(const FString & WeaponName, const FString & Description, int32 DefaultBP, int32 DefaultAP);
	void SetExplainTextEmpty();
};
