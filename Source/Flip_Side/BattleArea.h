#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BattleArea.generated.h"

UCLASS()
class ABattleArea : public AActor
{
    GENERATED_BODY()
    
public:
    ABattleArea();

    // 카메라 구도 데이터 (에디터에서 영역마다 다르게 설정)

    // 카메라가 이 영역을 비출 때 Pawn이 가야 할 월드 좌표
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Camera Config")
    FVector TargetLocation;

    // 카메라가 이 영역을 비출 때 SpringArm의 회전값 (Pitch 조절용)
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Camera Config")
    FRotator TargetRotation;

    // 카메라가 이 영역을 비출 때의 줌 정도 (SpringArm 길이)
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Camera Config")
    float TargetArmLength;

    // 하이라이트 상태를 변경
    void SetHighlight(bool bHighlight);
protected:
    // 에디터에서 영역의 크기를 조절하고 마우스 레이를 맞을 투명한 판
    UPROPERTY(VisibleAnywhere, Category = "Components")
    class UStaticMeshComponent* AreaMesh;

    // 에디터에서 설정할 하이라이트용 머티리얼 (색상이나 테두리)
    UPROPERTY(EditAnywhere, Category = "Appearance")
    class UMaterialInterface* HighlightMaterial;

    // 원래 머티리얼
    UPROPERTY()
    class UMaterialInterface* DefaultMaterial;
};