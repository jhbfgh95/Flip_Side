// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/W_ItemInfo.h"
#include "W_ShopItemDescription.generated.h"

UCLASS()
class FLIP_SIDE_API UW_ShopItemDescription : public UW_ItemInfo
{
	GENERATED_BODY()

	protected:
	virtual void NativeConstruct() override;

public:
	// UW_ItemInfo의 RichText Decorator가 아이템 키워드와 헤더를 조립할 수 있도록 전체 데이터를 받습니다.
	void SetItemInfo(const struct FItemData& ItemData);

	void ResetItemWidget();
};
