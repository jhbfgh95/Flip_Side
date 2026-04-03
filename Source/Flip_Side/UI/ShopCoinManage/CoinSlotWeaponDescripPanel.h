// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DataTypes/WeaponDataTypes.h"
#include "CoinSlotWeaponDescripPanel.generated.h"

UCLASS()
class FLIP_SIDE_API ACoinSlotWeaponDescripPanel : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACoinSlotWeaponDescripPanel();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;


private:
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"), Category = "Components")
	class USceneComponent* RootScene;
	// 몸통 메쉬
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"), Category = "Components")
	class UStaticMeshComponent* PanelMesh;
	
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"))
	class UWidgetComponent* DescripWidget;

private:
	class UDataManagerSubsystem* DataManager;
	class UShopCoinWSubsystem* CoinSubsystem;
	

private:
	class UW_WeaponDescription* DescripWidgetClass;

private:
	UFUNCTION()
	void SetPanelUnlockSlot();

	UFUNCTION()
	void SetPanelChangeSlot(bool IsChangeToBottom);

	UFUNCTION()
	void SetPanelCoinCreate(int32 CreatedCoinIndex, EWeaponClass CreateWeaponClass);

public:
	UFUNCTION()
	void SetPanel(bool IsSlotCoinFront);

	void SetPanelDescrip(FFaceData FaceData);
};
