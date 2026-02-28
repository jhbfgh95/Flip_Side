// StageCardWSubsystem.cpp
#include "Subsystem/StageCardWSubsystem.h"

#include "FlipSideDevloperSettings.h"
#include "Engine/World.h"
#include "Engine/GameInstance.h"
#include "GameFramework/PlayerController.h"
#include "Blueprint/UserWidget.h"

#include "Subsystem/CrossingLevelGISubsystem.h"
#include "Subsystem/DataManagerSubsystem.h"
#include "Subsystem/BattleLevel/GridManagerSubsystem.h"

#include "CoinActor.h"

#include "DataTypes/GridTypes.h"
#include "DataTypes/WeaponDataTypes.h"

bool UStageCardWSubsystem::ShouldCreateSubsystem(UObject* Outer) const
{
    if (!Super::ShouldCreateSubsystem(Outer))
        return false;

    UWorld* World = Cast<UWorld>(Outer);
    if (!World)
        return false;

    const FString MapName = World->GetName();
    return MapName.Contains(TEXT("L_Stage"));
}

void UStageCardWSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);

    HandCards.SetNum(HandCount);
    bHasCard.SetNum(HandCount);

    for (int32 i = 0; i < HandCount; ++i)
    {
        bHasCard[i] = false;
        HandCards[i] = FCardData();
    }

    const UFlipSideDevloperSettings* Settings = GetDefault<UFlipSideDevloperSettings>();
    if (Settings && !Settings->StageHUDWidgetClass.IsNull())
    {
        StageHUDClass = Settings->StageHUDWidgetClass.LoadSynchronous();
    }
}

void UStageCardWSubsystem::Deinitialize()
{
    StageHUDInstance = nullptr;
    GridSubsys = nullptr;
    CrossingGI = nullptr;
    DM = nullptr;
    CoinMods.Empty();

    Super::Deinitialize();
}

void UStageCardWSubsystem::OnWorldBeginPlay(UWorld& InWorld)
{
    Super::OnWorldBeginPlay(InWorld);

    if (!InWorld.IsGameWorld())
        return;

    GridSubsys = InWorld.GetSubsystem<UGridManagerSubsystem>();

    if (UGameInstance* GI = InWorld.GetGameInstance())
    {
        CrossingGI = GI->GetSubsystem<UCrossingLevelGISubsystem>();
        DM = GI->GetSubsystem<UDataManagerSubsystem>();
    }

    // ===== HUD ���� + ���� ����ȭ =====
    EnsureStageHUD(InWorld);
    RefreshHandFromGI();

    // ===== ���� ���� Ÿ�̹� ī�� ó��=====
    ClearAllModifiers();
}

void UStageCardWSubsystem::EnsureStageHUD(UWorld& InWorld)
{
    if (StageHUDInstance) return;

    if (!StageHUDClass)
    {
        UE_LOG(LogTemp, Warning, TEXT("[StageCardWSubsystem] StageHUDClass is null. Set it in defaults/BP."));
        return;
    }

    APlayerController* PC = InWorld.GetFirstPlayerController();
    if (!PC)
    {
        UE_LOG(LogTemp, Warning, TEXT("[StageCardWSubsystem] No PlayerController yet."));
        return;
    }

    StageHUDInstance = CreateWidget<UUserWidget>(PC, StageHUDClass);
    if (!StageHUDInstance)
    {
        UE_LOG(LogTemp, Warning, TEXT("[StageCardWSubsystem] CreateWidget failed."));
        return;
    }

    StageHUDInstance->AddToViewport();
}

void UStageCardWSubsystem::RefreshHandFromGI()
{
    UWorld* World = GetWorld();
    if (!World) return;

    UGameInstance* GI = World->GetGameInstance();
    if (!GI) return;

    UCrossingLevelGISubsystem* LocalCrossingGI = GI->GetSubsystem<UCrossingLevelGISubsystem>();
    if (!LocalCrossingGI)
    {
        UE_LOG(LogTemp, Warning, TEXT("[StageCardWSubsystem] CrossingLevelGISubsystem is null"));
        return;
    }
    CrossingGI = LocalCrossingGI;

    const TArray<int32> IDs = CrossingGI->GetBattleCardIDs();

    auto IsEmpty = [&]()
        {
            if (IDs.Num() < HandCount) return true;
            return (IDs[0] < 0 && IDs[1] < 0 && IDs[2] < 0);
        };

    // �׽�Ʈ�� �ڵ� ä��
    if (IsEmpty())
    {
        CrossingGI->SetBattleCardID(1, 0);
        CrossingGI->SetBattleCardID(2, 1);
        CrossingGI->SetBattleCardID(3, 2);

        UE_LOG(LogTemp, Warning, TEXT("[StageCard] BattleCardIDs were empty. Filled with 1,2,3 for test."));
    }

    for (int32 Slot = 0; Slot < HandCount; ++Slot)
    {
        const int32 CardID = IDs.IsValidIndex(Slot) ? IDs[Slot] : -1;

        if (CardID < 0)
        {
            ClearSlot(Slot, /*bNotify=*/true);
            continue;
        }

        FCardData CardData;
        if (!TryLoadCardData(CardID, CardData))
        {
            UE_LOG(LogTemp, Warning, TEXT("[StageCardWSubsystem] TryLoadCardData failed. CardID=%d Slot=%d"), CardID, Slot);
            ClearSlot(Slot, /*bNotify=*/true);
            continue;
        }

        HandCards[Slot] = CardData;
        bHasCard[Slot] = true;

        OnHandCardSet.Broadcast(Slot, CardData);
    }
}

