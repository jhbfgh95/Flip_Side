#include "UI/ShopTutorialFlow.h"

void UShopTutorialFlow::Init()
{
	ConfigureTutorial();
}
void UShopTutorialFlow::AddDefaultSizeAndPosition()
{
	DialogueBoxPositions.Add(FVector2D(-1,-1));
	DialogueBoxSizes.Add(FVector2D(-1,-1));

}

void UShopTutorialFlow::ConfigureTutorial()
{
	TutorialDialogueList.Reset();
	TutorialActionSequence.Reset();
	DialogueBoxPositions.Reset();
	DialogueBoxSizes.Reset();

	// 0
	TutorialDialogueList.Add(FText::FromString(TEXT("상점에서는 보스 전투를 준비할 수 있습니다.")));
	TutorialActionSequence.Add(EShopTutorialAction::DialogueClicked);

	// 1
	TutorialDialogueList.Add(FText::FromString(TEXT("전투에 직접적으로 사용되는 무기, 코인을 구매할 수 있으며 \n 전투에 도움이 되는 아이템과 카드를 구매할 수 있습니다.")));
	TutorialActionSequence.Add(EShopTutorialAction::DialogueClicked);

	// 2
	TutorialDialogueList.Add(FText::FromString(TEXT("우선 상단의 화살표를 눌러보세요")));
	TutorialActionSequence.Add(EShopTutorialAction::NavigatorBarOpened);

	// 3
	TutorialDialogueList.Add(FText::FromString(TEXT("방금 연 상단바의 버튼들을 통해 각 상점을 빠르게 이동할 수 있습니다. ")));
	TutorialActionSequence.Add(EShopTutorialAction::DialogueClicked);
	
	// 4
	TutorialDialogueList.Add(FText::FromString(TEXT("상단바 뿐만 아니라 책상위에 오브젝트들을 통해서도 각 상점으로 이동할 수 있습니다.")));
	TutorialActionSequence.Add(EShopTutorialAction::DialogueClicked);

	// 5
	TutorialDialogueList.Add(FText::FromString(TEXT("우선 보스 버튼 누르거나, 보스 조각상을 눌러 보스 정보 화면으로 이동해 보세요.")));
	TutorialActionSequence.Add(EShopTutorialAction::PageChangedBoss);
		
	// 6
	TutorialDialogueList.Add(FText::FromString(TEXT("이 곳에서는 보스의 정보의 정보를 미리 확인할 수 있습니다.")));
	TutorialActionSequence.Add(EShopTutorialAction::DialogueClicked);
	
	// 7
	TutorialDialogueList.Add(FText::FromString(TEXT("보스의 패시브, 공격 방식을 확인해 전략을 계획 할 수 있습니다.")));
	TutorialActionSequence.Add(EShopTutorialAction::DialogueClicked);

	// 8
	TutorialDialogueList.Add(FText::FromString(TEXT("이제 이 정보를 바탕으로 전투에 사용될 무기를 구매해 보겠습니다.")));
	TutorialActionSequence.Add(EShopTutorialAction::DialogueClicked);

	// 9
	TutorialDialogueList.Add(FText::FromString(TEXT("")));
	TutorialActionSequence.Add(EShopTutorialAction::PageChangedWeapon);

	// 10
	TutorialDialogueList.Add(FText::FromString(TEXT("이곳에서는 코인에 한쪽 면에 장착할 수 있는 무기를 구매할 수 있습니다.")));
	TutorialActionSequence.Add(EShopTutorialAction::DialogueClicked);

	// 11
	TutorialDialogueList.Add(FText::FromString(TEXT("오른쪽 슬롯을 길게 눌러 무기를 구매해 보세요")));
	TutorialActionSequence.Add(EShopTutorialAction::WeaponUnlocked);

	// 12
	TutorialDialogueList.Add(FText::FromString(TEXT("구매한 무기는 이번 전투가 끝나도 계속해서 코인에 장착할 수 있습니다.")));
	TutorialActionSequence.Add(EShopTutorialAction::DialogueClicked);

	// 13
	TutorialDialogueList.Add(FText::FromString(TEXT("이제 구매한 무기를 코인에 장착해 보겠습니다.")));
	TutorialActionSequence.Add(EShopTutorialAction::DialogueClicked);

	// 14
	TutorialDialogueList.Add(FText::FromString(TEXT("")));
	TutorialActionSequence.Add(EShopTutorialAction::PageChangedCoin);

	// 15
	TutorialDialogueList.Add(FText::FromString(TEXT("이 곳에서는 전투에 직접적으로 참여할 코인을 만들 수 있습니다.")));
	TutorialActionSequence.Add(EShopTutorialAction::DialogueClicked);

	// 16
	TutorialDialogueList.Add(FText::FromString(TEXT("우선 왼쪽 하단에서 코인슬롯을 구매해 보세요")));
	TutorialActionSequence.Add(EShopTutorialAction::CoinSlotPurchased);

	// 17
	TutorialDialogueList.Add(FText::FromString(TEXT("코인 슬롯은 가격마다 해당 슬롯에 장착되는 코인의 기본 체력이 다릅니다.")));
	TutorialActionSequence.Add(EShopTutorialAction::DialogueClicked);

	// 18
	TutorialDialogueList.Add(FText::FromString(TEXT("이제 코인에 무기를 장착해 보겠습니다.")));
	TutorialActionSequence.Add(EShopTutorialAction::DialogueClicked);
	
	// 19
	TutorialDialogueList.Add(FText::FromString(TEXT("무기를 장착해 보세요")));
	TutorialActionSequence.Add(EShopTutorialAction::CoinWeaponClicked);

	// 20
	TutorialDialogueList.Add(FText::FromString(TEXT("코인의 앞면에 무기가 장착되었습니다.")));
	TutorialActionSequence.Add(EShopTutorialAction::DialogueClicked);
	
	TutorialDialogueList.Add(FText::FromString(TEXT("코인은 양면에 무기를 장착해야지 전투에 참여할 수 있습니다.")));
	TutorialActionSequence.Add(EShopTutorialAction::DialogueClicked);

	// 21
	TutorialDialogueList.Add(FText::FromString(TEXT("코인 슬롯에 있는 뒷면 코인을 누르거나, 오른쪽 코인을 눌러 뒷면으로 전환해보세요")));
	TutorialActionSequence.Add(EShopTutorialAction::CoinSideChanged);

	// 22
	TutorialDialogueList.Add(FText::FromString(TEXT("이제 코인 뒷면에 무기를 장착할 수 있습니다. 뒷면에 무기를 장착해 보세요")));
	TutorialActionSequence.Add(EShopTutorialAction::CoinWeaponClicked);

	// 23
	TutorialDialogueList.Add(FText::FromString(TEXT("코인 앞뒤에는 같은 무기를 장착할 수 없습니다.\n 또힌 다른 코인 슬롯과 앞뒤 무기가 중복된 코인 역시 만들 수 없습니다.")));
	TutorialActionSequence.Add(EShopTutorialAction::DialogueClicked);

	// 24
	TutorialDialogueList.Add(FText::FromString(TEXT("코인의 앞뒤에 무기를 장착했다면 \n 슬롯의 위로 향하는 화살표 버튼을 눌러 코인 개수를 증가시키세요")));
	TutorialActionSequence.Add(EShopTutorialAction::CoinCountIncreased);

	// 25
	TutorialDialogueList.Add(FText::FromString(TEXT("코인 개수가 최소 1개 이상인 코인만 전투에 참여할 수 있습니다.")));
	TutorialActionSequence.Add(EShopTutorialAction::DialogueClicked);

	// 26
	TutorialDialogueList.Add(FText::FromString(TEXT("이제 전투에 도움을 주는 카드를 구매해 보겠습니다.")));
	TutorialActionSequence.Add(EShopTutorialAction::DialogueClicked);
	
	// 27
	TutorialDialogueList.Add(FText::FromString(TEXT("")));
	TutorialActionSequence.Add(EShopTutorialAction::PageChangedCard);

	// 28
	TutorialDialogueList.Add(FText::FromString(TEXT("이곳에서는 카드를 구매할 수 있습니다.")));
	TutorialActionSequence.Add(EShopTutorialAction::DialogueClicked);

	// 29
	TutorialDialogueList.Add(FText::FromString(TEXT("카드는 특정 조건이 만족될 때 강력한 효과를 냅니다.")));
	TutorialActionSequence.Add(EShopTutorialAction::DialogueClicked);

	// 30
	TutorialDialogueList.Add(FText::FromString(TEXT("구매한 카드는 다음 전투에서도 똑같이 장착할 수 있습니다.")));
	TutorialActionSequence.Add(EShopTutorialAction::DialogueClicked);

	// 31
	TutorialDialogueList.Add(FText::FromString(TEXT("카드를 구매해보세요")));
	TutorialActionSequence.Add(EShopTutorialAction::CardPurchased);

	// 32
	TutorialDialogueList.Add(FText::FromString(TEXT("구매한 카드는 왼쪽 하단에서 확인 할 수 있습니다.")));
	TutorialActionSequence.Add(EShopTutorialAction::DialogueClicked);

	// 33
	TutorialDialogueList.Add(FText::FromString(TEXT("이제 구매한 카드를 장착해보세요")));
	TutorialActionSequence.Add(EShopTutorialAction::CardSelected);
	
	// 34
	TutorialDialogueList.Add(FText::FromString(TEXT("카드는 최대 3개 장착 할 수 있습니다.")));
	TutorialActionSequence.Add(EShopTutorialAction::DialogueClicked);

	// 35
	TutorialDialogueList.Add(FText::FromString(TEXT("이제 마지막으로 아이템을 구매해보겠습니다.")));
	TutorialActionSequence.Add(EShopTutorialAction::DialogueClicked);

	// 36
	TutorialDialogueList.Add(FText::FromString(TEXT("")));
	TutorialActionSequence.Add(EShopTutorialAction::PageChangedItem);

	// 37
	TutorialDialogueList.Add(FText::FromString(TEXT("아이템은 코인 행동 턴에 사용할 수 있습니다.")));
	TutorialActionSequence.Add(EShopTutorialAction::DialogueClicked);

	// 38
	TutorialDialogueList.Add(FText::FromString(TEXT("아이템을 하나 선택해 구매해 보세요")));
	TutorialActionSequence.Add(EShopTutorialAction::ItemPurchased);

	// 39
	TutorialDialogueList.Add(FText::FromString(TEXT("구매한 아이템은 왼쪽 구매한 아이템 목록에 추가되며 \n 최대 3종류의 아이템을 구매할 수 있습니다.")));
	TutorialActionSequence.Add(EShopTutorialAction::DialogueClicked);

	// 40
	TutorialDialogueList.Add(FText::FromString(TEXT("왼쪽 패널에서 구매한 아이템을 판매해 보세요")));
	TutorialActionSequence.Add(EShopTutorialAction::ItemSold);

	// 41
	TutorialDialogueList.Add(FText::FromString(TEXT("구매한 아이템은 같은 가격에 재판매 할 수 있습니다.")));
	TutorialActionSequence.Add(EShopTutorialAction::DialogueClicked);
	
	// 42
	TutorialDialogueList.Add(FText::FromString(TEXT("")));
	TutorialActionSequence.Add(EShopTutorialAction::PageChangedMain);

	// 43
	TutorialDialogueList.Add(FText::FromString(TEXT("이제 상점에서 할 수 있는 모든 기능을 배웠습니다.")));
	TutorialActionSequence.Add(EShopTutorialAction::DialogueClicked);

	// 44
	TutorialDialogueList.Add(FText::FromString(TEXT("상점에서 구매한 코인과 슬롯은 전투가 끝나면 자동으로 정산되며, 구매한 무기와 카드는 계속해서 진행 할 수 있습니다.")));
	TutorialActionSequence.Add(EShopTutorialAction::DialogueClicked);

	// 45
	TutorialDialogueList.Add(FText::FromString(TEXT("이제 실제 전투를 통한 즐거운 게임되시길 바랍니다.")));
	TutorialActionSequence.Add(EShopTutorialAction::DialogueClicked);
	
	TutorialDialogueList.Add(FText::FromString(TEXT("")));
	TutorialActionSequence.Add(EShopTutorialAction::EndTutorial);

	// 각 단계의 위치·크기는 대사/행동 배열의 인덱스와 동일합니다.
	// (-1, -1)은 WBP에 배치한 기존 레이아웃을 그대로 사용한다는 뜻입니다.
	DialogueBoxPositions.Init(FVector2D(-1.0f, -1.0f), TutorialDialogueList.Num());
	DialogueBoxSizes.Init(FVector2D(-1.0f, -1.0f), TutorialDialogueList.Num());

	DialogueBoxPositions[0] = FVector2D(0.5f, 0.85f);
	DialogueBoxSizes[0] = FVector2D(800.f, 300.f);

	DialogueBoxPositions[29] = FVector2D(0.8f, 0.3f);
	DialogueBoxSizes[29] = FVector2D(800.f, 300.f);

	DialogueBoxPositions[34] = FVector2D(0.5f, 0.5f);
	DialogueBoxSizes[34] = FVector2D(800.f, 300.f);

	DialogueBoxPositions[36] = FVector2D(0.8f, 0.5f);
	DialogueBoxSizes[36] = FVector2D(700.f, 300.f);

	DialogueBoxPositions[41] = FVector2D(0.5f, 0.5f);
	DialogueBoxSizes[41] = FVector2D(700.f, 300.f);

	DialogueBoxPositions[44] = FVector2D(0.5f, 0.85f);
	DialogueBoxSizes[44] = FVector2D(800.f, 300.f);
}
