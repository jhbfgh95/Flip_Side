#pragma once

#include "CoreMinimal.h"
#include "Subsystem/ShopTutorialWSubsystem.h"
#include "UObject/Object.h"
#include "ShopTutorialFlow.generated.h"

// 상점 튜토리얼의 대사와 행동 순서를 한곳에서 설정합니다.
UCLASS()
class FLIP_SIDE_API UShopTutorialFlow : public UObject
{
	GENERATED_BODY()

public:
	void Init();

	const TArray<FText>& GetTutorialDialogueList() const { return TutorialDialogueList; }
	const TArray<EShopTutorialAction>& GetTutorialActionSequence() const { return TutorialActionSequence; }
	const TArray<FVector2D>& GetDialogueBoxPositions() const { return DialogueBoxPositions; }
	const TArray<FVector2D>& GetDialogueBoxSizes() const { return DialogueBoxSizes; }

private:
	void ConfigureTutorial();
	void AddDefaultSizeAndPosition();
	TArray<FText> TutorialDialogueList;
	TArray<EShopTutorialAction> TutorialActionSequence;
	TArray<FVector2D> DialogueBoxPositions;
	TArray<FVector2D> DialogueBoxSizes;


};
