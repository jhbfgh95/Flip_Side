#if WITH_DEV_AUTOMATION_TESTS
#include "Misc/AutomationTest.h"
#include "Actors/ShopWeaponRangePreviewActor.h"
#include "Components/ChildActorComponent.h"
#include "Components/SceneCaptureComponent2D.h"
#include "Components/StaticMeshComponent.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FShopWeaponRangePreviewComponentsTest,
	"FlipSide.UI.ShopWeaponRangePreview.Components",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FShopWeaponRangePreviewComponentsTest::RunTest(const FString& Parameters)
{
	const AShopWeaponRangePreviewActor* Preview = GetDefault<AShopWeaponRangePreviewActor>();
	TestFalse(TEXT("Shop actor does not inherit battle actor and its four grids"),
		Preview->IsA<AWeaponRangePreviewActor>());
	TArray<UChildActorComponent*> Cells;
	Preview->GetComponents(Cells);
	TestEqual(TEXT("Only 51 child cells"), Cells.Num(), 51);
	int32 AttackCells = 0;
	int32 AbilityCells = 0;
	for (const UChildActorComponent* Cell : Cells)
	{
		if (Cell->GetAttachParent() == Preview->AttackPreviewRangeComponent) ++AttackCells;
		if (Cell->GetAttachParent() == Preview->AbilityPreviewRangeComponent) ++AbilityCells;
	}
	TestEqual(TEXT("Attack 1x6"), AttackCells, 6);
	TestEqual(TEXT("Ability 9x5"), AbilityCells, 45);
	TArray<USceneCaptureComponent2D*> Captures;
	Preview->GetComponents(Captures);
	TestEqual(TEXT("Only two captures"), Captures.Num(), 2);
	for (const USceneCaptureComponent2D* Capture : Captures)
	{
		TestFalse(TEXT("Capture only on updates"), Capture->bCaptureEveryFrame);
		TestFalse(TEXT("No movement captures"), Capture->bCaptureOnMovement);
	}
	TestTrue(TEXT("Attack capture attachment"), Preview->AttackCapture->GetAttachParent() == Preview->AttackPreviewRangeComponent);
	TestTrue(TEXT("Ability capture attachment"), Preview->AbilityCapture->GetAttachParent() == Preview->AbilityPreviewRangeComponent);
	TArray<UStaticMeshComponent*> Backgrounds;
	Preview->GetComponents(Backgrounds);
	TestEqual(TEXT("No background plane"), Backgrounds.Num(), 0);
	// 팀원이 사용하는 세 진입점이 BP에서도 호출 가능해야 합니다.
	for (const FName Name : {FName(TEXT("ShowWeaponDefinition")), FName(TEXT("ShowDefinitionPreview")), FName(TEXT("ClearPreview"))})
	{
		const UFunction* Function = Preview->FindFunction(Name);
		if (TestNotNull(TEXT("Public data API exists"), Function))
			TestTrue(TEXT("Public data API is BlueprintCallable"), Function->HasAnyFunctionFlags(FUNC_BlueprintCallable));
	}
	return true;
}
#endif
