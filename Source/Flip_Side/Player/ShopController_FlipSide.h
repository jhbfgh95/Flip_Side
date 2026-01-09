#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "ShopController_FlipSide.generated.h"

class UInputMappingContext;
class AShopPlayerPawn_FlipSide;
class UW_ShopWidget;

UCLASS(abstract)
class AShopController_FlipSide : public APlayerController
{
	GENERATED_BODY()

/* Input And Default Pawn*/
protected:
	TObjectPtr<AShopPlayerPawn_FlipSide> ControlledPawn;

	UPROPERTY(EditAnywhere, Category = "Input")
	UInputMappingContext* InputContext;

/* UI */
protected:
	UPROPERTY(EditDefaultsOnly, Category = "UI", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UW_ShopWidget> ShopWigetClass;

	UPROPERTY()
	UW_ShopWidget* HudShopWidget;
public:
	AShopController_FlipSide();

	virtual void SetupInputComponent() override;

	virtual void OnPossess(APawn* InPawn);

protected:
	virtual void BeginPlay() override;
};
