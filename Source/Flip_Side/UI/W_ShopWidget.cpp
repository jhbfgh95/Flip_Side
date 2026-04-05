#include "W_ShopWidget.h"
#include "Components/Button.h"
#include "LevelGISubsystem.h"

void UW_ShopWidget::NativeOnInitialized()
{
    Button_NextLevel->OnClicked.AddDynamic(this, &UW_ShopWidget::HandleNextClicked);
}

void UW_ShopWidget::HandleNextClicked()
{
    UGameInstance* GI = GetGameInstance();
    if(!GI) return;

    ULevelGISubsystem* LevelSubsystem = GI->GetSubsystem<ULevelGISubsystem>();
    int32 CurrentBattleLevel = LevelSubsystem->GetBattleLevelIndex();
    
    LevelSubsystem->MoveBattleLevel();
  
}