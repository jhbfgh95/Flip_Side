#include "BattleArea.h"
#include "Components/StaticMeshComponent.h"

ABattleArea::ABattleArea()
{
    PrimaryActorTick.bCanEverTick = false;

    AreaMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("AreaMesh"));
    RootComponent = AreaMesh;

    // 마우스 레이트레이싱을 위한 설정

    // 1. 모든 충돌 무시 (테이블이나 코인 액터랑 겹쳐 있어도 서로 밀어내지 않음)
    AreaMesh->SetCollisionResponseToAllChannels(ECR_Ignore);
    
    // 2. '마우스 레이(Visibility)'만큼은 통과시키지 않고 막아라 (클릭 감지용)
    AreaMesh->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
    
    // 3. 판데기가 보이면 안 되니까 투명하게 처리
    AreaMesh->SetHiddenInGame(true);

    TargetArmLength = 1000.0f;
    TargetRotation = FRotator(-45.0f, 0.0f, 0.0f);
}

void ABattleArea::SetHighlight(bool bIsHighlighted)
{
    // 메시 컴포넌트가 없으면 중단
    if (!AreaMesh) return;

    if (bIsHighlighted)
    {
        // 1. 하이라이트 켤 때
        if (HighlightMaterial)
        {
            // 현재 머티리얼을 보관 (나중에 되돌리기 위해)
            DefaultMaterial = AreaMesh->GetMaterial(0);
            
            // 강조용 머티리얼로 교체
            AreaMesh->SetMaterial(0, HighlightMaterial);
            
            // 평소에 숨겨뒀다면, 하이라이트 될 때만 잠시 보이게 함
            AreaMesh->SetHiddenInGame(false);
        }
    }
    else
    {
        // 2. 하이라이트 끌 때
        if (DefaultMaterial)
        {
            // 원래 머티리얼로 복구
            AreaMesh->SetMaterial(0, DefaultMaterial);
        }
        
        // 다시 유령 상태(투명)로 숨김
        AreaMesh->SetHiddenInGame(true);
    }
}