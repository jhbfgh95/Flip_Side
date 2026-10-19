// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/W_WarningWidget.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "Engine/World.h"
#include "Subsystem/BattleLevel/SoundManagerWSubsystem.h"

void UW_WarningWidget::ActiveWarning(FString Text, bool bPlayWarningSound)
{
    WarningImage->SetVisibility(ESlateVisibility::Visible);
    WarningText->SetText(FText::FromString(Text));
    PlayAnimation(WarningTextAnim);

    if (UWorld* World = GetWorld(); bPlayWarningSound && IsValid(World))
    {
        if (USoundManagerWSubsystem* SoundManager = World->GetSubsystem<USoundManagerWSubsystem>())
        {
            SoundManager->PlayWarningSound();
        }
    }
}
