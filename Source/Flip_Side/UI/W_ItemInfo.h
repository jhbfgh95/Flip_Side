#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "W_ItemInfo.generated.h"

struct FItemData;

UCLASS()
class FLIP_SIDE_API UW_ItemInfo : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

private:
	UPROPERTY(meta = (BindWidgetOptional))
	class UImage* HoveredItemIcon;

	UPROPERTY(meta = (BindWidgetOptional))
	class UTextBlock* HoveredItemName;

	UPROPERTY(meta = (BindWidgetOptional))
	class UTextBlock* HoveredItemDes;

	UPROPERTY(meta = (BindWidgetOptional))
	class URichTextBlock* HoveredItemRichDes;

	UPROPERTY()
	class UMaterialInstanceDynamic* DynamicMaterial;

private:
	void SetDescriptionText(const FText& Description);

public:
	void UpdateItemInfo(
		class UTexture2D* Icon,
		const FText& ItemName,
		const FText& Description,
		FLinearColor ItemColor
	);

	void UpdateItemInfo(const FItemData& ItemData);
	
};
