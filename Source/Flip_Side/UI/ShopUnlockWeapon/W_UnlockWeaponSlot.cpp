// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/ShopUnlockWeapon/W_UnlockWeaponSlot.h"
#include "Components/Border.h"
#include "Components/Image.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "Input/Reply.h"
#include "InputCoreTypes.h"
#include "TimerManager.h"

void UW_UnlockWeaponSlot::NativeConstruct()
{
    Super::NativeConstruct();
    if (HoverBorder)
    {
        HoverBorder->SetVisibility(ESlateVisibility::Hidden);
    }

    if (HoldProgressBar)
    {
        HoldProgressBar->SetPercent(0.0f);
    }
}

FReply UW_UnlockWeaponSlot::NativeOnMouseButtonDown(
    const FGeometry& InGeometry,
    const FPointerEvent& InMouseEvent)
{
    if (InMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
    {
        bIsPointerPressed = true;
        PressSlot();
        return FReply::Handled().CaptureMouse(TakeWidget());
    }

    return Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
}

FReply UW_UnlockWeaponSlot::NativeOnMouseButtonUp(
    const FGeometry& InGeometry,
    const FPointerEvent& InMouseEvent)
{
    if (InMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton && bIsPointerPressed)
    {
        const bool bShouldClick = IsHovered();
        bIsPointerPressed = false;
        ReleaseSlot();

        if (bShouldClick)
        {
            ClickSlot();
        }

        return FReply::Handled().ReleaseMouseCapture();
    }

    return Super::NativeOnMouseButtonUp(InGeometry, InMouseEvent);
}

void UW_UnlockWeaponSlot::NativeOnMouseEnter(
    const FGeometry& InGeometry,
    const FPointerEvent& InMouseEvent)
{
    Super::NativeOnMouseEnter(InGeometry, InMouseEvent);
    HoverSlot();
}

void UW_UnlockWeaponSlot::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{
    Super::NativeOnMouseLeave(InMouseEvent);
    UnhoverSlot();
}

void UW_UnlockWeaponSlot::NativeDestruct()
{
    CancelHold();
    Super::NativeDestruct();
}

void UW_UnlockWeaponSlot::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
    Super::NativeTick(MyGeometry, InDeltaTime);

    if (!HoldProgressBar)
        return;

    if (bIsHolding && GetWorld())
    {
        const float ElapsedHoldTime = GetWorld()->GetTimeSeconds() - HoldStartTime;
        HoldProgressBar->SetPercent(FMath::Clamp(ElapsedHoldTime / HoldDuration, 0.0f, 1.0f));
        return;
    }

    if (bResettingProgress)
    {
        const float ResetPercent = FMath::FInterpTo(HoldProgressBar->GetPercent(), 0.0f, InDeltaTime, ProgressResetSpeed);
        HoldProgressBar->SetPercent(ResetPercent);

        if (FMath::IsNearlyZero(ResetPercent, KINDA_SMALL_NUMBER))
        {
            HoldProgressBar->SetPercent(0.0f);
            bResettingProgress = false;
        }
    }
}

void UW_UnlockWeaponSlot::InitWidget(const FFaceData& InitWeaponData)
{
    UnlockWeaponData = InitWeaponData;
    WeaponImage->SetBrushFromTexture(UnlockWeaponData.WeaponIcon);
    WeaponName->SetText(FText::FromString(UnlockWeaponData.WeaponName));
}


void UW_UnlockWeaponSlot::ClickSlot()
{
    // 홀드 구매 뒤 Release에서 선택 이벤트가 이어지지 않도록 막습니다.
    if (bHoldCompleted)
    {
        bHoldCompleted = false;
        return;
    }

    OnClickedUnlockWeaponSlot.Broadcast(UnlockWeaponData.WeaponID);
}

void UW_UnlockWeaponSlot::HoverSlot()
{
    if (HoverBorder)
    {
        HoverBorder->SetVisibility(ESlateVisibility::HitTestInvisible);
    }

    OnHoveredUnlockWeaponSlot.Broadcast(UnlockWeaponData.WeaponID);
}
	
void UW_UnlockWeaponSlot::UnhoverSlot()
{
	CancelHold();

    if (HoverBorder)
    {
        HoverBorder->SetVisibility(ESlateVisibility::Hidden);
    }

    OnUnhoveredUnlockWeaponSlot.Broadcast();
}

void UW_UnlockWeaponSlot::PressSlot()
{
    if (!GetWorld())
        return;

    bIsHolding = true;
    bHoldCompleted = false;
    bResettingProgress = false;
    HoldStartTime = GetWorld()->GetTimeSeconds();

    if (HoldProgressBar)
    {
        HoldProgressBar->SetPercent(0.0f);
    }

    OnHoldStartedUnlockWeaponSlot.Broadcast(UnlockWeaponData.WeaponID);

    GetWorld()->GetTimerManager().SetTimer(
        HoldTimerHandle, this, &UW_UnlockWeaponSlot::CompleteHold, HoldDuration, false);
}

void UW_UnlockWeaponSlot::ReleaseSlot()
{
    CancelHold();
}

void UW_UnlockWeaponSlot::CompleteHold()
{
    if (!bIsHolding)
        return;

    bIsHolding = false;
    bHoldCompleted = true;

    if (HoldProgressBar)
    {
        HoldProgressBar->SetPercent(1.0f);
    }

    OnHoldCompletedUnlockWeaponSlot.Broadcast(UnlockWeaponData.WeaponID);
}

void UW_UnlockWeaponSlot::CancelHold()
{
    if (GetWorld())
        GetWorld()->GetTimerManager().ClearTimer(HoldTimerHandle);

    // 누르기를 멈추면 즉시 취소되고, 남은 게이지는 빠르게 0으로 돌아갑니다.
    bResettingProgress = true;

    if (!bIsHolding || bHoldCompleted)
        return;

    bIsHolding = false;
    OnHoldCancelledUnlockWeaponSlot.Broadcast(UnlockWeaponData.WeaponID);
}
