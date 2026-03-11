// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/ShopItem/PlayerItemInven.h"

#include "Components/TimelineComponent.h"
#include "Components/BoxComponent.h"
#include "UI/ShopItem/ShopPlayerItemActor.h"

#include "Subsystem/ShopLevel/ShopItemWSubsystem.h"
// Sets default values
APlayerItemInven::APlayerItemInven()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	RootScene = CreateDefaultSubobject<USceneComponent>(TEXT("RootScene"));
	SetRootComponent(RootScene);
	InvenMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("InveMesh"));
	InvenMesh->SetupAttachment(RootScene);
	HandleCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("HandleBox"));
	HandleCollision->SetupAttachment(RootScene);
	InvenMoveTimeline= CreateDefaultSubobject<UTimelineComponent>(TEXT("InvenMovTimeline"));

}

// Called when the game starts or when spawned
void APlayerItemInven::BeginPlay()
{
	Super::BeginPlay();

	ShopItemSubsystem = GetWorld()->GetSubsystem<UShopItemWSubsystem>();
	ShopItemSubsystem->OnItemBuy.AddDynamic(this, &APlayerItemInven::SetItem);
	//잠김 판넬 운동 타임라인
	FOnTimelineFloat InveMoveCallBack;
	InveMoveCallBack.BindUFunction(this, FName("InvenMeshMovement"));
	InvenMoveTimeline->AddInterpFloat(InvenMeshCurve, InveMoveCallBack);	

	StartVector = GetActorLocation();
	ArriveVector = StartVector + InvenMoveDirection;

	RootComponent->GetChildrenComponents(false, ChildComponents);
	PlayerItems.Empty();
    for (USceneComponent* Component : ChildComponents)
    {
        if (UChildActorComponent* ChildComp = Cast<UChildActorComponent>(Component))
		{
			if (AShopPlayerItemActor* ItemActor = Cast<AShopPlayerItemActor>(ChildComp->GetChildActor()))
			{
				PlayerItems.Add(ItemActor);
			}
		}
    }

	PlayerItems.Sort([](const AShopPlayerItemActor& A, const AShopPlayerItemActor& B)
	{
		return A.ItemIndex < B.ItemIndex;
	});

	for(int i =0; i< PlayerItems.Num(); i++)
	{
		UE_LOG(LogTemp, Warning, TEXT("%f"), PlayerItems[i]->GetActorLocation().X);
	}
		


	for(int i =0; i< PlayerItems.Num();i++)
	{
		PlayerItems[i]->InitItem(i);
	}
}
void APlayerItemInven::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	
	ShopItemSubsystem->OnItemBuy.RemoveAll(this);
	Super::EndPlay(EndPlayReason);
}

void APlayerItemInven::SetItem(int32 ItemIndex)
{
	UE_LOG(LogTemp, Warning, TEXT("아이템 인덱스%d"), ItemIndex);
	if(0<=ItemIndex&&ItemIndex< PlayerItems.Num())
		PlayerItems[ItemIndex]->InitItem(ItemIndex);
}

void APlayerItemInven::InteractLeftClick_Implementation()
{
	ActiveInven();
}

void APlayerItemInven::InvenMeshMovement(float Value)
{
	FVector MoveVec = FMath::Lerp(StartVector, ArriveVector, Value);

	SetActorLocation(MoveVec);
}


void APlayerItemInven::ActiveInven()
{
	if(IsInvenOpen)
	{
		InvenMoveTimeline->ReverseFromEnd();
		IsInvenOpen = false;
	}
	else
	{
		InvenMoveTimeline->PlayFromStart();
		IsInvenOpen = true;
	}
}