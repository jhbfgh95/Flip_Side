// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/ShopItem/ShopItemExplainPanel.h"
#include "Components/StaticMeshComponent.h"
#include "Components/WidgetComponent.h"
#include "Subsystem/ShopLevel/ShopItemWSubsystem.h"
#include "Subsystem/DataManagerSubsystem.h"
#include "UI/ShopItem/W_ShopItemDescription.h"
// Sets default values
AShopItemExplainPanel::AShopItemExplainPanel()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	RootScene = CreateDefaultSubobject<USceneComponent>(TEXT("RootScene"));
	RootComponent = RootScene;
	
	ExplainPanel = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PanelMesh"));
	ExplainPanel->SetupAttachment(RootComponent);

	ExplainWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("ExplainWidget"));
	ExplainWidget->SetupAttachment(RootComponent);

}

// Called when the game starts or when spawned
void AShopItemExplainPanel::BeginPlay()
{
	Super::BeginPlay();
	ShopItemSubsystem = GetWorld()->GetSubsystem<UShopItemWSubsystem>();
	ExplainWidgetClass = Cast<UW_ShopItemDescription>(ExplainWidget->GetUserWidgetObject());
	DataManager = GetWorld()->GetGameInstance()->GetSubsystem<UDataManagerSubsystem>();

	ShopItemSubsystem->OnItemHovered.AddDynamic(this, &AShopItemExplainPanel::SetExplainPanel);
	ShopItemSubsystem->OnItemUnHovered.AddDynamic(this, &AShopItemExplainPanel::ResetExplainPanelText);
	ResetExplainPanelText();
}


void AShopItemExplainPanel::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	
	ShopItemSubsystem->OnItemHovered.RemoveAll(this);
	ShopItemSubsystem->OnItemUnHovered.RemoveAll(this);
	Super::EndPlay(EndPlayReason);
}
	
void AShopItemExplainPanel::SetExplainPanel(FItemData ItemData)
{
	ExplainWidgetClass->SetItemInfo(ItemData.ItemIcon, TEXT("ITemName"), ItemData.Item_DES);
}

void AShopItemExplainPanel::ResetExplainPanelText()
{
	ExplainWidgetClass->ResetItemWidget();
}
