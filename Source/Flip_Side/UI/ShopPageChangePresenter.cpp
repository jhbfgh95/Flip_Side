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
}

	
void UShopPageChangePresenter::InitShopUISelectActor()
{
	if(!IsValid(ShopUISelectRegistry))
		return;
	
	AShopUISelectActor* CoinSelectActor =ShopUISelectRegistry->GetCoinUISelectActor();
	AShopUISelectActor* WeaponSelectActor = ShopUISelectRegistry->GetWeaponUISelectActor();
	AShopUISelectActor* CardSelectActor = ShopUISelectRegistry->GetCardUISelectActor();
	AShopUISelectActor* ItemSelectActor = ShopUISelectRegistry->GetItemUISelectActor();
	AShopUISelectActor* BossSelectActor = ShopUISelectRegistry->GetBossUISelectActor();
	
	if(IsValid(CoinSelectActor))
	{
		CoinSelectActor->OnClickShopPageChangeActor.AddUniqueDynamic(this, &UShopPageChangePresenter::HandlePageRequested);
	}
	if(WeaponSelectActor)
	{
		WeaponSelectActor
			->OnClickShopPageChangeActor.AddUniqueDynamic(this, &UShopPageChangePresenter::HandlePageRequested);
	}
	if(CardSelectActor)
	{
		CardSelectActor
			->OnClickShopPageChangeActor.AddUniqueDynamic(this, &UShopPageChangePresenter::HandlePageRequested);
	}
	if(ItemSelectActor)
	{
		ItemSelectActor
			->OnClickShopPageChangeActor.AddUniqueDynamic(this, &UShopPageChangePresenter::HandlePageRequested);
	}
	if(BossSelectActor)
	{
		BossSelectActor
			->OnClickShopPageChangeActor.AddUniqueDynamic(this, &UShopPageChangePresenter::HandlePageRequested);
	}


}
