#include "DataTypes/CoinStatDataTypes.h"

#include "DataTypes/WeaponDataTypes.h"

FWeaponFaceStats BuildWeaponFaceStatsFromDefinition(const FFaceData& WeaponDefinition)
{
	FWeaponFaceStats FaceStats;
	FaceStats.WeaponID = WeaponDefinition.WeaponID;
	FaceStats.BaseNumericStats.AttackPoint = FMath::Max(0, WeaponDefinition.AttackPoint);
	FaceStats.BaseNumericStats.WeaponPoint = FMath::Max(0, WeaponDefinition.BehaviorPoint);
	FaceStats.BaseNumericStats.WeaponCnt = FMath::Max(0, WeaponDefinition.Count);

	FaceStats.AttackAreaSpec = WeaponDefinition.AttackAreaSpec;
	FaceStats.AttackAreaSpec.AnchorCell = FGridPoint(
		WeaponDefinition.AttackAnchorOffset.X,
		WeaponDefinition.AttackAnchorOffset.Y);
	FaceStats.AttackAreaSpec.Flags = WeaponDefinition.AttackAreaFlags;

	FaceStats.AbilityAreaSpec = WeaponDefinition.AbilityAreaSpec;
	FaceStats.bHasAbilityArea = WeaponDefinition.bHasAbilityArea;
	return FaceStats;
}
