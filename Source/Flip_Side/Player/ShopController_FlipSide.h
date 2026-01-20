#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "ShopController_FlipSide.generated.h"

class UInputMappingContext;
class AShopPlayerPawn_FlipSide;
class UW_ShopWidget;
class UW_CoinCreateWidget;
class UUserWidget;
UCLASS(abstract)
class AShopController_FlipSide : public APlayerController
{
	GENERATED_BODY()

private:
	TArray<UUserWidget*> openWidgetList;
/* Input And Default Pawn*/
protected:
	TObjectPtr<AShopPlayerPawn_FlipSide> ControlledPawn;

	UPROPERTY(EditAnywhere, Category = "Input")
	UInputMappingContext* InputContext;

/* UI */
protected:
	UPROPERTY(EditDefaultsOnly, Category = "UI", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UW_ShopWidget> ShopWigetClass;

	
	UPROPERTY(EditDefaultsOnly, Category = "UI", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UW_CoinCreateWidget> coinCreateWigetClass;

	UPROPERTY()
	UW_ShopWidget* HudShopWidget;
	UPROPERTY()
	UW_CoinCreateWidget* coinCreateWidget;

private:
	class AGameMode_Shop* ShopGameMode;


public:
	AShopController_FlipSide();

	virtual void SetupInputComponent() override;

	virtual void OnPossess(APawn* InPawn);

	UW_CoinCreateWidget* GetCoinCreateWidget();



	
	UFUNCTION()
	void ViewWidgetList();

	UFUNCTION()
	void HideWidgetList();

	UFUNCTION()
	void AddOpenWidgetList(UUserWidget* AddWidget);
//모드 변경 될 때 함수들
public:
	UFUNCTION()
	void SetCoinCreateUI();
	
	UFUNCTION()
	void SetMainModeUI();
protected:
	virtual void BeginPlay() override;
};
