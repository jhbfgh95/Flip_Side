// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "TemplateFunction_Utils.generated.h"

/**
	- 템플릿 함수 전용 클래스
 */
UCLASS()
class FLIP_SIDE_API UTemplateFunction_Utils : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	/* 모든 ENum에 대하여 랜덤으로 기능하는 함수(템플릿) */
	template<typename T>
	static T GetRandomENum()
	{
		//언리얼 전용 T의 그거 맞춰주는 함수임.이거까지 템플릿임ㅋㅋ
		static_assert(std::is_enum<T>::value, "T << ENum이어야함");

		const UEnum* EnumPtr = StaticEnum<T>();
		if(!EnumPtr) return T(0);

		int32 MaxIndex = EnumPtr->NumEnums() - 2;
		return static_cast<T>(EnumPtr->GetValueByIndex(FMath::RandRange(1, MaxIndex)));
	}

	/* 모든 UObject에 대하여 가중치에 따라 랜덤으로 기능하는 함수 (추후 )*/
};
