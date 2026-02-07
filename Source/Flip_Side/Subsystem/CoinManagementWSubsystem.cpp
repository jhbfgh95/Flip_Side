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

// 260126~, 김진수 수정
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
    const FVector Row1_Start = FVector(570.f, -2300.f, -690.f); // 1행 1열 (1~5번째 코인 시작점)
    const FVector Row2_Start = FVector(110.f, -2300.f, -690.f); // 2행 1열 (6~10번째 코인 시작점)
    const float ColumnOffset = 650.f; // 가로 칸 사이의 간격

    // 2. 현재 코인이 몇 번째인지 계산 (0~9번 인덱스)
    int32 CurrentIdx = BattleReadyCoins.Num() - 1;

    // 3. 몇 번째 줄(Row)인지와 그 줄에서 몇 번째 칸(Col)인지 계산
    int32 RowIndex = CurrentIdx / 5; // 0~4번은 0(1행), 5~9번은 1(2행)
    int32 ColIndex = CurrentIdx % 5; // 각 줄 내에서 0, 1, 2, 3, 4번째 칸

    // 4. 최종 타겟 좌표 결정
    FVector TargetLocation;

    if (RowIndex == 0)
    {
        // 1행 배치 로직
        TargetLocation = Row1_Start;
        TargetLocation.Y += (ColIndex * ColumnOffset); // Y축 기준 가로 배치 예시
    }
    else
    {
        // 2행 배치 로직 (6번째 코인부터 여기로 들어옴)
        TargetLocation = Row2_Start;
        TargetLocation.Y += (ColIndex * ColumnOffset);
    }

    // 5. 이동 및 회전 적용
    SelectCoinActor->SetActorLocation(TargetLocation);

    BattleReadyCoinNum++;

    UE_LOG(LogTemp, Warning, TEXT("Coin Added to BattleReady: Index %d"), BattleReadyCoinNum - 1);

    // [추가] 뽑힌 코인과 같은 종류(WeaponID)의 남은 코인들 인덱스 갱신
    TArray<AActor*> OutActors;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACoinActor::StaticClass(), OutActors);

    for (AActor* Actor : OutActors)
    {
        ACoinActor* Coin = Cast<ACoinActor>(Actor);
        // 같은 무기 종류이면서, 아직 슬롯에 남아있는(스케일이 1.0인) 코인들만 대상
        if (Coin && Coin->GetFrontWeaponID() == SelectCoinActor->GetFrontWeaponID() && Coin != SelectCoinActor)
        {
            // 스케일이 1.1보다 작다 = 아직 슬롯에 남아있는 코인이다
            if (Coin->GetActorScale3D().X < 1.1f) 
            {
                Coin->DecrementSameTypeIndex(); 
            }
        }
    }
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

                                // i(루프 인덱스) 값을 코인에게 전달
                                // i가 0인 코인이 뭉치의 가장 앞에 있는 코인이 됨
                                NewCoin->SameTypeIndex = i;

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
