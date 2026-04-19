// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DataTypes/CoinDataTypes.h"
#include "DataTypes/FlipSide_Enum.h"
#include "DataTypes/WeaponDataTypes.h"
#include "Interface/ShopMouseInterface.h"
#include "ShopCreateCoin.generated.h"

UCLASS()
class FLIP_SIDE_API AShopCreateCoin : public AActor, public IShopMouseInterface
{
	GENERATED_BODY()
	

protected:
	AShopCreateCoin();	
private:
	class UShopCoinWSubsystem* CoinWSubSystem;
	//무기 정보 서브시스템
	class UDataManagerSubsystem* DataManager;

	class AShopController_FlipSide* ShopController;
	
private:

	UPROPERTY(EditAnywhere,meta = (AllowPrivateAccess = "true"))
	class USceneComponent* RootScene;

	UPROPERTY(EditAnywhere,meta = (AllowPrivateAccess = "true"))
	class USphereComponent* SphereCollision;
	
	UPROPERTY(EditAnywhere,meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* CoinMesh;
	

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;


private:
	UFUNCTION()
	void SetWeaponAdaptor(int32 WeaponID);

private:

	void ChangeCoinSide();

	UFUNCTION()
	void SetCoin();
	//코인이 처음 설정 됬을 때 초기화
	UFUNCTION()
	void SetCoinWeaponID();
private: //코인정보

	//현재 코인 면 데이터
	FFaceData FrontFaceData; 
	FFaceData BackFaceData;


protected:

	//코인 메테리얼 색깔,무기 변경
	UFUNCTION()
	void SetCoinSideMatarial();
	
	//코인이 눌렸을 때
	UFUNCTION(BlueprintCallAble)
	void ClickCoin();

	//코인 회전
	UFUNCTION()
	void RotateCoin(float Value);
	
	UFUNCTION()
	void FinishedRotateCoin();



private:
	//코인 회전 애니메이션 타임라인 컴포넌트
	UPROPERTY(EditAnywhere,meta = (AllowPrivateAccess = "true"))
	class UTimelineComponent* Timeline;

	//코인 회전 커브
	UPROPERTY(EditAnywhere, Category = "Timeline", meta = (AllowPrivateAccess = "true"))
	class UCurveFloat* CoinRotationCurve;

private:

	FRotator FrontRotator;
	FRotator StartRotator;
	
	FRotator ArriveRotator;
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess))
	FRotator CoinTurnRotator;


public:
	virtual void InteractLeftClick_Implementation() override;
};
