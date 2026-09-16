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
	TObjectPtr<class UTexture2D> IconTexture = nullptr;
	// TODO(CoinDescription-DB-Style): DB 색상 API가 준비되면 이 값을 읽는 경로만 교체합니다.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Coin Description")
	FLinearColor DetailedTextColor = FLinearColor::White;
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
	void RefreshText();
	bool bDetailed = false;
};
