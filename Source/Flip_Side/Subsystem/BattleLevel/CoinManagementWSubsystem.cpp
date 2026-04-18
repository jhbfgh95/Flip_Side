#include "Subsystem/BattleLevel/CoinManagementWSubsystem.h"
#include "Subsystems/Subsystem.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Components/WidgetComponent.h"
#include "CoinActor.h"
#include "CoinSlotActor.h"
#include "CoinDataTypes.h"
#include "WeaponDataTypes.h"
#include "AttackAreaTypes.h"
#include "FlipSide_Enum.h"
#include "CoinActionManagementWSubsystem.h"
#include "CrossingLevelGISubsystem.h"
#include "DataManagerSubsystem.h"
#include "FlipSideDevloperSettings.h"
#include "W_ReadyAndSlotCoinInfo.h"
#include "Component_Status.h"
#include "Blueprint/UserWidget.h"
#include "WeaponRangePreviewActor.h"

#define READY_COIN_NUM 10

void UCoinManagementWSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);

    CoinActionManager = Collection.InitializeDependency<UCoinActionManagementWSubsystem>();
}

void UCoinManagementWSubsystem::OnWorldBeginPlay(UWorld& InWorld)
{
    Super::OnWorldBeginPlay(InWorld);

    if(InWorld.IsGameWorld())
    {
        InitCoinSlot();
        InitBattleReadyCoin(); // 코인 생성하기 전 서랍 배열 초기화
        InstanceCoins();

        if(!ReadyCoinInfoWidgetInstance)
        {
            const UFlipSideDevloperSettings* Settings = GetDefault<UFlipSideDevloperSettings>();
            if (Settings && !Settings->ReadyAndSlotCoinInfoWidget.IsNull())
            {
                UClass* ReadyCoinWidgetClass = Settings->ReadyAndSlotCoinInfoWidget.LoadSynchronous();
                
                if (ReadyCoinWidgetClass)
                {
                    ReadyCoinInfoWidgetInstance = CreateWidget<UW_ReadyAndSlotCoinInfo>(GetWorld(), ReadyCoinWidgetClass);
                    if (ReadyCoinInfoWidgetInstance)
                    {
                        ReadyCoinInfoWidgetInstance->AddToViewport();
                        ReadyCoinInfoWidgetInstance->SetVisibility(ESlateVisibility::Collapsed);

                    }
                }
            }
        }
    }
}

bool UCoinManagementWSubsystem::ShouldCreateSubsystem(UObject* Outer) const
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

void UCoinManagementWSubsystem::InitCoinSlot()
{
    UGameInstance* GI = GetWorld()->GetGameInstance();
    if(!GI) return;

    UCrossingLevelGISubsystem* CrossingLevelSubsystem = GI->GetSubsystem<UCrossingLevelGISubsystem>();

    for(int i = 0; i < CrossingLevelSubsystem->GetMakedCoinNum(); i++)
    {
        //없으면 그냥 다 -1로 채워지는 DefaultCoin이 들어감. 걍 나중에 이걸 보면 될듯
        FCoinTypeStructure CoinData = CrossingLevelSubsystem->GetSlotCoin(i);
        CoinSlotDataArray.Add(CoinData);
    }
}

void UCoinManagementWSubsystem::InitBattleReadyCoin()
{
    BattleReadyCoins.Empty();
    // 10칸을 미리 확보하고 비워둠
    for (int32 i = 0; i < 10; ++i)
    {
        BattleReadyCoins.Add(nullptr);
    }
}

void UCoinManagementWSubsystem::CheckBattleReadyCoinAlive()
{
    LiveCoinStacks.Empty();
    for (int32 i = 0; i < BattleReadyCoins.Num(); ++i)
    {
        if (BattleReadyCoins[i] != nullptr)
        {
            LiveCoinStacks.Add(BattleReadyCoins[i]);
            BattleReadyCoins[i]->SetCoinOnBattle(false);
            BattleReadyCoins[i]->SetCoinIsActed(false);
            BattleReadyCoins[i]->SetCoinIsReady(true);
            BattleReadyCoins[i]->CoinMesh->SetVisibility(false);
            BattleReadyCoins[i]->CoinHPUI->SetVisibility(false);
            BattleReadyCoins[i]->StatComponent->ClearTurnBasedBuffs();
        }
    }

    InitBattleReadyCoin();

    FTimerHandle DrawerWaitTimer;
    GetWorld()->GetTimerManager().SetTimer(DrawerWaitTimer, FTimerDelegate::CreateLambda([this]()
    {
        if (!IsValid(this)) return;

        TSet<int32> WeaponIDsToArrange; 

        for(ACoinActor* Coin : LiveCoinStacks)
        {
            if (IsValid(Coin))
            {
                Coin->SetActorScale3D(FVector(1.f, 1.f, 1.f));
                Coin->CoinMesh->SetVisibility(true);
                Coin->CoinHPUI->SetVisibility(true);
                Coin->StatComponent->ClearTurnBasedBuffs();
                AddBattleReadyCoins(Coin, false);
                LockCoinReady(Coin);
                WeaponIDsToArrange.Add(Coin->GetFrontWeaponID()); 
            }
        }

        for (int32 WeaponID : WeaponIDsToArrange)
        {
            ArrangeSlotCoins(WeaponID);
        }

    }), 2.0f, false);
}

