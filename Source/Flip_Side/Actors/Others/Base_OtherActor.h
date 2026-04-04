// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BattleClickInterface.h"
#include "Base_OtherActor.generated.h"

UENUM(BlueprintType)
enum class EOthersType : uint8
{
    None,
	Wall,
	Turret
};

UCLASS()
class FLIP_SIDE_API ABase_OtherActor : public AActor, public IBattleClickInterface
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "Others | Component", meta = (AllowPrivateAccess = "true"))
	class USceneComponent* OthersRootComp;
	
	UPROPERTY(EditAnywhere, Category = "Others | Component", meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* OthersMesh;

	UPROPERTY(VisibleAnywhere, Category = "Others | Component", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UWidgetComponent> OtherHPUI;

	UPROPERTY(EditAnywhere, Category = "Others | Type", meta = (AllowPrivateAccess = "true"))
	EOthersType MyType = EOthersType::None;

	UPROPERTY(EditAnywhere, Category = "Others | Stat", meta = (AllowPrivateAccess = "true"))
	int32 OtherActorMaxHP = 5;

	UPROPERTY(VisibleAnywhere, Category = "Others | Stat")
	int32 CurrentHP = 0;
public:	
	ABase_OtherActor();

	EOthersType GetOtherType() const { return MyType; }

	int32 GetHP() const { return CurrentHP; }

	void ApplyDamage(const int32 damage, AActor* damageCauser);

	void ApplyHeal(const int32 heal, AActor* healCauser);

	UFUNCTION(BlueprintImplementableEvent, Category = "Others | Dead")
	void ActingDeadEffect();

protected:
	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

	virtual void OnClicked_Implementation() override;

	void OnDead();

};
