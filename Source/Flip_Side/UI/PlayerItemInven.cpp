// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/PlayerItemInven.h"

#include "Components/TimelineComponent.h"
#include "Components/BoxComponent.h"
#include "UI/ShopPlayerItemActor.h"

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

	//잠김 판넬 운동 타임라인
	FOnTimelineFloat InveMoveCallBack;
	InveMoveCallBack.BindUFunction(this, FName("InvenMeshMovement"));
	InvenMoveTimeline->AddInterpFloat(InvenMeshCurve, InveMoveCallBack);	

	StartVector = InvenMesh->GetRelativeLocation();
	ArriveVector = StartVector + InvenMoveDirection;


	RootComponent->GetChildrenComponents(false, ChildComponents);

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
		return A.GetName() < B.GetName();
	});

	for(int i =0; i< PlayerItems.Num();i++)
	{
		PlayerItems[i]->InitItem(i);
	}
}

// Called every frame
void APlayerItemInven::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


void APlayerItemInven::InvenMeshMovement(float Value)
{
	FVector MoveVec = FMath::Lerp(StartVector, ArriveVector, Value);

	InvenMesh->SetRelativeLocation(MoveVec);
}