#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DataTypes/KeywordDataTypes.h"
#include "Styling/SlateTypes.h"
#include "TutorialDescriptionWidget.generated.h"

class UBorder;
class UDataTable;
class URichTextBlock;
class USizeBox;

UCLASS()
class FLIP_SIDE_API UTutorialDescriptionWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable, Category = "Tutorial")
	void SetDescriptionText(const FText& Text);

	UFUNCTION(BlueprintCallable, Category = "Tutorial")
	void SetDescriptionSize(const FVector2D& Size);

	void ConfigureFallbackAppearance(float Width, UDataTable* StyleSet);
	TSharedPtr<SWidget> CreateKeywordDisplay(FName Code, const FTextBlockStyle& Style);
protected:
	virtual void NativeOnInitialized() override;

	UPROPERTY(BlueprintReadOnly, Category = "Tutorial|Bindings", meta = (BindWidgetOptional))
	TObjectPtr<URichTextBlock> DescriptionText;
	UPROPERTY(BlueprintReadOnly, Category = "Tutorial|Bindings", meta = (BindWidgetOptional))
	TObjectPtr<UBorder> DescriptionBackground;
	UPROPERTY(BlueprintReadOnly, Category = "Tutorial|Bindings", meta = (BindWidgetOptional))
	TObjectPtr<USizeBox> DescriptionSizeBox;
	UPROPERTY(EditDefaultsOnly, Category = "Tutorial|Keywords", meta = (ClampMin = "1.0"))
	FVector2D KeywordIconSize = FVector2D(24.f, 24.f);
	UPROPERTY(EditDefaultsOnly, Category = "Tutorial|Keywords", meta = (ClampMin = "0.0"))
	float KeywordIconNameSpacing = 4.f;
private:
	UPROPERTY(Transient)
	TMap<FName, FKeywordDefinitionData> KeywordDefinitions;
	bool bUsingFallbackLayout = false;
	bool bDefaultSizeCached = false;
	bool bDefaultWidthOverride = false;
	bool bDefaultHeightOverride = false;
	FVector2D DefaultSize = FVector2D::ZeroVector;
	bool bDefaultAutoWrapText = false;
	float DefaultWrapTextAt = 0.f;
};
