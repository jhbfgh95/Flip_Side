// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/ShopCard/W_ShopCardBase.h"

#include "Subsystem/BattleLevel/SoundManagerWSubsystem.h"

void UW_ShopCardBase::NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseEnter(InGeometry, InMouseEvent);
	USoundManagerWSubsystem::PlayHoverSoundFromWidget(this, InMouseEvent);
}
