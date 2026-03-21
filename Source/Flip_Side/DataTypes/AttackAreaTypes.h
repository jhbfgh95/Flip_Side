#pragma once

#include "CoreMinimal.h"
#include "GridTypes.h"
#include "AttackAreaTypes.generated.h"


UENUM(BlueprintType)
enum class EAttackAreaPattern : uint8
{
    Column          UMETA(DisplayName = "Column"),
    Row             UMETA(DisplayName = "Row"),
    CrossOnCell     UMETA(DisplayName = "Cross On Cell"),
    CircleOnCell    UMETA(DisplayName = "Circle On Cell"),
    DiagonalMain    UMETA(DisplayName = "Diagonal Main"),  
    DiagonalAnti    UMETA(DisplayName = "Diagonal Anti"),   
    Border          UMETA(DisplayName = "Border"),
    ConeFromSide    UMETA(DisplayName = "Cone From Side"),
    SingleCell      UMETA(DisplayName = "Single Cell"),
    RectFromCell    UMETA(DisplayName = "Rect From Cell")
};

UENUM(BlueprintType)
enum class EAreaSide : uint8
{
    Up, Down, Left, Right
};

UENUM(BlueprintType)
enum class EAreaAnchor : uint8
{
    UseAnchorCell, // AnchorCell을 기준으로 만든다 (Cross/Circle/Single, Row/Column 제한형도 가능)
    UseIndex       // Index만 쓰는 패턴 (Column/Row 전체형, Cone center index 등)
};

USTRUCT(BlueprintType)
struct FAttackAreaSpec
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    EAttackAreaPattern Pattern = EAttackAreaPattern::Column;

    // Column이면 Col, Row면 Row, Cone이면 CenterIndex(Up/Down이면 X, Left/Right이면 Y)
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 Index = 0;

    // Cross/Circle/Single 등 기준점
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FGridPoint AnchorCell;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    EAreaAnchor AnchorMode = EAreaAnchor::UseIndex;

    // Cross: HalfX, Row/Column 제한형: HalfLen, Circle: Radius, Cone: HalfWidth0
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 ParamA = 1;

    // Cross: HalfY, Cone: Depth
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 ParamB = 1;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    EAreaSide Side = EAreaSide::Up;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 Flags = 0;
};
