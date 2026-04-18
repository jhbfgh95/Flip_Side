// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BattleHoverInterface.h"
#include "BattleClickInterface.h"
#include "FlipSide_Enum.h"
#include "UseableItemActor.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnHoverItemDelegate, AUseableItemActor*, HoveredItem);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnGridClickItemDelegate, AUseableItemActor*, ClickedItem);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCoinClickItemDelegate, AUseableItemActor*, ClickedItem);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnOtherClickItemDelegate, AUseableItemActor*, ClickedItem);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnUnhoverItemDelegate);

UCLASS()
class AUseableItemActor : public AActor, public IBattleHoverInterface, public IBattleClickInterface
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "UseableItem | Component", meta = (AllowPrivateAccess = "true"))
	class USceneComponent* UseableItemRootComp;

	UPROPERTY(EditAnywhere, Category = "UseableItem | Component", meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* UseableItemMesh;
	
	UPROPERTY(VisibleAnywhere, Category = "UseableItem | ID")
	int32 ItemID = 0;

	UPROPERTY(VisibleAnywhere, Category = "UseableItem | Type")
	EItemType ItemType = EItemType::CoinBuff;

	UPROPERTY(VisibleAnywhere)
	UTexture2D* ItemTexture;

	UPROPERTY(VisibleAnywhere)
	FLinearColor ItemColor;

public:	
	AUseableItemActor();

	int32 GetItemID() const;

	EItemType GetItemType() const { return ItemType; }

	UFUNCTION()
	void SetItemValues(int TheItemID, EItemType theItemType ,UTexture2D* ItemTex, FLinearColor Color);

	UFUNCTION()
    void SetOriginLocation(FVector InLoc) { OriginLocation = InLoc; }
	UFUNCTION()
    FVector GetOriginLocation() const { return OriginLocation; }

/* 인풋 관련 */
public:
	UFUNCTION()
	virtual void OnHover_Implementation() override;

	UFUNCTION()
	virtual void OnUnhover_Implementation() override;

	UFUNCTION()
	virtual void OnClicked_Implementation() override;
	
	UPROPERTY()
	FOnHoverItemDelegate OnHoverItem;

	UPROPERTY()
	FOnUnhoverItemDelegate OnUnhoverItem;

	UPROPERTY()
	FOnGridClickItemDelegate OnGridClickItem;

	UPROPERTY()
	FOnCoinClickItemDelegate OnCoinClickItem;

	UPROPERTY()
	FOnOtherClickItemDelegate OnOtherClickItem;

protected:
	virtual void BeginPlay() override;

	virtual void OnConstruction(const FTransform& Transform) override;
	
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item | State")
    FVector OriginLocation;
};
