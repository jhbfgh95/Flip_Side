#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "W_ShopTutorialWidget.generated.h"

class UBorder;
class UButton;
class UImage;
class UMaterialInstanceDynamic;
class UTextBlock;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnShopTutorialDialogueClicked);

UCLASS()
class FLIP_SIDE_API UW_ShopTutorialWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

	// 전체 화면 딤 마스크 머티리얼을 사용하는 Image입니다.
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> DimMaskImage;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> DialogueButton;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UBorder> DialogueBorder;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> DialogueText;

public:
	UFUNCTION(BlueprintCallable)
	void SetTutorialActive(bool bIsActive);

	// false면 화면에는 보이지만, 위젯 전체가 마우스 입력을 가로채지 않습니다.
	UFUNCTION(BlueprintCallable)
	void SetTutorialInputEnabled(bool bInEnabled);

	UFUNCTION(BlueprintCallable)
	void SetDialogueText(const FText& InText);

	// 화면 절대 좌표 기준 강조 대상의 위치와 크기로 딤 마스크의 구멍을 갱신합니다.
	UFUNCTION(BlueprintCallable)
	void SetDimMaskHole(FVector2D InAbsolutePosition, FVector2D InAbsoluteSize);

	// 딤 마스크를 표시하거나 숨깁니다.
	UFUNCTION(BlueprintCallable)
	void SetHighlightOverlayVisible(bool bIsHighlightVisible);

	UFUNCTION(BlueprintCallable)
	void SetDialogueBoxLayout(FVector2D InAnchorPosition, FVector2D InSize);

	UPROPERTY(BlueprintAssignable)
	FOnShopTutorialDialogueClicked OnDialogueClicked;

private:
	UPROPERTY(Transient)
	TObjectPtr<UMaterialInstanceDynamic> DimMaskMID;

	UFUNCTION()
	void HandleDialogueClicked();
};
