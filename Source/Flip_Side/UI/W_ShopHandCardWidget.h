// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/W_CardWidget.h"
#include "DataTypes/CardTypes.h"
#include "W_ShopHandCardWidget.generated.h"

/**
 * 
 */
UCLASS()
class FLIP_SIDE_API UW_ShopHandCardWidget : public UW_CardWidget
{
	GENERATED_BODY()
	
protected:
	virtual void NativeConstruct() override;

	class UShopCardWSubsystem* ShopCardSubSystem;

	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry,const FPointerEvent& InMouseEvent) override;

	virtual void NativeOnMouseEnter(const FGeometry& InGeometry,const FPointerEvent& InMouseEvent) override;

    virtual void NativeOnMouseLeave(const FPointerEvent& InMouseEvent) override;

protected:
	
    UPROPERTY(meta = (BindWidgetAnim), Transient)
    UWidgetAnimation* SelectCardAnim;

	UPROPERTY(meta = (BindWidgetAnim), Transient)
    UWidgetAnimation* UnSelectCardAnim;

	UPROPERTY(meta = (BindWidgetAnim), Transient)
    UWidgetAnimation* AddCardAnim;

	UPROPERTY(meta = (BindWidgetAnim), Transient)
    UWidgetAnimation* RemoveCardAnim;


	void PlayCardAnim(UWidgetAnimation* Anim);
private:
	
	UFUNCTION()
    void OnUnSelectCardAnimFinished();


private:
	bool CanControl;

public:
	UPROPERTY(EditAnywhere)
	int32 HandIndex;
	UFUNCTION()
	void SelectedCard(int32 Index, FCardData CardData);
	void RemoveCard();
};
