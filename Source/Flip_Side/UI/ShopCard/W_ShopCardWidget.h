// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UI/W_CardWidget.h"
#include "DataTypes/CardTypes.h"
#include "W_ShopCardWidget.generated.h"

/**
 * 
 */
UCLASS()
class FLIP_SIDE_API UW_ShopCardWidget : public UW_CardWidget
{
	GENERATED_BODY()
	
protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	class UUnlockGISubsystem* UnlockSubsystem;
	class UShopCardWSubsystem* ShopCardSubsystem;

protected:

	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry,const FPointerEvent& InMouseEvent) override;

private:
	UPROPERTY(meta = (BindWidget))
	class UImage* LockImage;

	UPROPERTY(meta = (BindWidget), Transient)
	class UCanvasPanel* MainCanvas;

	FCardData CardData;


private:

	void CheckSelectCard();	

	UFUNCTION()
	void SetCardSelect(int32 SelctCardID);

	UFUNCTION()
	void UnlockCard(int32 UnlockCardID);

public:
	UFUNCTION(BlueprintCallable)
	void HideLockIamge();

	void InitShopCard(FCardData CardDataValue, bool IsUnlock);

};
