#include "UI/W_ItemInfo.h"

#include "Components/Image.h"
#include "Components/Border.h"
#include "Components/Button.h"
#include "InputCoreTypes.h"
#include "Components/RichTextBlock.h"
#include "Components/TextBlock.h"
#include "DataTypes/ItemDataTypes.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Engine/GameInstance.h"
#include "Engine/Texture2D.h"
#include "Subsystem/DataManagerSubsystem.h"
#include "UI/KeywordDescriptionWidget.h"
#include "UI/ItemDescriptionFormatter.h"
#include "UI/ItemDescriptionRichTextDecorator.h"
#include "Widgets/SBoxPanel.h"
#include "Widgets/Images/SImage.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Text/STextBlock.h"

void UW_ItemInfo::NativeConstruct()
{
	Super::NativeConstruct();
	if (IsValid(CloseButton)) CloseButton->OnClicked.AddUniqueDynamic(this, &UW_ItemInfo::HandleCloseClicked);

	if (IsValid(HoveredItemIcon))
	{
		DynamicMaterial = HoveredItemIcon->GetDynamicMaterial();
	}
	for (URichTextBlock* Text : { HoveredItemRichDes, ItemKeywordHeader.Get() })
	{
		// 본문은 WBP의 Default Text Style을 사용하며, 제목용 DataTable은 필요하지 않습니다.
		if (IsValid(Text)) Text->SetDecorators({ UItemDescriptionRichTextDecorator::StaticClass() });
	}
	if (IsValid(ItemKeywordDescription)) ItemKeywordDescription->SetKeywordGroup(EKeywordDescriptionGroup::Item);
	SetDescriptionText(CurrentDescription);
}

void UW_ItemInfo::NativeDestruct()
{
	if (IsValid(CloseButton)) CloseButton->OnClicked.RemoveDynamic(this, &UW_ItemInfo::HandleCloseClicked);
	Super::NativeDestruct();
}

void UW_ItemInfo::HandleCloseClicked()
{
	// 우클릭과 같은 HUD 경로에서 표시 상태까지 정리합니다.
	OnCloseRequested.Broadcast();
}

FReply UW_ItemInfo::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	// 팝업 빈 공간을 눌러도 월드의 아이템 사용 입력으로 전달하지 않습니다.
	return InMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton
		? FReply::Handled() : Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
}

void UW_ItemInfo::UpdateItemInfo(UTexture2D* Icon, const FText& ItemName, const FText& Description, FLinearColor ItemColor)
{
	if (IsValid(HoveredItemIcon))
	{
		HoveredItemIcon->SetVisibility(IsValid(Icon) ? ESlateVisibility::HitTestInvisible : ESlateVisibility::Collapsed);
		if (IsValid(DynamicMaterial))
		{
			//이거 머테리얼 무기쪽이랑 공유한다. 어짜피 기능은 똑같기 때문.
			DynamicMaterial->SetTextureParameterValue(FName("Weapon_Icon"), Icon);
			DynamicMaterial->SetVectorParameterValue(FName("Weapon_Color"), ItemColor);
		}
		else if (IsValid(Icon))
		{
			HoveredItemIcon->SetBrushFromTexture(Icon, false);
		}
	}

	if (IsValid(HoveredItemName))
	{
		HoveredItemName->SetText(ItemName);
	}

	SetDescriptionText(Description);
}

void UW_ItemInfo::UpdateItemInfo(const FItemData& ItemData)
{
	UpdateItemInfo(
		ItemData.ItemIcon,
		FText::FromString(ItemData.ItemName),
		FText::FromString(ItemData.Item_DES),
		ItemData.TypeColor
	);
}

