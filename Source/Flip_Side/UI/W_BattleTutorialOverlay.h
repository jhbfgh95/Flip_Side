#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "BattleTutorialSequenceData.h"
#include "W_BattleTutorialOverlay.generated.h"

class UCanvasPanel;
class UBorder;
class URichTextBlock;
class UDataTable;
class UTutorialClickHintWidget;

DECLARE_MULTICAST_DELEGATE(FOnBattleTutorialOverlayClicked);

UCLASS()
class FLIP_SIDE_API UW_BattleTutorialOverlay : public UUserWidget
{
	GENERATED_BODY()
public:
	UW_BattleTutorialOverlay(const FObjectInitializer& ObjectInitializer);
	void ShowStep(const FBattleTutorialStep& Step);
	void SetWidgetTarget(UWidget* Widget);
	void SetActorTarget(AActor* Actor, const FVector2D& HoleSize);
	FOnBattleTutorialOverlayClicked OnBattleTutorialOverlayClicked;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Tutorial|Layout")
	TArray<FVector2D> ExplanationPositions = {FVector2D(0.f, 300.f), FVector2D(0.f, -300.f), FVector2D(-400.f, 0.f)};
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Tutorial|Layout")
	float ExplanationWidth = 620.f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Tutorial|Appearance", meta = (ClampMin = "0", ClampMax = "1"))
	float DimOpacity = 0.8f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Tutorial|Appearance")
	FLinearColor FrameColor = FLinearColor(1.f, 0.2f, 0.15f, 1.f);
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Tutorial|Appearance")
	float FrameThickness = 3.f;
	// 4:3, 1:1, 3:4 순서입니다. 비어 있으면 코드 테두리를 표시합니다.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Tutorial|Appearance")
	TArray<FSlateBrush> FrameBrushes;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Tutorial|Text")
	TObjectPtr<UDataTable> RichTextStyleSet;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Tutorial|Hint")
	TSubclassOf<UTutorialClickHintWidget> ClickHintWidgetClass;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Tutorial|Hint")
	FVector2D ClickHintOffset = FVector2D(0.f, -20.f);
protected:
	virtual void NativeOnInitialized() override;
	virtual void NativeTick(const FGeometry& Geometry, float DeltaTime) override;
	virtual FReply NativeOnMouseButtonDown(const FGeometry& Geometry, const FPointerEvent& Event) override;
	virtual int32 NativePaint(const FPaintArgs& Args, const FGeometry& Geometry, const FSlateRect& CullingRect,
		FSlateWindowElementList& Elements, int32 Layer, const FWidgetStyle& Style, bool bParentEnabled) const override;
private:
	UPROPERTY(Transient) TObjectPtr<UCanvasPanel> TutorialCanvas;
	UPROPERTY(Transient) TArray<TObjectPtr<UBorder>> DimRegions;
	UPROPERTY(Transient) TObjectPtr<UBorder> HoleInput;
	UPROPERTY(Transient) TObjectPtr<UBorder> ExplanationRoot;
	UPROPERTY(Transient) TObjectPtr<URichTextBlock> ExplanationText;
	UPROPERTY(Transient) TObjectPtr<UTutorialClickHintWidget> ClickHint;
	TWeakObjectPtr<UWidget> WidgetTarget;
	TWeakObjectPtr<AActor> ActorTarget;
	FVector2D ActorHoleSize = FVector2D(0.1f, 0.1f);
	FBattleTutorialStep CurrentStep;
	FVector2D HighlightPosition = FVector2D::ZeroVector;
	FVector2D HighlightSize = FVector2D::ZeroVector;
	bool bHasHighlight = false;
	int32 FrameIndex = 0;
	void Place(UWidget* Widget, const FVector2D& Position, const FVector2D& Size);
};
