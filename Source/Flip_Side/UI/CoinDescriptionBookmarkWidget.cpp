#include "UI/CoinDescriptionBookmarkWidget.h"
#include "Subsystem/DataManagerSubsystem.h"
#include "Engine/GameInstance.h"
#include "Components/Button.h"
#include "Components/Border.h"
#include "Components/HorizontalBox.h"
#include "Components/Spacer.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Engine/Texture2D.h"
#include "Materials/MaterialInstanceDynamic.h"

void UCoinDescriptionBookmarkWidget::NativeConstruct()
{
	Super::NativeConstruct();
	if (IsValid(BookmarkButton))
		BookmarkButton->OnClicked.AddUniqueDynamic(this, &UCoinDescriptionBookmarkWidget::HandleClicked);
	RefreshKeywords();
	OnBookmarkActiveChanged(bIsActive);
}

void UCoinDescriptionBookmarkWidget::NativeDestruct()
{
	if (IsValid(BookmarkButton))
		BookmarkButton->OnClicked.RemoveDynamic(this, &UCoinDescriptionBookmarkWidget::HandleClicked);
	Super::NativeDestruct();
}

void UCoinDescriptionBookmarkWidget::SetBookmarkData(const FCoinDescriptionSectionData& InData)
{
	BookmarkData = InData;
	RefreshKeywords();
}

void UCoinDescriptionBookmarkWidget::SetKeyword(UImage* Image, UTextBlock* Text, FName Code)
{
	const bool bHasKeyword = !Code.IsNone();
	UDataManagerSubsystem* DB = IsValid(GetGameInstance()) ? GetGameInstance()->GetSubsystem<UDataManagerSubsystem>() : nullptr;
	FKeywordDefinitionData Definition;
	const bool bHasDefinition = IsValid(DB) && DB->TryGetKeywordByCode(Code, Definition);
	const FLinearColor KeywordColor = bHasDefinition ? Definition.UIColor : FLinearColor::White;
	if (IsValid(Text))
	{
		const FText* Label = BookmarkData.KeywordLabels.Find(Code);
		Text->SetText(bHasKeyword ? (Label ? *Label : FText::FromName(Code)) : FText::GetEmpty());
		Text->SetColorAndOpacity(FSlateColor(KeywordColor));
		Text->SetVisibility(bHasKeyword ? ESlateVisibility::HitTestInvisible : ESlateVisibility::Collapsed);
	}
	if (!IsValid(Image)) return;
	const FName Key = bHasKeyword ? FName(*(TEXT("KW:") + Code.ToString())) : NAME_None;
	UTexture2D* Texture = nullptr;
	if (IsValid(DB)) DB->TryGetUIIcon(Key, Texture);
	// 머테리얼은 UI Vertex Color를 최종 색상에 곱해야 이 Tint가 반영됩니다.
	Image->SetColorAndOpacity(KeywordColor);
	// BP에서 Image Brush에 지정한 머테리얼을 유지하고 Icon 파라미터만 교체합니다.
	UMaterialInstanceDynamic* Material = Image->GetDynamicMaterial();
	if (IsValid(Material)) Material->SetTextureParameterValue(TEXT("Icon"), IsValid(Texture) ? Texture : nullptr);
	Image->SetVisibility(bHasKeyword && IsValid(Texture) && IsValid(Material)
		? ESlateVisibility::HitTestInvisible : ESlateVisibility::Collapsed);
}

