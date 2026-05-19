// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/W_BattleTutorialOverlay.h"

#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
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

	DimMaterial->SetScalarParameterValue(TEXT("DimOpacity"), 0.65f);
	DimMaterial->SetScalarParameterValue(TEXT("HoleCenterX"), 0.5f);
	DimMaterial->SetScalarParameterValue(TEXT("HoleCenterY"), 0.5f);
	DimMaterial->SetScalarParameterValue(TEXT("HoleSizeX"), 0.35f);
	DimMaterial->SetScalarParameterValue(TEXT("HoleSizeY"), 0.2f);
	DimMaterial->SetScalarParameterValue(TEXT("Feather"), 0.03f);
}

void UW_BattleTutorialOverlay::SetTutorialText(const FText& InText)
{
	if (TutorialText)
	{
		TutorialText->SetText(InText);
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
	if (!NextButton)
	{
		return;
	}

	NextButton->SetIsEnabled(bEnabled);
	NextButton->SetVisibility(bEnabled ? ESlateVisibility::Visible : ESlateVisibility::HitTestInvisible);
}

void UW_BattleTutorialOverlay::HandleNextButtonClicked()
{
	OnBattleTutorialOverlayClicked.Broadcast();
}
