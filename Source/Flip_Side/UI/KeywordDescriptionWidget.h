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
	// 코인 슬롯 책갈피에서만 표시 후보를 제한합니다. 기존 사전 사용처에는 적용하지 않습니다.
	void SetContextKeywords(const TArray<FName>& KeywordCodes);
protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Keyword Description")
	EKeywordDescriptionGroup KeywordGroup = EKeywordDescriptionGroup::Main;
	// 드롭다운을 펼치거나 닫을 때 화살표가 180도 도는 시간(초)입니다. 0이면 즉시 바뀝니다.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Keyword Description", meta = (ClampMin = "0.0"))
	float ArrowRotateDuration = 0.15f;
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UComboBoxString> KeywordDropdown;
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<class URichTextBlock> KeywordDescriptionText;
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<class UImage> KeywordIcon;
	UPROPERTY(BlueprintReadOnly, Category = "Keyword Description")
	FKeywordDefinitionData SelectedKeyword;
private:
	bool bUseContextKeywords = false;
	TArray<FName> ContextKeywordCodes;
	UFUNCTION()
	void HandleSelectionChanged(FString SelectedItem, ESelectInfo::Type SelectionType);
	void RefreshDescription();
	void UpdateDropdownArrow(float DeltaTime);
	void CollectDropdownArrowImages();
	// 콤보 버튼 내부의 화살표 SImage(그림자 포함)입니다. 위젯이 재생성되면 다시 찾습니다.
	TArray<TWeakPtr<SWidget>> DropdownArrowImages;
	float DropdownArrowAngle = 0.f;
	// 줄바꿈 위치를 계산한 설명 칸 폭입니다. 폭이 바뀌면 다시 계산합니다.
	float LastDescriptionWidth = 0.f;
	UPROPERTY(Transient)
	TArray<FKeywordDefinitionData> Options;
	UPROPERTY(Transient)
	TMap<FName, FKeywordDefinitionData> Definitions;
	UPROPERTY(Transient)
	TObjectPtr<class UDataTable> TextStyles;
};
