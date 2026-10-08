#include "UI/TutorialDescriptionWidget.h"
#include "Blueprint/WidgetTree.h"
#include "Components/Border.h"
#include "Components/PanelWidget.h"
#include "Components/RichTextBlock.h"
#include "Components/SizeBox.h"
#include "Engine/DataTable.h"
#include "Engine/GameInstance.h"
#include "Engine/Texture2D.h"
#include "Subsystem/DataManagerSubsystem.h"
#include "UI/ItemDescriptionRichTextDecorator.h"
#include "UObject/UnrealType.h"
#include "Widgets/Images/SImage.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/SBoxPanel.h"
#include "Widgets/Text/STextBlock.h"

void UTutorialDescriptionWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	if (!IsValid(WidgetTree)) return;
	const UPanelWidget* EmptyRoot = Cast<UPanelWidget>(WidgetTree->RootWidget);
	if (!WidgetTree->RootWidget || (IsValid(EmptyRoot) && EmptyRoot->GetChildrenCount() == 0))
	{
		DescriptionSizeBox = WidgetTree->ConstructWidget<USizeBox>(USizeBox::StaticClass(), TEXT("DescriptionSizeBox"));
		DescriptionSizeBox->SetWidthOverride(620.f);
		DescriptionBackground = WidgetTree->ConstructWidget<UBorder>(UBorder::StaticClass(), TEXT("DescriptionBackground"));
		DescriptionBackground->SetBrushColor(FLinearColor(0.04f, 0.04f, 0.05f, 1.f));
		DescriptionBackground->SetPadding(FMargin(24.f));
		DescriptionText = WidgetTree->ConstructWidget<URichTextBlock>(URichTextBlock::StaticClass(), TEXT("DescriptionText"));
		DescriptionText->SetAutoWrapText(true);
		DescriptionText->SetWrapTextAt(572.f);
		DescriptionBackground->SetContent(DescriptionText);
		DescriptionSizeBox->AddChild(DescriptionBackground);
		WidgetTree->RootWidget = DescriptionSizeBox;
		bUsingFallbackLayout = true;
	}
	else if (!IsValid(DescriptionSizeBox))
	{
		// Optional 바인딩이 없는 디자인도 런타임에 크기를 지정할 수 있도록 감쌉니다.
		UWidget* Content = WidgetTree->RootWidget;
		DescriptionSizeBox = WidgetTree->ConstructWidget<USizeBox>(USizeBox::StaticClass(), TEXT("TutorialLayoutSizeBox"));
		DescriptionSizeBox->SetContent(Content);
		WidgetTree->RootWidget = DescriptionSizeBox;
	}
	if (IsValid(DescriptionText))
	{
		// UMG에 클래스 배열 getter가 없어 리플렉션으로 읽고, 기존 BP Decorator를 유지합니다.
		if (const FArrayProperty* Property = FindFProperty<FArrayProperty>(URichTextBlock::StaticClass(), TEXT("DecoratorClasses")))
		{
			TArray<TSubclassOf<URichTextBlockDecorator>> Decorators =
				*Property->ContainerPtrToValuePtr<TArray<TSubclassOf<URichTextBlockDecorator>>>(DescriptionText.Get());
			if (!Decorators.ContainsByPredicate([](const TSubclassOf<URichTextBlockDecorator>& Class)
				{ return Class && Class->IsChildOf(UItemDescriptionRichTextDecorator::StaticClass()); }))
			{
				Decorators.Add(UItemDescriptionRichTextDecorator::StaticClass());
				DescriptionText->SetDecorators(Decorators);
			}
		}
	}
}

void UTutorialDescriptionWidget::SetDescriptionText(const FText& Text)
{
	if (!IsValid(DescriptionText)) return;
	KeywordDefinitions.Reset();
	UGameInstance* Instance = GetGameInstance();
	UDataManagerSubsystem* Manager = IsValid(Instance) ? Instance->GetSubsystem<UDataManagerSubsystem>() : nullptr;
	TArray<FKeywordDefinitionData> Definitions;
	if (IsValid(Manager)) Manager->GetAllEnabledKeywordDefinitions(Definitions);
	for (const FKeywordDefinitionData& Definition : Definitions)
		KeywordDefinitions.Add(Definition.KeywordCode, Definition);
	FString Value = Text.ToString();
	Value.ReplaceInline(TEXT("\\n"), TEXT("\n"));
	const TCHAR* Labels[] = {TEXT("공격"), TEXT("적중"), TEXT("기동")};
	const TCHAR* Codes[] = {TEXT("Attack"), TEXT("Hit"), TEXT("Mobility")};
	for (int32 Index = 0; Index < UE_ARRAY_COUNT(Codes); ++Index)
	{
		const FString Tag = FString::Printf(TEXT("<itemkw code=\"%s\"/>"), Codes[Index]);
		Value.ReplaceInline(*(FString(TEXT("[")) + Labels[Index] + TEXT("]")), *Tag, ESearchCase::CaseSensitive);
		Value.ReplaceInline(*(FString(TEXT("[KW:")) + Codes[Index] + TEXT("]")), *Tag, ESearchCase::CaseSensitive);
	}
	DescriptionText->SetText(FText::FromString(Value));
}

