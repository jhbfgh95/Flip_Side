#pragma once

#include "CoreMinimal.h"
#include "Components/RichTextBlockDecorator.h"
#include "Styling/SlateBrush.h"
#include "WeaponDescriptionRichTextDecorator.generated.h"

class UTexture2D;

/**
 * 무기 설명의 [KW:*], [STAT:*] 토큰을 키워드 배지와 인라인 스탯으로 렌더링합니다.
 * DB 로드는 하지 않고, W_CoinSlotInfo에서 전달받은 아이콘과 임시 색상만 사용합니다.
 */
UCLASS()
class FLIP_SIDE_API UWeaponDescriptionRichTextDecorator : public URichTextBlockDecorator
{
	GENERATED_BODY()

public:
	virtual TSharedPtr<ITextDecorator> CreateDecorator(class URichTextBlock* InOwner) override;

	void ConfigureStatPresentation(
		UTexture2D* InAttackPowerIcon,
		UTexture2D* InWeaponPowerIcon,
		UTexture2D* InCountIcon,
		const FLinearColor& InAttackPowerColor,
		const FLinearColor& InWeaponPowerColor,
		const FLinearColor& InCountColor
	);

	const FSlateBrush* FindStatIcon(FName StatCode) const;
	FLinearColor ResolveTokenColor(FName TokenCode) const;
	FText ResolveTokenDisplayName(FName TokenCode) const;

private:
	void SetBrushTexture(FSlateBrush& Brush, UTexture2D* Texture);

	UPROPERTY(Transient)
	TObjectPtr<UTexture2D> AttackPowerIcon;

	UPROPERTY(Transient)
	TObjectPtr<UTexture2D> WeaponPowerIcon;

	UPROPERTY(Transient)
	TObjectPtr<UTexture2D> CountIcon;

	FSlateBrush AttackPowerBrush;
	FSlateBrush WeaponPowerBrush;
	FSlateBrush CountBrush;

	FLinearColor AttackPowerColor = FLinearColor(1.0f, 0.32f, 0.28f, 1.0f);
	FLinearColor WeaponPowerColor = FLinearColor(0.25f, 0.65f, 1.0f, 1.0f);
	FLinearColor CountColor = FLinearColor(1.0f, 0.76f, 0.2f, 1.0f);
};
