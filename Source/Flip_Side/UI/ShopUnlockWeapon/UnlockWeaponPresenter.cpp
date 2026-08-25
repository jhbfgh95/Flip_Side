#include "UI/ShopUnlockWeapon/UnlockWeaponPresenter.h"

#include "Subsystem/ShopLevel/ShopUnlockWeaponWSubsystem.h"
#include "UI/ShopUnlockWeapon/W_UnlockWeaponSlot.h"
#include "UI/ShopUnlockWeapon/W_UnlockWeaponSlotContainer.h"
#include "UI/ShopUnlockWeapon/W_UnlockWeaponWidget.h"
#include "UI/ShopUnlockWeapon/W_SelectedUnlockWeapon.h"
#include "UI/ShopUnlockWeapon/W_UnlockSelectWeaponButton.h"
#include "UI/W_PriceWidget.h"
#include "UI/W_WeaponDescription.h"
#include "Subsystem/DataManagerSubsystem.h"
#include "Subsystem/UnlockGISubsystem.h"

void UUnlockWeaponPresenter::InitPresenter(UW_UnlockWeaponWidget* InUnlockWeaponWidget,
	UShopUnlockWeaponWSubsystem* InUnlockWeaponSubsystem,
	UDataManagerSubsystem* InDataManager,
	UUnlockGISubsystem* InUnlockSubsystem)
{
	UnlockWeaponWidget = InUnlockWeaponWidget;
	UnlockWeaponSubsystem = InUnlockWeaponSubsystem;
	DataManager = InDataManager;
	UnlockSubsystem = InUnlockSubsystem;

	if(!IsValid(UnlockWeaponWidget) || !IsValid(UnlockWeaponSubsystem))
		return;

	if(IsValid(UnlockSubsystem))
	{
		UnlockSubsystem->OnWeaponUnlock.AddUniqueDynamic(
			this, &UUnlockWeaponPresenter::RemoveUnlockedWeaponSlot);
	}

	InitUnlockWeaponContainer();
	InitUnlockButton();
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

		WeaponSlot->OnClickedUnlockWeaponSlot.AddDynamic(this, &UUnlockWeaponPresenter::SelectWeapon);
		WeaponSlot->OnHoveredUnlockWeaponSlot.AddDynamic(this, &UUnlockWeaponPresenter::HoverWeapon);
		WeaponSlot->OnUnhoveredUnlockWeaponSlot.AddDynamic(this, &UUnlockWeaponPresenter::UnhoverWeapon);
	}

}

void UUnlockWeaponPresenter::InitUnlockButton()
{
	UW_UnlockSelectWeaponButton* UnlockButton = UnlockWeaponWidget->GetUnlockButton();
	if(!IsValid(UnlockButton))
		return;

	UnlockButton->OnClickedUnlockSelectedWeapon.AddUniqueDynamic(
		this, &UUnlockWeaponPresenter::UnlockSelectedWeapon);

	HideUnlockControls();
}

void UUnlockWeaponPresenter::SelectWeapon(int32 WeaponID)
{
	CurrentSelectedWeaponID = WeaponID;
	UpdateSelectedWeapon(CurrentSelectedWeaponID);
	UpdateWeaponDescription(CurrentSelectedWeaponID);
	UpdateUnlockControls(CurrentSelectedWeaponID);
}

void UUnlockWeaponPresenter::HoverWeapon(int32 WeaponID)
{
	UpdateSelectedWeapon(WeaponID);
	UpdateWeaponDescription(WeaponID);
	UpdateUnlockControls(WeaponID);
}

void UUnlockWeaponPresenter::UnhoverWeapon()
{
	HideWeaponDescription();
	UpdateSelectedWeapon(CurrentSelectedWeaponID);
	if(CurrentSelectedWeaponID == -1)
		HideUnlockControls();
	else
		UpdateUnlockControls(CurrentSelectedWeaponID);
}

void UUnlockWeaponPresenter::UnlockSelectedWeapon()
{
	if(CurrentSelectedWeaponID != -1)
	{
		UnlockWeaponSubsystem->UnlockWeapon(CurrentSelectedWeaponID);
		UpdateUnlockControls(CurrentSelectedWeaponID);
	}
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
		UpdateSelectedWeapon(CurrentSelectedWeaponID);
		HideUnlockControls();
	}
}

void UUnlockWeaponPresenter::UpdateSelectedWeapon(int32 WeaponID)
{
	UW_SelectedUnlockWeapon* SelectedWeaponWidget = UnlockWeaponWidget->GetSelectedUnlockWeapon();
	if(!IsValid(SelectedWeaponWidget))
		return;

	FFaceData WeaponData;
	WeaponData.WeaponID = -1;
	if(WeaponID != -1 && IsValid(DataManager))
		DataManager->TryGetWeapon(WeaponID, WeaponData);

	SelectedWeaponWidget->SetSelectedImage(WeaponData);
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
	UW_UnlockSelectWeaponButton* UnlockButton = UnlockWeaponWidget->GetUnlockButton();
	UW_PriceWidget* PriceWidget = UnlockWeaponWidget->GetWeaponPriceWidget();
	if(!IsValid(UnlockButton) || !IsValid(PriceWidget))
		return;

	if(WeaponID == -1 || !IsValid(DataManager) ||
		(IsValid(UnlockSubsystem) && UnlockSubsystem->IsWeaponUnlockByID(WeaponID)))
	{
		HideUnlockControls();
		return;
	}
	
	UE_LOG(LogTemp, Warning, TEXT("잠겨있지 않음"));

	FFaceData WeaponData;
	WeaponData.WeaponID = -1;
	if(!DataManager->TryGetWeapon(WeaponID, WeaponData))
	{
		HideUnlockControls();
		return;
	}

	PriceWidget->SetPriceText(WeaponData.Price);
	PriceWidget->SetVisibility(ESlateVisibility::Visible);
	UnlockButton->ShowButton();
}

void UUnlockWeaponPresenter::HideUnlockControls()
{
	if(UW_UnlockSelectWeaponButton* UnlockButton = UnlockWeaponWidget->GetUnlockButton())
		UnlockButton->HideButton();

	if(UW_PriceWidget* PriceWidget = UnlockWeaponWidget->GetWeaponPriceWidget())
		PriceWidget->SetVisibility(ESlateVisibility::Collapsed);
}
