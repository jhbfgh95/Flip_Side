#include "UI/ShopPageChangePresenter.h"

#include "Player/ShopPlayerPawn_FlipSide.h"
#include "UI/W_ShopWidgetContainer.h"
#include "UI/ShopUISelectRegistry.h"
#include "UI/ShopUISelectActor.h"
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

	InitShopUISelectActor();
	HandlePageRequested(EShopPage::Main);
}

void UShopPageChangePresenter::HandlePageRequested(EShopPage Page)
{
	if (!IsValid(WidgetContainer) || !IsValid(ShopPawn))
	{
		return;
	}
	
	UE_LOG(LogTemp, Warning, TEXT("페이지 전환"));
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
		}
	}
}
