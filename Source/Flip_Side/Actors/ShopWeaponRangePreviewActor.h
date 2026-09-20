#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Actors/WeaponRangePreviewActor.h"
#include "DataTypes/WeaponDataTypes.h"
#include "ShopWeaponRangePreviewActor.generated.h"

/** 상점 한 면의 DB 기본 사거리만 표시합니다. 데이터 조회와 호버 수명은 호출자가 관리합니다. */
UCLASS()
class FLIP_SIDE_API AShopWeaponRangePreviewActor : public AActor
{
	GENERATED_BODY()
public:
	AShopWeaponRangePreviewActor();
	virtual void OnConstruction(const FTransform& Transform) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Preview|Components")
	TObjectPtr<USceneComponent> AttackPreviewRangeComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Preview|Components")
	TObjectPtr<USceneComponent> AbilityPreviewRangeComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Preview|Capture")
	TObjectPtr<USceneCaptureComponent2D> AttackCapture;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Preview|Capture")
	TObjectPtr<USceneCaptureComponent2D> AbilityCapture;

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

	/** TryGetWeapon 성공 결과 전달용. 내부에서 DB 쿼리/매니저 조회를 수행하지 않습니다. */
	UFUNCTION(BlueprintCallable, Category="Preview")
	void ShowWeaponDefinition(const FFaceData& WeaponDefinition);

	/** 같은 기능의 Spec 직접 입력 API. bHasAbilityArea=false이면 능력 강조와 기준 셀을 지웁니다. */
	UFUNCTION(BlueprintCallable, Category="Preview")
	void ShowDefinitionPreview(const FAttackAreaSpec& AttackSpec, const FAttackAreaSpec& AbilitySpec, bool bHasAbilityArea);

	/** 호버/상세창 종료 및 조회 실패 시 호출합니다. 기본 셀은 유지하고 강조만 지웁니다. */
	UFUNCTION(BlueprintCallable, Category="Preview")
	void ClearPreview();
	UFUNCTION(BlueprintCallable, CallInEditor, Category="Preview")
	void RebuildGrid();

protected:
	virtual void BeginPlay() override;
private:
	UPROPERTY()
	TArray<TObjectPtr<UChildActorComponent>> PreviewCells;
	void ConfigureCells();
	void ApplyRange(int32 Group, const FAttackAreaSpec& Spec, bool bEnabled);
	void CapturePreviews();
};
