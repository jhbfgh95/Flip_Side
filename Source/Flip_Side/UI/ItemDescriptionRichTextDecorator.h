#pragma once
#include "CoreMinimal.h"
#include "Components/RichTextBlockDecorator.h"
#include "ItemDescriptionRichTextDecorator.generated.h"

UCLASS()
class FLIP_SIDE_API UItemDescriptionRichTextDecorator : public URichTextBlockDecorator
{
	GENERATED_BODY()
public:
	virtual TSharedPtr<ITextDecorator> CreateDecorator(URichTextBlock* InOwner) override;
};
