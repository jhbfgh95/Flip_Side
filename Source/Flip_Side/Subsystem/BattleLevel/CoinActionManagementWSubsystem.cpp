#include "Subsystem/BattleLevel/CoinActionManagementWSubsystem.h"
#include "AttackAreaTypes.h"
#include "WeaponDataTypes.h"
#include "FlipSide_Enum.h"
#include "CoinActor.h"
#include "GridActor.h"
#include "Weapon_Action.h"
#include "GridManagerSubsystem.h"
#include "DataManagerSubsystem.h"

void UCoinActionManagementWSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);

    SelectedAction = NewObject<UWeapon_Action>(this);

    GridManager = Collection.InitializeDependency<UGridManagerSubsystem>();

    if(SelectedAction)
    {
        InitWeaponAction();
    }
   
    CreateTestSpec();
}

bool UCoinActionManagementWSubsystem::ShouldCreateSubsystem(UObject* Outer) const
{
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

void UCoinActionManagementWSubsystem::CancelSelectWeapon()
{
    //초기화, UI 빼기 등을 구현
    InitWeaponAction();
}

void UCoinActionManagementWSubsystem::InitWeaponAction()
{
    FGridPoint DefaultGrid;
    DefaultGrid.GridX = -1;
    DefaultGrid.GridY = -1;
    SelectedAction->SetActionRange(DefaultGrid);
    SelectedAction->SetInRangeCoins(nullptr);
    SelectedAction->SetLogicID(-1);
    SelectedAction->SetFinalAttackPoint(-1);
    SelectedAction->SetFinalBehaviorPoint(-1);
    CurrentInputState = EActionInputState::None;
    ValidTargetGrids.Empty();
}

bool UCoinActionManagementWSubsystem::ApplyRangedThings(const FGridPoint& TargetGridPoint)
{
    if(!GridManager) return false;

    if(CurrentInputState == EActionInputState::WaitingForGridClick)
    {
        if(!ValidTargetGrids.Contains(TargetGridPoint))
        {
            return false;
        }
    }

    FObjectOnGridInfo GridInfos;

    GridManager->GetObjectsAtRange(AreaSpec, SelectedAction->GetActionRange(), OutCells, GridInfos);
    for(AActor* Actor : GridInfos.Coins)
    {
        if(ACoinActor* Coin = Cast<ACoinActor>(Actor))
        {
            SelectedAction->SetInRangeCoins(Coin);
        }
    }

    /*
    SelectedAction->SetBossActor(Infos.Boss)

    for(AActor* Actor : Infos.Others)
    {
        if(AOtherActor* Others = Cast<AOtherActor>(Actor))
        {
            SelectedAction->SetInRangeOthers(Others);
        }
    }
    */

    return true;

}

void UCoinActionManagementWSubsystem::SetSelectedWeapon(const FActionTask& ActionTask, const FGridPoint& CoinGrid)
{
    if(!bIsCorrectTurn) return;
    if (UGameInstance* GI = GetWorld()->GetGameInstance())
    {
        UDataManagerSubsystem* DM = GI->GetSubsystem<UDataManagerSubsystem>();

        FFaceData SelectWeapon;
        FGridPoint LastGridPoint;
        UE_LOG(LogTemp, Warning, TEXT("1"));


        if(DM->TryGetWeapon(ActionTask.WeaponID, SelectWeapon) && SelectedAction && CurrentInputState == EActionInputState::None)
        {
            LastGridPoint.GridX = SelectWeapon.AttackRange.GridX + ActionTask.ModifiedRange.GridX;
            LastGridPoint.GridY = SelectWeapon.AttackRange.GridY + ActionTask.ModifiedRange.GridY;

            SelectedAction->SetLogicID(SelectWeapon.WeaponID);
            SelectedAction->SetActionRange(LastGridPoint);
            SelectedAction->SetFinalAttackPoint(SelectWeapon.AttackPoint + ActionTask.ModifiedAttackPoint);
            SelectedAction->SetFinalBehaviorPoint(SelectWeapon.BehaviorPoint + ActionTask.ModifiedBehaviorPoint);
            switch(SelectWeapon.ActionRepeatType)
            {
                case EActionRepeatType::Behavior :
                    RepeatActionCnt = SelectWeapon.BehaviorPoint + ActionTask.ModifiedBehaviorPoint;
                    break;
                case EActionRepeatType::Attack :
                    RepeatActionCnt = SelectWeapon.AttackPoint + ActionTask.ModifiedAttackPoint;
                    break;
            }
            AreaSpec = SelectWeapon.AttackAreaSpec;
            AreaSpec.AnchorCell = CoinGrid;

            UE_LOG(LogTemp, Warning, TEXT("2"));

            if(AreaSpec.Pattern == EAttackAreaPattern::SingleCell)
            {
                CurrentInputState = EActionInputState::WaitingForGridClick;
                GridManager->GetValidGridsForSingleCell(CoinGrid,AreaSpec,ValidTargetGrids);
                UE_LOG(LogTemp, Warning, TEXT("3"));
            }
            else
            {
                CurrentInputState = EActionInputState::ExecutingAction;
                ApplyRangedThings(CoinGrid);
                ExecuteNowAction();
                UE_LOG(LogTemp, Warning, TEXT("4"));
            }
        }
    }
}

void UCoinActionManagementWSubsystem::CreateTestSpec()
{
    AreaSpec.Index = 2;
}   

void UCoinActionManagementWSubsystem::ExecuteNowAction()
{
    if(RepeatActionCnt <= 0) return;
    UE_LOG(LogTemp, Warning, TEXT("5"));

    SelectedAction->ExecuteAction();
    RepeatActionCnt--;
    
    if(RepeatActionCnt > 0)
    {
        GetWorld()->GetTimerManager().SetTimer(
            AutoActionHandler,
            this,
            &UCoinActionManagementWSubsystem::ExecuteNowAction,
            CoinNaiagaraTime,
            false
        );
    }
    else
    {
        InitWeaponAction();
    }

}

void UCoinActionManagementWSubsystem::ExecuteTimeAction(const struct FGridPoint& TargetGridPoint)
{
    if (RepeatActionCnt <= 0) return;

    if(!ApplyRangedThings(TargetGridPoint))
    {
        return;
    }

    ACoinActor* TargetCoin = Cast<ACoinActor>(GridManager->GetGridActor(TargetGridPoint)->GetCurrentOccupied());
    //클릭한거 하나 세팅
    SelectedAction->SetSingleCellTargetCoin(TargetCoin);

    SelectedAction->ExecuteAction();
    RepeatActionCnt--;

    if(RepeatActionCnt <= 0)
    {
        InitWeaponAction();
    }
    
}