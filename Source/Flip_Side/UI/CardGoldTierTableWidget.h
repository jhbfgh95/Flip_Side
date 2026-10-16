#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Styling/SlateTypes.h"
#include "CardGoldTierTableWidget.generated.h"

struct FTextRunInfo;

/** 더 많은 황금의 골드 구간별 스탯 증가량을 표시합니다. */
UCLASS()
class FLIP_SIDE_API UCardGoldTierTableWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	UCardGoldTierTableWidget(const FObjectInitializer& ObjectInitializer);

	/** DB의 카드 ID 4에서 GoldTiers를 다시 읽어 표를 갱신합니다. */
	UFUNCTION(BlueprintCallable, Category = "Card|Gold Tier Table")
	void RefreshTable();

	TSharedPtr<SWidget> CreateStatDisplay(const FTextRunInfo& Run, const FTextBlockStyle& Style);
protected:
	virtual void NativeOnInitialized() override;
	virtual void NativeConstruct() override;

	// WBP에서 직접 배치하려면 이 이름의 GridPanel을 사용합니다. 빈 WBP에서는 자동 생성합니다.
	UPROPERTY(BlueprintReadOnly, Category = "Card|Gold Tier Table", meta = (BindWidgetOptional))
	TObjectPtr<class UGridPanel> GoldTierGrid;
	UPROPERTY(EditDefaultsOnly, Category = "Card|Gold Tier Table|Style")
	FTextBlockStyle HeaderTextStyle;
	UPROPERTY(EditDefaultsOnly, Category = "Card|Gold Tier Table|Style")
	FTextBlockStyle ValueTextStyle;
	UPROPERTY(EditDefaultsOnly, Category = "Card|Gold Tier Table|Style")
	FMargin CellPadding = FMargin(12.f, 8.f);
	UPROPERTY(EditDefaultsOnly, Category = "Card|Gold Tier Table|Style", meta = (ClampMin = "1.0"))
	FVector2D InlineIconSize = FVector2D(24.f, 24.f);
	UPROPERTY(EditDefaultsOnly, Category = "Card|Gold Tier Table|Style", meta = (ClampMin = "0.0"))
	float IconNameSpacing = 4.f;
	UPROPERTY(EditDefaultsOnly, Category = "Card|Gold Tier Table|Style", meta = (ClampMin = "0.0"))
	float IconValueSpacing = 4.f;
	UPROPERTY(EditDefaultsOnly, Category = "Card|Gold Tier Table|Style")
	FName DetailedTypeface = TEXT("Bold");
private:
	// 배치된 셀은 텍스트만 갱신하며, 해당 위치가 비어 있을 때만 생성합니다.
	void AddCell(int32 Row, int32 Column, const FText& Text);
};
