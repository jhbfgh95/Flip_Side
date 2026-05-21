#include "UI/W_StageHUDWidget.h"
#include "Subsystem/StageCardWSubsystem.h"
#include "UI/W_StageHandCardWidget.h"
#include "Components/VerticalBox.h"
#include "Components/Button.h"
#include "UI/W_CardWidget.h"
#include "Subsystem/FlipSideSoundUtils.h"

void UW_StageHUDWidget::NativeConstruct()
{
    Super::NativeConstruct();

    StageCardSubSystem = GetWorld()->GetSubsystem<UStageCardWSubsystem>();
    if (!StageCardSubSystem)
    {
        UE_LOG(LogTemp, Error, TEXT("[StageHUD] StageCardSubSystem is null"));
        return;
    }

    if (ToggleHandButton)
    {
        ToggleHandButton->OnClicked.AddDynamic(this, &UW_StageHUDWidget::OnClickToggleHandButton);
    }

    if (HandCard0)
    {
        HandCard0->HandIndex = 0;
        HandCard0->OnHoverHandCard.AddDynamic(this, &UW_StageHUDWidget::ShowPopupCard);
        HandCard0->OnUnHoverHandCard.AddDynamic(this, &UW_StageHUDWidget::HidePopupCard);
    }
    if (HandCard1)
    {
        HandCard1->HandIndex = 1;
        HandCard1->OnHoverHandCard.AddDynamic(this, &UW_StageHUDWidget::ShowPopupCard);
        HandCard1->OnUnHoverHandCard.AddDynamic(this, &UW_StageHUDWidget::HidePopupCard);
    }

    if(HandCard2)
    {
        HandCard2->HandIndex = 2;
        HandCard2->OnHoverHandCard.AddDynamic(this, &UW_StageHUDWidget::ShowPopupCard);
        HandCard2->OnUnHoverHandCard.AddDynamic(this, &UW_StageHUDWidget::HidePopupCard);
    }

    bHandVisible = true;
    if (HandCardPanel)
    {
        HandCardPanel->SetVisibility(ESlateVisibility::Visible);
    }

    StageCardSubSystem->RefreshHandFromGI();
    HidePopupCard();
}

void UW_StageHUDWidget::NativeDestruct()
{
    HandCard0->OnHoverHandCard.RemoveAll(this);
    HandCard0->OnUnHoverHandCard.RemoveAll(this);
    HandCard1->OnHoverHandCard.RemoveAll(this);
    HandCard1->OnUnHoverHandCard.RemoveAll(this);
    HandCard2->OnHoverHandCard.RemoveAll(this);
    HandCard2->OnUnHoverHandCard.RemoveAll(this);
    
    Super::NativeDestruct();
}

void UW_StageHUDWidget::OnClickToggleHandButton()
{
    FFlipSideSoundUtils::PlayDefaultClickSound(this);
    bHandVisible = !bHandVisible;

    if (!HandCardPanel)
        return;

    HandCardPanel->SetVisibility(bHandVisible ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
}
    
void UW_StageHUDWidget::ShowPopupCard(FCardData CardData)
{
    PopUpCardWidget->InitCard(CardData);
    PopUpCardWidget->SetVisibility(ESlateVisibility::HitTestInvisible);
}
    
void UW_StageHUDWidget::HidePopupCard()
{
    PopUpCardWidget->SetVisibility(ESlateVisibility::Collapsed);

}
