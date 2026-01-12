// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DataTypes/CoinDataTypes.h"
#include "DataTypes/GridTypes.h"
#include "DataTypes/FlipSide_Enum.h"
#include "CoinActor.generated.h"

UCLASS(abstract)
class ACoinActor : public AActor
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "Coin | Component", meta = (AllowPrivateAccess = "true"))
	class USceneComponent* CoinRootComp;

	UPROPERTY(EditAnywhere, Category = "Coin | Component", meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* CoinMesh;

	UPROPERTY(VisibleAnywhere, Category = "Coin | Component", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UWidgetComponent> CoinHPUI;

	//인스턴스화된 코인들의 각 번호
	UPROPERTY(VisibleAnywhere, Category = "Coin | ID")
	int32 CoinID = 0;

	//무기 타입(탱딜힐)의 아이디
	UPROPERTY(VisibleAnywhere, Category = "Coin | Type")
	int TypeID = 0;

	//무기 타입 ENum 위에거나 이거 둘 중 하나 없앨 예정
	UPROPERTY(VisibleAnywhere, Category = "Coin | Type")
	EWeaponClass WeaponType = EWeaponClass::None;

	//앞면 무기 ID
	UPROPERTY(VisibleAnywhere, Category = "Coin | WeaponID")
	int FrontWeaponID = 0;

	//뒷면 무기 ID
	UPROPERTY(VisibleAnywhere, Category = "Coin | WeaponID")
	int BackWeaponID = 0;

	//랜덤 앞뒤 정해질 때 즉, SetCoinFace할 때 그냥 해당 WeaponID 넣어버림
	int DecidedWeaponID = 0;

	//코인 전체 HP
	UPROPERTY(VisibleAnywhere, Category = "Coin | Mesh", meta = (AllowPrivateAccess = "true"))
	int32 HP = 0;
	
	//이거로 Getter, Setter로 앞뒤 판별
	UPROPERTY(VisibleAnywhere, Category = "Coin | Face")
	EFaceState CurrentFace = EFaceState::None;

	//판때기 위에 올라갈 때 어디에 올라갈지 정해줌
	UPROPERTY(VisibleAnywhere, Category = "Coin | Grid")
	FGridPoint CurrentGridPoint;

public:	
	ACoinActor();

	int32 GetCoinID();

	/* 앞,뒤 결정 */
	EFaceState GetCoinFace();

	void SetCoinFace(EFaceState DecidedFace);

	/* BattleGrid에 나올 위치 설정 */
	void SetGridPoint(FGridPoint DecidedGridPoint);

protected:
	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

};
