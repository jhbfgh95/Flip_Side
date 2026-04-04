// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CreateCoinDescriptionPanel.generated.h"

UCLASS()
class FLIP_SIDE_API ACreateCoinDescriptionPanel : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACreateCoinDescriptionPanel();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:
	class UCoinCreateWSubsystem* CoinCreateWSubSystem;
	class UDataManagerSubsystem* DataManagerSubsystem;

protected:
	UPROPERTY(EditAnywhere,meta = (AllowPrivateAccess = "true"))
	class USceneComponent* RootSecen;
	
	UPROPERTY(EditAnywhere,meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* PanelMesh;

private:
	//설명 위젯
	UPROPERTY(EditAnywhere,meta = (AllowPrivateAccess = "true"))
	class UWidgetComponent* ExplainWidget;

	class UW_WeaponDescription* ExplainWidgetClass;

public:
	UFUNCTION()
	void GetExplainByID(int32 ID, bool IsFront);

private:
	int32 CurrentID;
	void SetDescriptionText(int32 ID);


	

};
