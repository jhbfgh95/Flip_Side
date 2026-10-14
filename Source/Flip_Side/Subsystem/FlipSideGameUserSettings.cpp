// Fill out your copyright notice in the Description page of Project Settings.


#include "Subsystem/FlipSideGameUserSettings.h"

UFlipSideGameUserSettings* UFlipSideGameUserSettings::GetFlipSideGameUserSettings()
{
	return Cast<UFlipSideGameUserSettings>(UGameUserSettings::GetGameUserSettings());
}
