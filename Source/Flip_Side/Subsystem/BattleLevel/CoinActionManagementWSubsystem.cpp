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
#include "BattleLevelActingWSubsystem.h"
#include "SoundManagerWSubsystem.h"
#include "BossActor.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraSystem.h"

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
    GetWorld()->GetTimerManager().ClearTimer(AutoActionHandler);
    GetWorld()->GetTimerManager().ClearTimer(CommonVFXTimerHandle);
    bActionSequenceActive = false;
    bCurrentStepTargetValid = true;
    InitWeaponAction();
}

void UCoinActionManagementWSubsystem::SetTurn(const bool bIsTurn)
{
    bIsCorrectTurn = bIsTurn;

    if(!bIsTurn)
    {
        GetWorld()->GetTimerManager().ClearTimer(AutoActionHandler);
        GetWorld()->GetTimerManager().ClearTimer(CommonVFXTimerHandle);
        bActionSequenceActive = false;
        bCurrentStepTargetValid = true;
        InitWeaponAction();
    }
}

void UCoinActionManagementWSubsystem::StopActionSequenceForStageEnd()
{
    if(UWorld* World = GetWorld())
    {
        World->GetTimerManager().ClearTimer(AutoActionHandler);
        World->GetTimerManager().ClearTimer(CommonVFXTimerHandle);
    }

    if(SelectedAction)
    {
        if(ACoinActor* CasterCoin = SelectedAction->GetCasterCoin())
        {
            CasterCoin->SetCoinIsActing(false);
        }
    }

    bIsCorrectTurn = false;
    bActionSequenceActive = false;
    bCurrentStepTargetValid = true;
    bPendingFailedVFX = false;
    RepeatActionCnt = 0;

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
    SelectedAction->SetGridForAction(nullptr);
    SelectedAction->SetOtherForAction(nullptr);
    RepeatActionCnt = 1;
    bPendingFailedVFX = false;
    CurrentInputState = EActionInputState::None;
    ClearValidSingleCellTargets();
    if (GridManager)
    {
        GridManager->ResetBattleCoinPreview();
        GridManager->SetGridClickFlag(EGridClickFlag::None);
    }

    HideBattleCoinInfo();
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

    SelectedAction->SetInRangeCoins(nullptr);
    SelectedAction->SetInRangeOthers(nullptr);
    SelectedAction->SetInRangeBoss(nullptr);

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
    if(bActionSequenceActive) return;

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
            }

            SetBattleCoinInfo(
                SelectWeapon.WeaponIcon, FText::FromString(SelectWeapon.WeaponName), FText::FromString(SelectWeapon.KOR_DES), 
                SelectWeapon.BehaviorPoint, ActionTask.ModifiedBehaviorPoint, 
                SelectWeapon.AttackPoint, ActionTask.ModifiedAttackPoint, SelectWeapon.TypeColor,
                HoveredCoin->StatComponent->GetHP(), HoveredCoin->StatComponent->GetMaxHP(),
                HoveredCoin->StatComponent->ActiveBuffs
            );

            SetCasterCoin(HoveredCoin);

            if(AreaSpec.Pattern == EAttackAreaPattern::SingleCell)
            {
                GridManager->GetValidGridsForSingleCell(CoinGrid,AreaSpec,ValidTargetGrids);
                BuildValidSingleCellTargets();
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
    ABossActor* Boss = nullptr;
    if(SelectedAction && SelectedAction->GetInRangeBoss(Boss) && IsValid(Boss))
    {
        if(!HoveredCoin->GetCoinIsActed())
        {
            Boss->DisPlayOutline();
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
    if(CurrentInputState == EActionInputState::WaitingForGridClick ||
        CurrentInputState == EActionInputState::WaitingForOtherClick)
    {
        return;
    }

    if(CurrentInputState == EActionInputState::WaitingForCoinClick)
    {
        if(!ValidTargetCoins.Contains(ClickedCoin))
        {
            PlayFailedVFX();
            return;
        }

        ExecuteTimeAction(ClickedCoin);
        return;
    }

    if(ClickedCoin->GetCoinIsActed())
    {
        PlayFailedVFX();
        return;
    }
    ACoinActor* CasterCoin = SelectedAction ? SelectedAction->GetCasterCoin() : nullptr;
    if(!IsValid(CasterCoin) && IsValid(ClickedCoin))
    {
        SetSelectedWeapon(ClickedCoin);
        CasterCoin = SelectedAction ? SelectedAction->GetCasterCoin() : nullptr;
    }

    ABossActor* Boss = nullptr;
    if(SelectedAction && SelectedAction->GetInRangeBoss(Boss) && IsValid(Boss))
    {
        Boss->UnDisPlayOutline();
    }

    if(CurrentInputState == EActionInputState::None)
    {
        StartCoinActionSequence(CasterCoin);
    }
}

//즉발 즉, 하나 선택X
void UCoinActionManagementWSubsystem::ExecuteNowAction()
{
    RunCoinActionStep();
}

//하나 선택
void UCoinActionManagementWSubsystem::ExecuteTimeAction(ACoinActor* TargetCoin)
{
    if (RepeatActionCnt <= 0) return;

    //클릭한거 하나 세팅
    SelectedAction->SetSingleCellTargetCoin(TargetCoin);
    bCurrentStepTargetValid = IsValid(TargetCoin) && ApplyRangedThings(TargetCoin->GetDecidedGrid());

    RunCoinActionStep();
}

bool UCoinActionManagementWSubsystem::TryExecuteOtherAction(ABase_OtherActor* TargetOther)
{
    if(CurrentInputState != EActionInputState::WaitingForOtherClick)
    {
        return false;
    }

    if(RepeatActionCnt <= 0) return true;

    if(!ValidTargetOthers.Contains(TargetOther))
    {
        PlayFailedVFX();
        return true;
    }

    SelectedAction->SetOtherForAction(TargetOther);
    bCurrentStepTargetValid = IsValid(TargetOther) && TargetOther->GetOccupiedGrid() && ApplyRangedThings(TargetOther->GetOccupiedGrid()->GetGridPoint());
    RunCoinActionStep();

    return true;
}

//Grid를 클릭해야 하는 코인 액션
void UCoinActionManagementWSubsystem::ExecuteGridAction(AGridActor* targetGrid)
{
    if (RepeatActionCnt <= 0) return;

    if(CurrentInputState != EActionInputState::WaitingForGridClick)
    {
        return;
    }

    if(!ValidTargetGridActors.Contains(targetGrid))
    {
        PlayFailedVFX();
        return;
    }

    SelectedAction->SetGridForAction(targetGrid);
    bCurrentStepTargetValid = IsValid(targetGrid) && ApplyRangedThings(targetGrid->GetGridPoint());
    RunCoinActionStep();
}

//호버링 시 UI세팅
void UCoinActionManagementWSubsystem::SetBattleCoinInfo(
        UTexture2D* Icon, const FText& WeaponName, const FText& RawDescription, 
		int32 DefaultBP, int32 ModifiedBP, 
		int32 DefaultAP, int32 ModifiedAP, FLinearColor WeaponColor,
        int32 CurrentHP, int32 MaxHP,
        const TArray<FBuffInfo>& ActiveBuffs)
{
    if(BattleCoinInfoWidgetInstance)
    {
        BattleCoinInfoWidgetInstance->UpdateBattleCoinInfo(
            Icon, WeaponName, RawDescription,
            DefaultBP, ModifiedBP,
            DefaultAP, ModifiedAP,
            CurrentHP, MaxHP, WeaponColor,
            ActiveBuffs
        );
        BattleCoinInfoWidgetInstance->SetVisibility(ESlateVisibility::Visible);
    }
}

void UCoinActionManagementWSubsystem::HideBattleCoinInfo()
{
    if (BattleCoinInfoWidgetInstance)
    {
        BattleCoinInfoWidgetInstance->SetVisibility(ESlateVisibility::Collapsed);
    }
}

void UCoinActionManagementWSubsystem::HandleCoinUnHovered()
{
    if(!bIsCorrectTurn) return;

    HideBattleCoinInfo();

    if(bActionSequenceActive) return;

    if (CurrentInputState == EActionInputState::None)
    {
        if(SelectedAction)
        {
            SelectedAction->InitInRangeBoss();
        }
        InitWeaponAction();
    }
}

void UCoinActionManagementWSubsystem::StartCoinActionSequence(ACoinActor* CasterCoin)
{
    if(!bIsCorrectTurn || !SelectedAction || !IsValid(CasterCoin)) return;
    if(bActionSequenceActive) return;

    if (USoundManagerWSubsystem* SoundManager = GetWorld()->GetSubsystem<USoundManagerWSubsystem>())
    {
        SoundManager->PlayCoinClickSound();
    }

    bActionSequenceActive = true;
    bCurrentStepTargetValid = true;
    CasterCoin->SetCoinIsActed(true);

    if(UBattleLevelActingWSubsystem* ActingManager = GetActingManager())
    {
        ActingManager->RaiseCoinForAction(CasterCoin, FSimpleDelegate::CreateUObject(this, &UCoinActionManagementWSubsystem::RunCoinActionStep));
    }
    else
    {
        RunCoinActionStep();
    }
}

void UCoinActionManagementWSubsystem::RunCoinActionStep()
{
    if(!bActionSequenceActive || !SelectedAction) return;

    if(RepeatActionCnt <= 0)
    {
        FinishCoinActionSequence();
        return;
    }

    ACoinActor* CasterCoin = SelectedAction->GetCasterCoin();

    const bool bHadSingleCellTargetInput =
        CurrentInputState == EActionInputState::WaitingForCoinClick ||
        CurrentInputState == EActionInputState::WaitingForGridClick ||
        CurrentInputState == EActionInputState::WaitingForOtherClick;

    if(AreaSpec.Pattern == EAttackAreaPattern::SingleCell && !bHadSingleCellTargetInput)
    {
        if(IsValid(CasterCoin))
        {
            CasterCoin->SetCoinIsActing(true);
        }

        bCurrentStepTargetValid = true;
        SelectedAction->SetSingleCellTargetCoin(nullptr);
        SelectedAction->SetGridForAction(nullptr);
        SelectedAction->SetOtherForAction(nullptr);

        if(AreaSpec.ParamB == 0)
        {
            CurrentInputState = EActionInputState::WaitingForCoinClick;
            if(GridManager) GridManager->SetGridClickFlag(EGridClickFlag::CoinAction);
        }
        else if(AreaSpec.ParamB == 1)
        {
            CurrentInputState = EActionInputState::WaitingForGridClick;
            if(GridManager) GridManager->SetGridClickFlag(EGridClickFlag::CoinAction);
        }
        else if(AreaSpec.ParamB == 2)
        {
            CurrentInputState = EActionInputState::WaitingForOtherClick;
            if(GridManager) GridManager->SetGridClickFlag(EGridClickFlag::None);
        }
        return;
    }

    CurrentInputState = EActionInputState::ExecutingAction;
    if(GridManager)
    {
        GridManager->SetGridClickFlag(EGridClickFlag::None);
    }

    if(!IsValid(CasterCoin))
    {
        FinishCoinActionSequence();
        return;
    }

    if(AreaSpec.Pattern != EAttackAreaPattern::SingleCell)
    {
        bCurrentStepTargetValid = ApplyRangedThings(CasterCoin->GetDecidedGrid());
    }

    if(UBattleLevelActingWSubsystem* ActingManager = GetActingManager())
    {
        ActingManager->ShakeCoinForAction(CasterCoin, FSimpleDelegate::CreateUObject(this, &UCoinActionManagementWSubsystem::ResolveCurrentActionStep));
    }
    else
    {
        ResolveCurrentActionStep();
    }
}

void UCoinActionManagementWSubsystem::ResolveCurrentActionStep()
{
    if(!bActionSequenceActive || !SelectedAction) return;

    FWeaponActionResolveResult Result = SelectedAction->ResolveAction();
    if(!bCurrentStepTargetValid || !Result.bCanExecute)
    {
        bPendingFailedVFX = true;
        RepeatActionCnt = 0;
        FinishCoinActionSequence();
        return;
    }

    PlayCoinSpecificVFX();

    const UFlipSideDevloperSettings* Settings = GetDefault<UFlipSideDevloperSettings>();
    const float Delay = Settings ? Settings->CommonVFXDelayAfterCoinVFX : 0.0f;
    if(Delay > 0.0f)
    {
        GetWorld()->GetTimerManager().SetTimer(CommonVFXTimerHandle, FTimerDelegate::CreateWeakLambda(this, [this, Result]()
        {
            PlayCommonVFX(Result);
            if(SelectedAction)
            {
                SelectedAction->ExecuteAction();
            }
            RepeatActionCnt--;
            bCurrentStepTargetValid = true;
            RunCoinActionStep();
        }), Delay, false);
    }
    else
    {
        PlayCommonVFX(Result);
        SelectedAction->ExecuteAction();
        RepeatActionCnt--;
        bCurrentStepTargetValid = true;
        RunCoinActionStep();
    }
}

void UCoinActionManagementWSubsystem::FinishCoinActionSequence()
{
    GetWorld()->GetTimerManager().ClearTimer(CommonVFXTimerHandle);

    ACoinActor* CasterCoin = SelectedAction ? SelectedAction->GetCasterCoin() : nullptr;
    if(UBattleLevelActingWSubsystem* ActingManager = GetActingManager())
    {
        ActingManager->LowerCoinAfterAction(CasterCoin, FSimpleDelegate::CreateUObject(this, &UCoinActionManagementWSubsystem::HandleCoinActionLowerFinished));
    }
    else
    {
        HandleCoinActionLowerFinished();
    }
}

void UCoinActionManagementWSubsystem::HandleCoinActionLowerFinished()
{
    if(SelectedAction)
    {
        if(ACoinActor* CasterCoin = SelectedAction->GetCasterCoin())
        {
            CasterCoin->SetCoinIsActing(false);
        }
    }

    if(bPendingFailedVFX)
    {
        PlayFailedVFX();
        bPendingFailedVFX = false;
    }

    bActionSequenceActive = false;
    bCurrentStepTargetValid = true;
    InitWeaponAction();
}

void UCoinActionManagementWSubsystem::TryCancelCurrentAction()
{
    if (!bActionSequenceActive || !SelectedAction) return;
    if (AreaSpec.Pattern != EAttackAreaPattern::SingleCell) return;
    if (CurrentInputState != EActionInputState::WaitingForCoinClick &&
        CurrentInputState != EActionInputState::WaitingForGridClick &&
        CurrentInputState != EActionInputState::WaitingForOtherClick)
    {
        return;
    }

    GetWorld()->GetTimerManager().ClearTimer(AutoActionHandler);
    GetWorld()->GetTimerManager().ClearTimer(CommonVFXTimerHandle);

    bPendingFailedVFX = false;
    bCurrentStepTargetValid = true;
    RepeatActionCnt = 0;
    FinishCoinActionSequence();
}

void UCoinActionManagementWSubsystem::CancelSingleCellAction(ACoinActor* ClickedCoin)
{
    if(!bActionSequenceActive || !SelectedAction || AreaSpec.Pattern != EAttackAreaPattern::SingleCell) return;
    if(CurrentInputState != EActionInputState::WaitingForCoinClick &&
        CurrentInputState != EActionInputState::WaitingForGridClick &&
        CurrentInputState != EActionInputState::WaitingForOtherClick)
    {
        return;
    }

    ACoinActor* CasterCoin = SelectedAction->GetCasterCoin();
    if(!IsValid(CasterCoin) || ClickedCoin != CasterCoin) return;

    GetWorld()->GetTimerManager().ClearTimer(AutoActionHandler);
    GetWorld()->GetTimerManager().ClearTimer(CommonVFXTimerHandle);

    bPendingFailedVFX = false;
    bCurrentStepTargetValid = true;
    RepeatActionCnt = 0;
    FinishCoinActionSequence();
}

void UCoinActionManagementWSubsystem::PlayCoinSpecificVFX()
{
    if(!SelectedAction) return;

    const FFaceData& WeaponData = SelectedAction->GetWeaponData();

    if (WeaponData.WeaponSFX)
    {
        if (USoundManagerWSubsystem* SoundManager = GetWorld()->GetSubsystem<USoundManagerWSubsystem>())
        {
            SoundManager->PlayCoinActionSound(WeaponData.WeaponSFX);
        }
    }

    if(!WeaponData.WeaponVFX || WeaponData.WeaponVFXTarget == EWeaponVFXTarget::None) return;

    switch(WeaponData.WeaponVFXTarget)
    {
    case EWeaponVFXTarget::Caster:
        if(ACoinActor* CasterCoin = SelectedAction->GetCasterCoin())
        {
            SpawnVFXAtLocation(WeaponData.WeaponVFX, CasterCoin->GetActorLocation());
        }
        break;
    case EWeaponVFXTarget::TargetGrid:
        if(AGridActor* TargetGrid = SelectedAction->GetTargetGrid())
        {
            SpawnVFXAtLocation(WeaponData.WeaponVFX, FVector(TargetGrid->GetGridWorldXY().X, TargetGrid->GetGridWorldXY().Y, -80.0f));
        }
        break;
    case EWeaponVFXTarget::TargetCoin:
        for(ACoinActor* Coin : SelectedAction->GetInRangeCoins())
        {
            if(IsValid(Coin))
            {
                SpawnVFXAtLocation(WeaponData.WeaponVFX, Coin->GetActorLocation());
                break;
            }
        }
        break;
    case EWeaponVFXTarget::TargetOther:
        if(ABase_OtherActor* TargetOther = SelectedAction->GetTargetOther())
        {
            SpawnVFXAtLocation(WeaponData.WeaponVFX, TargetOther->GetActorLocation());
        }
        break;
    case EWeaponVFXTarget::Boss:
        {
            ABossActor* Boss = nullptr;
            if(SelectedAction->GetInRangeBoss(Boss) && IsValid(Boss))
            {
                SpawnVFXAtLocation(WeaponData.WeaponVFX, Boss->GetActorLocation());
            }
        }
        break;
    case EWeaponVFXTarget::RangeCells:
        if(GridManager)
        {
            for(const FGridPoint& Cell : OutCells)
            {
                if(AGridActor* Grid = GridManager->GetGridActor(Cell))
                {
                    SpawnVFXAtLocation(WeaponData.WeaponVFX, FVector(Grid->GetGridWorldXY().X, Grid->GetGridWorldXY().Y, -80.0f));
                }
            }
        }
        break;
    case EWeaponVFXTarget::AffectedCoins:
        for(ACoinActor* Coin : SelectedAction->GetInRangeCoins())
        {
            if(IsValid(Coin))
            {
                SpawnVFXAtLocation(WeaponData.WeaponVFX, Coin->GetActorLocation());
            }
        }
        break;
    default:
        break;
    }
}

void UCoinActionManagementWSubsystem::PlayCommonVFX(const FWeaponActionResolveResult& Result)
{
    if(!Result.Boss) return;

    const UFlipSideDevloperSettings* Settings = GetDefault<UFlipSideDevloperSettings>();
    if(!Settings) return;

    if(Result.bAppliesBossCC)
    {
        SpawnVFXAtLocation(Settings->Boss_CC_VFX.LoadSynchronous(), Result.Boss->GetActorLocation());
    }
    else if(Result.bDamagesBoss)
    {
        SpawnVFXAtLocation(Settings->Boss_Hit_VFX.LoadSynchronous(), Result.Boss->GetActorLocation());
    }
}

void UCoinActionManagementWSubsystem::PlayFailedVFX()
{
    if(!SelectedAction) return;

    ACoinActor* CasterCoin = SelectedAction->GetCasterCoin();
    if(!IsValid(CasterCoin)) return;

    const UFlipSideDevloperSettings* Settings = GetDefault<UFlipSideDevloperSettings>();
    if(!Settings) return;

    SpawnVFXAtLocation(Settings->Coin_Logic_Failed_VFX.LoadSynchronous(), CasterCoin->GetActorLocation());

    if (USoundManagerWSubsystem* SoundManager = GetWorld()->GetSubsystem<USoundManagerWSubsystem>())
    {
        SoundManager->PlayCoinActionFailedSound();
    }
}

void UCoinActionManagementWSubsystem::SpawnVFXAtLocation(UNiagaraSystem* VFX, const FVector& Location) const
{
    if(!VFX || !GetWorld()) return;

    UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), VFX, Location);
}

