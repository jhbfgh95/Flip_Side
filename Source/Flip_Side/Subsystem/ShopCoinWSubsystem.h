// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "ShopCoinWSubsystem.generated.h"

#define MAX_TOTAL_COIN 30
/**
 * 
 */
UCLASS()
class FLIP_SIDE_API UShopCoinWSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()
	
protected:
	//상점 레벨일 경우에만 생성
	virtual bool ShouldCreateSubsystem(UObject* Outer) const override;
	
private:
	//총 코인 개수
	int32 TotalCoinCount;

public:
	bool CanAddCoin();

	void AddTotalCoin();
	void RemoveTotalCoin();
};
