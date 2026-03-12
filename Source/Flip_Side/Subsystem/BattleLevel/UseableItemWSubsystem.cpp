// Fill out your copyright notice in the Description page of Project Settings.
#include "Subsystem/BattleLevel/UseableItemWSubsystem.h"
#include "Subsystem/CrossingLevelGISubsystem.h"
#include "Subsystems/Subsystem.h"
#include "FlipSideDevloperSettings.h"
#include "Kismet/GameplayStatics.h"
#include "DataManagerSubsystem.h"
#include "GridManagerSubsystem.h"
#include "UseableItemActor.h"
#include "FlipSide_Enum.h"
#include "SlotActor.h"
#include "CoinActor.h"
#include "GridActor.h"
#include "ItemDataTypes.h"
#include "GridTypes.h"
#include "Item_Action.h"

void UUseableItemWSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    InitUseitemSlot();

    SelectedItemAction = NewObject<UItem_Action>(this);

    if(SelectedItemAction)
    {
        InitSelectedItem();
        //SingleCell 기본 물어보기.
        ItemAreaSpec = {
            EAttackAreaPattern::SingleCell
        };
    }
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

void UUseableItemWSubsystem::InitSelectedItem()
{
    SelectedItemAction->SetActionRange(DefaultItemRange);
    SelectedItemAction->SetLogicID(-1);
    SelectedItemAction->SetItemType(-1);
    SelectedItemAction->SetTargetGrid(nullptr);
    SelectedItemAction->SetItemEffectValue(-1);
    SelectedItemAction->SetInRangeCoins(nullptr);
}

void UUseableItemWSubsystem::SelectWantUseItem(AUseableItemActor* TargetItem)
{
    if(!TargetItem) return;

    // SelectedItemAction이 혹시라도 Null인지 체크 // 크래시 발생해서 추가함
    if (!SelectedItemAction)
    {
        SelectedItemAction = NewObject<UItem_Action>(this);
        if (!SelectedItemAction) return;
    }

    if (UGameInstance* GI = GetWorld()->GetGameInstance())
    {
        UDataManagerSubsystem* DM = GI->GetSubsystem<UDataManagerSubsystem>();
        
        FItemData SelectItem;
        FGridPoint ItemRange;

        if(DM->TryGetItem(TargetItem->GetItemID(), SelectItem) && SelectedItemAction)
        {
            SelectedItemAction->SetLogicID(TargetItem->GetItemID());
            SelectedItemAction->SetItemType(SelectItem.ItemTypeID);
            SelectedItemAction->SetItemEffectValue(SelectItem.ItemEffectValue);
            //ItemAreaSpec = SeletItem.ItemSpec;
            //이거 왜 int임?
            SelectedItemAction->SetActionRange(ItemRange = {SelectItem.ItemRange, SelectItem.ItemRange});
        }
    }
}

//나중에 Spec나오면 이거 부르면 될듯함
//음..근데 그리드 기준으로 spec검사를 해야하긴 하는데
void UUseableItemWSubsystem::ApplyRangedThings()
{
    UGridManagerSubsystem* GridManager = GetWorld()->GetSubsystem<UGridManagerSubsystem>();
    if(GridManager)
    {
        FObjectOnGridInfo GridInfos;

        GridManager->GetObjectsAtRange(ItemAreaSpec, SelectedItemAction->GetActionRange(), OutCells, GridInfos);
        for(AActor* Actor : GridInfos.Coins)
        {
            if(ACoinActor* Coin = Cast<ACoinActor>(Actor))
            {
                SelectedItemAction->SetInRangeCoins(Coin);
            }
        }
    }
}

void UUseableItemWSubsystem::CancelWantUseItem()
{
    InitSelectedItem();
}

void UUseableItemWSubsystem::ExecuteItem(AGridActor* TargetGrid)
{
    if(TargetGrid->GetCurrentOccupyingThing() == EGridOccupyingType::Coin && !IsItemSetupInGrid())
    {
        ACoinActor* Coin = Cast<ACoinActor>(TargetGrid->GetCurrentOccupied());
        //ItemAreaSpec.AnchorCell = TargetGrid->GetGridPoint();
        SelectedItemAction->SetInRangeCoins(Coin);
        SelectedItemAction->SetTargetGrid(TargetGrid);
        SelectedItemAction->ExecuteAction();
    }
    else if(TargetGrid->GetCurrentOccupyingThing() == EGridOccupyingType::None && IsItemSetupInGrid())
    {
        SelectedItemAction->ExecuteAction();
    }
    InitSelectedItem();
}

bool UUseableItemWSubsystem::IsItemSetupInGrid() const
{
    //2가 설치
    if(SelectedItemAction->GetItemTypeID() == 2)
    {
        return true;
    }
    return false;
}