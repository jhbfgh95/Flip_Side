#include "Subsystem/CoinManagementWSubsystem.h"
#include "Subsystems/Subsystem.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "CoinActor.h"
#include "SlotActor.h"
#include "CoinDataTypes.h"
#include "WeaponDataTypes.h"
#include "FlipSide_Enum.h"
#include "CrossingLevelGISubsystem.h"
#include "DataManagerSubsystem.h"
#include "FlipSideDevloperSettings.h"

#define READY_COIN_NUM 10

void UCoinManagementWSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    InitCoinSlot();
}

void UCoinManagementWSubsystem::OnWorldBeginPlay(UWorld& InWorld)
{
    Super::OnWorldBeginPlay(InWorld);

    if(InWorld.IsGameWorld())
    {
        InitBattleReadyCoin(); // 코인 생성하기 전 서랍 배열 초기화
        InstanceCoins();
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
        CoinSlotArray.Add(CoinData);
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
    BattleReadyCoinNum = 0;
}

// 260126~, 김진수 수정
void UCoinManagementWSubsystem::AddBattleReadyCoins(ACoinActor* SelectCoinActor)
{
    if (BattleReadyCoinNum >= 10 || !SelectCoinActor) return;
    if (SelectCoinActor->GetActorScale3D().X > 1.2f) return;

    // 1. 빈 슬롯 찾기 및 등록
    int32 TargetIdx = INDEX_NONE;
    for (int32 i = 0; i < BattleReadyCoins.Num(); ++i)
    {
        if (BattleReadyCoins[i] == nullptr)
        {
            TargetIdx = i;
            BattleReadyCoins[i] = SelectCoinActor;
            break;
        }
    }
    if (TargetIdx == INDEX_NONE) return;

    // 2. 좌표 계산 (성공한 -500.f 좌표 유지)
    int32 RowIndex = TargetIdx / 5; 
    int32 ColIndex = TargetIdx % 5;
    const FVector Row1_Start = FVector(570.f, -2300.f, -650.f); 
    const FVector Row2_Start = FVector(110.f, -2300.f, -650.f);
    const float ColumnOffset = 650.f;

    FVector TargetLocation = (RowIndex == 0) ? Row1_Start : Row2_Start;
    TargetLocation.Y += (ColIndex * ColumnOffset);

    // 3. 물리 및 좌표 적용 (최적화)
    SelectCoinActor->SetActorScale3D(FVector(1.5f, 1.5f, 1.5f));
    SelectCoinActor->SetActorRotation(FRotator(0.f, 180.f, 0.f));

    // [필수] 이동 중 충돌로 튕겨나가지 않게 잠시 끄기
    SelectCoinActor->SetActorEnableCollision(false);
    SelectCoinActor->SetActorLocation(TargetLocation, false, nullptr, ETeleportType::TeleportPhysics);

    if (UStaticMeshComponent* MeshComp = SelectCoinActor->FindComponentByClass<UStaticMeshComponent>())
    {
        // [필수] 메쉬와 루트의 위치를 동기화
        MeshComp->SetRelativeLocationAndRotation(FVector::ZeroVector, FRotator::ZeroRotator);
        
        // [필수] 유령 상태 방지를 위한 물리 재생성
        MeshComp->RecreatePhysicsState();
        
        // [필수] 클릭 판정을 위해 카메라 채널은 확실히 Block
        MeshComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
        MeshComp->SetCollisionResponseToChannel(ECC_Camera, ECR_Block);
        
        MeshComp->UpdateComponentToWorld();
    }

    // [필수] 이동 완료 후 콜리전 재활성화
    SelectCoinActor->SetActorEnableCollision(true);
    if (SelectCoinActor->GetRootComponent())
    {
        SelectCoinActor->GetRootComponent()->UpdateComponentToWorld();
    }

    BattleReadyCoinNum++;

    // 4. 슬롯 칸 앞당김 로직 (기존 유지)
    TArray<AActor*> OutActors;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACoinActor::StaticClass(), OutActors);

    for (AActor* Actor : OutActors)
    {
        ACoinActor* Coin = Cast<ACoinActor>(Actor);
        if (Coin && Coin->GetFrontWeaponID() == SelectCoinActor->GetFrontWeaponID() && Coin != SelectCoinActor)
        {
            if (Coin->GetActorScale3D().X < 1.1f) 
            {
                Coin->DecrementSameTypeIndex();
                FVector EndLoc = Coin->GetOriginSlotLocation();
                EndLoc.Y += (Coin->GetSameTypeIndex() * 35.f);

                FLatentActionInfo LatentInfo;
                LatentInfo.CallbackTarget = this;
                LatentInfo.UUID = Coin->GetUniqueID();
                LatentInfo.Linkage = 0;

                UKismetSystemLibrary::MoveComponentTo(
                    Coin->GetRootComponent(), 
                    EndLoc, 
                    Coin->GetActorRotation(), 
                    true, true, 0.15f, false, EMoveComponentAction::Move, LatentInfo
                );
            }
        }
    }
}

