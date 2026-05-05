#pragma once

#include "CoreMinimal.h"
#include "BossActor.h"
#include "FlipSide_Enum.h"
#include "BossActor_RoleTarget.generated.h"

UCLASS()
class FLIP_SIDE_API ABossActor_RoleTarget : public ABossActor
{
    GENERATED_BODY()

public:
    ABossActor_RoleTarget();

    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;

    void StartRoleRoulette();
    void StopRoleRouletteAndLock();

    EWeaponClass GetLockedRoleClass() const { return LockedRoleClass; }
    bool IsRoleLocked() const { return bRoleLocked; }

private:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
    TObjectPtr<class UStaticMeshComponent> RoleIndicatorSphere;

    UPROPERTY()
    TObjectPtr<class UMaterialInstanceDynamic> IndicatorDynMat;

    EWeaponClass LockedRoleClass = EWeaponClass::None;
    bool bRoleLocked = false;

    bool bBlinking = false;
    float BlinkElapsed = 0.f;

    void SetIndicatorColor(FLinearColor Color);

    UFUNCTION()
    void ResetIndicatorColor();
};
