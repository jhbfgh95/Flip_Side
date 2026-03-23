// Fill out your copyright notice in the Description page of Project Settings.


#include "Subsystem/ShopTutorialWSubsystem.h"


bool UShopTutorialWSubsystem::SetTutorialEvent(int32 EventOrder, TScriptInterface<IShopTutorialInterface> SetEvent)
{
    if(-1<=EventOrder )
        return false;

    if(ReadyTutorialEvents.Num()<=EventOrder)
    {
        ReadyTutorialEvents.SetNum(EventOrder+1);
    }
    
    ReadyTutorialEvents[EventOrder].TutorialEvents.Add(SetEvent);
    return true;
}
	
void UShopTutorialWSubsystem::ExcuteEvents(int32 EventOrder)
{
    for(TScriptInterface<IShopTutorialInterface>& TutorialObject : ReadyTutorialEvents[EventOrder].TutorialEvents)
    {
        TutorialObject->Execute_ExecuteTutorial(TutorialObject.GetObject());
    }
}