void UCoinDescriptionBookmarkWidget::RefreshKeywords()
{
	UDataManagerSubsystem* DB = IsValid(GetGameInstance()) ? GetGameInstance()->GetSubsystem<UDataManagerSubsystem>() : nullptr;
	FKeywordDefinitionData MainStyle, AdditionalStyle;
	const bool bHasMainStyle = IsValid(DB) && DB->TryGetKeywordByCode(TEXT("Main"), MainStyle, true);
	const bool bHasAdditionalStyle = IsValid(DB) && DB->TryGetKeywordByCode(TEXT("Additional"), AdditionalStyle, true);
	if (IsValid(MainKeyWordBorder) && bHasMainStyle) MainKeyWordBorder->SetBrushColor(MainStyle.UIColor);
	if (IsValid(AdditonalKeywordBorder) && bHasAdditionalStyle) AdditonalKeywordBorder->SetBrushColor(AdditionalStyle.UIColor);
	SetKeyword(MainKeywordImage, MainKeywordText, BookmarkData.MainKeywordCode);
	UImage* Images[] = { AdditionalKeywordImage1, AdditionalKeywordImage2, AdditionalKeywordImage3, AdditionalKeywordImage4 };
	UTextBlock* Texts[] = { AdditionalKeywordText1, AdditionalKeywordText2, AdditionalKeywordText3, AdditionalKeywordText4 };
	UBorder* Borders[] = { AdditionalKeywordBorder1, AdditionalKeywordBorder2, AdditionalKeywordBorder3, AdditionalKeywordBorder4 };
	USpacer* Spacers[] = { AdditionalKeywordSpacer1, AdditionalKeywordSpacer2, AdditionalKeywordSpacer3 };
	bool HasKeyword[UE_ARRAY_COUNT(Images)] = {};
	for (int32 Index = 0; Index < UE_ARRAY_COUNT(Images); ++Index)
	{
		const FName Code = BookmarkData.AdditionalKeywordCodes.IsValidIndex(Index)
			? BookmarkData.AdditionalKeywordCodes[Index] : NAME_None;
		SetKeyword(Images[Index], Texts[Index], Code);
		HasKeyword[Index] = !Code.IsNone();
		if (IsValid(Borders[Index]))
		{
			if (bHasAdditionalStyle) Borders[Index]->SetBrushColor(AdditionalStyle.UIColor);
			Borders[Index]->SetVisibility(HasKeyword[Index] ? ESlateVisibility::HitTestInvisible : ESlateVisibility::Collapsed);
		}
	}
	// 빈 칸이 중간에 있어도 실제로 표시되는 두 키워드 사이에만 간격을 둡니다.
	bool bHasLaterKeyword = false;
	for (int32 Index = static_cast<int32>(UE_ARRAY_COUNT(Images)) - 1; Index >= 0; --Index)
	{
		if (Index < UE_ARRAY_COUNT(Spacers) && IsValid(Spacers[Index]))
			Spacers[Index]->SetVisibility(HasKeyword[Index] && bHasLaterKeyword
				? ESlateVisibility::HitTestInvisible : ESlateVisibility::Collapsed);
		bHasLaterKeyword |= HasKeyword[Index];
	}
	if (IsValid(AdditionalKeywordRow))
		AdditionalKeywordRow->SetVisibility(bHasLaterKeyword ? ESlateVisibility::HitTestInvisible : ESlateVisibility::Collapsed);
	if (IsValid(AdditonalKeywordBorder))
		AdditonalKeywordBorder->SetVisibility(bHasLaterKeyword ? ESlateVisibility::HitTestInvisible : ESlateVisibility::Collapsed);
	if (BookmarkData.AdditionalKeywordCodes.Num() > UE_ARRAY_COUNT(Images))
		UE_LOG(LogTemp, Warning, TEXT("Coin bookmark supports four additional keywords; received %d."), BookmarkData.AdditionalKeywordCodes.Num());
}

void UCoinDescriptionBookmarkWidget::InitializeBookmark(bool bInFrontFace, int32 InIndex)
{
	bFrontFace = bInFrontFace;
	SectionIndex = InIndex;
}

void UCoinDescriptionBookmarkWidget::SetBookmarkActive(bool bInActive)
{
	if (bIsActive == bInActive) return;
	bIsActive = bInActive;
	OnBookmarkActiveChanged(bIsActive);
}

void UCoinDescriptionBookmarkWidget::HandleClicked()
{
	if (SectionIndex != INDEX_NONE) OnBookmarkClicked.Broadcast(bFrontFace, SectionIndex);
}
