// Fill out your copyright notice in the Description page of Project Settings.


#include "Subsystem/MoneyGISubsystem.h"
#include "DataManagerSubsystem.h"
#include "Subsystem/FlipSideDevloperSettings.h"
#include "UI/W_MoneyDisplay.h"
#include "Blueprint/UserWidget.h"

void UMoneyGISubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);
    Collection.InitializeDependency<UDataManagerSubsystem>();

    InitMoney();
}

void UMoneyGISubsystem::InitMoney()
{
    if (UGameInstance* GI = GetGameInstance())
    {
        if (UDataManagerSubsystem* DM = GI->GetSubsystem<UDataManagerSubsystem>())
        {
           CurrentMoney = DM->GameConfig.InitialGold;    
        }
    }

    ResetStageMoneySummary();
    RunMoneySummary = FStageMoneySummary();
    UpdateMoneyDisplayWidget();
}

bool UMoneyGISubsystem::HasEnoughMoney(int32 Cost) const
{
    if(CurrentMoney < Cost)
    {
        return false;
    }

    return true;
}

bool UMoneyGISubsystem::SpendMoney(EMoneyRecordType RecordType, int32 Cost, int32 Count)
{
    if (Cost <= 0) { return false; }
    if (Count <= 0) { return false; }
    if(!HasEnoughMoney(Cost)) { return false; }

    CurrentMoney = FMath::Clamp(CurrentMoney - Cost, 0, 9999999);
    AddSpendToSummary(LastStageMoneySummary, RecordType, Cost, Count);
    AddSpendToSummary(RunMoneySummary, RecordType, Cost, Count);
    UpdateMoneyDisplayWidget();
    return true;
}

void UMoneyGISubsystem::AddSaleMoney(EMoneyRecordType RecordType, int32 Amount, int32 Count)
{
    if (Amount <= 0)
    {
        return;
    }
    if (Count <= 0)
    {
        return;
    }

    AddMoney(Amount);
    AddSaleToSummary(LastStageMoneySummary, RecordType, Amount, Count);
    AddSaleToSummary(RunMoneySummary, RecordType, Amount, Count);
    UpdateMoneyDisplayWidget();
}

void UMoneyGISubsystem::AddRewardMoney(int32 Amount)
{
    if (Amount <= 0)
    {
        return;
    }

    AddMoney(Amount);
    UpdateMoneyDisplayWidget();
}

void UMoneyGISubsystem::AddStageRefund(EMoneyRecordType RecordType, int32 Amount, int32 Count)
{
    if (Amount <= 0)
    {
        return;
    }
    if (Count <= 0)
    {
        return;
    }

    AddMoney(Amount);
    AddRefundToSummary(LastStageMoneySummary, RecordType, Amount, Count);
    AddRefundToSummary(RunMoneySummary, RecordType, Amount, Count);
    UpdateMoneyDisplayWidget();
}

void UMoneyGISubsystem::ResetStageMoneySummary()
{
    LastStageMoneySummary = FStageMoneySummary();
}

FStageMoneySummary UMoneyGISubsystem::GetLastStageMoneySummary() const
{
    return LastStageMoneySummary;
}

FStageMoneySummary UMoneyGISubsystem::GetRunMoneySummary() const
{
    return RunMoneySummary;
}

int32 UMoneyGISubsystem::GetCurrentMoney() const
{
    return CurrentMoney;
}

void UMoneyGISubsystem::EnsureMoneyDisplayWidget()
{
    if (MoneyDisplayWidgetInstance && MoneyDisplayWidgetInstance->IsInViewport())
    {
        return;
    }

    const UFlipSideDevloperSettings* Settings = GetDefault<UFlipSideDevloperSettings>();
    UClass* MoneyDisplayWidgetClass = Settings ? Settings->MoneyDisplayWidgetClass.LoadSynchronous() : nullptr;
    if (!MoneyDisplayWidgetClass)
    {
        return;
    }

    UWorld* World = GetWorld();
    if (!World)
    {
        return;
    }

    MoneyDisplayWidgetInstance = CreateWidget<UW_MoneyDisplay>(World, MoneyDisplayWidgetClass);
    if (MoneyDisplayWidgetInstance)
    {
        MoneyDisplayWidgetInstance->AddToViewport(50);
    }
}

