// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UI/W_CardWidget.h"
#include "W_UnlockCardWidget.generated.h"

/**
 * 
 */
UCLASS()
class FLIP_SIDE_API UW_UnlockCardWidget : public UW_CardWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	class UUnlockGISubsystem* UnlockSubsystem;
	class UShopUnlockCardWSubsystem* ShopUnlockCardSubsystem;

private:

	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry,const FPointerEvent& InMouseEvent) override;

	virtual void NativeOnMouseEnter(const FGeometry& InGeometry,const FPointerEvent& InMouseEvent) override;

    virtual void NativeOnMouseLeave(const FPointerEvent& InMouseEvent) override;

private:
	UPROPERTY(meta = (BindWidget))
	class UImage* LockImage;

	FCardData UnlockCardData;

private:
	
	UPROPERTY(meta = (BindWidgetAnim), Transient)
	class UWidgetAnimation* SelectAnim;
	UPROPERTY(meta = (BindWidgetAnim), Transient)
	class UWidgetAnimation* UnSelectAnim;
	UPROPERTY(meta = (BindWidgetAnim), Transient)
	class UWidgetAnimation* UnlockCardAnim;
private:
	void InitUnlockCard(FCardData CardData);

	void CheckSelectCard();	
	UFUNCTION()
	void SetCardSelect(int32 SelctCardID);
	UFUNCTION()
	void SetUnlockCard(int32 UnlockCardID);
private:

public:
	UFUNCTION(BlueprintCallable)
	void HideLockIamge();
};