void UCoinManagementWSubsystem::AddBattleReadyCoins(ACoinActor* SelectCoinActor, bool bArrangeSlot)
{
    if (!SelectCoinActor) return;
    
    if (BattleReadyCoins.Contains(SelectCoinActor)) return;

    int32 TargetIdx = INDEX_NONE;
    for (int32 i = 0; i < BattleReadyCoins.Num(); ++i)
    {
        if (BattleReadyCoins[i] == nullptr)
        {
            TargetIdx = i;
            break;
        }
    }
    if (TargetIdx == INDEX_NONE) return; 

    BattleReadyCoins[TargetIdx] = SelectCoinActor;
    SelectCoinActor->SetCoinIsReady(true);

    int32 RowIndex = TargetIdx / 5; 
    int32 ColIndex = TargetIdx % 5;
    const FVector Row1_Start = FVector(570.f, -2300.f, -800.f); 
    const FVector Row2_Start = FVector(110.f, -2300.f, -800.f);
    const float ColumnOffset = 650.f;

    FVector TargetLocation = (RowIndex == 0) ? Row1_Start : Row2_Start;
    TargetLocation.Y += (ColIndex * ColumnOffset);

    SelectCoinActor->SetActorEnableCollision(false);
    SelectCoinActor->SetActorScale3D(FVector(1.5f, 1.5f, 1.5f));
    SelectCoinActor->SetActorRotation(FRotator(0.f, 180.f, 0.f));
    SelectCoinActor->SetActorLocation(TargetLocation, false, nullptr, ETeleportType::TeleportPhysics);

    if (UStaticMeshComponent* MeshComp = SelectCoinActor->FindComponentByClass<UStaticMeshComponent>())
    {
        MeshComp->SetRelativeLocationAndRotation(FVector::ZeroVector, FRotator::ZeroRotator);
        MeshComp->RecreatePhysicsState();
        MeshComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
        MeshComp->SetCollisionResponseToChannel(ECC_Camera, ECR_Block);
        MeshComp->UpdateComponentToWorld();
    }

    SelectCoinActor->SetActorEnableCollision(true);
    if (SelectCoinActor->GetRootComponent())
    {
        SelectCoinActor->GetRootComponent()->UpdateComponentToWorld();
    }

    if (bArrangeSlot)
    {
        ArrangeSlotCoins(SelectCoinActor->GetFrontWeaponID());
    }
}

void UCoinManagementWSubsystem::ArrangeSlotCoins(int32 FrontWeaponID)
{
    TArray<AActor*> OutActors;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACoinActor::StaticClass(), OutActors);

    TArray<ACoinActor*> RemainingCoins;
    for (AActor* Actor : OutActors)
    {
        ACoinActor* Coin = Cast<ACoinActor>(Actor);
        if (Coin && Coin->GetFrontWeaponID() == FrontWeaponID && Coin->GetActorScale3D().X < 1.1f)
        {
            RemainingCoins.Add(Coin);
        }
    }


    RemainingCoins.Sort([](const ACoinActor& A, const ACoinActor& B) {
        return A.SameTypeIndex < B.SameTypeIndex;
    });

    for (int32 i = 0; i < RemainingCoins.Num(); ++i)
    {
        ACoinActor* Coin = RemainingCoins[i];
        Coin->SameTypeIndex = i; 

        FVector EndLoc = Coin->GetOriginSlotLocation();
        EndLoc.Y += (i * 35.f);

        FLatentActionInfo LatentInfo;
        LatentInfo.CallbackTarget = this;
        LatentInfo.UUID = Coin->GetUniqueID();
        LatentInfo.Linkage = 0;
        LatentInfo.ExecutionFunction = FName("OnArrangeSlotMoveComplete");

        UKismetSystemLibrary::MoveComponentTo(
            Coin->GetRootComponent(), 
            EndLoc, 
            Coin->GetActorRotation(), 
            true, true, 0.35f, false, EMoveComponentAction::Move, LatentInfo
        );
    }
}

