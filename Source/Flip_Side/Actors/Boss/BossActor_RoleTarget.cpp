#include "BossActor_RoleTarget.h"
#include "Components/StaticMeshComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "UObject/ConstructorHelpers.h"

ABossActor_RoleTarget::ABossActor_RoleTarget()
{
    PrimaryActorTick.bCanEverTick = true;

    RoleIndicatorSphere = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("RoleIndicatorSphere"));
    RoleIndicatorSphere->SetupAttachment(BossMesh);
    RoleIndicatorSphere->SetHiddenInGame(false);
    RoleIndicatorSphere->SetRelativeLocation(FVector(0.f, 0.f, 120.f));
    RoleIndicatorSphere->SetRelativeScale3D(FVector(0.4f));
    RoleIndicatorSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    static ConstructorHelpers::FObjectFinder<UStaticMesh> SphereMesh(TEXT("/Engine/BasicShapes/Sphere.Sphere"));
    if (SphereMesh.Succeeded())
        RoleIndicatorSphere->SetStaticMesh(SphereMesh.Object);

}

void ABossActor_RoleTarget::BeginPlay()
{
    Super::BeginPlay();

    SetActorTickEnabled(true);

    OnBossAttackEnded.AddDynamic(this, &ABossActor_RoleTarget::ResetIndicatorColor);

    IndicatorDynMat = RoleIndicatorSphere->CreateAndSetMaterialInstanceDynamic(0);
    if (IndicatorDynMat)
    {
        IndicatorDynMat->SetVectorParameterValue(TEXT("Color"), FLinearColor::Black);
        UE_LOG(LogTemp, Warning, TEXT("[RoleTarget] DynMat created OK"));
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("[RoleTarget] DynMat is NULL - material not set on sphere"));
    }
}

void ABossActor_RoleTarget::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (!bBlinking || !IndicatorDynMat) return;

    BlinkElapsed += DeltaTime;

    // 처음엔 빠르다가 점점 느려짐
    const float Speed = FMath::Max(0.3f, 3.f - BlinkElapsed * 0.5f);
    const float Cycle = FMath::Fmod(BlinkElapsed * Speed, 3.f);
    FLinearColor BlinkColor;
    if (Cycle < 1.f)
        BlinkColor = FMath::Lerp(FLinearColor(3.f, 0.2f, 0.2f), FLinearColor(0.2f, 0.5f, 3.f), Cycle);
    else if (Cycle < 2.f)
        BlinkColor = FMath::Lerp(FLinearColor(0.2f, 0.5f, 3.f), FLinearColor(0.2f, 3.f, 0.2f), Cycle - 1.f);
    else
        BlinkColor = FMath::Lerp(FLinearColor(0.2f, 3.f, 0.2f), FLinearColor(3.f, 0.2f, 0.2f), Cycle - 2.f);

    IndicatorDynMat->SetVectorParameterValue(TEXT("Color"), BlinkColor);
}

void ABossActor_RoleTarget::StartRoleRoulette()
{
    bRoleLocked = false;
    LockedRoleClass = EWeaponClass::None;
    bBlinking = true;
    BlinkElapsed = 0.f;
    if (IndicatorDynMat)
        IndicatorDynMat->SetVectorParameterValue(TEXT("Color"), FLinearColor::Black);
    UE_LOG(LogTemp, Warning, TEXT("[RoleTarget] Roulette started"));
}

void ABossActor_RoleTarget::StopRoleRouletteAndLock()
{
    bBlinking = false;

    const int32 Roll = FMath::RandRange(0, 2);
    switch (Roll)
    {
        case 0: LockedRoleClass = EWeaponClass::Deal; break;
        case 1: LockedRoleClass = EWeaponClass::Tank; break;
        case 2: LockedRoleClass = EWeaponClass::Heal; break;
        default: LockedRoleClass = EWeaponClass::Deal; break;
    }
    bRoleLocked = true;

    if (IndicatorDynMat)
    {
        FLinearColor LockedColor;
        switch (LockedRoleClass)
        {
            case EWeaponClass::Deal: LockedColor = FLinearColor(3.f, 0.2f, 0.2f); break;
            case EWeaponClass::Tank: LockedColor = FLinearColor(0.2f, 0.5f, 3.f); break;
            case EWeaponClass::Heal: LockedColor = FLinearColor(0.2f, 3.f, 0.2f); break;
            default:                 LockedColor = FLinearColor::White;            break;
        }
        IndicatorDynMat->SetVectorParameterValue(TEXT("Color"), LockedColor);
        UE_LOG(LogTemp, Warning, TEXT("[RoleTarget] Color set: R=%.1f G=%.1f B=%.1f"), LockedColor.R, LockedColor.G, LockedColor.B);
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("[RoleTarget] DynMat null at lock time"));
    }


    UE_LOG(LogTemp, Warning, TEXT("[RoleTarget] Roulette locked: %d"), (int32)LockedRoleClass);
}

void ABossActor_RoleTarget::SetIndicatorColor(FLinearColor Color)
{
    if (IndicatorDynMat)
        IndicatorDynMat->SetVectorParameterValue(TEXT("Color"), Color);
}

void ABossActor_RoleTarget::ResetIndicatorColor()
{
    if (IndicatorDynMat)
        IndicatorDynMat->SetVectorParameterValue(TEXT("Color"), FLinearColor::Black);
}
