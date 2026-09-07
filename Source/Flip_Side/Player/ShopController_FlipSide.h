#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "ShopController_FlipSide.generated.h"

class UInputMappingContext;
class AShopPlayerPawn_FlipSide;
class UW_ShopWidget;
class UUserWidget;
class AShopItemUIActor;
class AShopCoinUIActor;
class AShopUnlockWeaponUIActor;
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

public:
	virtual void BeginPlay() override;
	virtual void PlayerTick(float DeltaTime) override; // 매 프레임 마우스 호버 감지용
/* UI */
protected:
	//상점 메인 UI
	UPROPERTY(EditDefaultsOnly, Category = "UI", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UUserWidget> ShopMainWidgetClass;

	UPROPERTY(EditDefaultsOnly, Category = "UI", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UUserWidget> BlockWidgetClass;

	UPROPERTY(EditDefaultsOnly, Category = "UI", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UUserWidget> ShopModeWidgetClass;

	UPROPERTY(EditDefaultsOnly, Category = "UI", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UUserWidget> ShopWidgetContainerClass;

protected:
	UPROPERTY()
	TObjectPtr<AShopItemUIActor> ShopItemUIActor;

	UPROPERTY()
	TObjectPtr<AShopCoinUIActor> ShopCoinUIActor;

	UPROPERTY()
	TObjectPtr<AShopUnlockWeaponUIActor> ShopUnlockWeaponUIActor;

	UPROPERTY()
	TObjectPtr<class UShopItemPresenter> ItemPresenter;

	UPROPERTY()
	TObjectPtr<class UShopCardPresenter> CardPresenter;
	UPROPERTY()
	TObjectPtr<class UShopCoinPresenter> CoinPresenter;

	UPROPERTY()
	TObjectPtr<class UUnlockWeaponPresenter> UnlockWeaponPresenter;

	UPROPERTY()
	TObjectPtr<class UShopPageChangePresenter> PageChangePresenter;

	UPROPERTY()
	TObjectPtr<class AShopUISelectRegistry> ShopUISelectRegistry;

//UI 스크립트
private:

	UPROPERTY()
	UUserWidget* ShopMainWidget;
	UPROPERTY()
	UUserWidget* BlockWidget;
	UPROPERTY()
	UUserWidget* ShopModeWidget;
	UPROPERTY()
	TObjectPtr<class UW_ShopWidgetContainer> ShopWidgetContainer;
protected:
	void InitWidget(TSubclassOf<UUserWidget> WidgetClass, UUserWidget*& widget,int32 ZOrder);

public:
	AShopController_FlipSide();

	virtual void SetupInputComponent() override;

	virtual void OnPossess(APawn* InPawn);

private:
	void TryInitPageChangePresenter();

public:
	UFUNCTION()
	void ViewWidgetList();

	UFUNCTION()
	void HideWidgetList();

	UFUNCTION()
	void AddOpenWidgetList(UUserWidget* AddWidget);
//모드 변경 될 때 함수들
public:
	//튜토리얼 레거시 메인 위젯 전환
	UFUNCTION()
	void SetShopMainModeWidget();

	void SetLockMouse(bool IsMouseLock);

protected:
	void InitShopUISelectActor();
	
private:
    FVector DefaultCameraLocation;
    FRotator DefaultCameraRotation;
    float DefaultCameraArmLength;

protected:

    void OnLeftClick();

	void OnRightClick();

    void CheckMouseHover();

private:
	AActor* CurrentHoverActor;

	bool CanClick;

};
