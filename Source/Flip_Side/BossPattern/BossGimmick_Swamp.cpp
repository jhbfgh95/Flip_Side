#include "BossGimmick_Swamp.h"
#include "BossActor.h"
#include "GridActor.h"
#include "GridManagerSubsystem.h"
#include "BossManagerSubsystem.h"
#include "CoinActor.h"
#include "Component_Status.h"
#include "Actors/Others/Base_OtherActor.h"

FLinearColor UBossGimmick_Swamp::GetSwampColor(EWeaponClass Type)
{
	switch (Type)
	{
	case EWeaponClass::Deal: return FLinearColor(1.f,  0.1f, 0.1f, 1.f);
	case EWeaponClass::Tank: return FLinearColor(0.1f, 0.3f, 1.f,  1.f);
	case EWeaponClass::Heal: return FLinearColor(0.1f, 0.9f, 0.2f, 1.f);
	default:                 return FLinearColor(0.5f, 0.5f, 0.5f, 1.f);
	}
}

void UBossGimmick_Swamp::OnPlayerPhaseStart(ABossActor* Boss)
{
	if (!Boss) return;

	UWorld* World = Boss->GetWorld();
	if (!World) return;

	UGridManagerSubsystem* GridMgr = World->GetSubsystem<UGridManagerSubsystem>();
	if (!GridMgr) return;

	// 이미 깔린 늪 색 표시
	for (const FSwampCell& S : ActiveSwamps)
	{
		if (AGridActor* Grid = GridMgr->GetGridActor(S.GridPoint))
		{
			Grid->InitColor();
		}
	}
}

void UBossGimmick_Swamp::OnCoinLanded(ABossActor* Boss, FBossPhaseContext& Context)
{
	if (!Boss) return;

	UWorld* World = Boss->GetWorld();
	if (!World) return;

	UGridManagerSubsystem* GridMgr = World->GetSubsystem<UGridManagerSubsystem>();
	if (!GridMgr) return;

	// 설치 패턴이면 늪 설치
	if (Context.CurrentPattern
		&& Context.CurrentPattern->PatternData.IsValidIndex(Context.CurrentPatternIndex)
		&& Context.CurrentPattern->PatternData[Context.CurrentPatternIndex].bNoDamage
		&& Context.CurrentPattern->PatternData[Context.CurrentPatternIndex].GimmickType == EBossGimmickType::GridDebuff)
	{
		InstallSwamp(Boss, Context.LockedCells);
	}

	// 늪 위에 올라온 코인 중 대상 타입이면 공격력 디버프
	const int32 DebuffAmount = GimmickData.ParamIntB > 0 ? GimmickData.ParamIntB : 1;

	for (const FSwampCell& S : ActiveSwamps)
	{
		AGridActor* Grid = GridMgr->GetGridActor(S.GridPoint);
		if (!Grid) continue;

		ACoinActor* Coin = Cast<ACoinActor>(Grid->GetCurrentOccupied());
		if (!IsValid(Coin)) continue;
		if (Coin->GetWeaponType() != S.SwampType) continue;

		UComponent_Status* Status = Coin->FindComponentByClass<UComponent_Status>();
		if (!Status) continue;

		FBuffInfo Debuff;
		Debuff.bIsDebuff = true;
		Debuff.BuffName = TEXT("늪 디버프");
		const int32 Penalty = -DebuffAmount;
		Debuff.StatDelegate = FOnCalculateStats::FDelegate::CreateLambda([Penalty](FActionTask& Task)
		{
			Task.ModifiedAttackPoint += Penalty;
		});

		Status->AddBuffs(Debuff);
	}
}

void UBossGimmick_Swamp::OnBeforePatternExecute(ABossActor* Boss, FBossPhaseContext& Context)
{
	if (!Boss || !Context.CurrentPattern) return;

	const int32 PatternIndex = Context.CurrentPatternIndex;
	const bool bNoDamage = Context.CurrentPattern->PatternData.IsValidIndex(PatternIndex)
		&& Context.CurrentPattern->PatternData[PatternIndex].bNoDamage;
	if (bNoDamage) return;

	for (const FGridPoint& Cell : Context.LockedCells)
	{
		if (IsSwampCell(Cell))
		{
			const float Multiplier = GimmickData.ParamFloatB > 0.f ? GimmickData.ParamFloatB : 1.5f;
			Context.BonusDamage = FMath::RoundToInt(Context.BaseDamage * (Multiplier - 1.f));
			return;
		}
	}
}

void UBossGimmick_Swamp::OnPhaseEnd(ABossActor* Boss)
{
	if (!Boss) return;

	UWorld* World = Boss->GetWorld();
	if (!World) return;

	UGridManagerSubsystem* GridMgr = World->GetSubsystem<UGridManagerSubsystem>();
	if (!GridMgr) return;

	for (int32 i = ActiveSwamps.Num() - 1; i >= 0; --i)
	{
		ActiveSwamps[i].RemainingTurns--;
		if (ActiveSwamps[i].RemainingTurns <= 0)
		{
			if (AGridActor* Grid = GridMgr->GetGridActor(ActiveSwamps[i].GridPoint))
			{
				Grid->ClearSwamp();
				Grid->InitColor();
			}
			ActiveSwamps.RemoveAt(i);
		}
	}
}

bool UBossGimmick_Swamp::IsSwampCell(const FGridPoint& Cell) const
{
	for (const FSwampCell& S : ActiveSwamps)
	{
		if (S.GridPoint.GridX == Cell.GridX && S.GridPoint.GridY == Cell.GridY)
			return true;
	}
	return false;
}

void UBossGimmick_Swamp::InstallSwamp(ABossActor* Boss, const TArray<FGridPoint>& Cells)
{
	if (!Boss) return;

	UWorld* World = Boss->GetWorld();
	if (!World) return;

	UGridManagerSubsystem* GridMgr = World->GetSubsystem<UGridManagerSubsystem>();
	if (!GridMgr) return;

	const int32 RemainingTurns = GimmickData.ParamIntA > 0 ? GimmickData.ParamIntA : 2;
	const int32 DebuffAmount   = GimmickData.ParamIntB > 0 ? GimmickData.ParamIntB : 1;

	const TArray<EWeaponClass> SwampTypes = { EWeaponClass::Deal, EWeaponClass::Tank, EWeaponClass::Heal };
	const EWeaponClass PickedType = SwampTypes[FMath::RandRange(0, SwampTypes.Num() - 1)];
	const FLinearColor SwampColor = GetSwampColor(PickedType);

	for (const FGridPoint& Cell : Cells)
	{
		AGridActor* Grid = GridMgr->GetGridActor(Cell);
		if (!Grid) continue;

		Grid->SetSwamp(RemainingTurns, DebuffAmount, PickedType, SwampColor);
		Grid->ClearBossAttackFlag();
		Grid->InitColor();

		bool bAlreadyTracked = false;
		for (FSwampCell& S : ActiveSwamps)
		{
			if (S.GridPoint.GridX == Cell.GridX && S.GridPoint.GridY == Cell.GridY)
			{
				S.RemainingTurns = RemainingTurns;
				S.SwampType      = PickedType;
				bAlreadyTracked  = true;
				break;
			}
		}
		if (!bAlreadyTracked)
		{
			FSwampCell NewSwamp;
			NewSwamp.GridPoint      = Cell;
			NewSwamp.RemainingTurns = RemainingTurns;
			NewSwamp.SwampType      = PickedType;
			ActiveSwamps.Add(NewSwamp);
		}
	}
}