void UMoneyGISubsystem::UpdateMoneyDisplayWidget()
{
    EnsureMoneyDisplayWidget();

    if (!MoneyDisplayWidgetInstance)
    {
        return;
    }

    UFunction* SetMoneyTextFunction = MoneyDisplayWidgetInstance->FindFunction(TEXT("SetMoneyText"));
    if (!SetMoneyTextFunction)
    {
        return;
    }

    struct FSetMoneyTextParams
    {
        int32 Money;
    };

    FSetMoneyTextParams Params;
    Params.Money = CurrentMoney;
    MoneyDisplayWidgetInstance->ProcessEvent(SetMoneyTextFunction, &Params);
}

void UMoneyGISubsystem::AddSpendToSummary(FStageMoneySummary& Summary, EMoneyRecordType RecordType, int32 Amount, int32 Count)
{
    switch (RecordType)
    {
    case EMoneyRecordType::Coin:
        Summary.SpentCoinCount += Count;
        Summary.SpentCoin += Amount;
        break;
    case EMoneyRecordType::Item:
        Summary.SpentItemCount += Count;
        Summary.SpentItem += Amount;
        break;
    case EMoneyRecordType::Card:
        Summary.SpentCardCount += Count;
        Summary.SpentCard += Amount;
        break;
    case EMoneyRecordType::Weapon:
        Summary.SpentWeaponCount += Count;
        Summary.SpentWeapon += Amount;
        break;
    case EMoneyRecordType::CoinSlot:
        Summary.SpentCoinSlotCount += Count;
        Summary.SpentCoinSlot += Amount;
        break;
    default:
        break;
    }

    RecalculateSummaryTotal(Summary);
}

void UMoneyGISubsystem::AddSaleToSummary(FStageMoneySummary& Summary, EMoneyRecordType RecordType, int32 Amount, int32 Count)
{
    switch (RecordType)
    {
    case EMoneyRecordType::Coin:
        Summary.SaleCoinCount += Count;
        Summary.SaleCoin += Amount;
        break;
    case EMoneyRecordType::Item:
        Summary.SaleItemCount += Count;
        Summary.SaleItem += Amount;
        break;
    case EMoneyRecordType::Card:
        Summary.SaleCardCount += Count;
        Summary.SaleCard += Amount;
        break;
    case EMoneyRecordType::Weapon:
        Summary.SaleWeaponCount += Count;
        Summary.SaleWeapon += Amount;
        break;
    case EMoneyRecordType::CoinSlot:
        Summary.SaleCoinSlotCount += Count;
        Summary.SaleCoinSlot += Amount;
        break;
    default:
        break;
    }

    RecalculateSummaryTotal(Summary);
}

void UMoneyGISubsystem::AddRefundToSummary(FStageMoneySummary& Summary, EMoneyRecordType RecordType, int32 Amount, int32 Count)
{
    switch (RecordType)
    {
    case EMoneyRecordType::Coin:
        Summary.RefundCoinCount += Count;
        Summary.RefundCoin += Amount;
        break;
    case EMoneyRecordType::Item:
        Summary.RefundItemCount += Count;
        Summary.RefundItem += Amount;
        break;
    case EMoneyRecordType::Card:
        Summary.RefundCardCount += Count;
        Summary.RefundCard += Amount;
        break;
    default:
        break;
    }

    RecalculateSummaryTotal(Summary);
}

void UMoneyGISubsystem::RecalculateSummaryTotal(FStageMoneySummary& Summary)
{
    Summary.TotalSpent = Summary.SpentCoin + Summary.SpentItem + Summary.SpentCard + Summary.SpentWeapon + Summary.SpentCoinSlot;
    Summary.TotalSale = Summary.SaleCoin + Summary.SaleItem + Summary.SaleCard + Summary.SaleWeapon + Summary.SaleCoinSlot;
    Summary.TotalRefund = Summary.RefundCoin + Summary.RefundItem + Summary.RefundCard;
}

void UMoneyGISubsystem::AddMoney(int32 Amount)
{
    CurrentMoney = FMath::Clamp(CurrentMoney + Amount, 0, 99999999);
}
