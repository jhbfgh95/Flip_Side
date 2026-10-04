#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "TutorialClickHintWidget.generated.h"

UCLASS()
class FLIP_SIDE_API UTutorialClickHintWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	UTutorialClickHintWidget(const FObjectInitializer& ObjectInitializer);
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tutorial")
	FText Prompt = FText::FromString(TEXT("Click!"));
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tutorial")
	FLinearColor PromptColor = FLinearColor::White;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tutorial")
	TObjectPtr<class UTexture2D> IconTexture;
protected:
	virtual void NativeOnInitialized() override;
	virtual void NativePreConstruct() override;
	UPROPERTY(meta = (BindWidgetOptional)) TObjectPtr<class UTextBlock> ClickPrompt;
	UPROPERTY(meta = (BindWidgetOptional)) TObjectPtr<class UImage> MouseIcon;
};
