#include "GridAreaBuilder.h"
#include "Math/UnrealMathUtility.h"

static FORCEINLINE void AddIfIn(TArray<FGridPoint>& Out, const FGridPoint& P, int32 W, int32 H)
{
    if (P.GridX < 0 || P.GridX >= W) return;
    if (P.GridY < 0 || P.GridY >= H) return;

    Out.Add(P);
}

static FORCEINLINE bool IsSameCell(const FGridPoint& A, const FGridPoint& B)
{
    return A.GridX == B.GridX && A.GridY == B.GridY;
}

void FGridAreaBuilder::BuildCells(const FAttackAreaSpec& Spec, int32 W, int32 H, TArray<FGridPoint>& OutCells)
{
    OutCells.Reset();

    auto Add = [&](int32 X, int32 Y)
        {
            FGridPoint P; P.GridX = X; P.GridY = Y;
            AddIfIn(OutCells, P, W, H);
        };

    // Anchor 결정
    const FGridPoint T =
        (Spec.AnchorMode == EAreaAnchor::UseAnchorCell)
        ? Spec.AnchorCell
        : FGridPoint{ Spec.Index, 0 };

    auto RemoveCellIfExists = [&](const FGridPoint& Cell)
        {
            OutCells.RemoveAll([&](const FGridPoint& P)
                {
                    return IsSameCell(P, Cell);
                });
        };

    switch (Spec.Pattern)
    {
    case EAttackAreaPattern::Column:
    {
        const int32 Col = Spec.Index;

        // 크기 조절: AnchorMode가 UseAnchorCell이면 AnchorCell.Y 중심으로 ParamA(HalfLen)만큼
        if (Spec.AnchorMode == EAreaAnchor::UseAnchorCell)
        {
            const int32 HalfLen = FMath::Max(0, Spec.ParamA);
            const int32 CY = Spec.AnchorCell.GridY;
            for (int32 Y = CY - HalfLen; Y <= CY + HalfLen; ++Y) Add(Col, Y);
        }
        else
        {
            // 전체 컬럼
            for (int32 Y = 0; Y < H; ++Y) Add(Col, Y);
        }
        break;
    }

    case EAttackAreaPattern::Row:
    {
        const int32 Row = Spec.Index;

        // 크기 조절: AnchorMode가 UseAnchorCell이면 AnchorCell.X 중심으로 ParamA(HalfLen)만큼
        if (Spec.AnchorMode == EAreaAnchor::UseAnchorCell)
        {
            const int32 HalfLen = FMath::Max(0, Spec.ParamA);
            const int32 CX = Spec.AnchorCell.GridX;
            for (int32 X = CX - HalfLen; X <= CX + HalfLen; ++X) Add(X, Row);
        }
        else
        {
            // 전체 로우
            for (int32 X = 0; X < W; ++X) Add(X, Row);
        }
        break;
    }
    case EAttackAreaPattern::RectFromCell:
    {
        // RectFromCell은 AnchorCell을 기준으로 "앞(=Side)" 방향으로 직사각형을 만든다.
        // ParamA = Width (가로폭)
        // ParamB = Depth (전방으로 뻗는 길이)

        const FGridPoint Base =
            (Spec.AnchorMode == EAreaAnchor::UseAnchorCell)
            ? Spec.AnchorCell
            : FGridPoint{ Spec.Index, 0 };

        const int32 Width = FMath::Max(1, Spec.ParamA);
        const int32 Depth = FMath::Max(1, Spec.ParamB);

        const int32 HalfL = (Width - 1) / 2;
        const int32 HalfR = (Width / 2);

        if (Spec.Side == EAreaSide::Up)
        {
            for (int32 d = 1; d <= Depth; ++d) 
            {
                const int32 Y = Base.GridY + d;
                for (int32 dx = -HalfL; dx <= HalfR; ++dx)
                {
                    Add(Base.GridX + dx, Y);
                }
            }
        }
        else if (Spec.Side == EAreaSide::Down)
        {
            for (int32 d = 1; d <= Depth; ++d)
            {
                const int32 Y = Base.GridY - d;
                for (int32 dx = -HalfL; dx <= HalfR; ++dx)
                {
                    Add(Base.GridX + dx, Y);
                }
            }
        }
        else if (Spec.Side == EAreaSide::Left)
        {
            for (int32 d = 1; d <= Depth; ++d)
            {
                const int32 X = Base.GridX - d;
                for (int32 dy = -HalfL; dy <= HalfR; ++dy)
                {
                    Add(X, Base.GridY + dy);
                }
            }
        }
        else 
        {
            for (int32 d = 1; d <= Depth; ++d)
            {
                const int32 X = Base.GridX + d;
                for (int32 dy = -HalfL; dy <= HalfR; ++dy)
                {
                    Add(X, Base.GridY + dy);
                }
            }
        }

        break;
    }
    case EAttackAreaPattern::CrossOnCell:
    {
        // 십자가 크기 조절: ParamA=HalfX, ParamB=HalfY
        const int32 HalfX = FMath::Max(0, Spec.ParamA);
        const int32 HalfY = FMath::Max(0, Spec.ParamB);

        Add(T.GridX, T.GridY);

        for (int32 dx = 1; dx <= HalfX; ++dx) { Add(T.GridX + dx, T.GridY); Add(T.GridX - dx, T.GridY); }
        for (int32 dy = 1; dy <= HalfY; ++dy) { Add(T.GridX, T.GridY + dy); Add(T.GridX, T.GridY - dy); }
        break;
    }

    case EAttackAreaPattern::CircleOnCell:
    {
        const int32 R = FMath::Max(0, Spec.ParamA);
        for (int32 dy = -R; dy <= R; ++dy)
            for (int32 dx = -R; dx <= R; ++dx)
                Add(T.GridX + dx, T.GridY + dy);
        if (Spec.Flags == 1)
        {
            RemoveCellIfExists(T);
        }
        break;
    }

    case EAttackAreaPattern::DiagonalMain:
    {
        // 전체 대각선
        const int32 N = FMath::Min(W, H);
        for (int32 i = 0; i < N; ++i) Add(i, i);
        break;
    }

    case EAttackAreaPattern::DiagonalAnti:
    {
        const int32 N = FMath::Min(W, H);
        for (int32 i = 0; i < N; ++i) Add((W - 1) - i, i);
        break;
    }

    case EAttackAreaPattern::Border:
    {
        for (int32 X = 0; X < W; ++X) { Add(X, 0); Add(X, H - 1); }
        for (int32 Y = 1; Y < H - 1; ++Y) { Add(0, Y); Add(W - 1, Y); }
        break;
    }

    case EAttackAreaPattern::ConeFromSide:
    {
        const int32 HalfWidth0 = Spec.ParamA; // 시작 폭
        const int32 Depth = Spec.ParamB; // 깊이

        if (Spec.Side == EAreaSide::Up)
        {
            const int32 CenterX = Spec.Index;
            for (int32 d = 0; d < Depth && d < H; ++d)
            {
                const int32 hw = HalfWidth0 + d;
                const int32 Y = d;
                for (int32 X = CenterX - hw; X <= CenterX + hw; ++X) Add(X, Y);
            }
        }
        else if (Spec.Side == EAreaSide::Down)
        {
            const int32 CenterX = Spec.Index;
            for (int32 d = 0; d < Depth && d < H; ++d)
            {
                const int32 hw = HalfWidth0 + d;
                const int32 Y = (H - 1) - d;
                for (int32 X = CenterX - hw; X <= CenterX + hw; ++X) Add(X, Y);
            }
        }
        else if (Spec.Side == EAreaSide::Left)
        {
            const int32 CenterY = Spec.Index;
            for (int32 d = 0; d < Depth && d < W; ++d)
            {
                const int32 hw = HalfWidth0 + d;
                const int32 X = d;
                for (int32 Y = CenterY - hw; Y <= CenterY + hw; ++Y) Add(X, Y);
            }
        }
        else // Right
        {
            const int32 CenterY = Spec.Index;
            for (int32 d = 0; d < Depth && d < W; ++d)
            {
                const int32 hw = HalfWidth0 + d;
                const int32 X = (W - 1) - d;
                for (int32 Y = CenterY - hw; Y <= CenterY + hw; ++Y) Add(X, Y);
            }
        }
        break;
    }

    default:
        break;
    }
}
