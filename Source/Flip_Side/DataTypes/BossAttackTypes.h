// BossAttackTypes.h
#pragma once

#include "CoreMinimal.h"
#include "GridTypes.h"
#include "BossAttackTypes.generated.h"

UENUM(BlueprintType)
enum class EBossAttackPattern : uint8
{
    Column          UMETA(DisplayName = "Column"),
    Row             UMETA(DisplayName = "Row"),
    CrossOnCell     UMETA(DisplayName = "Cross On Cell"),
    CircleOnCell    UMETA(DisplayName = "Circle On Cell"),
    DiagonalMain    UMETA(DisplayName = "Diagonal Main"),
    DiagonalAnti    UMETA(DisplayName = "Diagonal Anti"),
    ConeFromSide    UMETA(DisplayName = "Cone From Side"),
    Border          UMETA(DisplayName = "Border"),
    SingleTargetCoin UMETA(DisplayName = "Single Target Coin")
};

UENUM(BlueprintType)
enum class EBossSide : uint8
{
    Up, Down, Left, Right
};

USTRUCT(BlueprintType)
struct FBossAttackSpec
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    EBossAttackPattern Pattern = EBossAttackPattern::Column;

    // Column이면 Col, Row면 Row, Cone면 CenterIndex (Up/Down이면 X, Left/Right이면 Y)
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 Index = 0;

    // Circle: Radius, Cone: 시작 HalfWidth
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 ParamA = 1;

    // Cone: Depth
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 ParamB = 3;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    EBossSide Side = EBossSide::Up;

    // 타겟 기반 공격이면 TargetCell 사용 (예: 플레이어 코인 칸)
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool bUseTargetCell = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FGridPoint TargetCell;
};
