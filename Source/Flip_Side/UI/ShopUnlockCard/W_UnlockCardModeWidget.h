// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "W_UnlockCardModeWidget.generated.h"

/**
 * 
 */
UCLASS()
class FLIP_SIDE_API UW_UnlockCardModeWidget : public UUserWidget
{
	GENERATED_BODY()


private:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

private:
	class UShopUnlockCardWSubsystem* ShopUnlockCardSubsystem;
	class UDataManagerSubsystem* DataSubsystem;
private:
	
	UPROPERTY(meta =(BindWidget))
	class UUniformGridPanel* CardGrid;

	UPROPERTY(meta = (BindWidget))
	class UButton* UnlockButton;


private:
	
	UPROPERTY(meta = (BindWidget))
	class UW_CardWidget* SelectUnlockCardWidget;

private:
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess))
	TSubclassOf<UUserWidget> UnlockCardWidget;

	TArray<class UW_UnlockCardWidget> UnlockCards;

private:
	int32 ColumnCount = 4;


private:
	class UUnlockGISubsystem* UnlockSubsystem;


	UFUNCTION()
	void UnlockCard();

	UFUNCTION()
	void ChangeCardsLeft();

	UFUNCTION()
	void ChangeCardsRight();

	UFUNCTION()
	void SelectCard(int32 SelectCardDataID);

	UFUNCTION()
	void UnSelectCard();

	UFUNCTION()
	void ExcuteUnlock(int32 unlockCardId);
};
