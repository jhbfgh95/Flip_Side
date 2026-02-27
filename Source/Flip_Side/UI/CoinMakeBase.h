// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DataTypes/FlipSide_Enum.h"
#include "CoinMakeBase.generated.h"

UCLASS()
class FLIP_SIDE_API ACoinMakeBase : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACoinMakeBase();

private:
	UPROPERTY(EditAnywhere,meta = (AllowPrivateAccess = "true"))
	class USceneComponent* RootSecen;
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* CoinBaseMesh;

private:
	class UCoinCreateWSubsystem* CoinCreateSubsystem;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
private:
	/*딜탱힐 색깔*/
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"))
	FLinearColor DealClassColor;
	
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"))
	FLinearColor TankClassColor;
	
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"))
	FLinearColor UtilClassColor;

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"))
	FLinearColor DefaultColor;
	//색변경 코드
	UFUNCTION()
	void SetMaterial(EWeaponClass WeaponClass);
};
