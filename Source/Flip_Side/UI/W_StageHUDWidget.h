#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/HorizontalBox.h"
#include "Components/Button.h"
#include "W_StageHUDWidget.generated.h"

UCLASS()
class FLIP_SIDE_API UW_StageHUDWidget : public UUserWidget
{
    GENERATED_BODY()

protected:
    virtual void NativeConstruct() override;

private:
    UPROPERTY()
    class UStageCardWSubsystem* StageCardSubSystem;

    // 카드 3장 묶음 컨테이너
    UPROPERTY(meta = (BindWidget))
    class UHorizontalBox* HandCardPanel;

    // 오른쪽 토글 버튼
    UPROPERTY(meta = (BindWidget))
    class UButton* ToggleHandButton;


protected:

    UPROPERTY(meta = (BindWidget))
    class UW_StageHandCardWidget* HandCard0;

    UPROPERTY(meta = (BindWidget))
    class UW_StageHandCardWidget* HandCard1;

    UPROPERTY(meta = (BindWidget))
    class UW_StageHandCardWidget* HandCard2;

    UFUNCTION()
    void OnClickToggleHandButton();

    bool bHandVisible = true;
};