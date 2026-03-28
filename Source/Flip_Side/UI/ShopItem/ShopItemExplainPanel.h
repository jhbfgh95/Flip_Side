// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DataTypes/ItemDataTypes.h"
#include "ShopItemExplainPanel.generated.h"

UCLASS()
class FLIP_SIDE_API AShopItemExplainPanel : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AShopItemExplainPanel();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
private:
	UPROPERTY(EditAnywhere, meta = (AccessPrivateAllow = "true"))
	class USceneComponent* RootScene;
	
	UPROPERTY(EditAnywhere, meta = (AccessPrivateAllow = "true"))
	class UStaticMeshComponent* ExplainPanel;
	
	UPROPERTY(EditAnywhere, meta = (AccessPrivateAllow = "true"))
	class UWidgetComponent* ExplainWidget;
private:
	class UW_ShopItemDescription* ExplainWidgetClass;
private:
	class UShopItemWSubsystem* ShopItemSubsystem;
	class UDataManagerSubsystem* DataManager;

private:
	UFUNCTION()
	void SetExplainPanel(FItemData ItemData);

	UFUNCTION()
	void ResetExplainPanelText();
};
