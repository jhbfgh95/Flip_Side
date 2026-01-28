// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "CardWSubsystem.generated.h"

/**
 * 
 */
UCLASS()
class FLIP_SIDE_API UCardWSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()


	//카드 총 정보를 받아와서 할당해야함
	private:
	//TArray로 카드 전체를 저장하는 배열

	int32 CurrentCardNum;
public:
	
	//현재 카드 배열중 선택하는함수
	void SelectCard(int CardID);
	

	//다음 카드 번호 설정
	void SetNextCard();
	//이전 카드 번호 설정
	void SetPreviousCard();

	private:
	//현재 내가 선택한 카드에 대한 정보를 담고 있는 배열
	//TArray ~~~
	public:
	//지금 제작중인 카드 번호
	int32 SelectedCardIndex;
	
	void UnSelectCard(int SelectArrayNum);
	
};
