#include "BossGimmick_Swamp.h"
#include "BossActor.h"
#include "GridActor.h"
#include "GridManagerSubsystem.h"
#include "BossManagerSubsystem.h"
#include "BossPatternBase.h"
#include "Actors/Others/Base_OtherActor.h"

void UBossGimmick_Swamp::OnPatternExecute(
	ABossActor* Boss,
	const TArray<FGridPoint>& LockedCells,
	const TArray<ACoinActor*>& LockedTargets,
	const TArray<ABase_OtherActor*>& LockedOthers)
{
	if (!Boss) return;

	UWorld* World = Boss->GetWorld();
	if (!World) return;

	UBossManagerSubsystem* BossMgr = World->GetSubsystem<UBossManagerSubsystem>();
	if (!BossMgr) return;

	UBossPatternBase* Pattern = BossMgr->GetCurrentTurnPattern();
	const int32 PatternIndex = BossMgr->GetCurrentTurnPatternIndex();
	if (!Pattern || !Pattern->PatternData.IsValidIndex(PatternIndex)) return;
	if (!Pattern->PatternData[PatternIndex].bNoDamage) return;

	InstallSwamp(Boss, LockedCells);
}

void UBossGimmick_Swamp::OnTurnEnd(ABossActor* Boss)
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
	const FLinearColor SwampColor = FLinearColor(0.1f, 0.6f, 0.1f, 1.f);

	for (const FGridPoint& Cell : Cells)
	{
		AGridActor* Grid = GridMgr->GetGridActor(Cell);
		if (!Grid) continue;

		Grid->SetSwamp(RemainingTurns, DebuffAmount, EWeaponClass::Deal, SwampColor);
		Grid->InitColor();

		bool bAlreadyTracked = false;
		for (FSwampCell& S : ActiveSwamps)
		{
			if (S.GridPoint.GridX == Cell.GridX && S.GridPoint.GridY == Cell.GridY)
			{
				S.RemainingTurns = RemainingTurns;
				bAlreadyTracked = true;
				break;
			}
		}
		if (!bAlreadyTracked)
		{
			FSwampCell NewSwamp;
			NewSwamp.GridPoint = Cell;
			NewSwamp.RemainingTurns = RemainingTurns;
			ActiveSwamps.Add(NewSwamp);
		}

		UE_LOG(LogTemp, Warning, TEXT("[Swamp] 설치 (%d,%d) 남은턴=%d"), Cell.GridX, Cell.GridY, RemainingTurns);
	}
}