bool UStageCardWSubsystem::TryLoadCardData(int32 CardID, FCardData& Out) const
{
    UWorld* World = GetWorld();
    if (!World) return false;

    UGameInstance* GI = World->GetGameInstance();
    if (!GI) return false;

    UDataManagerSubsystem* LocalDM = GI->GetSubsystem<UDataManagerSubsystem>();
    if (!LocalDM) return false;

    return LocalDM->TryGetCard(CardID, Out);
}

void UStageCardWSubsystem::RemoveHandCard(int32 HandIndex)
{
    if (HandIndex < 0 || HandIndex >= HandCount) return;

    UWorld* World = GetWorld();
    if (World)
    {
        if (UGameInstance* GI = World->GetGameInstance())
        {
            if (UCrossingLevelGISubsystem* LocalCrossingGI = GI->GetSubsystem<UCrossingLevelGISubsystem>())
            {
                // �Լ� �ñ״�ó: SetBattleCardID(CardID, Slot)
                LocalCrossingGI->SetBattleCardID(-1, HandIndex);
            }
        }
    }

    ClearSlot(HandIndex, /*bNotify=*/true);
}

bool UStageCardWSubsystem::TryGetHandCard(int32 HandIndex, FCardData& Out) const
{
    if (HandIndex < 0 || HandIndex >= HandCount) return false;
    if (!bHasCard[HandIndex]) return false;

    Out = HandCards[HandIndex];
    return true;
}

void UStageCardWSubsystem::ClearSlot(int32 HandIndex, bool bNotify)
{
    bHasCard[HandIndex] = false;
    HandCards[HandIndex] = FCardData();

    if (bNotify)
    {
        OnHandCardCleared.Broadcast(HandIndex);
    }
}

void UStageCardWSubsystem::ClearAllModifiers()
{
    CoinMods.Empty();
}

FCoinCardModifiers UStageCardWSubsystem::GetModifiersForCoin(ACoinActor* Coin) const
{
    if (!IsValid(Coin))
        return FCoinCardModifiers{};

    if (const FCoinCardModifiers* Found = CoinMods.Find(Coin))
        return *Found;

    return FCoinCardModifiers{};
}

void UStageCardWSubsystem::CollectCoinsOnField(TArray<FCoinOnGridInfo>& OutCoins) const
{
    OutCoins.Reset();
    if (!GridSubsys) return;

    GridSubsys->CollectOccupiedCoins(OutCoins);
}

bool UStageCardWSubsystem::TryGetCoinFaceData(ACoinActor* Coin, FFaceData& OutFace) const
{
    if (!IsValid(Coin)) return false;

    UWorld* World = GetWorld();
    if (!World) return false;

    UGameInstance* GI = World->GetGameInstance();
    if (!GI) return false;

    UDataManagerSubsystem* LocalDM = GI->GetSubsystem<UDataManagerSubsystem>();
    if (!LocalDM) return false;

    // ������ ���� WeaponID
    const int32 WeaponID = Coin->GetCoinFaceID();
    return LocalDM->TryGetWeapon(WeaponID, OutFace);
}

int32 UStageCardWSubsystem::GetCoinRangeValue(const FFaceData& Face) const
{
    // range = max(x,y) �� ����(���� ����)
    return FMath::Max(Face.AttackRange.GridX, Face.AttackRange.GridY);
}

bool UStageCardWSubsystem::AreAllFieldCoinsFront(const TArray<FCoinOnGridInfo>& FieldCoins) const
{
    for (const FCoinOnGridInfo& Info : FieldCoins)
    {
        ACoinActor* Coin = Info.CoinActor;
        if (!IsValid(Coin)) continue;

        if (Coin->GetCoinDecidedFace() != EFaceState::Front)
            return false;
    }
    return true;
}

