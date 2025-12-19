DecayPlayerCharacter.cpp 
// Copyright 2024, Studio Zenith. All Rights Reserved.

#include "Player/DecayPlayerCharacter.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/PlayerController.h"
#include "Engine/World.h"
#include "Kismet/KismetMathLibrary.h"
#include "Components/HealthComponent.h"
#include "Components/InventoryComponent.h"
#include "Net/UnrealNetwork.h"

ADecayPlayerCharacter::ADecayPlayerCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 720.0f, 0.0f); // ...at this rotation rate
	GetCharacterMovement()->GravityScale = 2.f;
	GetCharacterMovement()->AirControl = 0.5f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->SetUsingAbsoluteRotation(true); // Don't want arm to rotate with character
	CameraBoom->TargetArmLength = 1200.f;
	CameraBoom->SetRelativeRotation(FRotator(-60.f, 0.f, 0.f));
	CameraBoom->bDoCollisionTest = false; // Or true if you want the camera to fly over scenery

	// Create a top-down camera
	TopDownCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("TopDownCamera"));
	TopDownCameraComponent->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	TopDownCameraComponent->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

    // Create core components
    HealthComponent = CreateDefaultSubobject<UHealthComponent>(TEXT("HealthComponent"));
    InventoryComponent = CreateDefaultSubobject<UInventoryComponent>(TEXT("InventoryComponent"));

	// Set this character to call Tick() every frame.
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;
	NetUpdateFrequency = 66.f;
	MinNetUpdateFrequency = 33.f;
}

void ADecayPlayerCharacter::BeginPlay()
{
    Super::BeginPlay();

    if (HealthComponent)
    {
        HealthComponent->OnDeath.AddDynamic(this, &ADecayPlayerCharacter::HandleDeath);
    }
}

void ADecayPlayerCharacter::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);

    // We only need to do this logic on the client that controls this pawn
    if (IsLocallyControlled())
    {
        // Rotate player to face mouse cursor
        APlayerController* PC = Cast<APlayerController>(GetController());
        if (PC)
        {
            FHitResult HitResult;
            // Use a specific trace channel for targeting, to avoid hitting invisible collision or triggers
            PC->GetHitResultUnderCursor(ECC_Visibility, true, HitResult);

            if (HitResult.bBlockingHit)
            _            {
                FVector TargetLocation = HitResult.ImpactPoint;
                // Project to the same Z-plane as the character to ensure rotation is only on Yaw
                FVector Direction = (TargetLocation - GetActorLocation()).GetSafeNormal();
                Direction.Z = 0.0f;

                FRotator TargetRotation = Direction.Rotation();
                SetActorRotation(TargetRotation);
            }
        }
    }
}

void ADecayPlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	check(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveForward", this, &ADecayPlayerCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ADecayPlayerCharacter::MoveRight);

	PlayerInputComponent->BindAction("PrimaryAction", IE_Pressed, this, &ADecayPlayerCharacter::PrimaryAction);
    PlayerInputComponent->BindAction("SecondaryAction", IE_Pressed, this, &ADecayPlayerCharacter::SecondaryAction);
    PlayerInputComponent->BindAction("Interact", IE_Pressed, this, &ADecayPlayerCharacter::Interact);
}


void ADecayPlayerCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    // Replicate crucial state information. Components handle their own replication.
}

void ADecayPlayerCharacter::MoveForward(float Value)
{
	if ((Controller != nullptr) && (Value != 0.0f))
	{
        const FVector Direction = FVector::ForwardVector;
		AddMovementInput(Direction, Value);
	}
}

void ADecayPlayerCharacter::MoveRight(float Value)
{
	if ((Controller != nullptr) && (Value != 0.0f))
	{
		const FVector Direction = FVector::RightVector;
		AddMovementInput(Direction, Value);
	}
}


void ADecayPlayerCharacter::PrimaryAction()
{
    // Placeholder for firing weapon or using item
    if (InventoryComponent)
    {
        // InventoryComponent->UseEquippedItem();
    }
    Server_PrimaryAction(); // RPC call
}

bool ADecayPlayerCharacter::Server_PrimaryAction_Validate()
{ 
    // Add validation logic here (e.g., check for ammo, cooldowns) to prevent cheating
    return true; 
}

void ADecayPlayerCharacter::Server_PrimaryAction_Implementation()
{
    // Logic to be executed on the server
    // e.g., spawn projectile, apply damage
    // Then, if needed, multicast the cosmetic effects
    // Multicast_PrimaryAction();
}

void ADecayPlayerCharacter::SecondaryAction()
{
    // Aiming logic, etc.
}

void ADecayPlayerCharacter::Interact()
{
    // Sphere trace to find interactable objects in front of the player
    // Server call to perform the interaction
}

void ADecayPlayerCharacter::HandleDeath()
{
    // Disable input, ragdoll physics, notify game mode
    GetCharacterMovement()->DisableMovement();
    GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    GetMesh()->SetCollisionProfileName(TEXT("Ragdoll"));
    GetMesh()->SetSimulatePhysics(true);
    
    // Notify Game Director/Mode of player death
    // ADecayGameDirector* GameDirector = ...

    // On server, set a timer to handle respawn logic or spectator mode
    if (HasAuthority())
    {
        // Start spectating, or show a death screen etc.
    }

}
