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
    UseAnchorCell, // AnchorCell�� �������� ����� (Cross/Circle/Single, Row/Column �������� ����)
    UseIndex       // Index�� ���� ���� (Column/Row ��ü��, Cone center index ��)
};

USTRUCT(BlueprintType)
struct FAttackAreaSpec
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    EAttackAreaPattern Pattern = EAttackAreaPattern::Column;

    // Column�̸� Col, Row�� Row, Cone�̸� CenterIndex(Up/Down�̸� X, Left/Right�̸� Y)
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 Index = 0;

    // Cross/Circle/Single �� ������
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FGridPoint AnchorCell;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    EAreaAnchor AnchorMode = EAreaAnchor::UseIndex;

    // Cross: HalfX, Row/Column ������: HalfLen, Circle: Radius, Cone: HalfWidth0
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
DB.db에 item테이블에 item_name추가됐는데 이걸 datamanagersubsystem에서 loaditem 함수에 