void UStageCardWSubsystem::ExecuteCardsEffect()
{
    // 1) ���� ��� ��� ���� (�̹� ����/�Ͽ��� ���� ���)
    ClearAllModifiers();

    // 2) �ʵ��� ���� ����
    TArray<FCoinOnGridInfo> FieldCoins;
    CollectCoinsOnField(FieldCoins);

    if (FieldCoins.Num() == 0)
    {
        UE_LOG(LogTemp, Log, TEXT("[StageCard] ExecuteCardsEffect: No coins on field."));
        return;
    }

    // 3) ����(3��)�� CardID���� ���鼭 ȿ�� ����
    for (int32 Slot = 0; Slot < HandCount; ++Slot)
    {
        if (!bHasCard.IsValidIndex(Slot) || !bHasCard[Slot])
            continue;

        if (!HandCards.IsValidIndex(Slot))
            continue;

        const int32 CardID = HandCards[Slot].CardID;
        if (CardID < 0) // -1�� �� �������� ���� ��찡 ���Ƽ�
            continue;

        switch (CardID)
        {
        // 1) Encore
        case 1:
        {
            if (!AreAllFieldCoinsFront(FieldCoins))
                break;

            ACoinActor* TargetCoin = FieldCoins[0].CoinActor;
            if (!IsValid(TargetCoin))
                break;

            FCoinCardModifiers& Mods = CoinMods.FindOrAdd(TargetCoin);
            Mods.ExtraActions += 1;
            break;
        }
        // 2) Long Range Amplifier
        case 2:
        {
            int32 CountRange3 = 0;

            for (const FCoinOnGridInfo& Info : FieldCoins)
            {
                ACoinActor* Coin = Info.CoinActor;
                if (!IsValid(Coin)) continue;

                FFaceData Face;
                if (!TryGetCoinFaceData(Coin, Face)) continue;

                const int32 R = GetCoinRangeValue(Face);
                if (R >= 3) ++CountRange3;
            }

            if (CountRange3 < 6)
                break;

            for (const FCoinOnGridInfo& Info : FieldCoins)
            {
                ACoinActor* Coin = Info.CoinActor;
                if (!IsValid(Coin)) continue;

                FFaceData Face;
                if (!TryGetCoinFaceData(Coin, Face)) continue;

                const int32 R = GetCoinRangeValue(Face);

                FCoinCardModifiers& Mods = CoinMods.FindOrAdd(Coin);
                Mods.AttackAdd += R;
            }

            break;
        }
        // 3) Promotion
        case 3:
        {
            for (const FCoinOnGridInfo& Info : FieldCoins)
            {
                ACoinActor* Coin = Info.CoinActor;
                if (!IsValid(Coin)) continue;

                FFaceData Face;
                if (!TryGetCoinFaceData(Coin, Face)) continue;

                // weaponid==3 -> ������
                if (Face.WeaponID == 3) 
                {
                    FCoinCardModifiers& Mods = CoinMods.FindOrAdd(Coin);
                    Mods.BehaviorAdd += 3;
                    Mods.AttackAdd += 3;
                    Mods.RangeAdd += 2;
                    Mods.bLifeSteal = true;
                }
            }
            break;
        }
        // 4) Golden Opportunity
        case 4:
        {
            // TODO: ��� �ý���/�÷��̾� ���¿� AddGold ���� �Լ� ȣ��
            break;
        }
        // 5) One for All
        case 5:
        {
            ACoinActor* OnlyCoin = nullptr;
            int32 ValidCount = 0;

            for (const FCoinOnGridInfo& Info : FieldCoins)
            {
                if (!IsValid(Info.CoinActor)) continue;
                ++ValidCount;
                OnlyCoin = Info.CoinActor;
                if (ValidCount > 1) break;
            }

            if (ValidCount == 1 && IsValid(OnlyCoin))
            {
                FCoinCardModifiers& Mods = CoinMods.FindOrAdd(OnlyCoin);
                Mods.AttackAdd += 5;
                Mods.RangeAdd += 5;
                Mods.BehaviorAdd += 5;
            }
            break;
        }
        // 6) Alliance
        case 6:
        {
            TMap<EWeaponClass, int32> ClassCount;

            for (const FCoinOnGridInfo& Info : FieldCoins)
            {
                ACoinActor* Coin = Info.CoinActor;
                if (!IsValid(Coin)) continue;

                FFaceData Face;
                if (!TryGetCoinFaceData(Coin, Face)) continue;

                ClassCount.FindOrAdd(Face.WeaponType)++;
            }

            for (const auto& Pair : ClassCount)
            {
                const EWeaponClass TargetClass = Pair.Key;
                const int32 Count = Pair.Value;

                if (Count < 6) continue;

                for (const FCoinOnGridInfo& Info : FieldCoins)
                {
                    ACoinActor* Coin = Info.CoinActor;
                    if (!IsValid(Coin)) continue;

                    FFaceData Face;
                    if (!TryGetCoinFaceData(Coin, Face)) continue;

                    if (Face.WeaponType != TargetClass) continue;

                    FCoinCardModifiers& Mods = CoinMods.FindOrAdd(Coin);

                    if (TargetClass == EWeaponClass::Deal)
                        Mods.AttackAdd += 4;
                    else if (TargetClass == EWeaponClass::Tank)
                    {
                        // TODO: MaxHP +1 ���� �� CoinActor�� ���� �Լ� ����ų�, ���� ��꿡�� "MaxHPAdd" ���� modifier�� ���� �δ� ������� �ϱ�
                    }
                    else
                        Mods.BehaviorAdd += 4;
                }
            }

            break;
        }

        default:
            break;
        }
    }

    UE_LOG(LogTemp, Log, TEXT("[StageCard] ExecuteCardsEffect done. ModsCount=%d"), CoinMods.Num());
}