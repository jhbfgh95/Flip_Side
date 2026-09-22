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
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<class UButton> BookmarkButton;
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<class UImage> MainKeywordImage;
	// 분류 배경은 키워드 고유 색상과 별도로 DB의 Main/Additional을 사용합니다.
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<class UBorder> MainKeyWordBorder;
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<class UBorder> AdditonalKeywordBorder;
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
	TObjectPtr<class UHorizontalBox> AdditionalKeywordRow;
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<class UBorder> AdditionalKeywordBorder1;
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<class UBorder> AdditionalKeywordBorder2;
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<class UBorder> AdditionalKeywordBorder3;
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<class UBorder> AdditionalKeywordBorder4;
	// SpacerN은 BorderN 바로 뒤에 배치합니다. 마지막으로 표시되는 칸 뒤에는 간격을 남기지 않습니다.
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
