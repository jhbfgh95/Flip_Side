// Fill out your copyright notice in the Description page of Project Settings.
#include "Subsystem/UseableItemWSubsystem.h"
#include "Subsystem/CrossingLevelGISubsystem.h"
#include "Subsystems/Subsystem.h"
#include "FlipSideDevloperSettings.h"
#include "Kismet/GameplayStatics.h"
#include "DataManagerSubsystem.h"
#include "UseableItemActor.h"
#include "SlotActor.h"

void UUseableItemWSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    InitUseitemSlot();
}

void UUseableItemWSubsystem::OnWorldBeginPlay(UWorld& InWorld)
{
    Super::OnWorldBeginPlay(InWorld);

    if(InWorld.IsGameWorld())
    {
        InstanceUseItems();
    }
}

bool UUseableItemWSubsystem::ShouldCreateSubsystem(UObject* Outer) const
{
    if(!Super::ShouldCreateSubsystem(Outer)) return false;

    UWorld* World = Cast<UWorld>(Outer);

    if(World)
    {
        FString MapName = World->GetName();
        if(MapName.Contains(TEXT("L_Stage")))
        {
            return true;
        }
    }

    return false;
}

void UUseableItemWSubsystem::InitUseitemSlot()
{
    UGameInstance* GI = GetWorld()->GetGameInstance();
    if(!GI) return;

    UCrossingLevelGISubsystem* CrossingLevelSubsystem = GI->GetSubsystem<UCrossingLevelGISubsystem>();

    for(int i = 0; i < CrossingLevelSubsystem->GetMakedItemNum(); i++)
    {
        FSelectItem ItemData = CrossingLevelSubsystem->GetBattleUseItems(i);
        ItemSlotArray.Add(ItemData);
    }
}

void UUseableItemWSubsystem::InstanceUseItems()
{
    UGameInstance* GI = GetWorld()->GetGameInstance();
    if(!GI) return;

    const UFlipSideDevloperSettings* Settings = GetDefault<UFlipSideDevloperSettings>();
    if(!Settings) return;

    UClass* BPUseItem = Settings->UseableItemActor.LoadSynchronous();
    UClass* BPItemSlot = Settings->UseableItemSlotActor.LoadSynchronous();
    if(!BPUseItem || !BPItemSlot) return;

    TArray<AActor*> OutSlots;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), BPItemSlot, OutSlots);
    OutSlots.Sort([](const AActor& A, const AActor& B){
        const ASlotActor* SlotA = Cast<ASlotActor>(&A);
        const ASlotActor* SlotB = Cast<ASlotActor>(&B);

        if(SlotA && SlotB)
        {
            return SlotA->GetSlotIndex() < SlotB->GetSlotIndex();
        }
        return false;
    });

    UDataManagerSubsystem* DM = GI->GetSubsystem<UDataManagerSubsystem>();

    if(DM && DM->IsCacheReady() && BPUseItem->IsChildOf(AUseableItemActor::StaticClass()))
    {
        int32 UseItemSlotIndex = 0;

        for(const FSelectItem& SelectItemData : ItemSlotArray)
        {
            FItemData ItemData;
            if(DM->TryGetItem(SelectItemData.ItemID, ItemData))
            {
                //여기서 Action (UObject)인스턴스 먼저 하고 그 다음에 for문으로 액터 인스턴싱
                for(int i = 0; i < SelectItemData.SameItemNum; i++)
                {
                    if(OutSlots.IsValidIndex(UseItemSlotIndex))
                    {
                        ASlotActor* TargetSlot = Cast<ASlotActor>(OutSlots[UseItemSlotIndex]);
                        if(TargetSlot)
                        {
                            FTransform SpawnTransform = TargetSlot->GetSlotTransform();
                            if(i > 0)
                            {
                                FVector NewLocation = SpawnTransform.GetLocation() + FVector(0.f , 0.f , i * 35.f);
                                SpawnTransform.SetLocation(NewLocation);
                            }

                            AUseableItemActor* NewItem = GetWorld()->SpawnActorDeferred<AUseableItemActor>(
                                BPUseItem,
                                SpawnTransform
                            );
                            
                            if(NewItem)
                            {
                                NewItem->SetItemValues(ItemData.ItemID, ItemData.ItemIcon, ItemData.TypeColor);
                                NewItem->FinishSpawning(SpawnTransform);
                            }
                        }
                    }

                }
                UseItemSlotIndex++;
            }
        }
    }
}