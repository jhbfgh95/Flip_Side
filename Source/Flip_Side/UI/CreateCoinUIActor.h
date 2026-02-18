// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DataTypes/CoinDataTypes.h"
#include "DataTypes/FlipSide_Enum.h"
#include "DataTypes/WeaponDataTypes.h"
#include "CreateCoinUIActor.generated.h"

UCLASS()
class FLIP_SIDE_API ACreateCoinUIActor : public AActor
{
	GENERATED_BODY()
	
private:
	//코인 제작 서브 시스템
	class UCoinCreateWSubsystem* CoinCreateWSubSystem;
	//무기 정보 서브시스템
	class UShopWeaponDataWSubsystem* WeaponDataSubSystem;

private: // 컴포넌트	
//원 콜리전
	UPROPERTY(EditAnywhere,meta = (AllowPrivateAccess = "true"))
	class USphereComponent* SphereCollision;
//메쉬
	UPROPERTY(EditAnywhere,meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* CoinMesh;
//프레스머신 메쉬
	UPROPERTY(EditAnywhere,meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* PressMachineMesh;
	
private:
	ACreateCoinUIActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void ChangeCoinSide();


private: //코인정보
	
	//앞면인지
	bool IsCoinFront = true;

	//현재 코인 면 데이터
	const FFaceData* FrontFaceData; 
	const FFaceData* BackFaceData;
	//참조할 코인 인덱스
	int32 FrontWeaponIndex = -1;
	int32 BackWeaponIndex = -1;

	UPROPERTY(EditAnywhere,meta = (AllowPrivateAccess = "true"))
	FCoinTypeStructure CoinInfo;

	//코인 클래스
	EWeaponClass WeaponType = EWeaponClass::Tank;


private: //코인정보
	//코인무기가 업데이트 됬을 때 설정하는 함수
	UFUNCTION()
	void UpdateCoinWeapon(int32 WeaponIndex);
	//코인 클래스가 업데이트 됐을 때 설정하는 함수
	UFUNCTION()
	void UpdateWeaponClass(EWeaponClass weponClass);

	//코인 메테리얼 색깔,무기 변경
	UFUNCTION()
	void SetCoinSideMatarial();
	
	//코인이 눌렸을 때
	UFUNCTION(BlueprintCallAble)
	void ClickCoin();

	//코인 회전
	UFUNCTION()
	void RotateCoin(float Value);

	//코인이 처음 설정 됬을 때 초기화
	UFUNCTION()
	void InitCoin(FCoinTypeStructure CoinValue, EWeaponClass weponClass);

	//코안 면 초기화
	void ResetSideTexture();

	//코인 면의 무기 데이터 설정
	void SetCoinSideWeaponData(const FFaceData*& faceData , int32 ID);
private:
	//코인 회전 애니메이션 타임라인 컴포넌트
	UPROPERTY(EditAnywhere,meta = (AllowPrivateAccess = "true"))
	class UTimelineComponent* Timeline;

	//코인 회전 커브
	UPROPERTY(EditAnywhere, Category = "Timeline", meta = (AllowPrivateAccess = "true"))
	class UCurveFloat* CoinRotationCurve;

private:
	//코인 회전 시작 변수
	FRotator StartRotation;

	//UFUNCTION()
	//void FinishRotationPannel();

private:

	//프레스 머신 작동 변수
	UPROPERTY(EditAnywhere,Category = "PressMachine",meta = (AllowPrivateAccess = "true"))
	class UTimelineComponent* PressMachineTimeline;
	//프레스머신 작동 커브
	UPROPERTY(EditAnywhere, Category = "PressMachine", meta = (AllowPrivateAccess = "true"))
	class UCurveFloat* PressMachineCurve;

	//기계 시작 위치
	FVector MachineStartLocation;
	//기계 종료 위치
	FVector MachineEndLocation;

	//프레스 머신 애니메이션
	UFUNCTION()
	void PressCoin(float Value);

};
