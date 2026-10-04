#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
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

	void ConfigureFallbackAppearance(float Width, UDataTable* StyleSet);
protected:
	virtual void NativeOnInitialized() override;

	UPROPERTY(BlueprintReadOnly, Category = "Tutorial|Bindings", meta = (BindWidgetOptional))
	TObjectPtr<URichTextBlock> DescriptionText;
	UPROPERTY(BlueprintReadOnly, Category = "Tutorial|Bindings", meta = (BindWidgetOptional))
	TObjectPtr<UBorder> DescriptionBackground;
	UPROPERTY(BlueprintReadOnly, Category = "Tutorial|Bindings", meta = (BindWidgetOptional))
	TObjectPtr<USizeBox> DescriptionSizeBox;
private:
	bool bUsingFallbackLayout = false;
};
