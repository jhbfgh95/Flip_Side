// Fill out your copyright notice in the Description page of Project Settings.


#include "ShopTutorial/W_Tuto_UnlockCardWidget.h"
#include "Subsystem/ShopTutorialWSubsystem.h"


void UW_Tuto_UnlockCardWidget::NativeConstruct()
{
    Super::NativeConstruct();
    TutorialSubsytem = GetWorld()->GetSubsystem<UShopTutorialWSubsystem>();

}

void UW_Tuto_UnlockCardWidget::ExecuteNextTutorial()
{
    if(IsExecuteNextOrder)
    {
        TutorialSubsytem->ExecuteEvents(NextOrder);
    }
        
}



FReply UW_Tuto_UnlockCardWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry,const FPointerEvent& InMouseEvent)
{
    

    if (InMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
    {
        ExecuteNextTutorial();
    }
    Super::NativeOnMouseButtonDown(InGeometry,InMouseEvent);
    return FReply::Handled();
}