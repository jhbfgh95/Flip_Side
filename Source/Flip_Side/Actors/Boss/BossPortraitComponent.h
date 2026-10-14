// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneCaptureComponent2D.h"
#include "BossPortraitComponent.generated.h"

// 초상화에 무엇을 잡을지입니다. 머리 뼈가 얼굴 위치를 잘 알려주지 않는 리그는 UpperBody를 씁니다.
UENUM()
enum class EBossPortraitFraming : uint8
{
	// 머리 아래·위 뼈 사이를 얼굴로 봅니다.
	HeadBones,
	// 모델 전체 범위의 위쪽 절반을 잡습니다.
	UpperBody,
};

/**
 * 보스 얼굴만 찍어서 HUD 체력바의 둥근 창(초상화)에 보여주는 카메라입니다.
 * 보스가 플레이어 쪽을 보고 서 있으므로 머리에서 플레이어 카메라 쪽 수평 방향을 얼굴 앞으로 보고, 그 방향에 지면과 수평으로 둡니다.
 * 방향은 고정한 채 머리 위치(좌우·상하)만 따라가고, 보스 몸체만 그리고 배경은 투명하게 남깁니다.
 * 찍은 화면은 ABossActor::GetBossHUDData()를 통해 HUD로 전달됩니다.
 */
UCLASS(ClassGroup = (Boss), meta = (BlueprintSpawnableComponent))
class FLIP_SIDE_API UBossPortraitComponent : public USceneCaptureComponent2D
{
	GENERATED_BODY()

public:
	UBossPortraitComponent();

	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	class UTextureRenderTarget2D* GetPortraitRenderTarget() const { return PortraitRenderTarget; }

protected:
	UPROPERTY(EditAnywhere, Category = "Boss|Portrait")
	EBossPortraitFraming Framing = EBossPortraitFraming::HeadBones;

	// 머리 중심을 잡는 뼈 2개(머리 아래·위)입니다. 비워 두면 보스 뼈 이름에서 자동으로 찾습니다.
	UPROPERTY(EditAnywhere, Category = "Boss|Portrait")
	FName HeadBoneName;

	UPROPERTY(EditAnywhere, Category = "Boss|Portrait")
	FName HeadTopBoneName;

	// 화면 가운데를 머리 아래 뼈(0)와 머리 위 뼈(1) 사이 어디에 둘지입니다. 이마가 보이면 낮추고, 턱이 보이면 올립니다.
	UPROPERTY(EditAnywhere, Category = "Boss|Portrait", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float HeadCenterRatio = 0.5f;

	// 머리 높이(두 뼈 사이 거리)가 화면 높이에서 차지할 비율입니다. 작을수록 멀리서 찍습니다.
	UPROPERTY(EditAnywhere, Category = "Boss|Portrait", meta = (ClampMin = "0.1", ClampMax = "1.0"))
	float HeadFillRatio = 0.42f;

	// 머리 위 뼈가 없어서 머리 크기를 모를 때 쓰는 거리입니다(메시 단위, 메시 스케일을 곱해서 씁니다).
	UPROPERTY(EditAnywhere, Category = "Boss|Portrait", meta = (ClampMin = "1.0"))
	float DistanceInMeshUnits = 100.f;

	// 얼굴 앞 방향을 추가로 돌리는 각도입니다. 카메라가 뒤통수를 보면 180으로 바꿉니다.
	UPROPERTY(EditAnywhere, Category = "Boss|Portrait")
	float YawOffset = 0.f;

	UPROPERTY(EditAnywhere, Category = "Boss|Portrait")
	float PitchOffset = 0.f;

	// 화면 가운데를 머리 중심에서 위아래로 옮깁니다(메시 단위).
	UPROPERTY(EditAnywhere, Category = "Boss|Portrait")
	float VerticalOffsetInMeshUnits = 0.f;

	UPROPERTY(EditAnywhere, Category = "Boss|Portrait", meta = (ClampMin = "32", ClampMax = "1024"))
	int32 RenderTargetSize = 256;

	// 초당 찍는 횟수입니다. 작은 창이라 매 프레임 찍을 필요가 없습니다.
	UPROPERTY(EditAnywhere, Category = "Boss|Portrait", meta = (ClampMin = "1.0", ClampMax = "60.0"))
	float CaptureRate = 24.f;

private:
	UPROPERTY(Transient)
	TObjectPtr<class UTextureRenderTarget2D> PortraitRenderTarget;

	UPROPERTY(Transient)
	TObjectPtr<class USkeletalMeshComponent> TargetMesh;

	// 처음 한 번 정한 얼굴 앞 방향과 머리의 앞뒤 위치입니다. 이후에는 바꾸지 않습니다.
	FVector FaceForward = FVector::ForwardVector;
	float RestDepth = 0.f;
	bool bHasRestDepth = false;
	float CaptureAccumulator = 0.f;

	bool ResolveBones();
	// 머리 중심(월드)과 머리 높이(월드 단위, 모르면 0)를 돌려줍니다.
	bool GetHeadFrame(FVector& OutCenter, float& OutHeight) const;

	// 보스가 완전히 죽은 뒤에는 찍지 않습니다.
	UFUNCTION()
	void HandleBossDead();
};
