#include "Subsystem/BattleLevel/CoinActionManagementWSubsystem.h"
#include "AttackAreaTypes.h"
#include "WeaponDataTypes.h"
#include "FlipSide_Enum.h"
#include "CoinActor.h"
#include "Base_OtherActor.h"
#include "GridActor.h"
#include "Weapon_Action.h"
#include "W_BattleCoinInfo.h"
#include "Component_Status.h"
#include "GridManagerSubsystem.h"
#include "FlipSideDevloperSettings.h"
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
}

void UCoinActionManagementWSubsystem::OnWorldBeginPlay(UWorld& InWorld)
{
    Super::OnWorldBeginPlay(InWorld);

    if(!BattleCoinInfoWidgetInstance)
    {
        const UFlipSideDevloperSettings* Settings = GetDefault<UFlipSideDevloperSettings>();
        if (Settings && !Settings->BattleCoinInfoWidget.IsNull())
        {
            UClass* BattleCoinWidgetClass = Settings->BattleCoinInfoWidget.LoadSynchronous();
                
            if (BattleCoinWidgetClass)
            {
                BattleCoinInfoWidgetInstance = CreateWidget<UW_BattleCoinInfo>(GetWorld(), BattleCoinWidgetClass);
                if ( BattleCoinInfoWidgetInstance)
                {
                    BattleCoinInfoWidgetInstance->AddToViewport();
                    BattleCoinInfoWidgetInstance->SetVisibility(ESlateVisibility::Collapsed);
                }
            }
        }
    }

    if(GridManager)
    {
        GridManager->OnGridClickedForCoin.BindDynamic(this, &UCoinActionManagementWSubsystem::ExecuteGridAction);
    }
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
    SelectedAction->SetInRangeOthers(nullptr);
    SelectedAction->SetLogicID(-1);
    SelectedAction->SetFinalAttackPoint(-1);
    SelectedAction->SetFinalBehaviorPoint(-1);
    SelectedAction->SetCasterCoin(nullptr);
    SelectedAction->SetOtherForAction(nullptr);
    RepeatActionCnt = 1;
    CurrentInputState = EActionInputState::None;
    ValidTargetGrids.Empty();
    if (GridManager)
    {
        GridManager->ResetBattleCoinPreview();
        GridManager->SetGridClickFlag(EGridClickFlag::None);
    }

    if (BattleCoinInfoWidgetInstance)
    {
        BattleCoinInfoWidgetInstance->SetVisibility(ESlateVisibility::Collapsed);
    }
}

bool UCoinActionManagementWSubsystem::ApplyRangedThings(const FGridPoint& TargetGridPoint)
{
    if(!GridManager) return false;

    if(CurrentInputState == EActionInputState::WaitingForCoinClick ||
        CurrentInputState == EActionInputState::WaitingForGridClick ||
        CurrentInputState == EActionInputState::WaitingForOtherClick)
    {
        if(!ValidTargetGrids.Contains(TargetGridPoint))
        {
            return false;
        }
        else
        {
            return true;
        }
    }

    FObjectOnGridInfo GridInfos;

    const FGridPoint RangeForQuery = (AreaSpec.AnchorMode == EAreaAnchor::UseAnchorCell)
        ? FGridPoint{0, 0}
        : SelectedAction->GetActionRange();
    GridManager->GetObjectsAtRange(AreaSpec, RangeForQuery, OutCells, GridInfos);
    for(AActor* Actor : GridInfos.Coins)
    {
        if(ACoinActor* Coin = Cast<ACoinActor>(Actor))
        {
            SelectedAction->SetInRangeCoins(Coin);
        }
    }

    if(GridInfos.Boss)
    {
        SelectedAction->SetInRangeBoss(GridInfos.Boss);
    }

    for(AActor* Actor : GridInfos.Others)
    {
        if(ABase_OtherActor* Others = Cast<ABase_OtherActor>(Actor))
        {
            SelectedAction->SetInRangeOthers(Others);
        }
    }
    

    return true;

}