void UCoinManagementWSubsystem::RemoveBattleReadyCoins(ACoinActor* SelectCoinActor)
{
    if (!SelectCoinActor) return;

    int32 FoundIdx = BattleReadyCoins.Find(SelectCoinActor);
    if (FoundIdx != INDEX_NONE)
    {
        BattleReadyCoins[FoundIdx] = nullptr; 
    }
    else return;

    SelectCoinActor->SetCoinIsReady(false);
    SelectCoinActor->SetActorScale3D(FVector(1.0f, 1.0f, 1.0f)); // 1.0으로 줄여야 ArrangeSlotCoins가 슬롯 코인으로 인식함
    
    SelectCoinActor->SetActorRotation(FRotator(-90.f, -90.f, 0.f)); 
    
    if (UStaticMeshComponent* MeshComp = SelectCoinActor->FindComponentByClass<UStaticMeshComponent>())
    {
        MeshComp->SetRelativeRotation(FRotator::ZeroRotator);
    }
    if (UPrimitiveComponent* RootPrim = Cast<UPrimitiveComponent>(SelectCoinActor->GetRootComponent()))
    {
        RootPrim->RecreatePhysicsState();
        RootPrim->UpdateComponentToWorld();
    }

    SelectCoinActor->SameTypeIndex = 99;

    ArrangeSlotCoins(SelectCoinActor->GetFrontWeaponID());
}
bool UCoinManagementWSubsystem::IsCoinInBattleReady(ACoinActor* InCoin) const
{
    return BattleReadyCoins.Contains(InCoin);
}

void UCoinManagementWSubsystem::LockCoinReady(ACoinActor* TargetCoin)
{
    if (TargetCoin)
    {
        TargetCoin->SetActorEnableCollision(false); // 아예 클릭을 못하게 콜리전 끔
    }
}

bool UCoinManagementWSubsystem::IsCoinIdInBattleReady(int32 TargetID) const
{
    for (int32 i = 0; i < BattleReadyCoins.Num(); ++i)
    {
        if (BattleReadyCoins[i])
        {
            int32 ArrayCoinID = BattleReadyCoins[i]->GetCoinID();

            if (ArrayCoinID == TargetID) return true;
        }
    }
    return false;
}

