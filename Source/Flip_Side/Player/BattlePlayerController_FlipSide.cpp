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
    // 마우스 우클릭 바인딩
    InputComponent->BindKey(EKeys::RightMouseButton, IE_Pressed, this, &ABattlePlayerController_FlipSide::OnRightClick);
}

void ABattlePlayerController_FlipSide::PlayerTick(float DeltaTime)
{
    Super::PlayerTick(DeltaTime);
    CheckMouseHover();
}

void ABattlePlayerController_FlipSide::BeginPlay()
{
    Super::BeginPlay();

    if (ControlledPawn)
    {
        // 디폴트 카메라 시점 저장
        DefaultCameraLocation = FVector(-1656.599739f, -1043.456394f, 2234.021618f); 
        DefaultCameraRotation = FRotator(-35.0f, -0.20f, 0.0f);
        DefaultCameraArmLength = 0.0f;
    }
}

void ABattlePlayerController_FlipSide::ReturnToDefaultCamera()
{
    if (ControlledPawn)
    {
        ControlledPawn->MoveCameraToArea(DefaultCameraLocation, DefaultCameraRotation, DefaultCameraArmLength);
    }
}

// 좌클릭은 선택 또는 카메라 이동
void ABattlePlayerController_FlipSide::OnLeftClick()
{
    FHitResult Hit;

    // [1단계] 판(Plane)을 무시하고 코인을 먼저 찾기 (ECC_Camera 채널)
    // 에디터에서 영역 판의 Camera 응답이 Ignore로 되어 있어야 합니다.
    if (GetHitResultUnderCursor(ECC_Camera, false, Hit))
    {
        ACoinActor* ClickedCoin = Cast<ACoinActor>(Hit.GetActor());
        if (ClickedCoin)
        {
            // [조건 추가] 맨 앞에 있는 코인(SameTypeIndex == 0)만 선택 가능
            // SameTypeIndex 변수가 ACoinActor에 public으로 선언되어 있어야 합니다.
            if (ClickedCoin->GetSameTypeIndex() == 0)
            {
                UCoinManagementWSubsystem* CoinSubsystem = GetWorld()->GetSubsystem<UCoinManagementWSubsystem>();
                if (CoinSubsystem)
                {
                    CoinSubsystem->AddBattleReadyCoins(ClickedCoin);
                }
                return; // 코인 선택 성공 시 종료 (아래 영역 로직 방지)
            }
            // 맨 앞이 아니라면 클릭을 무시하고 영역 판 체크(2단계)로 넘어갑니다.
        }
    }

    // [2단계] 코인이 안 잡혔거나 맨 앞이 아니었다면, 영역 판(Visibility 채널)을 찾기
    if (GetHitResultUnderCursor(ECC_Visibility, false, Hit))
    {
        // 마우스가 영역(Plane) 위에 있고, 폰이 유효하다면 카메라 이동
        if (CurrentHoveredArea && ControlledPawn)
        {
            ControlledPawn->MoveCameraToArea(
                CurrentHoveredArea->TargetLocation,
                CurrentHoveredArea->TargetRotation,
                CurrentHoveredArea->TargetArmLength
            );
            return; // 영역 이동 성공 시 종료
        }
    }

    // 3단계: 아무것도 맞지 않았을 때 (빈 공간 클릭)
    ReturnToDefaultCamera();
}

// 우클릭은 취소
void ABattlePlayerController_FlipSide::OnRightClick()
{
    /* 기존
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
    */

    // 화면 어디를 누르든 상관없이 디폴트 시점으로 복귀
    ReturnToDefaultCamera();
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

void ABattlePlayerController_FlipSide::OnPossess(APawn* InPawn)
{
    Super::OnPossess(InPawn);

    ControlledPawn = Cast<ABattlePlayerPawn_FlipSide>(InPawn);
    check(ControlledPawn);
}