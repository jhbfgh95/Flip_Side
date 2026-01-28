// Fill out your copyright notice in the Description page of Project Settings.


#include "BattlePlayerController_FlipSide.h"
#include "EnhancedInputSubsystems.h"
#include "BattlePlayerPawn_FlipSide.h"
#include "BattleArea.h"
#include "CoinActor.h"
#include "Subsystem/CoinManagementWSubsystem.h"

ABattlePlayerController_FlipSide::ABattlePlayerController_FlipSide()
{
    bShowMouseCursor = true;
    DefaultMouseCursor = EMouseCursor::Crosshairs;
}

void ABattlePlayerController_FlipSide::SetupInputComponent()
{
	Super::SetupInputComponent();

    if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		Subsystem->AddMappingContext(InputContext, 0);
	}

    // 마우스 좌클릭 바인딩
    InputComponent->BindKey(EKeys::LeftMouseButton, IE_Pressed, this, &ABattlePlayerController_FlipSide::OnLeftClick);
}

void ABattlePlayerController_FlipSide::PlayerTick(float DeltaTime)
{
    Super::PlayerTick(DeltaTime);
    CheckMouseHover();
}

void ABattlePlayerController_FlipSide::CheckMouseHover()
{
    FHitResult Hit;
    // 마우스 아래 대상을 감지 (ECC_Visibility 채널 사용)
    if (GetHitResultUnderCursor(ECC_Visibility, false, Hit))
    {
        AActor* HitActor = Hit.GetActor();
        // 1. 영역 액터(ABattleArea)인지 확인
        ABattleArea* TargetArea = Cast<ABattleArea>(HitActor);
        
        // 이전에 호버링하던 곳과 지금 마우스 아래 있는 곳이 다를 때만 실행
        if (CurrentHoveredArea != TargetArea)
        {
            // 1. 이전 영역의 하이라이트 끄기
            if (CurrentHoveredArea)
            {
                CurrentHoveredArea->SetHighlight(false);
            }

            // 2. 새로운 영역 기억하기
            CurrentHoveredArea = TargetArea;

            // 3. 새로운 영역의 하이라이트 켜기
            if (CurrentHoveredArea)
            {
                CurrentHoveredArea->SetHighlight(true);
            }
        }
    }
    else
    {
        // 마우스가 허공을 보거나 아무 영역도 아닐 때
        if (CurrentHoveredArea)
        {
            CurrentHoveredArea->SetHighlight(false);
            CurrentHoveredArea = nullptr;
        }
    }
}

// 좌클릭은 선택 또는 카메라 이동
void ABattlePlayerController_FlipSide::OnLeftClick()
{
    FHitResult Hit;
    // 1. 마우스 레이저 발사 (Visibility 채널 사용)
    if (GetHitResultUnderCursor(ECC_Visibility, false, Hit))
    {
        AActor* HitActor = Hit.GetActor();
        if (!HitActor) return;

        // 2. 클릭한 게 코인인지 확인 (ACoinActor 클래스인지 체크)
        ACoinActor* ClickedCoin = Cast<ACoinActor>(HitActor);
        if (ClickedCoin)
        {
            // 코인매니저 서브시스템
            UCoinManagementWSubsystem* CoinSubsystem = GetWorld()->GetSubsystem<UCoinManagementWSubsystem>();
            if (CoinSubsystem)
            {
                CoinSubsystem->AddBattleReadyCoins(ClickedCoin);
            }
            return;
        }

        // 마우스가 어떤 영역 위에 있고, 폰이 유효하다면 카메라 이동 명령
        if (CurrentHoveredArea && ControlledPawn)
        {
            ControlledPawn->MoveCameraToArea(
                CurrentHoveredArea->TargetLocation,
                CurrentHoveredArea->TargetRotation,
                CurrentHoveredArea->TargetArmLength
            );
        }
    }
}

// 우클릭은 취소
void ABattlePlayerController_FlipSide::OnRightClick()
{
    FHitResult Hit;
    if (GetHitResultUnderCursor(ECC_Visibility, false, Hit))
    {
        ACoinActor* ClickedCoin = Cast<ACoinActor>(Hit.GetActor());
        if (ClickedCoin)
        {
            UCoinManagementWSubsystem* CoinSubsystem = GetWorld()->GetSubsystem<UCoinManagementWSubsystem>();
            if (CoinSubsystem)
            {
                // 코인매니저 서브시스템에 취소 함수. 논의 후 만들 예정
                // CoinSubsystem->RemoveBattleReadyCoin(ClickedCoin);
            }
        }
    }
}

void ABattlePlayerController_FlipSide::OnPossess(APawn* InPawn)
{
    Super::OnPossess(InPawn);

    ControlledPawn = Cast<ABattlePlayerPawn_FlipSide>(InPawn);
    check(ControlledPawn);
}