void UCoinActionManagementWSubsystem::SetSelectedWeapon(ACoinActor* HoveredCoin)
{
    if(!bIsCorrectTurn) return;
    if(!IsValid(HoveredCoin)) return;

    FActionTask ActionTask = HoveredCoin->StatComponent->GetModifiedStats();
    FGridPoint CoinGrid =  HoveredCoin->GetDecidedGrid();

    if (UGameInstance* GI = GetWorld()->GetGameInstance())
    {
        UDataManagerSubsystem* DM = GI->GetSubsystem<UDataManagerSubsystem>();

        FFaceData SelectWeapon;
        FGridPoint LastGridPoint;

        if(DM->TryGetWeapon(HoveredCoin->GetCoinFaceID(), SelectWeapon) && SelectedAction && CurrentInputState == EActionInputState::None)
        {
            LastGridPoint.GridX = SelectWeapon.AttackRange.GridX + ActionTask.ModifiedRange.GridX;
            LastGridPoint.GridY = SelectWeapon.AttackRange.GridY + ActionTask.ModifiedRange.GridY;

            SelectedAction->SetLogicID(SelectWeapon.WeaponID);
            SelectedAction->SetActionRange(LastGridPoint);
            SelectedAction->SetFinalAttackPoint(SelectWeapon.AttackPoint + ActionTask.ModifiedAttackPoint);
            SelectedAction->SetFinalBehaviorPoint(SelectWeapon.BehaviorPoint + ActionTask.ModifiedBehaviorPoint);
            SelectedAction->SetFinalRange(LastGridPoint.GridX, LastGridPoint.GridY);
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

            if (AreaSpec.Pattern == EAttackAreaPattern::CircleOnCell)
            {
                AreaSpec.ParamA += ActionTask.ModifiedRange.GridY;
            }
            else if (AreaSpec.Pattern == EAttackAreaPattern::RectFromCell)
            {
                AreaSpec.ParamB += ActionTask.ModifiedRange.GridY;
                UE_LOG(LogTemp, Warning, TEXT("[LensDebug] RectFromCell ParamB=%d, ModifiedRangeY=%d"), AreaSpec.ParamB, ActionTask.ModifiedRange.GridY);
            }

            SetBattleCoinInfo(
                SelectWeapon.WeaponIcon, FText::FromString(SelectWeapon.WeaponName), FText::FromString(SelectWeapon.KOR_DES), 
                SelectWeapon.BehaviorPoint, ActionTask.ModifiedBehaviorPoint, 
                SelectWeapon.AttackPoint, ActionTask.ModifiedAttackPoint, SelectWeapon.TypeColor,
                HoveredCoin->StatComponent->ActiveBuffs
            );

            SetCasterCoin(HoveredCoin);

            if(AreaSpec.Pattern == EAttackAreaPattern::SingleCell)
            {
                GridManager->GetValidGridsForSingleCell(CoinGrid,AreaSpec,ValidTargetGrids);
            }
            else
            {
                ApplyRangedThings(CoinGrid);
            }

            const FGridPoint PreviewFinalRange = (AreaSpec.AnchorMode == EAreaAnchor::UseAnchorCell)
                ? FGridPoint{0, 0}
                : LastGridPoint;
            GridManager->PreviewHoveredCoinRange(CoinGrid, AreaSpec, PreviewFinalRange);
        }
    }
}

void UCoinActionManagementWSubsystem::SetCasterCoin(ACoinActor* CasterCoin)
{
    if(SelectedAction)
    {
        SelectedAction->SetCasterCoin(CasterCoin);
    }
}


//ExecuteSelectedWeapon는 코인에서 델리게이트 받아서 분기 가장 처음에는 호버링 데이터 보고 어떻게 할지 정함
//코인은 나 선택됬다고 신호만 보내면 됨
void UCoinActionManagementWSubsystem::ExecuteSelectedWeapon(ACoinActor* ClickedCoin)
{
    if(CurrentInputState == EActionInputState::None)
    {
        if(AreaSpec.Pattern == EAttackAreaPattern::SingleCell)
        {
            if(AreaSpec.ParamB == 0)
            {
                CurrentInputState = EActionInputState::WaitingForCoinClick;
                GridManager->SetGridClickFlag(EGridClickFlag::CoinAction);
            }
            else if(AreaSpec.ParamB == 1)
            {
                CurrentInputState = EActionInputState::WaitingForGridClick;
                GridManager->SetGridClickFlag(EGridClickFlag::CoinAction);

            }
            else if(AreaSpec.ParamB == 2)
            {
                CurrentInputState = EActionInputState::WaitingForOtherClick;
            }
        }
        else
        {
            CurrentInputState = EActionInputState::ExecutingAction;
            ClickedCoin->SetCoinIsActed(true);
            ExecuteNowAction();
        }
    }
    else if(CurrentInputState == EActionInputState::WaitingForCoinClick)
    {
        ExecuteTimeAction(ClickedCoin);
    }
}

