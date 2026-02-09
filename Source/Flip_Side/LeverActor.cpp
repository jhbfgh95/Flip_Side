#include "LeverActor.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Subsystem/CoinManagementWSubsystem.h" 
#include "Subsystem/BattleManagerWSubsystem.h" 

ALeverActor::ALeverActor()
{
    PrimaryActorTick.bCanEverTick = false;

    // 1. 루트 컴포넌트 설정 (SceneComponent)
    USceneComponent* Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
    SetRootComponent(Root);

    // 2. 베이스 메쉬 설정
    BaseMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BaseMesh"));
    BaseMesh->SetupAttachment(Root);

    // 3. 레버 손잡이 메쉬 설정
    LeverMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("LeverMesh"));
    LeverMesh->SetupAttachment(Root);
    
    // 클릭 이벤트를 받기 위한 설정
    BaseMesh->SetGenerateOverlapEvents(false);
    LeverMesh->SetGenerateOverlapEvents(true);
}

void ALeverActor::BeginPlay()
{
    Super::BeginPlay();
    
    // 초기 각도 설정 (배틀 전 -60도)
    LeverMesh->SetRelativeRotation(FRotator(-60.f, 0.f, 0.f));
}

void ALeverActor::OnLeverInteracted()
{
    if (bIsPulled) return;

    UWorld* World = GetWorld();
    if (!World) return;

    // 1. 코인락 실행
    if (UCoinManagementWSubsystem* CoinSubsystem = World->GetSubsystem<UCoinManagementWSubsystem>())
    {
        CoinSubsystem->LockCoinReady();
    }

    // 2. 턴 진행 (방금 만드신 Public 래퍼 함수 호출)
    if (UBattleManagerWSubsystem* BattleSubsystem = World->GetSubsystem<UBattleManagerWSubsystem>())
    {
        BattleSubsystem->StartBattleFromLever();
    }

    // 3. 연출 실행
    PlayLeverAnimation();
    
    bIsPulled = true;
    UE_LOG(LogTemp, Warning, TEXT("#### 레버 상호작용 성공! ####"));
}

void ALeverActor::PlayLeverAnimation()
{
    FLatentActionInfo LatentInfo;
    LatentInfo.CallbackTarget = this;
    LatentInfo.UUID = GetUniqueID();
    LatentInfo.Linkage = 0;

    // Yaw를 -60에서 60으로 0.5초 동안 부드럽게 회전
    // 위치(FVector::ZeroVector)는 상대 좌표이므로 Pivot이 같다면 고정됨
    UKismetSystemLibrary::MoveComponentTo(
        LeverMesh,
        FVector::ZeroVector,      
        FRotator(60.f, 0.f, 0.f),  
        false,                    // bForceShortestRotationPath: 수치대로 120도 회전하도록 false
        true,                     // bEaseOut: 도착 시 부드럽게 감속
        0.5f, 
        false, 
        EMoveComponentAction::Move, 
        LatentInfo
    );
}