void UW_ItemInfo::SetDescriptionText(const FText& Description)
{
	CurrentDescription = Description;
	KeywordDefinitions.Reset();
	// 기존 읽기 API만 사용합니다. DataManager/상점에는 표시 책임을 추가하지 않습니다.
	UGameInstance* Instance = GetGameInstance();
	UDataManagerSubsystem* Manager = IsValid(Instance) ? Instance->GetSubsystem<UDataManagerSubsystem>() : nullptr;
	TArray<FKeywordDefinitionData> Definitions;
	if (IsValid(Manager)) Manager->GetAllEnabledKeywordDefinitions(Definitions);
	for (const FKeywordDefinitionData& Definition : Definitions)
		KeywordDefinitions.Add(Definition.KeywordCode, Definition);
	const FItemDescriptionDisplay Display = FItemDescriptionFormatter::Format(Description.ToString());
	if (IsValid(HeaderBorder))
	{
		// 본문 참조 키워드가 아닌 독립 헤더만 배경색을 결정합니다. 아이템 변경 시 항상 갱신합니다.
		const FKeywordDefinitionData* HeaderData = KeywordDefinitions.Find(Display.HeaderKeyword);
		HeaderBorder->SetBrushColor(HeaderData ? HeaderData->UIColor : FLinearColor::White);
		HeaderBorder->SetVisibility(Display.HeaderKeyword.IsNone() ? ESlateVisibility::Collapsed : ESlateVisibility::SelfHitTestInvisible);
	}
	if (IsValid(HoveredItemDes))
	{
		// BP 이전 중에도 일반 TextBlock에서는 태그 없는 원문을 읽을 수 있습니다.
		HoveredItemDes->SetText(FText::FromString(Display.PlainText));
		HoveredItemDes->SetVisibility(IsValid(HoveredItemRichDes) ? ESlateVisibility::Collapsed : ESlateVisibility::HitTestInvisible);
	}
	if (IsValid(ItemKeywordHeader))
	{
		ItemKeywordHeader->SetText(FText::FromString(Display.Header));
		ItemKeywordHeader->SetVisibility(Display.Header.IsEmpty() ? ESlateVisibility::Collapsed : ESlateVisibility::HitTestInvisible);
		ItemKeywordHeader->RefreshTextLayout();
	}
	if (IsValid(HoveredItemRichDes))
	{
		// 별도 헤더를 배치하지 않아도 설명 첫 줄에서 키워드를 보여줍니다.
		const FString Header = !IsValid(ItemKeywordHeader) && !Display.Header.IsEmpty() ? Display.Header + TEXT("\n") : FString();
		HoveredItemRichDes->SetText(FText::FromString(Header + Display.Body));
		HoveredItemRichDes->RefreshTextLayout();
	}
}

TSharedPtr<SWidget> UW_ItemInfo::CreateKeywordDisplay(FName Code, const FTextBlockStyle& Style, bool bHeader)
{
	const FKeywordDefinitionData* Definition = KeywordDefinitions.Find(Code);
	// DB 누락 시에도 키워드 의미는 남기고, 임의의 아이콘은 대입하지 않습니다.
	FString Label = Code == TEXT("Attack") ? TEXT("공격") : Code == TEXT("Hit") ? TEXT("적중") : TEXT("즉시");
	if (Definition && !Definition->DisplayName.IsEmpty()) Label = Definition->DisplayName.ToString();
	// 배경과 글자가 같은 DB 색상이 되지 않도록, Border 안의 헤더 글자색은 WBP 기본 서식을 사용합니다.
	const FSlateColor Color = bHeader && IsValid(HeaderBorder) ? Style.ColorAndOpacity
		: FSlateColor(Definition ? Definition->UIColor : FLinearColor::White);
	UTexture2D* Icon = Definition ? Definition->Icon.Get() : nullptr;
	TSharedRef<SHorizontalBox> Row = SNew(SHorizontalBox).Visibility(EVisibility::HitTestInvisible);
	if (IsValid(Icon))
	{
		// DB 텍스처는 KeywordDefinitions UPROPERTY가, Brush는 Slate 람다가 보유합니다.
		TSharedRef<FSlateBrush> Brush = MakeShared<FSlateBrush>();
		Brush->SetResourceObject(Icon);
		Brush->DrawAs = ESlateBrushDrawType::Image;
		const FVector2D Size(FMath::Max(1.0, KeywordIconSize.X), FMath::Max(1.0, KeywordIconSize.Y));
		Brush->ImageSize = Size;
		Row->AddSlot().AutoWidth().VAlign(VAlign_Center)
			[SNew(SBox).WidthOverride(Size.X).HeightOverride(Size.Y)
				[SNew(SImage).Image_Lambda([Brush]() -> const FSlateBrush* { return &Brush.Get(); })]];
	}
	Row->AddSlot().AutoWidth().VAlign(VAlign_Center)
		.Padding(IsValid(Icon) ? FMath::Max(0.0f, KeywordIconNameSpacing) : 0.0f, 0.0f, 0.0f, 0.0f)
		[SNew(STextBlock).Text(FText::FromString(Label)).Font(Style.Font).ColorAndOpacity(Color)];
	return Row;
}
