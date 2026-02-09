// Fill out your copyright notice in the Description page of Project Settings.


#include "BattlePlayerController_FlipSide.h"
#include "EnhancedInputSubsystems.h"
#include "BattlePlayerPawn_FlipSide.h"
#include "BattleArea.h"
#include "CoinActor.h"
#include "LeverActor.h"
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
    // 배틀 매니저를 가져와서 현재 턴이 '준비 단계'인지 확인합니다.
    UBattleManagerWSubsystem* BattleSubsystem = GetWorld()->GetSubsystem<UBattleManagerWSubsystem>();
    if (BattleSubsystem && BattleSubsystem->GetCurrentTurn() != ETurnState::CoinReadyTurn)
    {
        // 준비 단계가 아니면(즉, 레버를 당겼다면) 클릭 로직 전체를 무시
        return; 
    }

    FHitResult Hit;

    // [1단계] 코인 클릭 체크 (ECC_Camera)
    if (GetHitResultUnderCursor(ECC_Camera, false, Hit))
    {
        ACoinActor* ClickedCoin = Cast<ACoinActor>(Hit.GetActor());

        // [정리] 이제 코인이 제대로 잡히므로 StaticMeshActor 대조(GetCoinByName) 로직은 삭제합니다.
        if (ClickedCoin)
        {
            UCoinManagementWSubsystem* CoinSubsystem = GetWorld()->GetSubsystem<UCoinManagementWSubsystem>();
            if (!CoinSubsystem) return;

            int32 TargetID = ClickedCoin->GetCoinID();
            
            // 서랍에 있는지 확인
            if (CoinSubsystem->IsCoinIdInBattleReady(TargetID))
            {
                // [서랍 코인] 취소 실행
                UE_LOG(LogTemp, Warning, TEXT("#### [취소] 서랍의 %d번 코인을 슬롯으로 복귀시킵니다."), TargetID);
                CoinSubsystem->RemoveBattleReadyCoins(ClickedCoin);
                return;
            }
            // 슬롯의 맨 앞 코인인지 확인
            else if (ClickedCoin->GetSameTypeIndex() == 0)
            {
                // [슬롯 코인] 추가 실행
                UE_LOG(LogTemp, Warning, TEXT("#### [추가] 슬롯의 %d번 코인을 서랍으로 보냅니다."), TargetID);
                CoinSubsystem->AddBattleReadyCoins(ClickedCoin);
                return;
            }
        }

        // [추가 단계] 레버 클릭 체크
        // 코인을 클릭한 게 아니라면, 맞은 액터가 레버인지 확인
        else if (ALeverActor* ClickedLever = Cast<ALeverActor>(Hit.GetActor()))
        {
            ClickedLever->OnLeverInteracted();
            return; // 레버를 작동시켰으므로 함수 종료
        }
    }

    // [2단계] 영역 판(Plane) 클릭 체크 (ECC_Visibility)
    if (GetHitResultUnderCursor(ECC_Visibility, false, Hit))
    {
        if (CurrentHoveredArea && ControlledPawn)
        {
            ControlledPawn->MoveCameraToArea(
                CurrentHoveredArea->TargetLocation,
                CurrentHoveredArea->TargetRotation,
                CurrentHoveredArea->TargetArmLength
            );
            return;
        }
    }

    // [3단계] 빈 공간 클릭 시 기본 카메라로 복귀
    ReturnToDefaultCamera();
}

// 우클릭은 취소
void ABattlePlayerController_FlipSide::OnRightClick()
{
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