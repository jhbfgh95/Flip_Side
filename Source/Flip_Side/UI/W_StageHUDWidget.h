#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Button.h"
#include "DataTypes/CardTypes.h"
#include "W_StageHUDWidget.generated.h"

UCLASS()
class FLIP_SIDE_API UW_StageHUDWidget : public UUserWidget
{
    GENERATED_BODY()

protected:
    virtual void NativeConstruct() override;

    virtual void NativeDestruct() override;
private:
    UPROPERTY()
    class UStageCardWSubsystem* StageCardSubSystem;

    // ī�� 3�� ���� �����̳�
    UPROPERTY(meta = (BindWidget))
    class UVerticalBox* HandCardPanel;

    // ������ ��� ��ư
    UPROPERTY(meta = (BindWidget))
    class UButton* ToggleHandButton;

    UPROPERTY(meta = (BindWidget))
    class UW_CardWidget* PopUpCardWidget;

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

protected:
    UFUNCTION()
    void ShowPopupCard(FCardData CardData);
    UFUNCTION()
    void HidePopupCard();
};