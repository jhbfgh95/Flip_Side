// Fill out your copyright notice in the Description page of Project Settings.


#include "MyActor.h"
#include "DataManagerSubsystem.h"
#include "SQLitePreparedStatement.h"
#include "Blueprint/UserWidget.h"
#include "Components/Image.h"
#include "Engine/Texture2D.h"
#include "Misc/Paths.h"
// Sets default values
AMyActor::AMyActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AMyActor::BeginPlay()
{
    Super::BeginPlay();

    auto* DM = GetGameInstance()->GetSubsystem<UDataManagerSubsystem>();
    FFaceData Weapon;

    if (!DM || !DM->IsCacheReady() || !DM->TryGetWeapon(1, Weapon))
    {
        UE_LOG(LogTemp, Error, TEXT("Weapon data not found"));
        return;
    }

    if (!Weapon.WeaponIcon)
    {
        UE_LOG(LogTemp, Error, TEXT("WeaponIcon is NULL"));
        return;
    }

    UE_LOG(LogTemp, Log, TEXT("WeaponIcon Loaded: %s"),
        *Weapon.WeaponIcon->GetName());

    UUserWidget* Widget = CreateWidget<UUserWidget>(
        GetWorld(),
        LoadClass<UUserWidget>(
            nullptr,
            TEXT("/Game/WBP_DebugIcon.WBP_DebugIcon_C") 
        )
    );

    if (!Widget)
    {
        UE_LOG(LogTemp, Error, TEXT("Widget create failed"));
        return;
    }

    Widget->AddToViewport();

    if (UImage* Img = Cast<UImage>(Widget->GetWidgetFromName(TEXT("Img_Icon"))))
    {
        Img->SetBrushFromTexture(Weapon.WeaponIcon, true);
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Img_Icon not found"));
    }
}

// Called every frame
void AMyActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

