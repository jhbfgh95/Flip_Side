// Fill out your copyright notice in the Description page of Project Settings.


#include "ShopTutorial/W_Tuto_UnlockCardModeWidget.h"
#include "Subsystem/ShopTutorialWSubsystem.h"
#include "Components/Button.h"

void UW_Tuto_UnlockCardModeWidget::NativeOnInitialized()
{
    Super::NativeOnInitialized();
    TutoSubsystem = GetWorld()->GetSubsystem<UShopTutorialWSubsystem>();
    UnlockButton->OnClicked.AddDynamic(this, &UW_Tuto_UnlockCardModeWidget::UnlockExcuteEvent);
    SelectPlayerCardButton->OnClicked.AddDynamic(this, &UW_Tuto_UnlockCardModeWidget::SelectExcuteEvent);
    UnSelectPlayerCardButton->OnClicked.AddDynamic(this, &UW_Tuto_UnlockCardModeWidget::UnSelectExcuteEvent);
}


void UW_Tuto_UnlockCardModeWidget::UnlockExcuteEvent()
{
    TutoSubsystem->ExecuteEvents(UnlockExcuteOrder);
}

void UW_Tuto_UnlockCardModeWidget::SelectExcuteEvent()
{
    TutoSubsystem->ExecuteEvents(SelectExcuteOrder);
}

void UW_Tuto_UnlockCardModeWidget::UnSelectExcuteEvent()
{
    TutoSubsystem->ExecuteEvents(UnselectExcuteOrder);
}