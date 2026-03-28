// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DataTypes/FlipSide_Enum.h"
#include "DataTypes/WeaponDataTypes.h"
#include "UnlockWeaponExplainPanel.generated.h"

UCLASS()
class FLIP_SIDE_API AUnlockWeaponExplainPanel : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AUnlockWeaponExplainPanel();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:
	UPROPERTY(EditAnywhere, meta = (AccessPrivateAllow="true"))
	class USceneComponent* RootScene;
	
	UPROPERTY(EditAnywhere, meta = (AccessPrivateAllow="true"))
	class UStaticMeshComponent* ExplainPanel;
	
	UPROPERTY(EditAnywhere, meta = (AccessPrivateAllow="true"))
	class UWidgetComponent* ExplainWidget;
private:
	class UW_WeaponDescription* ExplainWidgetClass;
private:
	class UShopUnlockWeaponWSubsystem* ShopUnlockSubsystem;

	class UDataManagerSubsystem* DataManagerSubsystem;

private:
	UFUNCTION()
	void SetUnlockWeaponText(EWeaponClass WeaponClass,int32 ItemID, bool IsItemUnlock);
};
