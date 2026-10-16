#pragma once
#include "CoreMinimal.h"
#include "Components/RichTextBlockDecorator.h"
#include "CoinDescriptionRichTextDecorator.generated.h"

class UUserWidget;

UCLASS()
class FLIP_SIDE_API UCoinDescriptionRichTextDecorator : public URichTextBlockDecorator
{
	GENERATED_BODY()
public:
	virtual TSharedPtr<ITextDecorator> CreateDecorator(URichTextBlock* InOwner) override;

	// 황금 카드의 표와 적용 효과 창에서 같은 Shift 상세 표시 형식을 사용합니다.
	static TSharedPtr<SWidget> CreateDetailedStatDisplay(const UUserWidget* Widget, const FTextRunInfo& Run,
		const FTextBlockStyle& Style, const FVector2D& IconSize, float ValueSpacing, float NameSpacing, FName Typeface);
};
