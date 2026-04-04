#include "Base_OtherActor.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SceneComponent.h"
#include "Components/WidgetComponent.h"

ABase_OtherActor::ABase_OtherActor()
{
	PrimaryActorTick.bCanEverTick = true;
	
	OthersRootComp = CreateDefaultSubobject<USceneComponent>(TEXT("Root Scene Component"));
	RootComponent = OthersRootComp;

	OthersMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Others Mesh"));
	OthersMesh->SetupAttachment(RootComponent);

	OtherHPUI = CreateDefaultSubobject<UWidgetComponent>(TEXT("Others HP UI"));
	OtherHPUI->SetupAttachment(RootComponent);
}

void ABase_OtherActor::BeginPlay()
{
	Super::BeginPlay();
	
	CurrentHP = OtherActorMaxHP;
}

void ABase_OtherActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ABase_OtherActor::ApplyDamage(const int32 damage, AActor* damageCauser)
{
	if(!damageCauser) return;
	
	int32 FinalHP = CurrentHP - damage;
	if(FinalHP <= 0)
	{
		OnDead();
	}

	CurrentHP = FinalHP;
	//UI 바꾸는 함수
}

void ABase_OtherActor::ApplyHeal(const int32 heal, AActor* healCauser)
{
	if(!healCauser) return;
	CurrentHP = FMath::Clamp(CurrentHP + heal, 0, OtherActorMaxHP);
	//UI바꾸는 함수
}

void ABase_OtherActor::OnDead()
{
	ActingDeadEffect();
	//Timer 
	Destroy();
}

void ABase_OtherActor::OnClicked_Implementation()
{
	UE_LOG(LogTemp, Warning, TEXT("DFSAD"));
}