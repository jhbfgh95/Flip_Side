// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/ShopItem/W_ShopItemInven.h"
#include "Components/Button.h"
#include "Components/UniformGridPanel.h"
#include "UI/ShopItem/W_ShopInvenItemButton.h"
#include "Subsystem/ShopLevel/ShopItemWSubsystem.h"
#include "Subsystem/DataManagerSubsystem.h"
void UW_ShopItemInven::NativeConstruct()
{
    Super::NativeConstruct();

    ShopItemSubsystem = GetWorld()->GetSubsystem<UShopItemWSubsystem>();
    
    DataManagerSubsystem = GetWorld()->GetGameInstance()->GetSubsystem<UDataManagerSubsystem>();
    ShopItemSubsystem->OnItemBuy.AddDynamic(this, &UW_ShopItemInven::SetItem);
    int ButtonCount = ButtonGrid->GetChildrenCount();
    



    for (int32 i = 0; i < ButtonCount; i++)
    {
        UW_ShopInvenItemButton* Child = Cast<UW_ShopInvenItemButton>(ButtonGrid->GetChildAt(i));

        if (Child)
        {
            Child->SetCountText(ShopItemSubsystem->GetPlayerItem(i).SameItemNum);

            ItemButtons.Add(Child);
        }
    }
}


void UW_ShopItemInven::NativeDestruct()
{
    Super::NativeDestruct();
}
	
void UW_ShopItemInven::ActiveInvenHandle()
{
	if(IsInvenOpen)
    {
        PlayAnimation(CloseAnim);
    }
    else
    {
        PlayAnimation(OpenAnim);
    }
}
	
void UW_ShopItemInven::SetItem(int32 Index)
{

    ItemButtons[Index]->SetCountText(ShopItemSubsystem->GetPlayerItem(Index).SameItemNum);
    FItemData ItemData;
    DataManagerSubsystem->TryGetItem(ShopItemSubsystem->GetPlayerItem(Index).ItemID, ItemData);
    ItemButtons[Index]->SetItemData(ItemData);
}