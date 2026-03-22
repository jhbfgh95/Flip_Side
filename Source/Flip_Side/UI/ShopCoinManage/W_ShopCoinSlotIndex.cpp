// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/ShopCoinManage/W_ShopCoinSlotIndex.h"
#include "Components/TextBlock.h"

void UW_ShopCoinSlotIndex::SetSlotIndex(int32 Index)
{
    SlotText->SetText(FText::AsNumber(Index));
}