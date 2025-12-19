// Copyright 2024, Studio Zenith. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "DecayHUD.generated.h"

class UUserWidget;

UCLASS()
class UMBRALDECAY_API ADecayHUD : public AHUD
{
	GENERATED_BODY()

public:
    ADecayHUD();

    /** Primary entry point for creating and binding player-facing HUD elements */
    void InitializePlayerHUD();

    /** Toggles the visibility of the inventory UI */
    void ToggleInventoryWidget();

protected:
    virtual void BeginPlay() override;

    UFUNCTION()
    void OnHealthUpdate(float CurrentHealth, float MaxHealth);

private:
    /** Binds HUD update functions to player character's stat delegates. */
    void BindToPlayerStats();

    UPROPERTY(EditDefaultsOnly, Category = "Widgets")
    TSubclassOf<UUserWidget> PlayerHUDWidgetClass;

    UPROPERTY(EditDefaultsOnly, Category = "Widgets")
    TSubclassOf<UUserWidget> InventoryWidgetClass;

    UPROPERTY()
    TObjectPtr<UUserWidget> PlayerHUDWidget;
    
    UPROPERTY()
    TObjectPtr<UUserWidget> InventoryWidget;

    bool bIsInventoryVisible = false;
};
