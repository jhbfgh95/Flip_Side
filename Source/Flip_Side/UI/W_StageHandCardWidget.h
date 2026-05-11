#pragma once

#include "CoreMinimal.h"
#include "UI/W_CardWidget.h"
#include "DataTypes/CardTypes.h"
#include "W_StageHandCardWidget.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FHoverHandCard, FCardData, HoverCardData);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FUnhoverHandCard);
UCLASS()
class FLIP_SIDE_API UW_StageHandCardWidget : public UW_CardWidget
{
    GENERATED_BODY()

protected:
    virtual void NativeConstruct() override;
    virtual void NativeDestruct() override;

    UPROPERTY()
    class UStageCardWSubsystem* StageCardSubSystem;

    virtual void NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
    virtual void NativeOnMouseLeave(const FPointerEvent& InMouseEvent) override;

public:
    FHoverHandCard OnHoverHandCard;
    FUnhoverHandCard OnUnHoverHandCard;

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
    UPROPERTY(meta = (BindWidget))
    class UBorder* ActiveWidget;    
private:
    UFUNCTION()
    void OnRemoveAnimFinished();

    UFUNCTION()
    void OnHandCardSet(int32 Index, FCardData CardData);

    UFUNCTION()
    void OnHandCardCleared(int32 Index);

    
    UFUNCTION()
    void HoverCard();

    UFUNCTION()
    void UnhoverCard();


private:
    bool CanControl = true;

private:
    FCardData CurrentCardData;
public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 HandIndex = 0;

    UFUNCTION()
    void SetActiveCardEffect(int32 index, bool IsActive);
};