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
	// 키는 KW:Attack 등 설명과 동일합니다. 각 Image Brush의 기존 머테리얼을 사용합니다.
	UPROPERTY(EditDefaultsOnly, Category = "Coin Description|Style", meta = (GetKeyOptions = "GetKeywordIconKeys"))
	TMap<FName, TObjectPtr<class UTexture2D>> KeywordIcons;
	UFUNCTION()
	TArray<FString> GetKeywordIconKeys() const;
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
