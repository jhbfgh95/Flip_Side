#include "UI/ShopPageChangePresenter.h"

#include "Player/ShopPlayerPawn_FlipSide.h"
#include "UI/W_ShopWidgetContainer.h"
#include "UI/ShopUISelectRegistry.h"
#include "UI/W_ShopCheckStartGame.h"
#include "UI/ShopUISelectActor.h"
#include "Engine/SpotLight.h"
#include "Components/LightComponent.h"
#include "TimerManager.h"
void UShopPageChangePresenter::InitPresenter(AShopUISelectRegistry* InShopUISelectRegistry, 
	UW_ShopWidgetContainer* InWidgetContainer,
	AShopPlayerPawn_FlipSide* InShopPawn)
{
	ShopUISelectRegistry = InShopUISelectRegistry;
	WidgetContainer = InWidgetContainer;
	ShopPawn = InShopPawn;

	if (!ensure(IsValid(WidgetContainer)) || !ensure(IsValid(ShopPawn)) || !ensure(IsValid(ShopUISelectRegistry)))
	{
		return;
	}

	SelectActors = {
		ShopUISelectRegistry->GetCoinUISelectActor(),
		ShopUISelectRegistry->GetWeaponUISelectActor(),
		ShopUISelectRegistry->GetCardUISelectActor(),
		ShopUISelectRegistry->GetItemUISelectActor(),
		ShopUISelectRegistry->GetBossUISelectActor()
	};

	WidgetContainer->OnShopPageRequested.AddUniqueDynamic(
		this, &UShopPageChangePresenter::HandlePageRequested);

	ShopPawn->OnShopPawnMoveCompleted.AddUniqueDynamic(
		this, &UShopPageChangePresenter::HandleMoveCompleted);

	WidgetContainer->GetShopCheckStartWidget()->OnShopStartGameCancelConfirmed.AddDynamic(
		this, &UShopPageChangePresenter::HandlePageRequested);

	InitShopUISelectActor();
	HandlePageRequested(EShopPage::Main);
}

void UShopPageChangePresenter::HandlePageRequested(EShopPage Page)
{
	if (!IsValid(WidgetContainer) || !IsValid(ShopPawn))
	{
		return;
	}
	
	PendingPage = Page;
	bIsTransitioning = true;
	SetShopUISelectActorsEnabled(false);
	WidgetContainer->HideShopContent();
	ShopPawn->MoveToShopPage(Page);
}

void UShopPageChangePresenter::HandleMoveCompleted(EShopPage Page)
{
	if (!bIsTransitioning || Page != PendingPage || !IsValid(WidgetContainer))
	{
		return;
	}

	bIsTransitioning = false;
	WidgetContainer->ShowShopPage(Page);
	SetLight(Page);
	SetShopUISelectActorsEnabled(Page == EShopPage::Main);
}

void UShopPageChangePresenter::SetShopUISelectActorsEnabled(bool bEnabled)
{
	for (AShopUISelectActor* SelectActor : SelectActors)
	{
		if (IsValid(SelectActor))
		{
			SelectActor->SetClickCollisionEnabled(bEnabled);
		}
	}
}

void UShopPageChangePresenter::InitShopUISelectActor()
{
	for (AShopUISelectActor* SelectActor : SelectActors)
	{
		if (IsValid(SelectActor))
		{
			SelectActor->OnClickShopPageChangeActor.AddUniqueDynamic(
				this, &UShopPageChangePresenter::HandlePageRequested);

			SelectActor->OnHoverShopPageChangeActor.AddUniqueDynamic(
				this, &UShopPageChangePresenter::SetLight);

			SelectActor->OnUnhoverShopPageChangeActor.AddUniqueDynamic(
				this, &UShopPageChangePresenter::SetLightMain);

		}
	}
}

