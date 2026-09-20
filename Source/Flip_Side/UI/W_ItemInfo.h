#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DataTypes/KeywordDataTypes.h"
#include "Styling/SlateTypes.h"
#include "W_ItemInfo.generated.h"

struct FItemData;
DECLARE_MULTICAST_DELEGATE(FOnItemInfoCloseRequested);

UCLASS()
class FLIP_SIDE_API UW_ItemInfo : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

private:
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<class UButton> CloseButton;
	UFUNCTION()
	void HandleCloseClicked();
	UPROPERTY(meta = (BindWidgetOptional))
	class UImage* HoveredItemIcon;

	UPROPERTY(meta = (BindWidgetOptional))
	class UTextBlock* HoveredItemName;

	UPROPERTY(meta = (BindWidgetOptional))
	class UTextBlock* HoveredItemDes;

	UPROPERTY(meta = (BindWidgetOptional))
	class URichTextBlock* HoveredItemRichDes;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<class URichTextBlock> ItemKeywordHeader;
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<class UBorder> HeaderBorder;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<class UKeywordDescriptionWidget> ItemKeywordDescription;

	// 아이콘/색상은 DB에서만 받으며, 크기와 간격은 배치용 WBP 설정입니다.
	UPROPERTY(EditDefaultsOnly, Category = "Item Description", meta = (ClampMin = "1.0"))
	FVector2D KeywordIconSize = FVector2D(24.0f, 24.0f);
	UPROPERTY(EditDefaultsOnly, Category = "Item Description", meta = (ClampMin = "0.0"))
	float KeywordIconNameSpacing = 4.0f;
	UPROPERTY(Transient)
	TMap<FName, FKeywordDefinitionData> KeywordDefinitions;
	FText CurrentDescription;

	UPROPERTY()
	class UMaterialInstanceDynamic* DynamicMaterial;

private:
	void SetDescriptionText(const FText& Description);

public:
	FOnItemInfoCloseRequested OnCloseRequested;
	TSharedPtr<SWidget> CreateKeywordDisplay(FName Code, const FTextBlockStyle& Style, bool bHeader = false);
	void UpdateItemInfo(
		class UTexture2D* Icon,
		const FText& ItemName,
		const FText& Description,
		FLinearColor ItemColor
	);

	void UpdateItemInfo(const FItemData& ItemData);
	
};
