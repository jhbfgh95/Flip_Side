#pragma once
#include "CoreMinimal.h"
#include "Components/RichTextBlockDecorator.h"
#include "CoinDescriptionRichTextDecorator.generated.h"

UCLASS()
class FLIP_SIDE_API UCoinDescriptionRichTextDecorator : public URichTextBlockDecorator
{
	GENERATED_BODY()
public:
	virtual TSharedPtr<ITextDecorator> CreateDecorator(URichTextBlock* InOwner) override;
};
