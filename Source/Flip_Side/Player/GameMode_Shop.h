// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "GameMode_Shop.generated.h"

/**
 * 
 */
//코인이 클래스가 선택 됬을 때 델리게이트

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FShopMainMode);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FCoinCreateMode);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FCoinManageMode);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FShopItemMode);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FSelectCardMode);
UCLASS()
class FLIP_SIDE_API AGameMode_Shop : public AGameModeBase
{
	GENERATED_BODY()
	

//기능을 실행시킬 배열을 만들고 연출이 나온 다음 끝나면 해당 배열에 있는 기능들을 실행시키는 것은?
//TArray< 게임모드 변경시 일어나는 활동을 담은 인터페이스 > 
//아니라면 해당 버튼을 누르면 애니메이션이 나오고 애니메이션이 끝난 다음 델리게이트를 실행시키는 방법도 괜찮음


	//
public:
	FShopMainMode OnShopMainMode;
	
	FCoinManageMode OnCoinManageMode;

	FCoinCreateMode OnCoinCreateMode;

	FShopItemMode OnShopItemMode;
	
	FSelectCardMode OnSelectCardMode;
	
public:
	void SetShopMainMode();
	void SetCheckBossMode();
	void SetCoinManageMode();
	void SetCoinCreateMode();
	void SetShopItemMode();
	void SetCardSelectMode();


	void CheckCanGameStart();
};
