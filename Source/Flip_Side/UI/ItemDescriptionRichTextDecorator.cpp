#include "UI/ItemDescriptionRichTextDecorator.h"
#include "UI/W_ItemInfo.h"
#include "Components/RichTextBlock.h"

namespace ItemDescriptionPrivate
{
	class FKeywordDecorator : public FRichTextDecorator
	{
	public:
		explicit FKeywordDecorator(URichTextBlock* Owner) : FRichTextDecorator(Owner),
			Info(Owner ? Owner->GetTypedOuter<UW_ItemInfo>() : nullptr),
			bHeader(Owner && Owner->GetFName() == TEXT("ItemKeywordHeader")) {}
		virtual bool Supports(const FTextRunParseResults& Run, const FString& Text) const override
		{
			return Run.Name == TEXT("itemkw") && Run.MetaData.Contains(TEXT("code"));
		}
	protected:
		virtual TSharedPtr<SWidget> CreateDecoratorWidget(const FTextRunInfo& Run, const FTextBlockStyle& Style) const override
		{
			const FString* Code = Run.MetaData.Find(TEXT("code"));
			return Info.IsValid() && Code ? Info->CreateKeywordDisplay(FName(**Code), Style, bHeader) : nullptr;
		}
	private:
		TWeakObjectPtr<UW_ItemInfo> Info;
		bool bHeader = false;
	};
}

TSharedPtr<ITextDecorator> UItemDescriptionRichTextDecorator::CreateDecorator(URichTextBlock* InOwner)
{
	return MakeShared<ItemDescriptionPrivate::FKeywordDecorator>(InOwner);
}
