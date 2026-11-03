#include "UI/CoinDescriptionBookmarkWidget.h"
#include "Subsystem/DataManagerSubsystem.h"
#include "Engine/GameInstance.h"
#include "Components/Button.h"
#include "Components/Border.h"
#include "Components/PanelWidget.h"
#include "Components/Spacer.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Engine/Texture2D.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Rendering/DrawElements.h"

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

int32 UCoinDescriptionBookmarkWidget::NativePaint(const FPaintArgs& Args, const FGeometry& Geometry,
	const FSlateRect& CullingRect, FSlateWindowElementList& Elements, int32 Layer,
	const FWidgetStyle& Style, bool bParentEnabled) const
{
	const int32 Painted = Super::NativePaint(Args, Geometry, CullingRect, Elements, Layer, Style, bParentEnabled);
	const bool bPreview = IsDesignTime();
	if (!bDrawKeywordHierarchy || (!bPreview && BookmarkData.MainKeywordCode.IsNone())) return Painted;
	auto GetWidgetRect = [&Geometry](const UWidget* Widget, FSlateRect& OutRect)
	{
		if (!IsValid(Widget) || !Widget->GetCachedWidget().IsValid()) return false;
		for (const UWidget* Parent = Widget; IsValid(Parent); Parent = Parent->GetParent())
			if (!Parent->IsVisible() || Parent->GetRenderOpacity() <= 0.f) return false;
		// 자식이 그려진 이번 프레임의 좌표를 사용해 DPI, 패딩, 배치 변경을 반영합니다.
		const FGeometry& WidgetGeometry = Widget->GetPaintSpaceGeometry();
		const FVector2D Size = WidgetGeometry.GetLocalSize();
		if (Size.X <= 0.f || Size.Y <= 0.f) return false;
		const FVector2D Min = Geometry.AbsoluteToLocal(WidgetGeometry.LocalToAbsolute(FVector2D::ZeroVector));
		const FVector2D Max = Geometry.AbsoluteToLocal(WidgetGeometry.LocalToAbsolute(Size));
		OutRect = FSlateRect(Min, Max);
		return OutRect.Right > OutRect.Left && OutRect.Bottom > OutRect.Top;
	};
	FSlateRect MainRect;
	if (!GetWidgetRect(MainKeyWordBorder, MainRect)) return Painted;
	MainRect = MainRect.ExtendBy(FMargin(FMath::Max(0.f, MainKeywordFramePaddingHorizontal),
		FMath::Max(0.f, MainKeywordFramePaddingVertical)));
	const FLinearColor Tint = Style.GetColorAndOpacityTint();
	const ESlateDrawEffect DrawEffect = bParentEnabled && GetIsEnabled()
		? ESlateDrawEffect::None : ESlateDrawEffect::DisabledEffect;
	auto DrawFrame = [&](const FSlateRect& Rect, const FLinearColor& Color, float Thickness)
	{
		if (Thickness <= 0.f || Rect.Right - Rect.Left <= Thickness || Rect.Bottom - Rect.Top <= Thickness) return;
		// 패딩이 반영된 영역 안쪽에 선 굵기의 절반을 맞춥니다.
		const FSlateRect Frame = Rect.InsetBy(FMargin(Thickness * 0.5f));
		const TArray<FVector2D> Points = {FVector2D(Frame.Left, Frame.Top), FVector2D(Frame.Right, Frame.Top),
			FVector2D(Frame.Right, Frame.Bottom), FVector2D(Frame.Left, Frame.Bottom), FVector2D(Frame.Left, Frame.Top)};
		FSlateDrawElement::MakeLines(Elements, Painted + 2, Geometry.ToPaintGeometry(), Points,
			DrawEffect, Color * Tint, true, Thickness);
	};
	DrawFrame(MainRect, MainKeywordFrameColor, MainKeywordFrameThickness);
	if (IsValid(AdditionalKeywordRow) &&
		(!AdditionalKeywordRow->IsVisible() || AdditionalKeywordRow->GetRenderOpacity() <= 0.f)) return Painted + 2;
	const UBorder* Borders[] = { AdditionalKeywordBorder0, AdditionalKeywordBorder1, AdditionalKeywordBorder2, AdditionalKeywordBorder3 };
	TArray<FSlateRect, TInlineAllocator<4>> AdditionalRects;
	for (int32 Index = 0; Index < UE_ARRAY_COUNT(Borders); ++Index)
	{
		if (!bPreview && (!BookmarkData.AdditionalKeywordCodes.IsValidIndex(Index) || BookmarkData.AdditionalKeywordCodes[Index].IsNone())) continue;
		FSlateRect Rect;
		if (GetWidgetRect(Borders[Index], Rect)) AdditionalRects.Add(Rect);
	}
	if (AdditionalRects.IsEmpty()) return Painted + 2;
	FSlateRect AdditionalRect = AdditionalRects[0];
	for (int32 Index = 1; Index < AdditionalRects.Num(); ++Index) AdditionalRect = AdditionalRect.Expand(AdditionalRects[Index]);
	AdditionalRect = AdditionalRect.ExtendBy(FMargin(FMath::Max(0.f, AdditionalKeywordFramePaddingHorizontal),
		FMath::Max(0.f, AdditionalKeywordFramePaddingVertical)));
	DrawFrame(AdditionalRect, AdditionalKeywordFrameColor, AdditionalKeywordFrameThickness);
	// 서브 여러 개는 하나의 하위 틀에 묶고 칸 사이만 나눕니다.
	const FSlateRect AdditionalFrame = AdditionalRect.InsetBy(FMargin(FMath::Max(0.f, AdditionalKeywordFrameThickness) * 0.5f));
	if (AdditionalKeywordFrameThickness > 0.f)
	{
		for (int32 Index = 1; Index < AdditionalRects.Num(); ++Index)
		{
			const float X = (AdditionalRects[Index - 1].Right + AdditionalRects[Index].Left) * 0.5f;
			const TArray<FVector2D> Points = {FVector2D(X, AdditionalFrame.Top), FVector2D(X, AdditionalFrame.Bottom)};
			FSlateDrawElement::MakeLines(Elements, Painted + 2, Geometry.ToPaintGeometry(), Points,
				DrawEffect, AdditionalKeywordFrameColor * Tint, true, AdditionalKeywordFrameThickness);
		}
	}
	if (KeywordBranchThickness > 0.f)
	{
		const FSlateRect MainFrame = MainRect.InsetBy(FMargin(FMath::Max(0.f, MainKeywordFrameThickness) * 0.5f));
		const float TrunkX = FMath::Max(KeywordBranchThickness * 0.5f,
			FMath::Min(MainFrame.Left, AdditionalFrame.Left - FMath::Max(0.f, KeywordBranchInset)));
		const float ChildY = (AdditionalFrame.Top + AdditionalFrame.Bottom) * 0.5f;
		const TArray<FVector2D> Points = {FVector2D(MainFrame.Left, MainFrame.Bottom), FVector2D(TrunkX, MainFrame.Bottom),
			FVector2D(TrunkX, ChildY), FVector2D(AdditionalFrame.Left, ChildY)};
		// 연결선은 테두리 아래 레이어에 두어 접점이 테두리를 덮지 않게 합니다.
		FSlateDrawElement::MakeLines(Elements, Painted + 1, Geometry.ToPaintGeometry(), Points,
			DrawEffect, KeywordBranchColor * Tint, true, KeywordBranchThickness);
	}
	return Painted + 2;
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
	SetKeyword(MainKeywordImage, MainKeywordText, BookmarkData.MainKeywordCode);
	UImage* Images[] = { AdditionalKeywordImage1, AdditionalKeywordImage2, AdditionalKeywordImage3, AdditionalKeywordImage4 };
	UTextBlock* Texts[] = { AdditionalKeywordText1, AdditionalKeywordText2, AdditionalKeywordText3, AdditionalKeywordText4 };
	UBorder* Borders[] = { AdditionalKeywordBorder0, AdditionalKeywordBorder1, AdditionalKeywordBorder2, AdditionalKeywordBorder3 };
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