void UShopPageChangePresenter::SetLight(EShopPage Page)
{
	FadeLightTo(ShopUISelectRegistry->GetCoinUILight()->GetLightComponent(), 0.0f);
	FadeLightTo(ShopUISelectRegistry->GetItemUILight()->GetLightComponent(), 0.0f);
	FadeLightTo(ShopUISelectRegistry->GetCardUILight()->GetLightComponent(), 0.0f);
	FadeLightTo(ShopUISelectRegistry->GetBossUILight()->GetLightComponent(), 0.0f);
	FadeLightTo(ShopUISelectRegistry->GetWeaponUILight()->GetLightComponent(), 0.0f);
	switch (Page)
	{
	case EShopPage::Main:
	case EShopPage::GameStart:
		SetLightMain();
		break;
	case EShopPage::Coin:
		FadeLightTo(ShopUISelectRegistry->GetCoinUILight()->GetLightComponent(), 2000.0f);
		break;
	case EShopPage::Item:
		FadeLightTo(ShopUISelectRegistry->GetItemUILight()->GetLightComponent(), 2000.0f);
		break;
	case EShopPage::Card:
		FadeLightTo(ShopUISelectRegistry->GetCardUILight()->GetLightComponent(), 2000.0f);
		break;
	case EShopPage::UnlockWeapon:
		FadeLightTo(ShopUISelectRegistry->GetWeaponUILight()->GetLightComponent(), 2000.0f);
		break;
	case EShopPage::Boss:
		FadeLightTo(ShopUISelectRegistry->GetBossUILight()->GetLightComponent(), 2000.0f);
		break;
	default:
		return;
	}
}

void UShopPageChangePresenter::SetLightMain()
{
	FadeLightTo(ShopUISelectRegistry->GetMainUILight()->GetLightComponent(), 500.0f);
	FadeLightTo(ShopUISelectRegistry->GetCoinUILight()->GetLightComponent(), 2000.0f);
	FadeLightTo(ShopUISelectRegistry->GetItemUILight()->GetLightComponent(), 2000.0f);
	FadeLightTo(ShopUISelectRegistry->GetCardUILight()->GetLightComponent(), 2000.0f);
	FadeLightTo(ShopUISelectRegistry->GetBossUILight()->GetLightComponent(), 2000.0f);
	FadeLightTo(ShopUISelectRegistry->GetWeaponUILight()->GetLightComponent(), 2000.0f);
}

void UShopPageChangePresenter::FadeLightTo(ULightComponent* Light, float TargetIntensity)
{
	if (!IsValid(Light) || !GetWorld())
	{
		return;
	}

	for (int32 Index = LightFadeTargets.Num() - 1; Index >= 0; --Index)
	{
		if (LightFadeTargets[Index].Light.Get() == Light)
		{
			LightFadeTargets.RemoveAtSwap(Index);
		}
	}

	LightFadeTargets.Add({ Light, TargetIntensity });

	if (!GetWorld()->GetTimerManager().IsTimerActive(LightFadeTimer))
	{
		GetWorld()->GetTimerManager().SetTimer(
			LightFadeTimer,
			this,
			&UShopPageChangePresenter::UpdateLightFade,
			0.016f,
			true);
	}
}

void UShopPageChangePresenter::UpdateLightFade()
{
	constexpr float TimerInterval = 0.016f;

	for (int32 Index = LightFadeTargets.Num() - 1; Index >= 0; --Index)
	{
		FShopLightFadeTarget& Target = LightFadeTargets[Index];
		ULightComponent* Light = Target.Light.Get();

		if (!IsValid(Light))
		{
			LightFadeTargets.RemoveAtSwap(Index);
			continue;
		}

		const float NewIntensity = FMath::FInterpTo(
			Light->Intensity,
			Target.TargetIntensity,
			TimerInterval,
			LightFadeInterpSpeed);

		Light->SetIntensity(NewIntensity);

		if (FMath::IsNearlyEqual(NewIntensity, Target.TargetIntensity, 1.0f))
		{
			Light->SetIntensity(Target.TargetIntensity);
			LightFadeTargets.RemoveAtSwap(Index);
		}
	}

	if (LightFadeTargets.IsEmpty() && GetWorld())
	{
		GetWorld()->GetTimerManager().ClearTimer(LightFadeTimer);
	}
}