void UCoinManagementWSubsystem::RemoveBattleReadyCoins(ACoinActor* SelectCoinActor)
{
    if (!SelectCoinActor) return;

    // 1. 서랍 배열에서 제거
    int32 FoundIdx = BattleReadyCoins.Find(SelectCoinActor);
    if (FoundIdx != INDEX_NONE)
    {
        BattleReadyCoins[FoundIdx] = nullptr; 
        BattleReadyCoinNum--;
        UE_LOG(LogTemp, Warning, TEXT("#### [취소] %d번 칸 코인 슬롯 복귀 시작"), FoundIdx);
    }
    else return;

    // 2. 슬롯으로 돌아갈 '순번' 계산
    TArray<AActor*> OutActors;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACoinActor::StaticClass(), OutActors);
    
    int32 CurrentCountInSlot = 0;
    for (AActor* Actor : OutActors)
    {
        ACoinActor* Coin = Cast<ACoinActor>(Actor);
        // 자기 자신 제외, 슬롯에 대기 중인(Scale < 1.1) 같은 종류 코인 개수 파악
        if (Coin && Coin != SelectCoinActor && 
            Coin->GetFrontWeaponID() == SelectCoinActor->GetFrontWeaponID() &&
            Coin->GetActorScale3D().X < 1.1f)
        {
            CurrentCountInSlot++;
        }
    }

    // 3. 복귀 설정 업데이트
    SelectCoinActor->SameTypeIndex = CurrentCountInSlot; 
    FVector ReturnLoc = SelectCoinActor->GetOriginSlotLocation();
    ReturnLoc.Y += (CurrentCountInSlot * 35.f);

    // 4. 물리 상태 초기화 후 복귀 (중요)
    // [정리] 이동 시작 전에 외형을 먼저 복구하여 물리 끼임을 방지합니다.
    SelectCoinActor->SetActorScale3D(FVector(1.0f, 1.0f, 1.0f));
    
    // [중요] 시작점을 180으로 확실히 고정합니다. (Add에서 180으로 보냈으므로)
    SelectCoinActor->SetActorRotation(FRotator(0.f, 180.f, 0.f));

    // [보강] 자식 메쉬의 상대 회전이 돌아가 있을 수 있으므로 0으로 리셋합니다.
    if (UStaticMeshComponent* MeshComp = SelectCoinActor->FindComponentByClass<UStaticMeshComponent>())
    {
        MeshComp->SetRelativeRotation(FRotator::ZeroRotator);
    }

    // [추가] 서랍에서 나올 때도 물리 상태를 한 번 갱신해줘야 부드럽게 움직입니다.
    if (UPrimitiveComponent* RootPrim = Cast<UPrimitiveComponent>(SelectCoinActor->GetRootComponent()))
    {
        RootPrim->RecreatePhysicsState();
        RootPrim->UpdateComponentToWorld();
    }

    // 5. 부드러운 이동 연출
    FLatentActionInfo LatentInfo;
    LatentInfo.CallbackTarget = this;
    LatentInfo.UUID = SelectCoinActor->GetUniqueID();
    LatentInfo.Linkage = 0;

    UKismetSystemLibrary::MoveComponentTo(
        SelectCoinActor->GetRootComponent(),
        ReturnLoc,
        FRotator(-90.f, -90.f, 0.f),
        false, true, 1.0f, false, EMoveComponentAction::Move, LatentInfo
    );
}

int32 UCoinManagementWSubsystem::GetBattleReadyCoinNum() { return BattleReadyCoinNum; }

