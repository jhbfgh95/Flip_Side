// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DataTypes/CardTypes.h"
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
	class UShopCardWSubsystem* ShopCardSubsystem;
	class UDataManagerSubsystem* DataSubsystem;
	class UUnlockGISubsystem* UnlockSubsystem;
private:
	
	UPROPERTY(meta =(BindWidget))
	class UUniformGridPanel* CardGrid;

	UPROPERTY(meta = (BindWidget))
	class UButton* UnlockButton;

	UPROPERTY(meta = (BindWidget))
	class UButton* SelectPlayerCardButton;

	UPROPERTY(meta = (BindWidget))
	class UButton* UnSelectPlayerCardButton;

	UPROPERTY(meta = (BindWidget))
	class UUserWidget* CancelImage;

private:
	
	UPROPERTY(meta = (BindWidget))
	class UW_CardWidget* SelectCardWidget;

private:
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess))
	TSubclassOf<UUserWidget> ShopCardWidget;

	TArray<class UW_ShopCardWidget*> ShopCards;

private:
	int32 ColumnCount = 4;

private:
	FCardData CurrentCardData;
private:

	UFUNCTION()
	void UnlockCard();

	UFUNCTION()
	void SelectCard(FCardData SelectCardData);

	UFUNCTION()
	void UnSelectCard();

	UFUNCTION()
	void SelectPlayerCard();

	UFUNCTION()
	void UnSelectPlayerCard();

	UFUNCTION()
	void ExcuteUnlock(int32 unlockCardId);

	UFUNCTION()
	void UnlockCardAdaptor(int32 ID);
};
