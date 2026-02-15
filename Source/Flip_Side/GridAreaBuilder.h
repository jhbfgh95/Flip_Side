#pragma once

#include "CoreMinimal.h"
#include "AttackAreaTypes.h"

struct FGridAreaBuilder
{
    static void BuildCells(
        const FAttackAreaSpec& Spec,
        int32 GridW,
        int32 GridH,
        TArray<FGridPoint>& OutCells);
};