bool UCoinManagementWSubsystem::IsCoinInBattleReady(ACoinActor* InCoin) const
{
    //return BattleReadyCoins.Contains(InCoin);
    if (!InCoin) return false;

    for (int32 i = 0; i < BattleReadyCoins.Num(); i++)
    {
        if (BattleReadyCoins[i])
        {
            // [해결책] 주소 대신 이름을 비교하거나, 
            // 사용자님이 만드신 고유 ID(예: WeaponID + 개별번호)를 비교하세요.
            if (BattleReadyCoins[i]->GetName() == InCoin->GetName()) 
            {
                return true; 
            }
        }
    }
    return false;
}

void UCoinManagementWSubsystem::LockCoinReady()
{
    // 대기석에 있는 모든 코인을 돌며 콜리전을 끕니다.
    // 그러면 더 이상 클릭하거나 드래그할 수 없게 됩니다.
    for (ACoinActor* Coin : BattleReadyCoins)
    {
        if (Coin)
        {
            // 방법 1: 아예 클릭을 못하게 콜리전 끄기
            Coin->SetActorEnableCollision(false);
            
            // 방법 2: 코인 내부에 bIsLocked 변수가 있다면 그것을 활용
            // Coin->bIsLocked = true;
        }
    }
    UE_LOG(LogTemp, Warning, TEXT("#### 모든 배틀 코인이 잠겼습니다! 전투 준비 완료 ####"));
}

ACoinActor* UCoinManagementWSubsystem::GetCoinByName(FString TargetName)
{
    for (ACoinActor* Coin : BattleReadyCoins)
    {
        if (Coin && Coin->GetName() == TargetName)
        {
            return Coin;
        }
    }
    return nullptr;
}

bool UCoinManagementWSubsystem::IsCoinIdInBattleReady(int32 TargetID) const
{
    UE_LOG(LogTemp, Error, TEXT("#### [대조시작] 찾는 ID: %d"), TargetID);

    for (int32 i = 0; i < BattleReadyCoins.Num(); ++i)
    {
        if (BattleReadyCoins[i])
        {
            int32 ArrayCoinID = BattleReadyCoins[i]->GetCoinID();
            UE_LOG(LogTemp, Log, TEXT("배열 %d번 칸 코인 이름: %s | ID: %d"), 
                   i, *BattleReadyCoins[i]->GetName(), ArrayCoinID);

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
    UClass* BPSlot = Settings->SlotActor.LoadSynchronous();
    if(!BPCoin || !BPSlot) return;

    TArray<AActor*> OutSlots;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), BPSlot, OutSlots);
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

    if(DM && DM->IsCacheReady() && BPCoin->IsChildOf(ACoinActor::StaticClass()))
    {
        int32 CoinNum = 0;
        int32 SlotIndex = 0;

        for(const FCoinTypeStructure& CoinData : CoinSlotArray)
        {
            FFaceData FrontWP;
            FFaceData BackWP;
            EWeaponClass CoinType = EWeaponClass::None;

            if(DM->TryGetWeapon(CoinData.FrontWeaponID, FrontWP) && DM->TryGetWeapon(CoinData.BackWeaponID, BackWP))
            {
                for(int i = 0; i < CoinData.SameTypeCoinNum; i++)
                {
                    if(OutSlots.IsValidIndex(SlotIndex))
                    {
                        ASlotActor* TargetSlot = Cast<ASlotActor>(OutSlots[SlotIndex]);
                        if(TargetSlot)
                        {
                            FTransform SpawnTransform = TargetSlot->GetSlotTransform();
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
                                }
                                int32 FinalIndex = i;

                                NewCoin->SetCoinValues(
                                    CoinNum, 
                                    CoinData.FrontWeaponID, 
                                    CoinData.BackWeaponID,
                                    CoinType,
                                    FrontWP.WeaponIcon,
                                    BackWP.WeaponIcon
                                );

                                // 슬롯의 순수 시작 위치(i=0일 때의 위치)를 저장
                                NewCoin->SetOriginSlotLocation(TargetSlot->GetSlotTransform().GetLocation());

                                NewCoin->FinishSpawning(SpawnTransform);

                                NewCoin->SameTypeIndex = FinalIndex;

                                // [중요 로그] 이 로그가 0, 1, 2 순서대로 찍히는지 확인하세요!
                                UE_LOG(LogTemp, Error, TEXT("#### [DEBUG] 코인: %s | i: %d | 총수: %d | 부여된 Index: %d"), 
                                *NewCoin->GetName(), i, CoinData.SameTypeCoinNum, NewCoin->SameTypeIndex);

                                CoinNum++;
                            }
                        }
                    }
                }
                SlotIndex++;
            }
        }
    }
}
