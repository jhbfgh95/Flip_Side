// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interface/ShopMouseInterface.h"
#include "DataTypes/ShopPageTypes.h"
#include "ShopUISelectActor.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnClickShopPageChangeActor, EShopPage, ConnectedPage);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnHoverShopPageChangeActor, EShopPage, ConnectedPage);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnUnhoverShopPageChangeActor);

UCLASS()
class FLIP_SIDE_API AShopUISelectActor : public AActor, public IShopMouseInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AShopUISelectActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Shop UI Select | Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class USceneComponent> SceneRoot;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Shop UI Select | Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UStaticMeshComponent> SelectMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Shop UI Select | Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UBoxComponent> ClickCollision;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Shop UI Select | Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UWidgetComponent> SelectWidget;

public:
	FOnClickShopPageChangeActor OnClickShopPageChangeActor;
	FOnHoverShopPageChangeActor OnHoverShopPageChangeActor;
	FOnUnhoverShopPageChangeActor OnUnhoverShopPageChangeActor;
protected:

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"))
	EShopPage ConnectedPage;
public:
	/** 월드 상점 선택 오브젝트의 마우스 클릭/호버 충돌을 켜거나 끕니다. */
	void SetClickCollisionEnabled(bool bEnabled);

	virtual void InteractLeftClick_Implementation() override;
	virtual void InteractRightClick_Implementation() override;
	virtual void InteractHover_Implementation() override;
	virtual void InteractUnHover_Implementation() override;

};
