// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DataTypes/WeaponDataTypes.h"
#include "GameFramework/Actor.h"
#include "ShopUnlockWeaponUIActor.generated.h"

class UCurveFloat;
class UMaterialInstanceDynamic;
class UNiagaraComponent;
class USceneComponent;
class UStaticMeshComponent;
class UTimelineComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnShopUnlockWeaponShakeFinished);

UCLASS()
class FLIP_SIDE_API AShopUnlockWeaponUIActor : public AActor
{
	GENERATED_BODY()
	
public:
	AShopUnlockWeaponUIActor();

    // 선택한 무기 아이콘을 메시에 표시하고, 필요하면 흔들림을 재생합니다.
	UFUNCTION(BlueprintCallable, Category = "Shop Unlock Weapon")
	void SetWeaponIcon(const FFaceData& InWeaponData);

	UFUNCTION(BlueprintCallable, Category = "Shop Unlock Weapon")
	void ResetWeaponIcon();

	UFUNCTION(BlueprintCallable, Category = "Shop Unlock Weapon")
	bool PlayBuyWeaponAnim();

	UFUNCTION(BlueprintCallable, Category = "Shop Unlock Weapon")
	void StartHoldShake();

	UFUNCTION(BlueprintCallable, Category = "Shop Unlock Weapon")
	void StopHoldShake();

	void CompleteHoldShake();

	UPROPERTY(BlueprintAssignable, Category = "Shop Unlock Weapon")
	FOnShopUnlockWeaponShakeFinished OnShakeFinished;

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, Category = "Shop Unlock Weapon|Component")
	TObjectPtr<USceneComponent> PreviewRoot;

	UPROPERTY(EditAnywhere, Category = "Shop Unlock Weapon|Component")
	TObjectPtr<UStaticMeshComponent> WeaponMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Shop Unlock Weapon|Component")
	TObjectPtr<UNiagaraComponent> UnlockWeaponEffect;

	UPROPERTY(EditAnywhere, Category = "Shop Unlock Weapon|Material")
	FLinearColor TypeColor = FLinearColor::White;

	UPROPERTY(VisibleAnywhere, Category = "Shop Unlock Weapon|Animation")
	TObjectPtr<UTimelineComponent> ShakeTimeline;

	UPROPERTY(VisibleAnywhere, Category = "Shop Unlock Weapon|Animation")
	TObjectPtr<UTimelineComponent> HoldShakeTimeline;

	UPROPERTY(EditAnywhere, Category = "Shop Unlock Weapon|Animation")
	TObjectPtr<UCurveFloat> ShakeCurve;

	// 비워 두면 ShakeCurve를 공유합니다.
	UPROPERTY(EditAnywhere, Category = "Shop Unlock Weapon|Animation")
	TObjectPtr<UCurveFloat> HoldShakeCurve;

	// 각 축의 최대 흔들림 거리입니다. ShakeCurve 값은 전체 흔들림 세기로 사용됩니다.
	UPROPERTY(EditAnywhere, Category = "Shop Unlock Weapon|Animation")
	FVector ShakeExtent = FVector(8.0f, 8.0f, 8.0f);

	UPROPERTY(EditAnywhere, Category = "Shop Unlock Weapon|Animation")
	FVector HoldShakeExtent = FVector(2.0f, 2.0f, 2.0f);

	UPROPERTY(Transient)
	TObjectPtr<UMaterialInstanceDynamic> WeaponMaterialInstance;

	UPROPERTY(VisibleInstanceOnly, Category = "Shop Unlock Weapon|Data")
	FFaceData SelectedWeaponData;

private:
	FVector WeaponMeshStartLocation;
	bool bIsHoldShaking = false;

	UFUNCTION()
	void UpdateShakeMovement(float CurveValue);

	UFUNCTION()
	void FinishShakeMovement();

	UFUNCTION()
	void UpdateHoldShakeMovement(float CurveValue);

};
