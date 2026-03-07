// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/ShopUnlockWeapon/SelectUnlockWeaponUIActor.h"
#include "Components/StaticMeshComponent.h"
#include "Subsystem/ShopLevel/ShopUnlockWeaponWSubsystem.h"
#include "Subsystem/ShopLevel/ShopWeaponDataWSubsystem.h"
// Sets default values
ASelectUnlockWeaponUIActor::ASelectUnlockWeaponUIActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	CoinMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CoinMeshComponent"));
}

// Called when the game starts or when spawned
void ASelectUnlockWeaponUIActor::BeginPlay()
{
	Super::BeginPlay();
	ShopUnlockWeaponSubSystem = GetWorld()->GetSubsystem<UShopUnlockWeaponWSubsystem>();
	WeaponDataSubsystem = GetWorld()->GetSubsystem<UShopWeaponDataWSubsystem>();

	ShopUnlockWeaponSubSystem->OnSelectUnlockWeapon.AddDynamic(this , &ASelectUnlockWeaponUIActor::SetUnlockCoin);
}

void ASelectUnlockWeaponUIActor::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	ShopUnlockWeaponSubSystem->OnSelectUnlockWeapon.RemoveAll(this);
}
// Called every frame
void ASelectUnlockWeaponUIActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


void ASelectUnlockWeaponUIActor::SetUnlockCoin(EWeaponClass WeaponClass, int32 index, bool IsItemUnlock)
{
	UMaterialInstanceDynamic* MID = CoinMesh->CreateDynamicMaterialInstance(0);

	if(MID)
	{
		const FFaceData* FrontFaceData = WeaponDataSubsystem->GetWeaponDataByIndex(ShopUnlockWeaponSubSystem->GetCurrentWeaponClass()
		, ShopUnlockWeaponSubSystem->GetCurrentUnlockWeaponIndex());
		
		if(FrontFaceData)
		{
			UE_LOG(LogTemp, Warning, TEXT("앞면 아이콘 설정"));
			MID->SetTextureParameterValue(FName("Front_Texture"), FrontFaceData->WeaponIcon);
			MID->SetVectorParameterValue(FName("Front_Color"), FrontFaceData->TypeColor);

		}

		if(ShopUnlockWeaponSubSystem->GetCurrentUnlockWeaponIndex() == -1)
		{
			MID->SetVectorParameterValue(FName("Front_Color"), FLinearColor(0.f, 0.f, 0.f, 0.f));
		}
	}
}