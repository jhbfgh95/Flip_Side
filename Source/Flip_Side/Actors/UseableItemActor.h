// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BattleHoverInterface.h"
#include "BattleClickInterface.h"
#include "BattleRightClickInterface.h"
#include "FlipSide_Enum.h"
#include "ItemDataTypes.h"
#include "UseableItemActor.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnHoverItemDelegate, AUseableItemActor*, HoveredItem);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnGridClickItemDelegate, AUseableItemActor*, ClickedItem);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCoinClickItemDelegate, AUseableItemActor*, ClickedItem);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnOtherClickItemDelegate, AUseableItemActor*, ClickedItem);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnItemRightClickDelegate, AUseableItemActor*, ClickedItem);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnUnhoverItemDelegate);

UCLASS()
class AUseableItemActor : public AActor, public IBattleHoverInterface, public IBattleClickInterface, public IBattleRightClickInterface
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

	UPROPERTY(VisibleAnywhere, Category = "UseableItem | Price")
	int32 ItemPrice = 0;

	UPROPERTY(VisibleAnywhere)
	UTexture2D* ItemTexture;

	UPROPERTY(VisibleAnywhere)
	FLinearColor ItemColor;

public:	
	AUseableItemActor();

	int32 GetItemID() const;

	EItemType GetItemType() const { return ItemType; }

	class UStaticMeshComponent* GetUseableItemMesh() const { return UseableItemMesh; }

	UFUNCTION()
	void SetItemValues(int TheItemID, EItemType theItemType ,UTexture2D* ItemTex, FLinearColor Color, int32 price);

	// 전투 HUD에서 선택한 아이템의 3D 커서 프리뷰 전용 초기화입니다.
	void InitializeAsCursorPreview(const FItemData& ItemData, float PreviewScaleMultiplier);

	UFUNCTION()
	int32 GetItemPrice() const { return ItemPrice; }

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

	UFUNCTION()
	virtual void OnRightClicked_Implementation() override;
	
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

	UPROPERTY()
	FOnItemRightClickDelegate OnItemRightClick;

protected:
	virtual void BeginPlay() override;

	virtual void OnConstruction(const FTransform& Transform) override;
	
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item | State")
	FVector OriginLocation;

private:
	void ApplyItemVisual();
};
