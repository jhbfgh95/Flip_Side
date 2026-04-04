// Fill out your copyright notice in the Description page of Project Settings.


#include "ShopTutorial/W_Tuto_CardSelectWidget.h"
#include "Subsystem/ShopTutorialWSubsystem.h"
#include "Components/Button.h"

void UW_Tuto_CardSelectWidget::NativeConstruct()
{
    Super::NativeConstruct();
    TutorialSubsytem = GetWorld()->GetSubsystem<UShopTutorialWSubsystem>();
    SelectCardButton->OnClicked.AddDynamic(this, &UW_Tuto_CardSelectWidget::ExecuteNextTutorial);
}

void UW_Tuto_CardSelectWidget::ExecuteNextTutorial()
{
    if(IsExecuteNextOrder)
        TutorialSubsytem->ExecuteEvents(NextOrder);
}
