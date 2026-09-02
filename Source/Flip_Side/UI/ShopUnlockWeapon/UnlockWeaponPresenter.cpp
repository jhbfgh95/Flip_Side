#include "UI/ShopUnlockWeapon/UnlockWeaponPresenter.h"

#include "Subsystem/ShopLevel/ShopUnlockWeaponWSubsystem.h"
#include "UI/ShopUnlockWeapon/W_UnlockWeaponSlot.h"
#include "UI/ShopUnlockWeapon/W_UnlockWeaponSlotContainer.h"
#include "UI/ShopUnlockWeapon/W_UnlockWeaponWidget.h"
#include "UI/ShopUnlockWeapon/ShopUnlockWeaponUIActor.h"
#include "UI/W_PriceWidget.h"
#include "UI/W_WeaponDescription.h"
#include "Subsystem/DataManagerSubsystem.h"
#include "Subsystem/UnlockGISubsystem.h"

void UUnlockWeaponPresenter::InitPresenter(UW_UnlockWeaponWidget* InUnlockWeaponWidget,
	UShopUnlockWeaponWSubsystem* InUnlockWeaponSubsystem,
	UDataManagerSubsystem* InDataManager,
	UUnlockGISubsystem* InUnlockSubsystem,
	AShopUnlockWeaponUIActor* InShopUnlockWeaponUIActor)
{
	UnlockWeaponWidget = InUnlockWeaponWidget;
	UnlockWeaponSubsystem = InUnlockWeaponSubsystem;
	DataManager = InDataManager;
	UnlockSubsystem = InUnlockSubsystem;
	ShopUnlockWeaponUIActor = InShopUnlockWeaponUIActor;

	if(!IsValid(UnlockWeaponWidget) || !IsValid(UnlockWeaponSubsystem) || !IsValid(ShopUnlockWeaponUIActor))
		return;

	ShopUnlockWeaponUIActor->OnShakeFinished.AddUniqueDynamic(
		this, &UUnlockWeaponPresenter::FinishedUnlockWeapon);

	InitUnlockWeaponContainer();
}

void UUnlockWeaponPresenter::InitUnlockWeaponContainer()
{
	UW_UnlockWeaponSlotContainer* SlotContainer = UnlockWeaponWidget->GetUnlockWeaponSlotContainer();
	if(!IsValid(SlotContainer))
		return;
	TArray<FFaceData> LockedWeapons;
	for(const FFaceData& WeaponData : UnlockWeaponSubsystem->GetUnlockWeapons())
	{
		if(!IsValid(UnlockSubsystem) || !UnlockSubsystem->IsWeaponUnlockByID(WeaponData.WeaponID))
		{
			LockedWeapons.Add(WeaponData);
		}
	}

	SlotContainer->InitWidget(LockedWeapons);
	WeaponSlotViews = SlotContainer->GetWeaponSlots();

	for(UW_UnlockWeaponSlot* WeaponSlot : WeaponSlotViews)
	{
		if(!IsValid(WeaponSlot))
			continue;

		//WeaponSlot->OnClickedUnlockWeaponSlot.AddDynamic(this, &UUnlockWeaponPresenter::SelectWeapon);
		WeaponSlot->OnHoveredUnlockWeaponSlot.AddDynamic(this, &UUnlockWeaponPresenter::HoverWeapon);
		WeaponSlot->OnUnhoveredUnlockWeaponSlot.AddDynamic(this, &UUnlockWeaponPresenter::UnhoverWeapon);
		WeaponSlot->OnHoldStartedUnlockWeaponSlot.AddDynamic(this, &UUnlockWeaponPresenter::StartHoldWeapon);
		WeaponSlot->OnHoldCancelledUnlockWeaponSlot.AddDynamic(this, &UUnlockWeaponPresenter::CancelHoldWeapon);
		WeaponSlot->OnHoldCompletedUnlockWeaponSlot.AddDynamic(this, &UUnlockWeaponPresenter::CompleteHoldWeapon);
	}

}


void UUnlockWeaponPresenter::HoverWeapon(int32 WeaponID)
{
	CurrentSelectedWeaponID = WeaponID;
	UpdateWeaponUIActor(WeaponID);
	UpdateWeaponDescription(WeaponID);
	UpdateUnlockControls(WeaponID);
}

void UUnlockWeaponPresenter::UnhoverWeapon()
{
	CurrentSelectedWeaponID = -1;
	HideWeaponDescription();
	HideUnlockControls();
}

void UUnlockWeaponPresenter::UnlockSelectedWeapon()
{
	TryUnlockWeapon(CurrentSelectedWeaponID);
}

void UUnlockWeaponPresenter::StartHoldWeapon(int32 WeaponID)
{
	if (!IsValid(ShopUnlockWeaponUIActor) || WeaponID == -1)
		return;

	ShopUnlockWeaponUIActor->StartHoldShake();
}