//즉발 즉, 하나 선택X
void UCoinActionManagementWSubsystem::ExecuteNowAction()
{
    if(RepeatActionCnt <= 0) return;

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

//하나 선택
void UCoinActionManagementWSubsystem::ExecuteTimeAction(ACoinActor* TargetCoin)
{
    if (RepeatActionCnt <= 0) return;

    //여기서 코인 적용 칸도 제한함
    if(!ApplyRangedThings(TargetCoin->GetDecidedGrid()))
    {   //사거리 바깥 선택 시 걍 초기화.
        //로직 상 문제가 살짝 있음. 이거 아예 초기화 말고 안된다고 띄워주는 피드백이 필요함. 왜냐면 이거 넘어가면 그냥 해당 코인은 Acted가 켜져서 한 번 클릭 미스하면 다시 못누름
        CancelSelectWeapon(); 
        return;
    }

    if (SelectedAction && SelectedAction->GetCasterCoin())
    {
        SelectedAction->GetCasterCoin()->SetCoinIsActed(true);
    }

    //클릭한거 하나 세팅
    SelectedAction->SetSingleCellTargetCoin(TargetCoin);

    SelectedAction->ExecuteAction();
    RepeatActionCnt--;

    if(RepeatActionCnt <= 0) 
    {
        InitWeaponAction();
    }
    
}

bool UCoinActionManagementWSubsystem::TryExecuteOtherAction(ABase_OtherActor* TargetOther)
{
    if(CurrentInputState != EActionInputState::WaitingForOtherClick)
    {
        return false;
    }

    if(RepeatActionCnt <= 0)
    {
        return true;
    }

    if(!TargetOther || !TargetOther->GetOccupiedGrid())
    {
        CancelSelectWeapon();
        return true;
    }

    if(!ApplyRangedThings(TargetOther->GetOccupiedGrid()->GetGridPoint()))
    {
        CancelSelectWeapon();
        return true;
    }

    if(SelectedAction && SelectedAction->GetCasterCoin())
    {
        SelectedAction->GetCasterCoin()->SetCoinIsActed(true);
        SelectedAction->SetOtherForAction(TargetOther);
        SelectedAction->ExecuteAction();
        RepeatActionCnt--;
    }

    if(RepeatActionCnt <= 0)
    {
        InitWeaponAction();
    }

    return true;
}

//Grid를 클릭해야 하는 코인 액션
void UCoinActionManagementWSubsystem::ExecuteGridAction(AGridActor* targetGrid)
{
    if (RepeatActionCnt <= 0) return;

    if(!ApplyRangedThings(targetGrid->GetGridPoint()))
    {
        //위와 같은 문제점 공유
        CancelSelectWeapon(); 
        return;
    }

    
    if (SelectedAction && SelectedAction->GetCasterCoin())
    {
        SelectedAction->GetCasterCoin()->SetCoinIsActed(true);
        SelectedAction->SetGridForAction(targetGrid);
        SelectedAction->ExecuteAction();
        RepeatActionCnt--;
    }

    if(RepeatActionCnt <= 0) 
    {
        InitWeaponAction();
    }
}

//호버링 시 UI세팅
void UCoinActionManagementWSubsystem::SetBattleCoinInfo(
        UTexture2D* Icon, const FText& WeaponName, const FText& RawDescription, 
		int32 DefaultBP, int32 ModifiedBP, 
		int32 DefaultAP, int32 ModifiedAP, FLinearColor WeaponColor,
        const TArray<FBuffInfo>& ActiveBuffs)
{
    if(BattleCoinInfoWidgetInstance)
    {
        BattleCoinInfoWidgetInstance->UpdateBattleCoinInfo(
            Icon, WeaponName, RawDescription,
            DefaultBP, ModifiedBP,
            DefaultAP, ModifiedAP, WeaponColor,
            ActiveBuffs
        );
        BattleCoinInfoWidgetInstance->SetVisibility(ESlateVisibility::Visible);
    }
}

void UCoinActionManagementWSubsystem::HandleCoinUnHovered()
{
    if (CurrentInputState == EActionInputState::None)
    {
        if(SelectedAction)
        {
            SelectedAction->InitInRangeBoss();
        }
        InitWeaponAction();
    }
}
