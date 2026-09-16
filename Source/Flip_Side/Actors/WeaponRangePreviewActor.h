#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GridTypes.h"
#include "AttackAreaTypes.h"
#include "WeaponRangePreviewActor.generated.h"

class AGridActor;
class UChildActorComponent;
class USceneCaptureComponent2D;

UENUM(BlueprintType)
enum class EPreviewCoinOrigin : uint8
{
	Bottom UMETA(DisplayName="Bottom (4,0)"),
	Center UMETA(DisplayName="Center (4,2)")
};

/** DB 기본 사거리 전용. 전장 GridManager/버프 스냅숏과 독립적으로 슬롯 팝업을 촬영합니다. */
UCLASS()
class FLIP_SIDE_API AWeaponRangePreviewActor : public AActor
{
	GENERATED_BODY()
public:
	AWeaponRangePreviewActor();
	virtual void OnConstruction(const FTransform& Transform) override;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Preview|Components")
	TObjectPtr<USceneComponent> FrontAttackPreviewRangeComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Preview|Components")
	TObjectPtr<USceneComponent> FrontAbilityPreviewRangeComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Preview|Components")
	TObjectPtr<USceneComponent> BackAttackPreviewRangeComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Preview|Components")
	TObjectPtr<USceneComponent> BackAbilityPreviewRangeComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Preview|Capture")
	TObjectPtr<USceneCaptureComponent2D> FrontAttackCapture;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Preview|Capture")
	TObjectPtr<USceneCaptureComponent2D> FrontAbilityCapture;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Preview|Capture")
	TObjectPtr<USceneCaptureComponent2D> BackAttackCapture;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Preview|Capture")
	TObjectPtr<USceneCaptureComponent2D> BackAbilityCapture;

	// 기존 BP_Grid 재사용. 네 그룹의 상대 Transform은 컴포넌트에서 편집합니다.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Preview|Grid")
	TSubclassOf<AGridActor> GridActorClass;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Preview|Grid", meta=(ClampMin="1"))
	float SpacingX = 440.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Preview|Grid", meta=(ClampMin="1"))
	float SpacingY = 440.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Preview|Ability")
	TMap<EAttackAreaPattern, EPreviewCoinOrigin> AbilityCoinOrigins;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Preview|Visual")
	FLinearColor AttackHighlightColor = FLinearColor(1.f, 0.5f, 0.f);
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Preview|Visual")
	FLinearColor AbilityHighlightColor = FLinearColor(0.2f, 0.5f, 1.f);
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Preview|Visual")
	FLinearColor CoinCellColor = FLinearColor(0.f, 1.f, 0.3f);
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Preview|Visual")
	FLinearColor DefaultCellColor = FLinearColor::White;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Preview|Visual")
	float RangeFillIntensity = 0.8f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Preview|Visual")
	float DefaultFillIntensity = 0.4f;

	// PlayerController에서 TryGetWeapon으로 조회한 기본 정의만 전달합니다.
	void ShowDefinitionPreview(const FAttackAreaSpec& FrontAttack, const FAttackAreaSpec& FrontAbility,
		bool bFrontAbility, const FAttackAreaSpec& BackAttack, const FAttackAreaSpec& BackAbility, bool bBackAbility);
	UFUNCTION(BlueprintCallable, Category="Preview")
	void ClearPreview();
	UFUNCTION(BlueprintCallable, CallInEditor, Category="Preview")
	void RebuildGrid();
	// 기존 BP/C++ 참조 보존용. 새 슬롯 경로에서는 호출하지 않습니다.
	UFUNCTION(BlueprintCallable, Category="Preview", meta=(DeprecatedFunction, DeprecationMessage="Use definition-only slot preview"))
	void ShowBothFacePreview(const FAttackAreaSpec& FrontSpec, const FAttackAreaSpec& BackSpec);
	UFUNCTION(BlueprintCallable, Category="Preview", meta=(DeprecatedFunction, DeprecationMessage="World-cell preview is no longer supported"))
	void ShowPreview(const FGridPoint& CoinXY, const TArray<FGridPoint>& RangeCells);

	// 설명용 보드의 순수 계산 함수. 실제 전장 범위를 변경하지 않습니다.
	static void BuildPreviewCells(const FAttackAreaSpec& Spec, bool bAttack, EPreviewCoinOrigin Origin,
		TArray<FGridPoint>& OutCells, FGridPoint& OutCoin, bool& bOutShowCoin);
protected:
	virtual void BeginPlay() override;
private:
	UPROPERTY()
	TArray<TObjectPtr<UChildActorComponent>> PreviewCells;
	void ConfigureCells();
	void ApplyRange(int32 Group, const FAttackAreaSpec& Spec, bool bEnabled);
	void CapturePreviews();
};
