#include "UI/CoinDescriptionRichTextDecorator.h"
#include "UI/CoinDescriptionSectionWidget.h"
#include "Components/RichTextBlock.h"

namespace CoinDescriptionRichTextPrivate
{
	class FInlineDecorator : public FRichTextDecorator
	{
	public:
		explicit FInlineDecorator(URichTextBlock* InOwner) : FRichTextDecorator(InOwner),
			Section(InOwner ? InOwner->GetTypedOuter<UCoinDescriptionSectionWidget>() : nullptr) {}
		virtual bool Supports(const FTextRunParseResults& Run, const FString& Text) const override
		{
			return Run.Name == TEXT("coin") && Run.MetaData.Contains(TEXT("key"));
		}
	protected:
		virtual TSharedPtr<SWidget> CreateDecoratorWidget(const FTextRunInfo& Run, const FTextBlockStyle& Style) const override
		{
			const FString* Key = Run.MetaData.Find(TEXT("key"));
				return Section.IsValid() && Key ? Section->CreateInlineDisplay(FName(**Key), Style) : nullptr;
		}
	private:
		TWeakObjectPtr<UCoinDescriptionSectionWidget> Section;
	};
}
TSharedPtr<ITextDecorator> UCoinDescriptionRichTextDecorator::CreateDecorator(URichTextBlock* InOwner)
{
	return MakeShared<CoinDescriptionRichTextPrivate::FInlineDecorator>(InOwner);
}
