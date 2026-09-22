#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DataTypes/CoinDescriptionDataTypes.h"
#include "Styling/SlateTypes.h"
#include "CoinDescriptionSectionWidget.generated.h"

USTRUCT(BlueprintType)
struct FCoinDescriptionInlineStyle
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Coin Description")
	bool bShowValue = true;
};

UCLASS()
class FLIP_SIDE_API UCoinDescriptionSectionWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable, Category = "Coin Description")
	void SetSectionData(const FCoinDescriptionSectionData& InData);
	void SetDetailed(bool bInDetailed);
	TSharedPtr<SWidget> CreateInlineDisplay(FName Key, const FTextBlockStyle& Style);
	TSharedPtr<SWidget> CreateExpressionDisplay(const FString& Expression, const FTextBlockStyle& Style);
protected:
	virtual void NativeConstruct() override;
	// 토큰 BP 없이 두 RichTextBlock에서 직접 아이콘과 문자를 표시합니다.
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<class URichTextBlock> HeaderRichText;
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<class URichTextBlock> DescriptionRichText;
	UPROPERTY(EditDefaultsOnly, Category = "Coin Description|Style", meta = (GetKeyOptions = "GetInlineStyleKeys"))
	TMap<FName, FCoinDescriptionInlineStyle> InlineStyles;
	UPROPERTY(EditDefaultsOnly, Category = "Coin Description|Style", meta = (ClampMin = "1.0"))
	FVector2D InlineIconSize = FVector2D(24.0f, 24.0f);
	// 일반: 아이콘 → 수치, Shift: 수치 → [아이콘] 사이의 Slate 간격입니다.
	UPROPERTY(EditDefaultsOnly, Category = "Coin Description|Style", meta = (ClampMin = "0.0", UIMin = "0.0"))
	float IconValueSpacing = 4.0f;
	// Shift 상세 표시의 [아이콘 이름] 내부 간격을 설명 WBP에서 조절합니다.
	UPROPERTY(EditDefaultsOnly, Category = "Coin Description|Style", meta = (ClampMin = "0.0", UIMin = "0.0"))
	float IconNameSpacing = 4.0f;
	// RichTextBlock에서 선택한 폰트의 Bold 서체 이름입니다.
	UPROPERTY(EditDefaultsOnly, Category = "Coin Description|Style")
	FName DetailedTypeface = TEXT("Bold");
	UFUNCTION()
	TArray<FString> GetInlineStyleKeys() const;
	UPROPERTY(BlueprintReadOnly, Category = "Coin Description")
	FCoinDescriptionSectionData SectionData;
	UFUNCTION(BlueprintImplementableEvent, Category = "Coin Description")
	void OnSectionDataChanged(const FCoinDescriptionSectionData& InData);
private:
	FLinearColor GetTokenColor(FName Key) const;
	TSharedRef<SWidget> CreateInlineIcon(FName Key, const FTextBlockStyle& Style);
	void RefreshText();
	bool bDetailed = false;
};
