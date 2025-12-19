// Copyright 2024, Studio Zenith. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "DecayGameInstance.generated.h"

class UDecaySaveGame;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnSaveLoadComplete, const FString&, SlotName, bool, bSuccess);

UCLASS()
class UMBRALDECAY_API UDecayGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
    virtual void Init() override;

    /** Starts an async process to save the current game state */
    UFUNCTION(BlueprintCallable, Category = "Save/Load")
    void SaveGame(const FString& SlotName = ""); // If empty, uses default

    /** Starts an async process to load game state */
    UFUNCTION(BlueprintCallable, Category = "Save/Load")
    void LoadGame(const FString& SlotName = ""); // If empty, uses default

    UPROPERTY(BlueprintAssignable, Category = "Save/Load")
    FOnSaveLoadComplete OnGameSaved;

    UPROPERTY(BlueprintAssignable, Category = "Save/Load")
    FOnSaveLoadComplete OnGameLoaded;

protected:

    void OnSaveGameComplete(const FString& SlotName, const int32 UserIndex, bool bSuccess);
    void OnLoadGameComplete(const FString& SlotName, const int32 UserIndex, class USaveGame* LoadedData);

    /** Gathers data from game systems and populates the save game object */
    void PopulateSaveData(UDecaySaveGame* SaveData);

    /** Applies data from a loaded save game object to the current game state */
    void ApplySaveData(UDecaySaveGame* SaveData);

private:
    static const FString DEFAULT_SLOT_NAME;
    static const int32 DEFAULT_USER_INDEX;

};

// Separate file: DecaySaveGame.h
UCLASS()
class UMBRALDECAY_API UDecaySaveGame : public USaveGame
{
    GENERATED_BODY()
public:
    UPROPERTY(VisibleAnywhere, Category = Basic)
    FTransform PlayerTransform;

    UPROPERTY(VisibleAnywhere, Category = Basic)
    float TimeOfDay;

    // Add TArrays for inventory items, world state, etc.
};
