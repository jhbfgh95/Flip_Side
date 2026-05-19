// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/W_BattleTutorialOverlay.h"

#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Components/Widget.h"
#include "GameFramework/PlayerController.h"
#include "Materials/MaterialInstanceDynamic.h"

void UW_BattleTutorialOverlay::NativeConstruct()
{
	Super::NativeConstruct();

	if (NextButton)
	{
		NextButton->OnClicked.RemoveAll(this);
		NextButton->OnClicked.AddDynamic(this, &UW_BattleTutorialOverlay::HandleNextButtonClicked);
	}

	if (TopNextButton)
	{
		TopNextButton->OnClicked.RemoveAll(this);
		TopNextButton->OnClicked.AddDynamic(this, &UW_BattleTutorialOverlay::HandleNextButtonClicked);
	}

	if (BottomNextButton)
	{
		BottomNextButton->OnClicked.RemoveAll(this);
		BottomNextButton->OnClicked.AddDynamic(this, &UW_BattleTutorialOverlay::HandleNextButtonClicked);
	}

	if (!DimMaskImage)
	{
		return;
	}

	if (DimMaskMaterial)
	{
		DimMaterial = UMaterialInstanceDynamic::Create(DimMaskMaterial, this);
		DimMaskImage->SetBrushFromMaterial(DimMaterial);
	}
	else
	{
		DimMaterial = DimMaskImage->GetDynamicMaterial();
	}

	if (!DimMaterial)
	{
		return;
	}

	DimMaterial->SetScalarParameterValue(TEXT("DimOpacity"), 0.8f);
	DimMaterial->SetScalarParameterValue(TEXT("HoleCenterX"), 0.5f);
	DimMaterial->SetScalarParameterValue(TEXT("HoleCenterY"), 0.5f);
	DimMaterial->SetScalarParameterValue(TEXT("HoleSizeX"), 0.35f);
	DimMaterial->SetScalarParameterValue(TEXT("HoleSizeY"), 0.2f);
	DimMaterial->SetScalarParameterValue(TEXT("Feather"), 0.03f);
}

void UW_BattleTutorialOverlay::SetTutorialText(const FText& InText, bool bUseTopTextBox)
{
	SetUseTopTextBox(bUseTopTextBox);

	if (TopTutorialText && BottomTutorialText)
	{
		TopTutorialText->SetText(InText);
		BottomTutorialText->SetText(InText);
		return;
	}

	if (TutorialText)
	{
		TutorialText->SetText(InText);
	}
}

void UW_BattleTutorialOverlay::SetUseTopTextBox(bool bUseTopTextBox)
{
	bCurrentUseTopTextBox = bUseTopTextBox;

	if (TopTutorialRoot)
	{
		TopTutorialRoot->SetVisibility(bCurrentUseTopTextBox ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
	}

	if (BottomTutorialRoot)
	{
		BottomTutorialRoot->SetVisibility(bCurrentUseTopTextBox ? ESlateVisibility::Collapsed : ESlateVisibility::Visible);
	}

	if (TopTutorialText)
	{
		TopTutorialText->SetVisibility(bCurrentUseTopTextBox ? ESlateVisibility::HitTestInvisible : ESlateVisibility::Collapsed);
	}

	if (BottomTutorialText)
	{
		BottomTutorialText->SetVisibility(bCurrentUseTopTextBox ? ESlateVisibility::Collapsed : ESlateVisibility::HitTestInvisible);
	}
}

void UW_BattleTutorialOverlay::SetFocusFromWorldLocation(APlayerController* PlayerController, const FVector& WorldLocation, const FVector2D& HoleSize)
{
	if (!PlayerController || !DimMaterial)
	{
		return;
	}

	FVector2D ScreenPosition;
	if (!PlayerController->ProjectWorldLocationToScreen(WorldLocation, ScreenPosition))
	{
		return;
	}

	int32 ViewportSizeX = 0;
	int32 ViewportSizeY = 0;
	PlayerController->GetViewportSize(ViewportSizeX, ViewportSizeY);

	if (ViewportSizeX <= 0 || ViewportSizeY <= 0)
	{
		return;
	}

	const FVector2D CenterUV(
		ScreenPosition.X / static_cast<float>(ViewportSizeX),
		ScreenPosition.Y / static_cast<float>(ViewportSizeY)
	);

	SetFocusUV(CenterUV, HoleSize);
}

void UW_BattleTutorialOverlay::SetFocusUV(const FVector2D& CenterUV, const FVector2D& HoleSize)
{
	if (!DimMaterial)
	{
		return;
	}

	DimMaterial->SetScalarParameterValue(TEXT("HoleCenterX"), CenterUV.X);
	DimMaterial->SetScalarParameterValue(TEXT("HoleCenterY"), CenterUV.Y);
	DimMaterial->SetScalarParameterValue(TEXT("HoleSizeX"), HoleSize.X);
	DimMaterial->SetScalarParameterValue(TEXT("HoleSizeY"), HoleSize.Y);
}

void UW_BattleTutorialOverlay::SetDimOpacity(float InOpacity)
{
	if (DimMaterial)
	{
		DimMaterial->SetScalarParameterValue(TEXT("DimOpacity"), InOpacity);
	}
}

void UW_BattleTutorialOverlay::SetFeather(float InFeather)
{
	if (DimMaterial)
	{
		DimMaterial->SetScalarParameterValue(TEXT("Feather"), InFeather);
	}
}

void UW_BattleTutorialOverlay::SetNextButtonEnabled(bool bEnabled)
{
	const ESlateVisibility ActiveVisibility = bEnabled ? ESlateVisibility::Visible : ESlateVisibility::HitTestInvisible;
	const ESlateVisibility InactiveVisibility = ESlateVisibility::Collapsed;
	const bool bUsesSplitButtons = TopNextButton || BottomNextButton;

	if (NextButton)
	{
		NextButton->SetIsEnabled(true);
		NextButton->SetVisibility(bUsesSplitButtons ? ESlateVisibility::Collapsed : ActiveVisibility);
	}

	if (TopNextButton)
	{
		TopNextButton->SetIsEnabled(true);
		TopNextButton->SetVisibility(bCurrentUseTopTextBox ? ActiveVisibility : InactiveVisibility);
	}

	if (BottomNextButton)
	{
		BottomNextButton->SetIsEnabled(true);
		BottomNextButton->SetVisibility(bCurrentUseTopTextBox ? InactiveVisibility : ActiveVisibility);
	}
}

void UW_BattleTutorialOverlay::HandleNextButtonClicked()
{
	OnBattleTutorialOverlayClicked.Broadcast();
}
