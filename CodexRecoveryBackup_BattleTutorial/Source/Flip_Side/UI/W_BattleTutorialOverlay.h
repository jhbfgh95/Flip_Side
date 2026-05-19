// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "W_BattleTutorialOverlay.generated.h"

class UImage;
class UMaterialInstanceDynamic;
class UTextBlock;

/**
 * 
 */
UCLASS()
class FLIP_SIDE_API UW_BattleTutorialOverlay : public UUserWidget
{
	GENERATED_BODY()
	
public:
	virtual void NativeConstruct() override;

	UFUNCTION(BlueprintCallable)
	void SetTutorialText(const FText& InText);

	UFUNCTION(BlueprintCallable)
	void SetFocusFromWorldLocation(APlayerController* PlayerController, const FVector& WorldLocation, const FVector2D& HoleSize);

	UFUNCTION(BlueprintCallable)
	void SetFocusUV(const FVector2D& CenterUV, const FVector2D& HoleSize);

	UFUNCTION(BlueprintCallable)
	void SetDimOpacity(float InOpacity);

	UFUNCTION(BlueprintCallable)
	void SetFeather(float InFeather);

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> DimMaskImage;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> TutorialText;

private:
	UPROPERTY()
	TObjectPtr<UMaterialInstanceDynamic> DimMaterial;
};
