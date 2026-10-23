#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DataTypes/CoinDataTypes.h"
#include "BuffsInfoPopup.generated.h"

struct FTextRunInfo;
struct FTextBlockStyle;

/** HUD PopupLayer에서 재사용하는 개별 버프/디버프 호버 정보입니다. */
UCLASS()
class FLIP_SIDE_API UBuffsInfoPopup : public UUserWidget
{
	GENERATED_BODY()
public:
	void SetBuffInfo(const FBattleStatusEffectViewData& InData);
	TSharedPtr<SWidget> CreateStatDisplay(const FTextRunInfo& Run, const FTextBlockStyle& Style);

protected:
	virtual void NativeOnInitialized() override;
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<class UTextBlock> BuffNameText;
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<class URichTextBlock> BuffDescriptionText;
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<class UImage> BuffIconImage;
	UPROPERTY(EditDefaultsOnly, Category = "Buff Info|Description")
	FVector2D InlineIconSize = FVector2D(24.f, 24.f);
	UPROPERTY(EditDefaultsOnly, Category = "Buff Info|Description")
	float IconNameSpacing = 4.f;
};
