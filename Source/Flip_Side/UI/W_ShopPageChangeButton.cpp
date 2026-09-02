// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/W_ShopPageChangeButton.h"

#include "Components/Button.h"

void UW_ShopPageChangeButton::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	if (IsValid(Button))
	{
		Button->OnHovered.AddDynamic(this, &UW_ShopPageChangeButton::HandleButtonHovered);
		Button->OnUnhovered.AddDynamic(this, &UW_ShopPageChangeButton::HandleButtonUnhovered);
	}
}

void UW_ShopPageChangeButton::HandleButtonHovered()
{
	if (HoverAnim)
	{
		PlayAnimation(HoverAnim);
	}
}

void UW_ShopPageChangeButton::HandleButtonUnhovered()
{
	if (UnhoverAnim)
	{
		PlayAnimation(UnhoverAnim);
	}
}
