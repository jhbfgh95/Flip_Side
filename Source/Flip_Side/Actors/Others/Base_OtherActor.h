// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BattleClickInterface.h"
#include "Base_OtherActor.generated.h"

class ABase_OtherActor;

DECLARE_DYNAMIC_DELEGATE_RetVal_OneParam(bool, FOnOtherClickedDelegate, ABase_OtherActor*, ClickedOther);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnOtherDeadDelegate, ABase_OtherActor*, DeadOther);

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

	UPROPERTY(VisibleAnywhere, Category = "Others | Grid")
	class AGridActor* OccupiedGrid = nullptr;
public:	
	ABase_OtherActor();

	UPROPERTY()
	FOnOtherClickedDelegate OnOtherClicked;

	UPROPERTY()
	FOnOtherDeadDelegate OnOtherDead;

	EOthersType GetOtherType() const { return MyType; }

	int32 GetHP() const { return CurrentHP; }

	void ApplyDamage(const int32 damage, AActor* damageCauser);

	void ApplyHeal(const int32 heal, AActor* healCauser);

	void SetOccupiedGrid(class AGridActor* InGrid) { OccupiedGrid = InGrid; }

	class AGridActor* GetOccupiedGrid() const { return OccupiedGrid; }

	UFUNCTION(BlueprintImplementableEvent, Category = "Others | Dead")
	void ActingDeadEffect();

	UFUNCTION(BlueprintImplementableEvent, Category = "Others | Click")
	void OnDefaultClicked();

protected:
	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

	virtual void OnClicked_Implementation() override;

	bool TryBroadcastOtherClicked();

	void OnDead();

};
