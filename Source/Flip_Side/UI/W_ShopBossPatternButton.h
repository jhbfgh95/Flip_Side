// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "W_ShopBossPatternButton.generated.h"

class UButton;
class UTextBlock;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnShopBossPatternButtonClicked, int32, ButtonIndex);

UCLASS()
class FLIP_SIDE_API UW_ShopBossPatternButton : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeOnInitialized() override;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> PatternButton;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> PatternButtonText;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Boss Pattern")
	int32 ButtonIndex = INDEX_NONE;

public:
	FOnShopBossPatternButtonClicked OnShopBossPatternButtonClicked;

	UFUNCTION(BlueprintCallable, Category = "Boss Pattern")
	void InitPatternButton(int32 SetButtonIndex, const FText& InButtonText);

private:
	UFUNCTION()
	void HandlePatternButtonClicked();
};
