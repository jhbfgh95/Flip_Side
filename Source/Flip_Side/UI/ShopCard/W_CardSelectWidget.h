// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "W_CardSelectWidget.generated.h"

/**
 * 
 */
UCLASS()
class FLIP_SIDE_API UW_CardSelectWidget : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
private:
	class UShopCardWSubsystem* ShopCardSubSystem;
	class UUnlockGISubsystem* UnlockSubsystem;
protected:
	UPROPERTY(meta = (BindWidget))
	class UButton* LeftCardButton;
	
	UPROPERTY(meta = (BindWidget))
	class UButton* RightCardButton;
	
	UPROPERTY(meta = (BindWidget))
	class UButton* SelectCardButton;

	UPROPERTY(meta = (BindWidget))
	class UW_CardWidget* CurrentCardWidget;
public:
	UFUNCTION()
	void ClickLeftCardButton();
	
	UFUNCTION()
	void ClickRightCardButton();

	UFUNCTION()
	void ClickSelectCardButton();
	
	UFUNCTION()
	void InitCard(FCardData CardData);

	UFUNCTION()
	void SetShowCard(int32 UnlockCardID);
};
