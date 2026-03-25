// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/W_Tuto_ExplainWidget.h"
#include "Components/TextBlock.h"
#include "Subsystem/ShopTutorialWSubsystem.h"

void UW_Tuto_ExplainWidget::NativeConstruct()
{
    Super::NativeConstruct();
    TutorialSubsystem = GetWorld()->GetSubsystem<UShopTutorialWSubsystem>();
    SetVisibility(ESlateVisibility::Collapsed);
    TutorialSubsystem->SetTutorialEvent(EvenOrder,this);
    
}

void UW_Tuto_ExplainWidget::ExecuteTutorialEvent_Implementation()
{
    SetVisibility(ESlateVisibility::Visible);
    CurrentIndex = 0;
    if (CurrentIndex< Messages.Num())
    {
        MessageText->SetText(FText::FromString(Messages[CurrentIndex]));
    }
}

void UW_Tuto_ExplainWidget::FinishTutorialEvent_Implementation()
{
    SetVisibility(ESlateVisibility::Collapsed);
}
	
void UW_Tuto_ExplainWidget::OnClickNext()
{
    CurrentIndex++;
    if (CurrentIndex<Messages.Num())
    {
        MessageText->SetText(FText::FromString(Messages[CurrentIndex]));
    }

    if(Messages.Num()<=CurrentIndex)
    {
        if(IsExecuteNextOrder)
            TutorialSubsystem->ExecuteEvents(NextOrder);
    }
}

FReply UW_Tuto_ExplainWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry,const FPointerEvent& InMouseEvent)
{
    if (InMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
    {
        OnClickNext();
        return FReply::Handled();
    }

    return FReply::Unhandled();
}