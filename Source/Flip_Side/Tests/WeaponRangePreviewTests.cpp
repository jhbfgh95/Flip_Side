#if WITH_DEV_AUTOMATION_TESTS
#include "Misc/AutomationTest.h"
#include "Actors/WeaponRangePreviewActor.h"
#include "Actors/GridActor.h"
#include "Components/ChildActorComponent.h"
#include "Components/SceneCaptureComponent2D.h"
#include "Components/StaticMeshComponent.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FWeaponRangePreviewCellsTest,
	"FlipSide.UI.WeaponRangePreview.Cells", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FWeaponRangePreviewCellsTest::RunTest(const FString& Parameters)
{
	TArray<FGridPoint> Cells;
	FGridPoint Coin;
	bool bShowCoin = false;
	FAttackAreaSpec Spec;
	Spec.Pattern = EAttackAreaPattern::RectFromCell;
	Spec.ParamA = 1;
	Spec.ParamB = 5;
	for (int32 Length = 1; Length <= 5; ++Length)
	{
		Spec.ParamB = Length;
		AWeaponRangePreviewActor::BuildPreviewCells(Spec, true, EPreviewCoinOrigin::Center, Cells, Coin, bShowCoin);
		TestEqual(TEXT("Attack excludes caster from range count"), Cells.Num(), Length);
		TestTrue(TEXT("Attack origin"), bShowCoin && Coin == FGridPoint(0, 0));
		TestTrue(TEXT("Attack endpoint"), Cells.Contains(FGridPoint(0, Length)));
	}
	Spec.Pattern = EAttackAreaPattern::CircleOnCell;
	Spec.AnchorMode = EAreaAnchor::UseAnchorCell;
	Spec.ParamA = 1;
	AWeaponRangePreviewActor::BuildPreviewCells(Spec, false, EPreviewCoinOrigin::Center, Cells, Coin, bShowCoin);
	TestEqual(TEXT("3x3 ability"), Cells.Num(), 9);
	TestTrue(TEXT("Centered origin"), Coin == FGridPoint(4, 2) && bShowCoin);
	Spec.Flags = 1;
	AWeaponRangePreviewActor::BuildPreviewCells(Spec, false, EPreviewCoinOrigin::Center, Cells, Coin, bShowCoin);
	TestEqual(TEXT("Exclude caster"), Cells.Num(), 8);
	Spec.Flags = 0;
	Spec.ParamA = 8;
	AWeaponRangePreviewActor::BuildPreviewCells(Spec, false, EPreviewCoinOrigin::Bottom, Cells, Coin, bShowCoin);
	TestEqual(TEXT("Full board representative"), Cells.Num(), 45);
	TestFalse(TEXT("Full board has no caster marker"), bShowCoin);
	Spec.Pattern = EAttackAreaPattern::RectFromCell;
	Spec.ParamA = 1;
	Spec.ParamB = 5;
	Spec.Side = EAreaSide::Down;
	AWeaponRangePreviewActor::BuildPreviewCells(Spec, false, EPreviewCoinOrigin::Bottom, Cells, Coin, bShowCoin);
	TestEqual(TEXT("Rear five fits without clipping"), Cells.Num(), 5);
	TestTrue(TEXT("Rear rotated endpoint"), Cells.Contains(FGridPoint(3, 2)));
	TestTrue(TEXT("Rear fixed origin"), Coin == FGridPoint(8, 2));
	Spec.Side = EAreaSide::Up;
	Spec.ParamA = 3;
	Spec.ParamB = 3;
	AWeaponRangePreviewActor::BuildPreviewCells(Spec, false, EPreviewCoinOrigin::Bottom, Cells, Coin, bShowCoin);
	TestEqual(TEXT("Forward rectangle"), Cells.Num(), 9);
	TestTrue(TEXT("Bottom origin selection"), Coin == FGridPoint(4, 0));
	Spec.Pattern = EAttackAreaPattern::SingleCell;
	Spec.AnchorMode = EAreaAnchor::UseAnchorCell;
	Spec.ParamA = 1;
	AWeaponRangePreviewActor::BuildPreviewCells(Spec, false, EPreviewCoinOrigin::Center, Cells, Coin, bShowCoin);
	TestEqual(TEXT("SingleCell selection candidates"), Cells.Num(), 9);
	Spec.Pattern = EAttackAreaPattern::Border;
	AWeaponRangePreviewActor::BuildPreviewCells(Spec, false, EPreviewCoinOrigin::Center, Cells, Coin, bShowCoin);
	TestEqual(TEXT("Border"), Cells.Num(), 24);
	TestFalse(TEXT("Fixed shape hides caster"), bShowCoin);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FWeaponRangePreviewComponentsTest,
	"FlipSide.UI.WeaponRangePreview.Components", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FWeaponRangePreviewComponentsTest::RunTest(const FString& Parameters)
{
	const AWeaponRangePreviewActor* Preview = GetDefault<AWeaponRangePreviewActor>();
	TArray<UChildActorComponent*> Cells;
	Preview->GetComponents(Cells);
	TestEqual(TEXT("102 child grid slots"), Cells.Num(), 102);
	TArray<USceneCaptureComponent2D*> Captures;
	Preview->GetComponents(Captures);
	TestEqual(TEXT("Four integrated captures"), Captures.Num(), 4);
	TArray<UStaticMeshComponent*> Backgrounds;
	Preview->GetComponents(Backgrounds);
	TestEqual(TEXT("No background planes on preview actor"), Backgrounds.Num(), 0);
	for (const USceneCaptureComponent2D* Capture : Captures)
	{
		TestFalse(TEXT("No per-frame capture"), Capture->bCaptureEveryFrame);
		TestFalse(TEXT("No movement capture"), Capture->bCaptureOnMovement);
		TestTrue(TEXT("Capture moves with its range group"), Capture->GetAttachParent() != nullptr);
	}
	// 기존 BP 에셋은 저장/수정하지 않고 새 네이티브 컴포넌트 상속만 검증합니다.
	UClass* PreviewBP = LoadClass<AWeaponRangePreviewActor>(nullptr,
		TEXT("/Game/PreviewGrid/BP_WeaponRangePreviewActor.BP_WeaponRangePreviewActor_C"));
	if (TestNotNull(TEXT("Existing preview BP loads"), PreviewBP))
	{
		const AWeaponRangePreviewActor* BPDefaults = Cast<AWeaponRangePreviewActor>(PreviewBP->GetDefaultObject());
		if (TestNotNull(TEXT("Existing BP parent"), BPDefaults))
		{
			BPDefaults->GetComponents(Cells);
			BPDefaults->GetComponents(Captures);
			TestEqual(TEXT("Existing BP inherits 102 cells"), Cells.Num(), 102);
			TestEqual(TEXT("Existing BP inherits four captures"), Captures.Num(), 4);
			BPDefaults->GetComponents(Backgrounds);
			TestEqual(TEXT("Existing BP has no background plane"), Backgrounds.Num(), 0);
			TestNotNull(TEXT("Existing grid class retained"), BPDefaults->GridActorClass.Get());
		}
	}
	return true;
}
#endif
