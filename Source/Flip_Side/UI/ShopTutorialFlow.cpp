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
	TutorialDialogueList.Add(FText::FromString(TEXT("상점에서는 보스 전투를 준비하고 나만의 전략을 세울 수 있습니다.")));
	TutorialActionSequence.Add(EShopTutorialAction::DialogueClicked);

	// 1
	TutorialDialogueList.Add(FText::FromString(TEXT("전투에 직접 사용하는 무기와 코인을 구매할 수 있으며,\n전투에 도움이 되는 아이템과 카드도 구매할 수 있습니다.")));
	TutorialActionSequence.Add(EShopTutorialAction::DialogueClicked);

	// 2
	TutorialDialogueList.Add(FText::FromString(TEXT("우선 상점에서 이동하는 방법을 알려 드리겠습니다.")));
	TutorialActionSequence.Add(EShopTutorialAction::DialogueClicked);

	// 3
	TutorialDialogueList.Add(FText::FromString(TEXT("상단의 화살표를 눌러 보세요.")));
	TutorialActionSequence.Add(EShopTutorialAction::NavigatorBarOpened);

	// 4
	TutorialDialogueList.Add(FText::FromString(TEXT("방금 연 상단 바의 버튼을 통해 각 상점으로 빠르게 이동할 수 있습니다.")));
	TutorialActionSequence.Add(EShopTutorialAction::DialogueClicked);

	// 5
	TutorialDialogueList.Add(FText::FromString(TEXT("상단 바뿐만 아니라 책상 위의 오브젝트를 통해서도 각 상점으로 이동할 수 있습니다.")));
	TutorialActionSequence.Add(EShopTutorialAction::DialogueClicked);

	// 6
	TutorialDialogueList.Add(FText::FromString(TEXT("보스 버튼이나 보스 조각상을 눌러 보스 정보 화면으로 이동해 보세요.")));
	TutorialActionSequence.Add(EShopTutorialAction::PageChangedBoss);

	// 7
	TutorialDialogueList.Add(FText::FromString(TEXT("이곳에서는 보스의 정보를 미리 확인할 수 있습니다.")));
	TutorialActionSequence.Add(EShopTutorialAction::DialogueClicked);

	// 8
	TutorialDialogueList.Add(FText::FromString(TEXT("보스의 패시브와 공격 방식을 확인해 전략을 세울 수 있습니다.")));
	TutorialActionSequence.Add(EShopTutorialAction::DialogueClicked);

	// 9
	TutorialDialogueList.Add(FText::FromString(TEXT("이제 이 정보를 바탕으로 전투에 사용할 무기를 구매해 보겠습니다.")));
	TutorialActionSequence.Add(EShopTutorialAction::DialogueClicked);

	// 10
	TutorialDialogueList.Add(FText::FromString(TEXT("")));
	TutorialActionSequence.Add(EShopTutorialAction::PageChangedWeapon);

	// 11
	TutorialDialogueList.Add(FText::FromString(TEXT("이곳에서는 코인의 한쪽 면에 장착할 수 있는 무기를 구매할 수 있습니다.")));
	TutorialActionSequence.Add(EShopTutorialAction::DialogueClicked);

	// 12
	TutorialDialogueList.Add(FText::FromString(TEXT("오른쪽 슬롯을 길게 눌러 무기를 구매해 보세요.")));
	TutorialActionSequence.Add(EShopTutorialAction::WeaponUnlocked);

	// 13
	TutorialDialogueList.Add(FText::FromString(TEXT("이렇게 구매한 무기는 이번 전투가 끝나도 계속해서 코인에 장착할 수 있습니다.")));
	TutorialActionSequence.Add(EShopTutorialAction::DialogueClicked);

	// 14
	TutorialDialogueList.Add(FText::FromString(TEXT("이제 구매한 무기를 코인에 장착해 보겠습니다.")));
	TutorialActionSequence.Add(EShopTutorialAction::DialogueClicked);

	// 15
	TutorialDialogueList.Add(FText::FromString(TEXT("")));
	TutorialActionSequence.Add(EShopTutorialAction::PageChangedCoin);

	// 16
	TutorialDialogueList.Add(FText::FromString(TEXT("코인 상점에서는 전투에 직접적으로 참여할 코인을 만들 수 있습니다.")));
	TutorialActionSequence.Add(EShopTutorialAction::DialogueClicked);

	// 17
	TutorialDialogueList.Add(FText::FromString(TEXT("우선 왼쪽 하단에서 코인 슬롯 창을 연 뒤 코인 슬롯을 구매해 보세요.")));
	TutorialActionSequence.Add(EShopTutorialAction::CoinSlotPurchased);

	// 18
	TutorialDialogueList.Add(FText::FromString(TEXT("코인 슬롯은 가격에 따라 해당 슬롯에 장착되는 코인의 최대 체력이 다릅니다.")));
	TutorialActionSequence.Add(EShopTutorialAction::DialogueClicked);

	// 19
	TutorialDialogueList.Add(FText::FromString(TEXT("이제 코인에 무기를 장착해 보겠습니다.")));
	TutorialActionSequence.Add(EShopTutorialAction::DialogueClicked);

	// 20
	TutorialDialogueList.Add(FText::FromString(TEXT("쇠파이프를 장착해 보세요.")));
	TutorialActionSequence.Add(EShopTutorialAction::FrontCoinWeaponClicked);

	// 21
	TutorialDialogueList.Add(FText::FromString(TEXT("코인의 앞면에 무기가 장착되었습니다.")));
	TutorialActionSequence.Add(EShopTutorialAction::DialogueClicked);
	// 22
	TutorialDialogueList.Add(FText::FromString(TEXT("코인은 양면에 무기를 장착해야 전투에 참여할 수 있습니다.")));
	TutorialActionSequence.Add(EShopTutorialAction::DialogueClicked);

	// 23
	TutorialDialogueList.Add(FText::FromString(TEXT("코인 슬롯의 뒷면 버튼이나 오른쪽 코인을 눌러 뒷면으로 전환해 보세요.")));
	TutorialActionSequence.Add(EShopTutorialAction::CoinSideChanged);

	// 24
	TutorialDialogueList.Add(FText::FromString(TEXT("이제 코인 뒷면에 무기를 장착할 수 있습니다.\n뒷면에 무기를 장착해 보세요.")));
	TutorialActionSequence.Add(EShopTutorialAction::BackCoinWeaponClicked);

	// 25
	TutorialDialogueList.Add(FText::FromString(TEXT("코인의 앞뒤에는 같은 무기를 장착할 수 없습니다.\n또한 다른 코인 슬롯과 앞뒤 무기가 중복되는 코인은 만들 수 없습니다.")));
	TutorialActionSequence.Add(EShopTutorialAction::DialogueClicked);

	// 26
	TutorialDialogueList.Add(FText::FromString(TEXT("코인의 앞뒤에 무기를 장착했다면\n슬롯의 위쪽 화살표 버튼을 눌러 코인 개수를 늘려 보세요.")));
	TutorialActionSequence.Add(EShopTutorialAction::CoinCountIncreased);

	// 27
	TutorialDialogueList.Add(FText::FromString(TEXT("코인 개수가 1개 이상인 슬롯의 코인만 전투에 참여할 수 있습니다.")));
	TutorialActionSequence.Add(EShopTutorialAction::DialogueClicked);

	// 28
	TutorialDialogueList.Add(FText::FromString(TEXT("이제 전투에 도움을 주는 카드를 구매해 보겠습니다.")));
	TutorialActionSequence.Add(EShopTutorialAction::DialogueClicked);

	// 29
	TutorialDialogueList.Add(FText::FromString(TEXT("")));
	TutorialActionSequence.Add(EShopTutorialAction::PageChangedCard);

	// 30
	TutorialDialogueList.Add(FText::FromString(TEXT("카드 상점에서는 카드를 구매할 수 있습니다.")));
	TutorialActionSequence.Add(EShopTutorialAction::DialogueClicked);

	// 31
	TutorialDialogueList.Add(FText::FromString(TEXT("카드는 특정 조건을 만족할 때 강력한 효과를 냅니다.")));
	TutorialActionSequence.Add(EShopTutorialAction::DialogueClicked);

	// 32
	TutorialDialogueList.Add(FText::FromString(TEXT("구매한 카드는 전투가 끝나도 사라지지 않으며,\n다음 전투에서도 장착할 수 있습니다.")));
	TutorialActionSequence.Add(EShopTutorialAction::DialogueClicked);

	// 33
	TutorialDialogueList.Add(FText::FromString(TEXT("카드를 구매해 보세요.")));
	TutorialActionSequence.Add(EShopTutorialAction::CardPurchased);

	// 34
	TutorialDialogueList.Add(FText::FromString(TEXT("구매한 카드는 왼쪽 하단에서 확인할 수 있습니다.")));
	TutorialActionSequence.Add(EShopTutorialAction::DialogueClicked);

	// 35
	TutorialDialogueList.Add(FText::FromString(TEXT("이제 구매한 카드를 장착해 보세요.")));
	TutorialActionSequence.Add(EShopTutorialAction::CardSelected);

	// 36
	TutorialDialogueList.Add(FText::FromString(TEXT("카드는 최대 3개까지 장착할 수 있습니다.")));
	TutorialActionSequence.Add(EShopTutorialAction::DialogueClicked);

	// 37
	TutorialDialogueList.Add(FText::FromString(TEXT("이제 마지막으로 아이템을 구매해 보겠습니다.")));
	TutorialActionSequence.Add(EShopTutorialAction::DialogueClicked);

	// 38
	TutorialDialogueList.Add(FText::FromString(TEXT("")));
	TutorialActionSequence.Add(EShopTutorialAction::PageChangedItem);

	// 39
	TutorialDialogueList.Add(FText::FromString(TEXT("아이템은 코인 행동 턴에 사용할 수 있습니다.")));
	TutorialActionSequence.Add(EShopTutorialAction::DialogueClicked);

	// 40
	TutorialDialogueList.Add(FText::FromString(TEXT("아이템을 하나 선택해 구매해 보세요.")));
	TutorialActionSequence.Add(EShopTutorialAction::ItemPurchased);

	// 41
	TutorialDialogueList.Add(FText::FromString(TEXT("구매한 아이템은 왼쪽의 구매한 아이템 목록에 추가되며\n최대 3종류의 아이템을 구매할 수 있습니다.")));
	TutorialActionSequence.Add(EShopTutorialAction::DialogueClicked);

	// 42
	TutorialDialogueList.Add(FText::FromString(TEXT("왼쪽 패널에서 구매한 아이템을 판매해 보세요.")));
	TutorialActionSequence.Add(EShopTutorialAction::ItemSold);

	// 43
	TutorialDialogueList.Add(FText::FromString(TEXT("구매한 아이템은 같은 가격에 재판매할 수 있습니다.")));
	TutorialActionSequence.Add(EShopTutorialAction::DialogueClicked);

	// 44
	TutorialDialogueList.Add(FText::FromString(TEXT("")));
	TutorialActionSequence.Add(EShopTutorialAction::PageChangedMain);

	// 45
	TutorialDialogueList.Add(FText::FromString(TEXT("이제 상점에서 할 수 있는 모든 기능을 배웠습니다.")));
	TutorialActionSequence.Add(EShopTutorialAction::DialogueClicked);

	// 46
	TutorialDialogueList.Add(FText::FromString(TEXT("상점에서 구매한 코인과 슬롯은 전투가 끝나면 자동으로 정산되며,\n구매한 무기와 카드는 계속해서 사용할 수 있습니다.")));
	TutorialActionSequence.Add(EShopTutorialAction::DialogueClicked);

	// 47
	TutorialDialogueList.Add(FText::FromString(TEXT("이제 실제 전투를 진행해 보겠습니다.")));
	TutorialActionSequence.Add(EShopTutorialAction::DialogueClicked);

	// 48
	TutorialDialogueList.Add(FText::FromString(TEXT("")));
	TutorialActionSequence.Add(EShopTutorialAction::EndTutorial);

	// 각 단계의 위치·크기는 대사/행동 배열의 인덱스와 동일합니다.
	// (-1, -1)은 WBP에 배치한 기존 레이아웃을 그대로 사용한다는 뜻입니다.
	DialogueBoxPositions.Init(FVector2D(-1.0f, -1.0f), TutorialDialogueList.Num());
	DialogueBoxSizes.Init(FVector2D(-1.0f, -1.0f), TutorialDialogueList.Num());

	DialogueBoxPositions[0] = FVector2D(0.5f, 0.85f);
	DialogueBoxSizes[0] = FVector2D(1300.f, 210.f);

	DialogueBoxPositions[3] = FVector2D(0.5f, 0.25f);
	DialogueBoxSizes[3] = FVector2D(1300.f, 100.f);

	DialogueBoxPositions[6] = FVector2D(0.7f, 0.35f);
	DialogueBoxSizes[6] = FVector2D(1000.f, 110.f);

	DialogueBoxPositions[7] = FVector2D(0.5f, 0.1f);
	DialogueBoxSizes[7] = FVector2D(1300.f, 110.f);

	DialogueBoxPositions[11] = FVector2D(0.5f, 0.85f);
	DialogueBoxSizes[11] = FVector2D(1100.f, 110.f);

	DialogueBoxPositions[12] = FVector2D(0.5f, 0.5f);
	DialogueBoxSizes[12] = FVector2D(800.f, 110.f);

	DialogueBoxPositions[13] = FVector2D(0.5f, 0.85f);
	DialogueBoxSizes[13] = FVector2D(1100.f, 110.f);

	DialogueBoxPositions[16] = FVector2D(0.5f, 0.85f);
	DialogueBoxSizes[16] = FVector2D(1100.f, 110.f);

	DialogueBoxPositions[17] = FVector2D(0.5f, 0.85f);
	DialogueBoxSizes[17] = FVector2D(800.f, 110.f);

	DialogueBoxPositions[18] = FVector2D(0.5f, 0.85f);
	DialogueBoxSizes[18] = FVector2D(1100.f, 110.f);

	DialogueBoxPositions[20] = FVector2D(0.5f, 0.7f);
	DialogueBoxSizes[20] = FVector2D(800.f, 110.f);

	DialogueBoxPositions[21] = FVector2D(0.5f, 0.85f);
	DialogueBoxSizes[21] = FVector2D(1100.f, 110.f);

	DialogueBoxPositions[26] = FVector2D(0.4f, 0.2f);
	DialogueBoxSizes[26] = FVector2D(800.f, 110.f);


	DialogueBoxPositions[30] = FVector2D(0.5f, 0.85f);
	DialogueBoxSizes[30] = FVector2D(1100.f, 110.f);

	DialogueBoxPositions[33] = FVector2D(0.8f, 0.5f);
	DialogueBoxSizes[33] = FVector2D(700.f, 110.f);

	DialogueBoxPositions[35] = FVector2D(0.6f, 0.5f);
	DialogueBoxSizes[35] = FVector2D(700.f, 110.f);

	DialogueBoxPositions[37] = FVector2D(0.5f, 0.85f);
	DialogueBoxSizes[37] = FVector2D(1100.f, 110.f);

	DialogueBoxPositions[39] = FVector2D(0.8f, 0.5f);
	DialogueBoxSizes[39] = FVector2D(600.f, 210.f);

	DialogueBoxPositions[41] = FVector2D(0.5f, 0.5f);
	DialogueBoxSizes[41] = FVector2D(700.f, 210.f);

	DialogueBoxPositions[44] = FVector2D(0.5f, 0.85f);
	DialogueBoxSizes[44] = FVector2D(1100.f, 210.f);
}