void UUnlockWeaponPresenter::CancelHoldWeapon(int32 WeaponID)
{
	if (CurrentSelectedWeaponID != WeaponID)
		return;

	CurrentSelectedWeaponID = -1;
	if (IsValid(ShopUnlockWeaponUIActor))
		ShopUnlockWeaponUIActor->StopHoldShake();
}

void UUnlockWeaponPresenter::CompleteHoldWeapon(int32 WeaponID)
{
	if (CurrentSelectedWeaponID != WeaponID)
		return;

	if (IsValid(ShopUnlockWeaponUIActor))
		ShopUnlockWeaponUIActor->CompleteHoldShake();

	TryUnlockWeapon(WeaponID);
}

void UUnlockWeaponPresenter::TryUnlockWeapon(int32 WeaponID)
{
	if (WeaponID == -1 || !IsValid(UnlockWeaponSubsystem))
		return;

	if (UnlockWeaponSubsystem->UnlockWeapon(WeaponID))
	{
		if (!IsValid(ShopUnlockWeaponUIActor) || !ShopUnlockWeaponUIActor->PlayBuyWeaponAnim())
		{
			FinishedUnlockWeapon();
		}
		UpdateUnlockControls(WeaponID);
	}
}

void UUnlockWeaponPresenter::FinishedUnlockWeapon()
{
	RemoveUnlockedWeaponSlot(CurrentSelectedWeaponID);
}

void UUnlockWeaponPresenter::RemoveUnlockedWeaponSlot(int32 WeaponID)
{
	UW_UnlockWeaponSlotContainer* SlotContainer = UnlockWeaponWidget->GetUnlockWeaponSlotContainer();
	if(!IsValid(SlotContainer) || !SlotContainer->RemoveWeaponSlot(WeaponID))
		return;

	WeaponSlotViews = SlotContainer->GetWeaponSlots();
	
	if(CurrentSelectedWeaponID == WeaponID)
	{
		CurrentSelectedWeaponID = -1;
		HideWeaponDescription();
		UpdateWeaponUIActor(CurrentSelectedWeaponID);
		HideUnlockControls();
	}
}

void UUnlockWeaponPresenter::UpdateWeaponUIActor(int32 WeaponID)
{
	if(!IsValid(ShopUnlockWeaponUIActor))
		return;

	FFaceData WeaponData;
	WeaponData.WeaponID = -1;
	if(WeaponID != -1 && IsValid(DataManager))
		DataManager->TryGetWeapon(WeaponID, WeaponData);

	ShopUnlockWeaponUIActor->SetWeaponIcon(WeaponData);
}

void UUnlockWeaponPresenter::UpdateWeaponDescription(int32 WeaponID)
{
	if(!IsValid(UnlockWeaponWidget) || !IsValid(DataManager) || WeaponID == -1)
	{
		HideWeaponDescription();
		return;
	}

	FFaceData WeaponData;
	WeaponData.WeaponID = -1;
	if(!DataManager->TryGetWeapon(WeaponID, WeaponData))
	{
		HideWeaponDescription();
		return;
	}

	if(UW_WeaponDescription* DescriptionWidget = UnlockWeaponWidget->GetWeaponDescription())
	{
		DescriptionWidget->SetExplainText(
			WeaponData.WeaponName,
			WeaponData.KOR_DES,
			WeaponData.BehaviorPoint,
			WeaponData.AttackPoint);
	}
}

void UUnlockWeaponPresenter::HideWeaponDescription()
{
	if(IsValid(UnlockWeaponWidget))
	{
		if(UW_WeaponDescription* DescriptionWidget = UnlockWeaponWidget->GetWeaponDescription())
		{
			DescriptionWidget->SetExplainTextEmpty();
		}
	}
}

void UUnlockWeaponPresenter::UpdateUnlockControls(int32 WeaponID)
{
	UW_PriceWidget* PriceWidget = UnlockWeaponWidget->GetWeaponPriceWidget();
	if(!IsValid(PriceWidget))
		return;

	if(WeaponID == -1 || !IsValid(DataManager) ||
		(IsValid(UnlockSubsystem) && UnlockSubsystem->IsWeaponUnlockByID(WeaponID)))
	{
		HideUnlockControls();
		return;
	}

	FFaceData WeaponData;
	WeaponData.WeaponID = -1;
	if(!DataManager->TryGetWeapon(WeaponID, WeaponData))
	{
		HideUnlockControls();
		return;
	}

	PriceWidget->SetPriceText(WeaponData.Price);
	PriceWidget->SetVisibility(ESlateVisibility::Visible);
}

void UUnlockWeaponPresenter::HideUnlockControls()
{
	if(UW_PriceWidget* PriceWidget = UnlockWeaponWidget->GetWeaponPriceWidget())
		PriceWidget->SetVisibility(ESlateVisibility::Collapsed);
}
