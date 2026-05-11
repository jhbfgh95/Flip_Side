#include "UI/W_StageHandCardWidget.h"
#include "Subsystem/StageCardWSubsystem.h"
#include "Input/Reply.h"
#include "InputCoreTypes.h"
#include "Components/Border.h"
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

        StageCardSubSystem->OnStageHandCardActive.AddDynamic(this, &UW_StageHandCardWidget::SetActiveCardEffect);
    }
    
}
void UW_StageHandCardWidget::NativeDestruct()
{
    if (OnHoverHandCard.IsBound())
    {
        OnHoverHandCard.Clear();
    }
    if (OnUnHoverHandCard.IsBound())
    {
        OnUnHoverHandCard.Clear();
    }
    StageCardSubSystem->OnStageHandCardActive.RemoveAll(this);
    Super::NativeDestruct();
}

void UW_StageHandCardWidget::OnHandCardSet(int32 Index, FCardData CardData)
{
    if (HandIndex != Index) return;

    InitCard(CardData);
    SetVisibility(ESlateVisibility::Visible);
    CanControl = true;
    CurrentCardData = CardData;
}

void UW_StageHandCardWidget::OnHandCardCleared(int32 Index)
{
    if (HandIndex != Index) return;

    SetVisibility(ESlateVisibility::Collapsed);
    CanControl = true;
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
    HoverCard();
    //PlayCardAnim(HoverCardAnim);
}

void UW_StageHandCardWidget::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{
    Super::NativeOnMouseLeave(InMouseEvent);
    UnhoverCard();
    //PlayCardAnim(UnHoverCardAnim);
}

void UW_StageHandCardWidget::PlayCardAnim(UWidgetAnimation* Anim)
{
    if (!CanControl) return;
    if (Anim) PlayAnimation(Anim);
}
void UW_StageHandCardWidget::SetActiveCardEffect(int32 index, bool IsActive)
{
    if(index != HandIndex)
        return;
    if(IsActive)
        ActiveWidget->SetVisibility(ESlateVisibility::Visible);
    else
        ActiveWidget->SetVisibility(ESlateVisibility::Collapsed);
}
    
void UW_StageHandCardWidget::HoverCard()
{
    UE_LOG(LogTemp, Warning, TEXT("카드 호버링"));
    OnHoverHandCard.Broadcast(CurrentCardData);
}
    
void UW_StageHandCardWidget::UnhoverCard()
{
    UE_LOG(LogTemp, Warning, TEXT("카드 언호버링"));
    OnUnHoverHandCard.Broadcast();
}