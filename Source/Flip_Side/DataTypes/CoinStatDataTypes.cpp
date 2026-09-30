#include "DataTypes/CoinStatDataTypes.h"

#include "DataTypes/WeaponDataTypes.h"

int32 GetWeaponAreaRange(const FAttackAreaSpec& Spec)
{
	switch (Spec.Pattern)
	{
	case EAttackAreaPattern::SingleCell:
		return FMath::Max(0, Spec.AnchorMode == EAreaAnchor::UseAnchorCell ? Spec.ParamA : Spec.ParamB);
	case EAttackAreaPattern::RectFromCell:
	case EAttackAreaPattern::ConeFromSide:
		return FMath::Max(0, Spec.ParamB);
	case EAttackAreaPattern::CircleOnCell:
		return FMath::Max(0, Spec.ParamA);
	case EAttackAreaPattern::CrossOnCell:
		return FMath::Max(0, FMath::Max(Spec.ParamA, Spec.ParamB));
	case EAttackAreaPattern::Column:
	case EAttackAreaPattern::Row:
		return Spec.AnchorMode == EAreaAnchor::UseAnchorCell ? FMath::Max(0, Spec.ParamA) : 0;
	default:
		return 0;
	}
}

FAttackAreaSpecModifier MakeWeaponRangeModifier(const FAttackAreaSpec& Spec, int32 RangeAdd)
{
	FAttackAreaSpecModifier Modifier;
	switch (Spec.Pattern)
	{
	case EAttackAreaPattern::SingleCell:
		if (Spec.AnchorMode == EAreaAnchor::UseAnchorCell) Modifier.ParamA = RangeAdd;
		else Modifier.ParamB = RangeAdd;
		break;
	case EAttackAreaPattern::RectFromCell:
	case EAttackAreaPattern::ConeFromSide:
		Modifier.ParamB = RangeAdd;
		break;
	case EAttackAreaPattern::CircleOnCell:
		Modifier.ParamA = RangeAdd;
		break;
	case EAttackAreaPattern::CrossOnCell:
		Modifier.ParamA = RangeAdd;
		Modifier.ParamB = RangeAdd;
		break;
	case EAttackAreaPattern::Column:
	case EAttackAreaPattern::Row:
		if (Spec.AnchorMode == EAreaAnchor::UseAnchorCell) Modifier.ParamA = RangeAdd;
		break;
	default:
		break;
	}
	return Modifier;
}

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
