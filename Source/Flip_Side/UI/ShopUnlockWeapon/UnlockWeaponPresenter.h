#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "UnlockWeaponPresenter.generated.h"

class UW_UnlockWeaponWidget;
class UW_UnlockWeaponSlot;
class AShopUnlockWeaponUIActor;
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
		UUnlockGISubsystem* InUnlockSubsystem,
		AShopUnlockWeaponUIActor* InShopUnlockWeaponUIActor);

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
	TObjectPtr<AShopUnlockWeaponUIActor> ShopUnlockWeaponUIActor;

	UPROPERTY()
	TArray<TObjectPtr<UW_UnlockWeaponSlot>> WeaponSlotViews;

	int32 CurrentSelectedWeaponID = -1;
	int32 PendingUnlockedWeaponID = -1;

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

	UFUNCTION()
	void FinishedUnlockWeapon();

	void UpdateSelectedWeapon(int32 WeaponID);

	void UpdateUnlockControls(int32 WeaponID);

	void HideUnlockControls();

protected:
	void UpdateWeaponDescription(int32 WeaponID);

	void HideWeaponDescription();


protected:
	void InitUnlockWeaponContainer();
	void InitUnlockButton();
};