UBattleLevelActingWSubsystem* UCoinActionManagementWSubsystem::GetActingManager() const
{
    return GetWorld() ? GetWorld()->GetSubsystem<UBattleLevelActingWSubsystem>() : nullptr;
}

void UCoinActionManagementWSubsystem::BuildValidSingleCellTargets()
{
    ValidTargetCoins.Empty();
    ValidTargetGridActors.Empty();
    ValidTargetOthers.Empty();

    if(!GridManager) return;

    for(const FGridPoint& TargetCell : ValidTargetGrids)
    {
        AGridActor* GridActor = GridManager->GetGridActor(TargetCell);
        if(!IsValid(GridActor)) continue;

        ValidTargetGridActors.Add(GridActor);

        AActor* OccupiedActor = GridActor->GetCurrentOccupied();
        if(ACoinActor* Coin = Cast<ACoinActor>(OccupiedActor))
        {
            ValidTargetCoins.Add(Coin);
        }
        else if(ABase_OtherActor* Other = Cast<ABase_OtherActor>(OccupiedActor))
        {
            ValidTargetOthers.Add(Other);
        }
    }
}

void UCoinActionManagementWSubsystem::ClearValidSingleCellTargets()
{
    ValidTargetGrids.Empty();
    ValidTargetCoins.Empty();
    ValidTargetGridActors.Empty();
    ValidTargetOthers.Empty();
}
