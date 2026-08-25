#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "UnlockWeaponPresenter.generated.h"

class UW_UnlockWeaponWidget;
class UW_UnlockWeaponSlot;
class UW_SelectedUnlockWeapon;
class UShopUnlockWeaponWSubsystem;
class UDataManagerSubsystem;
class UUnlockGISubsystem;

UCLASS()
class FLIP_SIDE_API UUnlockWeaponPresenter : public UObject
{
	GENERATED_BODY()

public:
	void InitPresenter(UW_UnlockWeaponWidget* InUnlockWeaponWidget,
		UShopUnlockWeaponWSubsystem* InUnlockWeaponSubsystem,
		UDataManagerSubsystem* InDataManager,
		UUnlockGISubsystem* InUnlockSubsystem);

private:
	UPROPERTY()
	TObjectPtr<UW_UnlockWeaponWidget> UnlockWeaponWidget;

	UPROPERTY()
	TObjectPtr<UShopUnlockWeaponWSubsystem> UnlockWeaponSubsystem;

	UPROPERTY()
	TObjectPtr<UDataManagerSubsystem> DataManager;

	UPROPERTY()
	TObjectPtr<UUnlockGISubsystem> UnlockSubsystem;

	UPROPERTY()
	TArray<TObjectPtr<UW_UnlockWeaponSlot>> WeaponSlotViews;

	int32 CurrentSelectedWeaponID = -1;

	UFUNCTION()
	void SelectWeapon(int32 WeaponID);

	UFUNCTION()
	void HoverWeapon(int32 WeaponID);

	UFUNCTION()
	void UnhoverWeapon();

	UFUNCTION()
	void UnlockSelectedWeapon();

	UFUNCTION()
	void RemoveUnlockedWeaponSlot(int32 WeaponID);

	void UpdateSelectedWeapon(int32 WeaponID);
	void UpdateWeaponDescription(int32 WeaponID);
	void HideWeaponDescription();
	void UpdateUnlockControls(int32 WeaponID);
	void HideUnlockControls();

protected:
	void InitUnlockWeaponContainer();
	void InitUnlockButton();
};
