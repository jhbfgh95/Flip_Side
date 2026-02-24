#include "UI/W_StageHandCardWidget.h"
#include "Subsystem/StageCardWSubsystem.h"
#include "Input/Reply.h"
#include "InputCoreTypes.h"

void UW_StageHandCardWidget::NativeConstruct()
{
    Super::NativeConstruct();

    StageCardSubSystem = GetWorld()->GetSubsystem<UStageCardWSubsystem>();

    if (StageCardSubSystem)
    {
        StageCardSubSystem->OnHandCardSet.AddDynamic(this, &UW_StageHandCardWidget::OnHandCardSet);
        StageCardSubSystem->OnHandCardCleared.AddDynamic(this, &UW_StageHandCardWidget::OnHandCardCleared);
    }

    SetVisibility(ESlateVisibility::Collapsed);

    if (RemoveCardAnim)
    {
        FWidgetAnimationDynamicEvent EndEvent;
        EndEvent.BindDynamic(this, &UW_StageHandCardWidget::OnRemoveAnimFinished);
        BindToAnimationFinished(RemoveCardAnim, EndEvent);
    }

    if (StageCardSubSystem)
    {
        FCardData Card;
        if (StageCardSubSystem->TryGetHandCard(HandIndex, Card))
        {
            InitCard(Card);
            SetVisibility(ESlateVisibility::Visible);
        }
    }
}

void UW_StageHandCardWidget::OnHandCardSet(int32 Index, FCardData CardData)
{
    if (HandIndex != Index) return;

    InitCard(CardData);
    SetVisibility(ESlateVisibility::Visible);
    CanControl = true;
    PlayCardAnim(AddCardAnim);
}

void UW_StageHandCardWidget::OnHandCardCleared(int32 Index)
{
    if (HandIndex != Index) return;

    SetVisibility(ESlateVisibility::Collapsed);
    CanControl = true;
}

FReply UW_StageHandCardWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
    if (InMouseEvent.GetEffectingButton() == EKeys::RightMouseButton)
    {
        if (RemoveCardAnim && CanControl)
        {
            CanControl = false;
            PlayCardAnim(RemoveCardAnim);
        }
        return FReply::Handled();
    }

    return Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
}

void UW_StageHandCardWidget::OnRemoveAnimFinished()
{
    if (StageCardSubSystem)
    {
        StageCardSubSystem->RemoveHandCard(HandIndex);
    }
    CanControl = true;
}

void UW_StageHandCardWidget::NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
    Super::NativeOnMouseEnter(InGeometry, InMouseEvent);
    PlayCardAnim(HoverCardAnim);
}

void UW_StageHandCardWidget::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{
    Super::NativeOnMouseLeave(InMouseEvent);
    PlayCardAnim(UnHoverCardAnim);
}

void UW_StageHandCardWidget::PlayCardAnim(UWidgetAnimation* Anim)
{
    if (!CanControl) return;
    if (Anim) PlayAnimation(Anim);
}