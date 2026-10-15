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
class AShopWeaponRangePreviewActor;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnShopWeaponUnlocked, int32, WeaponID);

UCLASS()
class FLIP_SIDE_API UUnlockWeaponPresenter : public UObject
{
	GENERATED_BODY()

public:
	void InitPresenter(UW_UnlockWeaponWidget* InUnlockWeaponWidget,
		UShopUnlockWeaponWSubsystem* InUnlockWeaponSubsystem,
		UDataManagerSubsystem* InDataManager,
		UUnlockGISubsystem* InUnlockSubsystem,
		AShopUnlockWeaponUIActor* InShopUnlockWeaponUIActor,
		AShopWeaponRangePreviewActor* InRangePreviewActor);

	UPROPERTY(BlueprintAssignable)
	FOnShopWeaponUnlocked OnWeaponUnlocked;

	UW_UnlockWeaponWidget* GetShopUnlockWidget() const { return UnlockWeaponWidget; }

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
	TObjectPtr<AShopWeaponRangePreviewActor> RangePreviewActor;

	UPROPERTY()
	TArray<TObjectPtr<UW_UnlockWeaponSlot>> WeaponSlotViews;

	int32 CurrentSelectedWeaponID = -1;
	
private:
	UFUNCTION()
	void HoverWeapon(int32 WeaponID);

	UFUNCTION()
	void UnlockSelectedWeapon();

	UFUNCTION()
	void StartHoldWeapon(int32 WeaponID);

	UFUNCTION()
	void CancelHoldWeapon(int32 WeaponID);

	UFUNCTION()
	void CompleteHoldWeapon(int32 WeaponID);

	UFUNCTION()
	void RemoveUnlockedWeaponSlot(int32 WeaponID);

	UFUNCTION()
	void FinishedUnlockWeapon();

	void UpdateWeaponUIActor(int32 WeaponID);

	void UpdateUnlockControls(int32 WeaponID);

	void HideUnlockControls();

	// 해금 누르기·완료 소리를 낼 사운드 관리자입니다.
	class USoundManagerWSubsystem* GetSoundManager() const;

protected:
	void UpdateWeaponDescription(int32 WeaponID);

	void HideWeaponDescription();

	// 해금에 성공하면 true입니다(골드 부족이면 false).
	bool TryUnlockWeapon(int32 WeaponID);


protected:
	void InitUnlockWeaponContainer();
};
