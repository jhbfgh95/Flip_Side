#include "Subsystem/CoinManagementWSubsystem.h"
#include "Subsystems/Subsystem.h"
#include "Kismet/GameplayStatics.h"
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
}

// 260126, 김진수 수정
void UCoinManagementWSubsystem::AddBattleReadyCoins(ACoinActor* SelectCoinActor)
{    
    /* 기존
    if(BattleReadyCoinNum > 9) return;
    BattleReadyCoins.Add(SelectCoinActor);
    BattleReadyCoinNum++;
    */
    
    if(BattleReadyCoinNum > 9 || !SelectCoinActor) return;
    // 중복 클릭 방지: 이미 1.5배인 놈은 무시
    if (SelectCoinActor->GetActorScale3D().X > 1.2f) return;

    BattleReadyCoins.Add(SelectCoinActor);
    
    // 코인 서랍에 들어갈 때, 사이즈 조정 및 회전
    SelectCoinActor->SetActorScale3D(FVector(1.5f, 1.5f, 1.5f)); // 1.5배 스케일업 -> 박시우 유지보수 내용
    FRotator CurrentRot = SelectCoinActor->GetActorRotation();
    SelectCoinActor->SetActorRotation(FRotator(0.f, 180.f, 0.f));

    // 서랍 좌표로 이동
    FVector BaseDrawerLocation = FVector(760.f, -2600.f, -160.f); // 서랍 첫 칸
    float Offset = 400.f; // 다음 코인과의 간격
    
    FVector TargetLocation = BaseDrawerLocation + (FVector(0.f, BattleReadyCoinNum * Offset, 0.f));
    SelectCoinActor->SetActorLocation(TargetLocation);

    BattleReadyCoinNum++;

    UE_LOG(LogTemp, Warning, TEXT("Coin Added to BattleReady: Index %d"), BattleReadyCoinNum - 1);
}

int32 UCoinManagementWSubsystem::GetBattleReadyCoinNum() { return BattleReadyCoinNum; }

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
                                NewCoin->SetCoinValues(
                                    CoinNum, 
                                    CoinData.FrontWeaponID, 
                                    CoinData.BackWeaponID,
                                    CoinType,
                                    FrontWP.WeaponIcon,
                                    BackWP.WeaponIcon
                                );

                                NewCoin->FinishSpawning(SpawnTransform);

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
