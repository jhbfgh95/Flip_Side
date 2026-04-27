#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "ShopController_FlipSide.generated.h"

class UInputMappingContext;
class AShopPlayerPawn_FlipSide;
class UW_ShopWidget;
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

public:

	virtual void BeginPlay() override;

	virtual void PlayerTick(float DeltaTime) override; // 매 프레임 마우스 호버 감지용
/* UI */
protected:
	UPROPERTY(EditDefaultsOnly, Category = "UI", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UUserWidget> ShopWidgetClass;

	UPROPERTY(EditDefaultsOnly, Category = "UI", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UUserWidget> BossWidgetClass;
	//상점 메인 UI
	UPROPERTY(EditDefaultsOnly, Category = "UI", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UUserWidget> ShopMainWigetClass;
	
	//코인 관리 UI
	UPROPERTY(EditDefaultsOnly, Category = "UI", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UUserWidget> CoinManageWidgetClass;

	//코인 관리 UI
	UPROPERTY(EditDefaultsOnly, Category = "UI", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UUserWidget> ShopItemWidgetClass;

	UPROPERTY(EditDefaultsOnly, Category = "UI", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UUserWidget> SelectCardWidgetClass;

	UPROPERTY(EditDefaultsOnly, Category = "UI", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UUserWidget> UnlockWeaponWidgetClass;

	UPROPERTY(EditDefaultsOnly, Category = "UI", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UUserWidget> BlockWidgetClass;

//UI 스크립트
private:
	UPROPERTY()
	UUserWidget* HudShopWidget;
	UPROPERTY()
	UUserWidget* ShopMainWiget;
	UPROPERTY()
	UUserWidget* CoinManageWidget;
	UPROPERTY()
	UUserWidget* ShopItemWidget;
	UPROPERTY()
	UUserWidget* SelectCardWidget;
	UPROPERTY()
	UUserWidget* UnlockWeaponWidget;
	UPROPERTY()
	UUserWidget* BossWidget;
	UPROPERTY()
	UUserWidget* BlockWidget;
private:
	void InitWidget(TSubclassOf<UUserWidget> WidgetClass, UUserWidget*& widget,int32 ZOrder);

private:
	class AGameMode_Shop* ShopGameMode;

public:
	AShopController_FlipSide();

	virtual void SetupInputComponent() override;

	virtual void OnPossess(APawn* InPawn);
	
	UFUNCTION()
	void ViewWidgetList();

	UFUNCTION()
	void HideWidgetList();

	UFUNCTION()
	void AddOpenWidgetList(UUserWidget* AddWidget);
//모드 변경 될 때 함수들
public:
	
	UFUNCTION()
	void SetCoinManageModeWidget();

	UFUNCTION()
	void SetShopMainModeWidget();
	
	UFUNCTION()
	void SetShopItemModeWidget();

	UFUNCTION()
	void SetSelectCardModeWidget();

	UFUNCTION()
	void SetUnlockWeaponModeWidget();

	UFUNCTION()
	void SetBossStateModeWidget();

	void HideModeChangeWidget();

	void ShowModeChangeWidget();

	void SetLockMouse(bool IsMouseLock);
	
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