TSharedPtr<SWidget> UTutorialDescriptionWidget::CreateKeywordDisplay(FName Code, const FTextBlockStyle& Style)
{
	const FKeywordDefinitionData* Definition = KeywordDefinitions.Find(Code);
	FString Label = Code == TEXT("Attack") ? TEXT("공격") : Code == TEXT("Hit") ? TEXT("적중")
		: Code == TEXT("Mobility") ? TEXT("기동") : Code.ToString();
	if (Definition && !Definition->DisplayName.IsEmpty()) Label = Definition->DisplayName.ToString();
	const FSlateColor Color = Definition ? FSlateColor(Definition->UIColor) : Style.ColorAndOpacity;
	UTexture2D* Icon = Definition ? Definition->Icon.Get() : nullptr;
	FSlateFontInfo Font = Style.Font;
	Font.TypefaceFontName = TEXT("Bold");
	TSharedRef<SHorizontalBox> Row = SNew(SHorizontalBox).Visibility(EVisibility::HitTestInvisible);
	// 튜토리얼은 Shift 입력 없이 항상 상세 키워드([아이콘 이름])를 표시합니다.
	Row->AddSlot().AutoWidth().VAlign(VAlign_Center)
		[SNew(STextBlock).Text(FText::FromString(TEXT("["))).Font(Font).ColorAndOpacity(Color)];
	if (IsValid(Icon))
	{
		// Texture는 KeywordDefinitions가, Brush는 Slate 람다가 보유합니다.
		TSharedRef<FSlateBrush> Brush = MakeShared<FSlateBrush>();
		Brush->SetResourceObject(Icon);
		Brush->DrawAs = ESlateBrushDrawType::Image;
		const FVector2D Size(FMath::Max(1.0, KeywordIconSize.X), FMath::Max(1.0, KeywordIconSize.Y));
		Brush->ImageSize = Size;
		Row->AddSlot().AutoWidth().VAlign(VAlign_Center)
			[SNew(SBox).WidthOverride(Size.X).HeightOverride(Size.Y)
				[SNew(SImage).Image_Lambda([Brush]() -> const FSlateBrush* { return &Brush.Get(); }).ColorAndOpacity(Color)]];
	}
	Row->AddSlot().AutoWidth().VAlign(VAlign_Center)
		.Padding(IsValid(Icon) ? FMath::Max(0.f, KeywordIconNameSpacing) : 0.f, 0.f, 0.f, 0.f)
		[SNew(STextBlock).Text(FText::FromString(Label + TEXT("]"))).Font(Font).ColorAndOpacity(Color)];
	return Row;
}

void UTutorialDescriptionWidget::SetDescriptionSize(const FVector2D& Size)
{
	if (!IsValid(DescriptionSizeBox)) return;
	if (!bDefaultSizeCached)
	{
		bDefaultWidthOverride = DescriptionSizeBox->IsWidthOverride();
		bDefaultHeightOverride = DescriptionSizeBox->IsHeightOverride();
		DefaultSize = FVector2D(DescriptionSizeBox->GetWidthOverride(), DescriptionSizeBox->GetHeightOverride());
		if (IsValid(DescriptionText))
		{
			bDefaultAutoWrapText = DescriptionText->GetAutoWrapText();
			DefaultWrapTextAt = DescriptionText->GetWrapTextAt();
		}
		bDefaultSizeCached = true;
	}

	// 단계가 바뀌면 이전 단계의 지정 크기가 남지 않도록 BP 기본 설정부터 복원합니다.
	if (bDefaultWidthOverride) DescriptionSizeBox->SetWidthOverride(DefaultSize.X);
	else DescriptionSizeBox->ClearWidthOverride();
	if (bDefaultHeightOverride) DescriptionSizeBox->SetHeightOverride(DefaultSize.Y);
	else DescriptionSizeBox->ClearHeightOverride();
	if (IsValid(DescriptionText))
	{
		DescriptionText->SetAutoWrapText(bDefaultAutoWrapText);
		DescriptionText->SetWrapTextAt(DefaultWrapTextAt);
	}

	if (Size.X > 0.f)
	{
		DescriptionSizeBox->SetWidthOverride(Size.X);
		if (Size.Y <= 0.f) DescriptionSizeBox->ClearHeightOverride();
		if (IsValid(DescriptionText))
		{
			DescriptionText->SetWrapTextAt(0.f);
			DescriptionText->SetAutoWrapText(true);
		}
	}
	if (Size.Y > 0.f) DescriptionSizeBox->SetHeightOverride(Size.Y);
}

void UTutorialDescriptionWidget::ConfigureFallbackAppearance(float Width, UDataTable* StyleSet)
{
	if (bUsingFallbackLayout)
	{
		if (IsValid(DescriptionSizeBox)) DescriptionSizeBox->SetWidthOverride(FMath::Max(100.f, Width));
		if (IsValid(DescriptionText)) DescriptionText->SetWrapTextAt(FMath::Max(100.f, Width - 48.f));
	}
	if (IsValid(DescriptionText) && !IsValid(DescriptionText->GetTextStyleSet()) && IsValid(StyleSet))
		DescriptionText->SetTextStyleSet(StyleSet);
}
