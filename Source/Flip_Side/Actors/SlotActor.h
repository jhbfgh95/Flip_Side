#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SlotActor.generated.h"

class USceneComponent;

UCLASS(abstract)
class ASlotActor : public AActor
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"))
	USceneComponent* SlotRootComponent;

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* SlotMesh;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	USceneComponent* CoinSlotLocation;

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"))
	int32 SlotIndex;
public:	
	ASlotActor();

	UFUNCTION(BlueprintCallable, Category = "Slot")
	FTransform GetSlotTransform() const;

	int32 GetSlotIndex() const;
protected:
	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;
};
