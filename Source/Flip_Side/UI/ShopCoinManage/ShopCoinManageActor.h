// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ShopCoinManageActor.generated.h"

UCLASS()
class FLIP_SIDE_API AShopCoinManageActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AShopCoinManageActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
private:
	class UShopCoinWSubsystem* ShopCoinSubsystem;
private:
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"), Category = "Components")
	class USceneComponent* RootScene;

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"), Category = "Components")
	class UChildActorComponent* CoinMPanel1;

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"), Category = "Components")
	class UChildActorComponent* CoinMPanel2;

private:
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"))
	class UTimelineComponent* MoveToBottomTimeline;

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"))
	class UTimelineComponent* MoveToTopTimeline;

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "ture"))
	class UCurveFloat* PanelMoveCurve;

private:
	
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"))
	class UChildActorComponent* CurrentShowPanel;
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"))
	class UChildActorComponent* CurrentTopPanel;
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"))
	class UChildActorComponent* CurrentBottomPanel;

	class AShopCoinManagePanel* Panel1Class;
	class AShopCoinManagePanel* Panel2Class;

public:
	UPROPERTY(EditAnywhere)
	FVector ShowVector;
	UPROPERTY(EditAnywhere)
	FVector ReadyTopVector;
	UPROPERTY(EditAnywhere)
	FVector ReadyBottomVector;

private:
	class AShopController_FlipSide* ShopController;


public:

	UFUNCTION(BlueprintCallAble)
	void ChangePanel(bool IsChangeToBottom);

	UFUNCTION(BlueprintCallAble)
	void ChangePanelToBottom();
	
	UFUNCTION(BlueprintCallAble)
	void ChangePanelToTop();

	UFUNCTION()
	void MovePanelToBottom(float Value);
	UFUNCTION()
	void FinishMovePanelToBottom();
	
	UFUNCTION()
	void MovePanelToTop(float Value);
	UFUNCTION()
	void FinishMovePanelToTop();
	
	UFUNCTION()
	void UnlockCurrentPanel();
	
	UFUNCTION()
	void ClickChangeSlotTop();
	
	UFUNCTION()
	void ClickChangeSlotBottom();

	UFUNCTION()
	void InitSlotPanelCoin(int32 CreateCoinIndex, EWeaponClass WeaponClass);

};
