#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "BattleTutorialSequenceData.h"
#include "W_BattleTutorialOverlay.generated.h"

class UCanvasPanel;
class UBorder;
class UDataTable;
class UTutorialClickHintWidget;
class UTutorialDescriptionWidget;

USTRUCT(BlueprintType)
struct FBattleTutorialExplanationLayout
{
	GENERATED_BODY()

	FBattleTutorialExplanationLayout() = default;
	explicit FBattleTutorialExplanationLayout(const FVector2D& InPosition) : Position(InPosition) {}

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Tutorial|Layout", meta = (ToolTip = "화면 중앙 앵커 기준 설명창 위치입니다. X: 좌우, Y: 상하."))
	FVector2D Position = FVector2D::ZeroVector;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Tutorial|Layout", meta = (ClampMin = "0", ToolTip = "X: 설명창 폭, Y: 높이. 0,0은 설명 위젯 BP의 크기를 유지합니다. X만 지정하면 높이는 내용에 맞춥니다."))
	FVector2D Size = FVector2D::ZeroVector;
};

DECLARE_MULTICAST_DELEGATE(FOnBattleTutorialOverlayClicked);

UCLASS()
class FLIP_SIDE_API UW_BattleTutorialOverlay : public UUserWidget
{
	GENERATED_BODY()
public:
	UW_BattleTutorialOverlay(const FObjectInitializer& ObjectInitializer);
	virtual void PostLoad() override;
	void ShowStep(const FBattleTutorialStep& Step);
	void SetWidgetTarget(UWidget* Widget);
	void SetActorTarget(AActor* Actor, const FVector2D& HoleSize);
	void SetActorTargets(const TArray<AActor*>& Actors);
	void SetTransitionBusy(bool bBusy);
	void SetAdvancePending(bool bPending);
	FOnBattleTutorialOverlayClicked OnBattleTutorialOverlayClicked;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Tutorial|Layout")
	TArray<FBattleTutorialExplanationLayout> ExplanationLayouts;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Tutorial|Description")
	TSoftClassPtr<UTutorialDescriptionWidget> DescriptionWidgetClass;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Tutorial|Layout", meta = (AdvancedDisplay, ToolTip = "빈 설명 위젯의 기본 폭. BP 디자인이 있으면 DescriptionSizeBox의 설정을 사용합니다."))
	float ExplanationWidth = 620.f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Tutorial|Appearance", meta = (ClampMin = "0", ClampMax = "1"))
	float DimOpacity = 0.8f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Tutorial|Appearance")
	FLinearColor FrameColor = FLinearColor(1.f, 0.2f, 0.15f, 1.f);
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Tutorial|Appearance", meta = (ClampMin = "0"))
	float FrameThickness = 3.f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Tutorial|Text", meta = (AdvancedDisplay, ToolTip = "설명 위젯에 Text Style Set이 없을 때 사용하는 기본 RichText 스타일입니다."))
	TObjectPtr<UDataTable> RichTextStyleSet;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Tutorial|Hint")
	TSubclassOf<UTutorialClickHintWidget> ClickHintWidgetClass;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Tutorial|Hint")
	FVector2D ClickHintOffset = FVector2D(0.f, -20.f);
protected:
	virtual void NativeOnInitialized() override;
	virtual void NativeTick(const FGeometry& Geometry, float DeltaTime) override;
	virtual FReply NativeOnPreviewMouseButtonDown(const FGeometry& Geometry, const FPointerEvent& Event) override;
	virtual FReply NativeOnMouseButtonDown(const FGeometry& Geometry, const FPointerEvent& Event) override;
	virtual int32 NativePaint(const FPaintArgs& Args, const FGeometry& Geometry, const FSlateRect& CullingRect,
		FSlateWindowElementList& Elements, int32 Layer, const FWidgetStyle& Style, bool bParentEnabled) const override;
private:
	// 기존 BP에 저장된 위치 배열을 ExplanationLayouts로 옮기기 위한 직렬화 호환 필드입니다.
	UPROPERTY() TArray<FVector2D> ExplanationPositions;
	UPROPERTY(Transient) TObjectPtr<UCanvasPanel> TutorialCanvas;
	UPROPERTY(Transient) TArray<TObjectPtr<UBorder>> DimRegions;
	UPROPERTY(Transient) TObjectPtr<UBorder> HoleInput;
	UPROPERTY(Transient) TObjectPtr<UTutorialDescriptionWidget> ExplanationWidget;
	UPROPERTY(Transient) TObjectPtr<UTutorialClickHintWidget> ClickHint;
	TWeakObjectPtr<UWidget> WidgetTarget;
	TWeakObjectPtr<AActor> ActorTarget;
	TArray<TWeakObjectPtr<AActor>> AreaTargets;
	bool bTransitionBusy = false;
	bool bAdvancePending = false;
	FVector2D ActorHoleSize = FVector2D(0.1f, 0.1f);
	FBattleTutorialStep CurrentStep;
	FVector2D HighlightPosition = FVector2D::ZeroVector;
	FVector2D HighlightSize = FVector2D::ZeroVector;
	bool bHasHighlight = false;
	void Place(UWidget* Widget, const FVector2D& Position, const FVector2D& Size);
};
