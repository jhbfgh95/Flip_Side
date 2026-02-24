#pragma once

#include "CoreMinimal.h"
#include "UI/W_CardWidget.h"
#include "DataTypes/CardTypes.h"
#include "W_StageHandCardWidget.generated.h"

UCLASS()
class FLIP_SIDE_API UW_StageHandCardWidget : public UW_CardWidget
{
    GENERATED_BODY()

protected:
    virtual void NativeConstruct() override;

    UPROPERTY()
    class UStageCardWSubsystem* StageCardSubSystem;

    virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
    virtual void NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
    virtual void NativeOnMouseLeave(const FPointerEvent& InMouseEvent) override;

protected:
    UPROPERTY(meta = (BindWidgetAnim), Transient)
    UWidgetAnimation* HoverCardAnim;

    UPROPERTY(meta = (BindWidgetAnim), Transient)
    UWidgetAnimation* UnHoverCardAnim;

    UPROPERTY(meta = (BindWidgetAnim), Transient)
    UWidgetAnimation* AddCardAnim;

    UPROPERTY(meta = (BindWidgetAnim), Transient)
    UWidgetAnimation* RemoveCardAnim;

    void PlayCardAnim(UWidgetAnimation* Anim);

private:
    UFUNCTION()
    void OnRemoveAnimFinished();

    UFUNCTION()
    void OnHandCardSet(int32 Index, FCardData CardData);

    UFUNCTION()
    void OnHandCardCleared(int32 Index);

private:
    bool CanControl = true;

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 HandIndex = 0;
};