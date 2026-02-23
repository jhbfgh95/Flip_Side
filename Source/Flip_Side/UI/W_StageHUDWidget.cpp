#include "UI/W_StageHUDWidget.h"
#include "Subsystem/StageCardWSubsystem.h"
#include "UI/W_StageHandCardWidget.h"
#include "Components/HorizontalBox.h"
#include "Components/Button.h"

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

    if (HandCard0) HandCard0->HandIndex = 0;
    if (HandCard1) HandCard1->HandIndex = 1;
    if (HandCard2) HandCard2->HandIndex = 2;

    bHandVisible = true;
    if (HandCardPanel)
    {
        HandCardPanel->SetVisibility(ESlateVisibility::Visible);
    }

    StageCardSubSystem->RefreshHandFromGI();
}

void UW_StageHUDWidget::OnClickToggleHandButton()
{
    bHandVisible = !bHandVisible;

    if (!HandCardPanel)
        return;

    HandCardPanel->SetVisibility(bHandVisible ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
}