void UCoinManagementWSubsystem::InstanceCoins()
{
    UGameInstance* GI = GetWorld()->GetGameInstance();
    if(!GI) return;

    const UFlipSideDevloperSettings* Settings = GetDefault<UFlipSideDevloperSettings>();
    if(!Settings) return;

    UClass* BPCoin = Settings->CoinActor.LoadSynchronous();
    UClass* BPSlot = Settings->CoinSlotActor.LoadSynchronous();
    if(!BPCoin || !BPSlot) return;

    TArray<AActor*> OutSlots;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), BPSlot, OutSlots);
    OutSlots.Sort([](const AActor& A, const AActor& B){
        const ACoinSlotActor* SlotA = Cast<ACoinSlotActor>(&A);
        const ACoinSlotActor* SlotB = Cast<ACoinSlotActor>(&B);

        if(SlotA && SlotB)
        {
            return SlotA->GetSlotIndex() < SlotB->GetSlotIndex();
        }
        return false;
    });

    //설치해둔 슬롯 월드에서 스캔 후 캐시
    for(AActor* SlotActor : OutSlots)
    {
        if(ACoinSlotActor* CoinSlot = Cast<ACoinSlotActor>(SlotActor))
        {
            CoinSlots.Add(CoinSlot);
            CoinSlot->OnCoinSlotHovered.AddDynamic(this, &UCoinManagementWSubsystem::HandleCoinSlotHovered);
            CoinSlot->OnCoinSlotClicked.AddDynamic(this, &UCoinManagementWSubsystem::HandleCoinSlotClicked);
            CoinSlot->OnCoinSlotUnHovered.AddDynamic(this, &UCoinManagementWSubsystem::HandleCoinSlotUnHovered);
        }
    }

    UDataManagerSubsystem* DM = GI->GetSubsystem<UDataManagerSubsystem>();

    if(DM && DM->IsCacheReady() && BPCoin->IsChildOf(ACoinActor::StaticClass()))
    {
        int32 CoinNum = 0;
        int32 SlotIndex = 0;

        for(const FCoinTypeStructure& CoinData : CoinSlotDataArray)
        {
            FFaceData FrontWP;
            FFaceData BackWP;
            EWeaponClass CoinType = EWeaponClass::None;
            FWeaponType TypeDatas; 

            if(DM->TryGetWeapon(CoinData.FrontWeaponID, FrontWP) && DM->TryGetWeapon(CoinData.BackWeaponID, BackWP))
            {
                for(int i = 0; i < CoinData.SameTypeCoinNum; i++)
                {
                    if(CoinSlots.IsValidIndex(SlotIndex))
                    {
                        //ACoinSlotActor* TargetSlot = Cast<ACoinSlotActor>(OutSlots[SlotIndex]);
                        if(CoinSlots[SlotIndex])
                        {
                            FTransform SpawnTransform = CoinSlots[SlotIndex]->GetSlotTransform();
                            if(i > 0)
                            {
                                FVector NewLocation = SpawnTransform.GetLocation() + FVector(0.f , i * 35.f, 0.f);
                                SpawnTransform.SetLocation(NewLocation);
                            }
                            
                            ACoinActor* NewCoin = GetWorld()->SpawnActorDeferred<ACoinActor>(
                                BPCoin,
                                SpawnTransform
                                );

                            if(NewCoin)
                            {
                                if(FrontWP.WeaponType == BackWP.WeaponType)
                                {
                                    CoinType = FrontWP.WeaponType;
                                    TypeDatas = DM->WeaponTypes[FrontWP.TypeID - 1];
                                }
                                int32 FinalIndex = i;

                                NewCoin->SetCoinValues(
                                    CoinNum, 
                                    CoinData.FrontWeaponID, 
                                    CoinData.BackWeaponID,
                                    CoinType,
                                    FrontWP.WeaponIcon,
                                    BackWP.WeaponIcon,
                                    TypeDatas.TypeColor,
                                    TypeDatas.HP,
                                    SlotIndex
                                );

                                NewCoin->SetOriginSlotLocation(CoinSlots[SlotIndex]->GetSlotTransform().GetLocation());
                                CoinSlots[SlotIndex]->AllowcatedCoins.Add(NewCoin);

                                NewCoin->FinishSpawning(SpawnTransform);
                                NewCoin->SameTypeIndex = FinalIndex;

                                BindCoinEvents(NewCoin);

                                CoinNum++;
                            }
                        }
                    }
                }
                CoinSlots[SlotIndex]->SetFrontFaceInfo(FrontWP.WeaponIcon, FText::FromString(FrontWP.WeaponName),FText::FromString(FrontWP.KOR_DES),FrontWP.BehaviorPoint,FrontWP.AttackPoint, FrontWP.AttackAreaSpec, TypeDatas.TypeColor);
                CoinSlots[SlotIndex]->SetBackFaceInfo(BackWP.WeaponIcon, FText::FromString(BackWP.WeaponName),FText::FromString(BackWP.KOR_DES),BackWP.BehaviorPoint,BackWP.AttackPoint, BackWP.AttackAreaSpec, TypeDatas.TypeColor);
                SlotIndex++;
            }
        }
    }
}

TArray<ACoinActor*> UCoinManagementWSubsystem::GetReadyCoins() const
{
    return BattleReadyCoins;
}

void UCoinManagementWSubsystem::SetBattleCoinItemFlags(bool bEnabled)
{
    for (ACoinActor* Coin : BattleReadyCoins)
    {
        if (IsValid(Coin) && Coin->GetCoinOnBattle())
        {
            Coin->SetCoinItemFlag(bEnabled);
        }
    }
}

//여기서 코인 액션 매니저 가지고와서 바인딩
void UCoinManagementWSubsystem::BindCoinEvents(ACoinActor* CoinActor)
{
    CoinActor->OnHoverReadyCoin.AddDynamic(this, &UCoinManagementWSubsystem::HandleReadyCoinHovered);
    CoinActor->OnHoverBattleCoin.AddDynamic(CoinActionManager, &UCoinActionManagementWSubsystem::SetSelectedWeapon);
    CoinActor->OnUnhoverCoin.AddDynamic(this,  &UCoinManagementWSubsystem::HandleCoinUnHovered);
    CoinActor->OnUnhoverCoin.AddDynamic(CoinActionManager,  &UCoinActionManagementWSubsystem::HandleCoinUnHovered);
    CoinActor->OnClickReadyCoin.AddDynamic(this, &UCoinManagementWSubsystem::HandleReadyCoinClicked);
    CoinActor->OnClickBattleCoin.AddDynamic(CoinActionManager, &UCoinActionManagementWSubsystem::ExecuteSelectedWeapon);
}

