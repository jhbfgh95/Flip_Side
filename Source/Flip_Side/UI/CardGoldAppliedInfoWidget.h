#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DataTypes/CardTypes.h"
#include "Styling/SlateTypes.h"
#include "CardGoldAppliedInfoWidget.generated.h"

struct FTextRunInfo;

/** 배틀의 더 많은 황금 카드에 현재 적용된 효과를 표시합니다. */
UCLASS()
class FLIP_SIDE_API UCardGoldAppliedInfoWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable, Category = "Card|Applied Info")
	void SetAppliedEffects(const FCoinCardModifiers& InModifiers, bool bInHasAppliedEffects);

	TSharedPtr<SWidget> CreateStatDisplay(const FTextRunInfo& Run, const FTextBlockStyle& Style);
protected:
	virtual void NativeConstruct() override;

	// 배치와 스타일은 WBP에서 설정하고, 코드에서는 표시 내용만 갱신합니다.
	UPROPERTY(BlueprintReadOnly, Category = "Card|Applied Info", meta = (BindWidgetOptional))
	TObjectPtr<class UTextBlock> AppliedEffectTitle;
	UPROPERTY(BlueprintReadOnly, Category = "Card|Applied Info", meta = (BindWidgetOptional))
	TObjectPtr<class URichTextBlock> AppliedEffectsText;
	// 한 줄에 표시하는 스탯 항목 사이의 간격입니다.
	UPROPERTY(EditDefaultsOnly, Category = "Card|Applied Info|Style", meta = (ClampMin = "0.0"))
	float StatSpacing = 12.f;
	UPROPERTY(EditDefaultsOnly, Category = "Card|Applied Info|Style", meta = (ClampMin = "1.0"))
	FVector2D InlineIconSize = FVector2D(24.f, 24.f);
	UPROPERTY(EditDefaultsOnly, Category = "Card|Applied Info|Style", meta = (ClampMin = "0.0"))
	float IconValueSpacing = 4.f;
	UPROPERTY(EditDefaultsOnly, Category = "Card|Applied Info|Style", meta = (ClampMin = "0.0"))
	float IconNameSpacing = 4.f;
private:
	void RefreshText();
	FCoinCardModifiers AppliedModifiers;
	bool bHasAppliedEffects = false;
};
