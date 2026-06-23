#pragma once

#include "CoreMinimal.h"
#include "BossActor.h"
#include "Scene_Test_Boss.generated.h"

UCLASS()
class FLIP_SIDE_API AScene_Test_Boss : public ABossActor
{
	GENERATED_BODY()

public:
	AScene_Test_Boss();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss")
	TObjectPtr<class UStaticMeshComponent> BossStaticMesh;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss")
	bool bShowSkeletalMesh = false;

protected:
	virtual void OnConstruction(const FTransform& Transform) override;
	virtual void BeginPlay() override;

private:
	void UpdateSkeletalMeshVisibility();
};
