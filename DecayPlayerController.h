// Copyright 2024, Studio Zenith. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "DecayPlayerController.generated.h"

class UUserWidget;

UCLASS()
class UMBRALDECAY_API ADecayPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
    ADecayPlayerController();

protected:
    virtual void SetupInputComponent() override;

    virtual void OnPossess(APawn* InPawn) override;

    void UpdateCameraArmLength(float NewArmLength);
    void ZoomIn();
    void ZoomOut();

    void ToggleInventory();

private:
    /** Initializes the HUD after a short delay to ensure all actors are ready. */
    void InitializeHUD();

    UPROPERTY(EditDefaultsOnly, Category = "Camera")
    float MinZoomLevel;
    UPROPERTY(EditDefaultsOnly, Category = "Camera")
    float MaxZoomLevel;
    UPROPERTY(EditDefaultsOnly, Category = "Camera")
    float ZoomStep;
};
