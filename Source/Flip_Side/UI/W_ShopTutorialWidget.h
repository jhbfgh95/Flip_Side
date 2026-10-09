#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "W_ShopTutorialWidget.generated.h"

class UBorder;
class UButton;
class UImage;
class UMaterialInstanceDynamic;
class URichTextBlock;
class AActor;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnShopTutorialDialogueClicked);

UCLASS()
class FLIP_SIDE_API UW_ShopTutorialWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;
	virtual int32 NativePaint(const FPaintArgs& Args, const FGeometry& Geometry,
		const FSlateRect& CullingRect, FSlateWindowElementList& Elements, int32 Layer,
		const FWidgetStyle& Style, bool bParentEnabled) const override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Tutorial|Appearance")
	FLinearColor FrameColor = FLinearColor(0.f, 0.4f, 1.f, 1.f);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Tutorial|Appearance", meta = (ClampMin = "0"))
	float FrameThickness = 3.f;

	// 전체 화면 딤 마스크 머티리얼을 사용하는 Image입니다.
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> DimMaskImage;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> DialogueButton;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UBorder> DialogueBorder;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<URichTextBlock> DialogueText;

public:
	UFUNCTION(BlueprintCallable)
	void SetTutorialActive(bool bIsActive);

	// false면 화면에는 보이지만, 위젯 전체가 마우스 입력을 가로채지 않습니다.
	UFUNCTION(BlueprintCallable)
	void SetTutorialInputEnabled(bool bInEnabled);

	UFUNCTION(BlueprintCallable)
	void SetDialogueText(const FText& InText);

	UFUNCTION(BlueprintCallable)
	void SetDialogueBorderVisible(bool bInVisible);

	// 화면 절대 좌표 기준 강조 대상의 위치와 크기로 딤 마스크의 구멍을 갱신합니다.
	UFUNCTION(BlueprintCallable)
	void SetDimMaskHole(FVector2D InAbsolutePosition, FVector2D InAbsoluteSize);

	// 호출 시점의 대상 위치와 크기로 강조 영역을 한 번 갱신합니다.
	void SetHighlightTargetWidget(UWidget* InTargetWidget, bool bKeepExistingBoxes = false);
	void SetHighlightTargetActor(AActor* InTargetActor, FVector2D InMinimumScreenSize, bool bKeepExistingBoxes = false);

	// 딤 마스크를 표시하거나 숨깁니다.
	UFUNCTION(BlueprintCallable)
	void SetHighlightOverlayVisible(bool bIsHighlightVisible);

	UFUNCTION(BlueprintCallable)
	void SetDialogueBoxLayout(FVector2D InAnchorPosition, FVector2D InSize);

	UPROPERTY(BlueprintAssignable)
	FOnShopTutorialDialogueClicked OnDialogueClicked;

private:
	void UpdateDimMaskParameters();
	void UpdateHighlightGeometry(UWidget* Target, AActor* Actor, FVector2D ActorMinimumScreenSize);
	FVector2D HighlightPosition = FVector2D::ZeroVector;
	FVector2D HighlightSize = FVector2D::ZeroVector;
	// 각 항목은 박스의 위치와 크기입니다.
	TArray<TPair<FVector2D, FVector2D>> HighlightBoxes;
	bool bHighlightOverlayVisible = false;
	bool bHasHighlight = false;

	UPROPERTY(Transient)
	TObjectPtr<UMaterialInstanceDynamic> DimMaskMID;

	UFUNCTION()
	void HandleDialogueClicked();
};
