#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "CursorGISubsystem.generated.h"

UCLASS()
class FLIP_SIDE_API UCursorGISubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    virtual void Deinitialize() override;

    UFUNCTION(BlueprintCallable)
    void SetCursorState(int32 State);

    UFUNCTION(BlueprintPure)
    int32 GetCursorState() const { return CurrentCursorState; }

private:
    FDelegateHandle PostLoginHandle;

    int32 CurrentCursorState = 0;

    void OnPlayerLogin(AGameModeBase* GameMode, APlayerController* NewPlayer);
};
