#include "Subsystem/CursorGISubsystem.h"
#include "GameFramework/GameMode.h"

void UCursorGISubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);

    PostLoginHandle = FGameModeEvents::GameModePostLoginEvent.AddUObject(
        this, &UCursorGISubsystem::OnPlayerLogin
    );
}

void UCursorGISubsystem::Deinitialize()
{
    FGameModeEvents::GameModePostLoginEvent.Remove(PostLoginHandle);
    Super::Deinitialize();
}

void UCursorGISubsystem::OnPlayerLogin(AGameModeBase* GameMode, APlayerController* NewPlayer)
{
    if (!NewPlayer) return;

    UWorld* World = NewPlayer->GetWorld();
    if (!World || !World->IsGameWorld()) return;

    NewPlayer->bShowMouseCursor = true;
}

void UCursorGISubsystem::SetCursorState(int32 State)
{
    CurrentCursorState = State;
}
