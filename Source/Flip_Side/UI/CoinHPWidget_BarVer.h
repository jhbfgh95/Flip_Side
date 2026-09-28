#pragma once

#include "CoreMinimal.h"
#include "UI/W_CoinHPWidget.h"
#include "CoinHPWidget_BarVer.generated.h"

// HpImage의 공통 머테리얼에 연결: 고정 폭을 최대 HP만큼 나누고 왼쪽부터 비웁니다. 한 칸=HP 1입니다.
UCLASS()
class FLIP_SIDE_API UCoinHPWidget_BarVer : public UW_CoinHPWidget
{
	GENERATED_BODY()
protected:
	virtual bool UsesSegmentedBar() const override;
};
