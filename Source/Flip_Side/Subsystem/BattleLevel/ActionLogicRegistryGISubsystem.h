#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "ActionLogicRegistryGISubsystem.generated.h"

using FActionLogic = TFunction<void(class UActionBase* Context)>;

UCLASS()
class FLIP_SIDE_API UActionLogicRegistryGISubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

private:
	TMap<int32, FActionLogic> WeaponMap;
	TMap<int, FActionLogic> ItemMap;
	
public:
	void RegisterWeaponLogic(int ID, FActionLogic Logic) { WeaponMap.Add(ID, Logic); }
	void RegisterItemLogic(int ID, FActionLogic Logic) { ItemMap.Add(ID, Logic); }
	

	FActionLogic GetWeaponLogic(int ID) { return WeaponMap.FindRef(ID); }
	FActionLogic GetItemLogic(int ID) { return ItemMap.FindRef(ID); }

protected:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	void RegisterWeaponLogic();

	void RegisterItemLogic();
};
