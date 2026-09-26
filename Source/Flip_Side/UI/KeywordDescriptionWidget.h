#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/ComboBoxString.h"
#include "DataTypes/KeywordDataTypes.h"
#include "KeywordDescriptionWidget.generated.h"

UENUM(BlueprintType)
enum class EKeywordDescriptionGroup : uint8
{
	Main,
	Additional,
	Item
};

// 다른 화면에서도 독립적으로 배치할 수 있는 DB 기반 키워드 사전입니다.
UCLASS()
class FLIP_SIDE_API UKeywordDescriptionWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable, Category = "Keyword Description")
	void SetKeywordGroup(EKeywordDescriptionGroup InGroup);
	UFUNCTION(BlueprintCallable, Category = "Keyword Description")
	void RefreshFromDatabase();
protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Keyword Description")
	EKeywordDescriptionGroup KeywordGroup = EKeywordDescriptionGroup::Main;
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UComboBoxString> KeywordDropdown;
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<class URichTextBlock> KeywordDescriptionText;
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<class UImage> KeywordIcon;
	UPROPERTY(BlueprintReadOnly, Category = "Keyword Description")
	FKeywordDefinitionData SelectedKeyword;
private:
	UFUNCTION()
	void HandleSelectionChanged(FString SelectedItem, ESelectInfo::Type SelectionType);
	void RefreshDescription();
	UPROPERTY(Transient)
	TArray<FKeywordDefinitionData> Options;
	UPROPERTY(Transient)
	TMap<FName, FKeywordDefinitionData> Definitions;
	UPROPERTY(Transient)
	TObjectPtr<class UDataTable> TextStyles;
};
