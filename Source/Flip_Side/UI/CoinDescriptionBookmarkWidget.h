#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DataTypes/CoinDescriptionDataTypes.h"
#include "CoinDescriptionBookmarkWidget.generated.h"

DECLARE_MULTICAST_DELEGATE_TwoParams(FOnCoinDescriptionBookmarkClicked, bool, int32);

UCLASS()
class FLIP_SIDE_API UCoinDescriptionBookmarkWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	void SetBookmarkData(const FCoinDescriptionSectionData& InData);
	void InitializeBookmark(bool bInFrontFace, int32 InIndex);
	void SetBookmarkActive(bool bInActive);
	FOnCoinDescriptionBookmarkClicked OnBookmarkClicked;
protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	virtual int32 NativePaint(const FPaintArgs& Args, const FGeometry& Geometry, const FSlateRect& CullingRect,
		FSlateWindowElementList& Elements, int32 Layer, const FWidgetStyle& Style, bool bParentEnabled) const override;
	// 테두리는 실제 위젯 위치를 따라 그립니다. WBP의 배치와 패딩은 변경하지 않습니다.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Coin Description|Keyword Hierarchy")
	bool bDrawKeywordHierarchy = true;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Coin Description|Keyword Hierarchy")
	FLinearColor MainKeywordFrameColor = FLinearColor::FromSRGBColor(FColor(209, 173, 97));
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Coin Description|Keyword Hierarchy")
	FLinearColor AdditionalKeywordFrameColor = FLinearColor::FromSRGBColor(FColor(140, 119, 78));
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Coin Description|Keyword Hierarchy")
	FLinearColor KeywordBranchColor = FLinearColor::FromSRGBColor(FColor(189, 163, 107));
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Coin Description|Keyword Hierarchy", meta = (ClampMin = "0.0"))
	float MainKeywordFrameThickness = 1.f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Coin Description|Keyword Hierarchy", meta = (ClampMin = "0.0"))
	float AdditionalKeywordFrameThickness = 0.8f;
	// 양수일수록 테두리를 바깥으로 넓힙니다. 위젯의 배치와 콘텐츠 패딩은 변경하지 않습니다.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Coin Description|Keyword Hierarchy|Main Frame Padding", meta = (ClampMin = "0.0", DisplayName = "Horizontal (Left / Right)"))
	float MainKeywordFramePaddingHorizontal = 0.f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Coin Description|Keyword Hierarchy|Main Frame Padding", meta = (ClampMin = "0.0", DisplayName = "Vertical (Top / Bottom)"))
	float MainKeywordFramePaddingVertical = 0.f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Coin Description|Keyword Hierarchy|Additional Frame Padding", meta = (ClampMin = "0.0", DisplayName = "Horizontal (Left / Right)"))
	float AdditionalKeywordFramePaddingHorizontal = 0.f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Coin Description|Keyword Hierarchy|Additional Frame Padding", meta = (ClampMin = "0.0", DisplayName = "Vertical (Top / Bottom)"))
	float AdditionalKeywordFramePaddingVertical = 0.f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Coin Description|Keyword Hierarchy", meta = (ClampMin = "0.0"))
	float KeywordBranchThickness = 2.f;
	// 서브 테두리 왼쪽에 연결선을 그릴 거리입니다. 필요한 공간은 WBP에서 확보합니다.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Coin Description|Keyword Hierarchy", meta = (ClampMin = "0.0"))
	float KeywordBranchInset = 6.f;
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<class UButton> BookmarkButton;
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<class UImage> MainKeywordImage;
	// 분류 배경은 키워드 고유 색상과 별도로 DB의 Main/Additional을 사용합니다.
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<class UBorder> MainKeyWordBorder;
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<class UTextBlock> MainKeywordText;
	// WBP에 고정 배치한 부가 키워드 네 쌍입니다. 사용하지 않는 쌍은 함께 숨깁니다.
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UImage> AdditionalKeywordImage1;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTextBlock> AdditionalKeywordText1;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UImage> AdditionalKeywordImage2;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTextBlock> AdditionalKeywordText2;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UImage> AdditionalKeywordImage3;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTextBlock> AdditionalKeywordText3;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UImage> AdditionalKeywordImage4;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTextBlock> AdditionalKeywordText4;
	// 키워드 이미지/텍스트뿐 아니라 배경과 간격까지 접어서 빈 칸의 크기를 없앱니다.
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<class UPanelWidget> AdditionalKeywordRow;
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<class UBorder> AdditionalKeywordBorder0;
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<class UBorder> AdditionalKeywordBorder1;
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<class UBorder> AdditionalKeywordBorder2;
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<class UBorder> AdditionalKeywordBorder3;
	// Border0~3은 Image/Text1~4에 대응합니다. Spacer1~3은 Border0~2 바로 뒤에 배치합니다.
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<class USpacer> AdditionalKeywordSpacer1;
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<class USpacer> AdditionalKeywordSpacer2;
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<class USpacer> AdditionalKeywordSpacer3;
	UPROPERTY(BlueprintReadOnly, Category = "Coin Description")
	bool bIsActive = false;
	UFUNCTION(BlueprintImplementableEvent, Category = "Coin Description")
	void OnBookmarkActiveChanged(bool bActive);
private:
	void RefreshKeywords();
	void SetKeyword(class UImage* Image, class UTextBlock* Text, FName Code);
	UPROPERTY(Transient)
	FCoinDescriptionSectionData BookmarkData;
	UFUNCTION()
	void HandleClicked();
	bool bFrontFace = true;
	int32 SectionIndex = INDEX_NONE;
};
