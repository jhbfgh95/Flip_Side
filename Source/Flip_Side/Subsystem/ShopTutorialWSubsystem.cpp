// Fill out your copyright notice in the Description page of Project Settings.


#include "Subsystem/ShopTutorialWSubsystem.h"


bool UShopTutorialWSubsystem::ShouldCreateSubsystem(UObject* Outer) const
{
    if (!Super::ShouldCreateSubsystem(Outer))
        return false;

    UWorld* World = Cast<UWorld>(Outer);
    if (!World)
        return false;

    const FString MapName = World->GetName();
    return MapName.Contains(TEXT("L_ShopTutorialLevel"));
}

void UShopTutorialWSubsystem::OnWorldBeginPlay(UWorld& InWorld)
{
    Super::OnWorldBeginPlay(InWorld);
}

bool UShopTutorialWSubsystem::SetTutorialEvent(int32 EventOrder, TScriptInterface<IShopTutorialInterface> SetEvent)
{
    if(EventOrder <=-1)
        return false;
    
    if(ReadyTutorialEvents.Num()<=EventOrder)
    {
        ReadyTutorialEvents.SetNum(EventOrder+1);
    }
    
    ReadyTutorialEvents[EventOrder].TutorialEvents.Add(SetEvent);

    if(EventOrder == 0)
    {
        SetEvent->Execute_ExecuteTutorialEvent(SetEvent.GetObject());
    }
    return true;
}
	
void UShopTutorialWSubsystem::ExecuteEvents(int32 EventOrder)
{
    if(ReadyTutorialEvents.Num()<= EventOrder)
        return;
    
    if(0<EventOrder-1)
    {
        for(TScriptInterface<IShopTutorialInterface>& TutorialObject : ReadyTutorialEvents[EventOrder-1].TutorialEvents)
        {
            TutorialObject->Execute_FinishTutorialEvent(TutorialObject.GetObject());
        }
    }

    for(TScriptInterface<IShopTutorialInterface>& TutorialObject : ReadyTutorialEvents[EventOrder].TutorialEvents)
    {
        TutorialObject->Execute_ExecuteTutorialEvent(TutorialObject.GetObject());
    }
    
}