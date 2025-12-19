// Copyright 2024, Studio Zenith. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "DecayPlayerCharacter.generated.h"

class UCameraComponent;
class USpringArmComponent;
class UHealthComponent;
class UInventoryComponent;

UCLASS(config=Game)
class UMBRALDECAY_API ADecayPlayerCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	ADecayPlayerCharacter();

	/** Returns TopDownCameraComponent subobject **/ 
	FORCEINLINE UCameraComponent* GetTopDownCameraComponent() const { return TopDownCameraComponent; }
	/** Returns CameraBoom subobject **/ 
	FORCEINLINE USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
    /** Returns HealthComponent subobject **/
    FORCEINLINE UHealthComponent* GetHealthComponent() const { return HealthComponent; }
    /** Returns InventoryComponent subobject **/
    FORCEINLINE UInventoryComponent* GetInventoryComponent() const { return InventoryComponent; }

protected:
	virtual void BeginPlay() override;
	
    virtual void Tick(float DeltaSeconds) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	// Movement handlers
	void MoveForward(float Value);
	void MoveRight(float Value);

    // Action handlers
    void PrimaryAction();
    void SecondaryAction();
    void Interact();

    /** Server-side implementation of PrimaryAction. */
    UFUNCTION(Server, Reliable, WithValidation)
    void Server_PrimaryAction();

    UFUNCTION()
    void HandleDeath();

private:
	/** Top down camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* TopDownCameraComponent;

	/** Camera boom positioning the camera above the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* CameraBoom;

    /** Player's health component */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
    UHealthComponent* HealthComponent;

    /** Player's inventory component */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
    UInventoryComponent* InventoryComponent;
};
