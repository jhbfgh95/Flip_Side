// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DataTypes/CoinDataTypes.h"
#include "DataTypes/GridTypes.h"
#include "DataTypes/FlipSide_Enum.h"
#include "CoinActor.generated.h"

UCLASS()
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

	//코인 전체 HP
	UPROPERTY(VisibleAnywhere, Category = "Coin | Mesh", meta = (AllowPrivateAccess = "true"))
	int32 HP = 0;

	UPROPERTY(VisibleAnywhere)
	UTexture2D* FrontIconTexture;

	UPROPERTY(VisibleAnywhere)
	UTexture2D* BackIconTexture;

/* Battle상태 변수들 */
protected:
	//랜덤 앞뒤 정해질 때 즉, SetCoinFace할 때 그냥 해당 WeaponID 넣어버림
	int DecidedWeaponID = 0;

	//이거로 Getter, Setter로 앞뒤 판별
	UPROPERTY(VisibleAnywhere, Category = "Coin | Face")
	EFaceState CurrentFace = EFaceState::None;

	//판때기 위에 올라갈 때 어디에 올라갈지 정해줌
	UPROPERTY(VisibleAnywhere, Category = "Coin | Grid")
	FGridPoint CurrentGridPoint;

public:	
	ACoinActor();

	// 같은 타입 코인들 중에서 몇 번째 코인인지 나타내는 인덱스
	UPROPERTY(VisibleAnywhere, Category = "Coin | Battle")
    int32 SameTypeIndex = 0;
	int32 GetSameTypeIndex() const;

	int32 GetFrontWeaponID() const;
	void DecrementSameTypeIndex(); // 인덱스 감소를 위한 함수

	int32 GetCoinID();

	void SetCoinValues(
		int CoinId,
		int FrontId, 
		int BackId,
		EWeaponClass WeaponTypes, 
		UTexture2D* FrontTexture, 
		UTexture2D* BackTexture
	);

	/* 앞,뒤 결정 */
	EFaceState GetCoinFace();

	void SetCoinFace(EFaceState DecidedFace);

	/* BattleGrid에 나올 위치 설정 */
	void SetGridPoint(FGridPoint DecidedGridPoint);

	//순서대로 0 탱 1 딜 2 힐
	UPROPERTY(EditAnywhere)
	TArray<FLinearColor> TypeColors;

protected:
	virtual void BeginPlay() override;

	virtual void OnConstruction(const FTransform& Transform) override;

	virtual void Tick(float DeltaTime) override;

};