void UCoinManagementWSubsystem::HandleReadyCoinHovered(ACoinActor* HoveredCoin)
{
    if(!HoveredCoin) return;

    int32 SlotIndex = HoveredCoin->GetSlotNum();
    if(SlotIndex != -1)
    {
        if (ReadyCoinInfoWidgetInstance)
        {
            SetCoinInfoWidgetData(CoinSlots[SlotIndex]->FrontFaceInfo, CoinSlots[SlotIndex]->BackFaceInfo);
            ReadyCoinInfoWidgetInstance->SetVisibility(ESlateVisibility::Visible);
        }

        // 사거리 미리보기 그리드에 앞뒷면 사거리 표시
        TArray<AActor*> Found;
        UGameplayStatics::GetAllActorsOfClass(GetWorld(), AWeaponRangePreviewActor::StaticClass(), Found);
        if (Found.Num() > 0)
        {
            if (AWeaponRangePreviewActor* Preview = Cast<AWeaponRangePreviewActor>(Found[0]))
            {
                Preview->ShowBothFacePreview(
                    CoinSlots[SlotIndex]->FrontFaceInfo.AttackAreaSpec,
                    CoinSlots[SlotIndex]->BackFaceInfo.AttackAreaSpec
                );
            }
        }
        OnRangeWanted.Broadcast();
    }
}

void UCoinManagementWSubsystem::HandleCoinUnHovered()
{
    ReadyCoinInfoWidgetInstance->SetVisibility(ESlateVisibility::Collapsed);

    // 사거리 미리보기 초기화
    TArray<AActor*> Found;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), AWeaponRangePreviewActor::StaticClass(), Found);
    if (Found.Num() > 0)
    {
        if (AWeaponRangePreviewActor* Preview = Cast<AWeaponRangePreviewActor>(Found[0]))
        {
            Preview->ClearPreview();
        }
    }
}

void UCoinManagementWSubsystem::HandleReadyCoinClicked(ACoinActor* ClickedCoin)
{
    RemoveBattleReadyCoins(ClickedCoin);
}

void UCoinManagementWSubsystem::HandleCoinSlotHovered(ACoinSlotActor* TargetCoinSlot)
{
    if(!TargetCoinSlot) return;

    SetCoinInfoWidgetData(TargetCoinSlot->FrontFaceInfo, TargetCoinSlot->BackFaceInfo);
    ReadyCoinInfoWidgetInstance->SetVisibility(ESlateVisibility::Visible);

    // 사거리 미리보기 그리드에 앞뒷면 사거리 표시
    TArray<AActor*> Found;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), AWeaponRangePreviewActor::StaticClass(), Found);
    if (Found.Num() > 0)
    {
        if (AWeaponRangePreviewActor* Preview = Cast<AWeaponRangePreviewActor>(Found[0]))
        {
            Preview->ShowBothFacePreview(
                TargetCoinSlot->FrontFaceInfo.AttackAreaSpec,
                TargetCoinSlot->BackFaceInfo.AttackAreaSpec
            );
        }
        OnRangeWanted.Broadcast();

    }
}

void UCoinManagementWSubsystem::HandleCoinSlotClicked(ACoinActor* ReadyTargetCoin)
{
    if(!ReadyTargetCoin) return;
    AddBattleReadyCoins(ReadyTargetCoin);
}

void UCoinManagementWSubsystem::HandleCoinSlotUnHovered() 
{
    ReadyCoinInfoWidgetInstance->SetVisibility(ESlateVisibility::Collapsed);
}

void UCoinManagementWSubsystem::SetCoinInfoWidgetData(FCoinWidgetInfoData& FrontWeaponData, FCoinWidgetInfoData& BackWeaponData)
{
    ReadyCoinInfoWidgetInstance->SetReadyCoinInfo(
        true,
        FrontWeaponData.Icon,
        FrontWeaponData.WeaponName,
        FrontWeaponData.RawDescription,
        FrontWeaponData.DefaultBP,
        FrontWeaponData.DefaultAP,
        FrontWeaponData.TypeColor
    );
                    
    ReadyCoinInfoWidgetInstance->SetReadyCoinInfo(
        false,       
        BackWeaponData.Icon,
        BackWeaponData.WeaponName,
        BackWeaponData.RawDescription,
        BackWeaponData.DefaultBP,
        BackWeaponData.DefaultAP,
        BackWeaponData.TypeColor
    );
}
