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
	UCardGoldAppliedInfoWidget(const FObjectInitializer& ObjectInitializer);

	UFUNCTION(BlueprintCallable, Category = "Card|Applied Info")
	void SetAppliedEffects(const FCoinCardModifiers& InModifiers, bool bInHasAppliedEffects);

	TSharedPtr<SWidget> CreateStatDisplay(const FTextRunInfo& Run, const FTextBlockStyle& Style);
protected:
	virtual void NativeOnInitialized() override;
	virtual void NativeConstruct() override;

	UPROPERTY(BlueprintReadOnly, Category = "Card|Applied Info", meta = (BindWidgetOptional))
	TObjectPtr<class UTextBlock> AppliedEffectTitle;
	UPROPERTY(BlueprintReadOnly, Category = "Card|Applied Info", meta = (BindWidgetOptional))
	TObjectPtr<class URichTextBlock> AppliedEffectsText;
	UPROPERTY(EditDefaultsOnly, Category = "Card|Applied Info|Style")
	FTextBlockStyle AppliedTextStyle;
	UPROPERTY(EditDefaultsOnly, Category = "Card|Applied Info|Style", meta = (ClampMin = "1.0"))
	FVector2D InlineIconSize = FVector2D(24.f, 24.f);
	UPROPERTY(EditDefaultsOnly, Category = "Card|Applied Info|Style", meta = (ClampMin = "0.0"))
	float IconValueSpacing = 4.f;
	UPROPERTY(EditDefaultsOnly, Category = "Card|Applied Info|Style", meta = (ClampMin = "0.0"))
	float IconNameSpacing = 4.f;
	UPROPERTY(EditDefaultsOnly, Category = "Card|Applied Info|Style")
	FName DetailedTypeface = TEXT("Bold");
private:
	void RefreshText();
	FCoinCardModifiers AppliedModifiers;
	bool bHasAppliedEffects = false;
};
