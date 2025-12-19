// Copyright 2024, Studio Zenith. All Rights Reserved.

#include "AI/DecayZombieAIController.h"
#include "AI/DecayZombieCharacter.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Perception/AISenseConfig_Hearing.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"

ADecayZombieAIController::ADecayZombieAIController()
{
    PrimaryActorTick.bCanEverTick = true;

    PerceptionComponent = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("AIPerceptionComponent"));

    SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("SightConfig"));
    SightConfig->SightRadius = 2000.0f;
    SightConfig->LoseSightRadius = 2500.0f;
    SightConfig->PeripheralVisionAngleDegrees = 90.0f;
    SightConfig->DetectionByAffiliation.bDetectEnemies = true;
    SightConfig->DetectionByAffiliation.bDetectNeutrals = true;
    SightConfig->DetectionByAffiliation.bDetectFriendlies = true; // Can be useful for zombies detecting each other
    PerceptionComponent->ConfigureSense(*SightConfig);

    HearingConfig = CreateDefaultSubobject<UAISenseConfig_Hearing>(TEXT("HearingConfig"));
    HearingConfig->HearingRange = 3000.0f;
    HearingConfig->DetectionByAffiliation.bDetectEnemies = true;
    HearingConfig->DetectionByAffiliation.bDetectNeutrals = true;
    HearingConfig->DetectionByAffiliation.bDetectFriendlies = false;
    PerceptionComponent->ConfigureSense(*HearingConfig);

    PerceptionComponent->SetDominantSense(SightConfig->GetSenseImplementation());

    BlackboardComponent = CreateDefaultSubobject<UBlackboardComponent>(TEXT("BlackboardComponent"));
    BehaviorTreeComponent = CreateDefaultSubobject<UBehaviorTreeComponent>(TEXT("BehaviorTreeComponent"));
}

void ADecayZombieAIController::OnPossess(APawn* InPawn)
{
    Super::OnPossess(InPawn);

    ADecayZombieCharacter* ZombieCharacter = Cast<ADecayZombieCharacter>(InPawn);
    if (ZombieCharacter && ZombieCharacter->GetBehaviorTree())
    {
        BlackboardComponent->InitializeBlackboard(*(ZombieCharacter->GetBehaviorTree()->BlackboardAsset));
        BehaviorTreeComponent->StartTree(*(ZombieCharacter->GetBehaviorTree()));

        // Set default blackboard values
        BlackboardComponent->SetValueAsVector(TEXT("HomeLocation"), ZombieCharacter->GetActorLocation());
    }

    if (PerceptionComponent)
    {
        PerceptionComponent->OnTargetPerceptionUpdated.AddDynamic(this, &ADecayZombieAIController::OnTargetPerceptionUpdated);
    }
}

void ADecayZombieAIController::OnUnPossess()
{
    Super::OnUnPossess();
    BehaviorTreeComponent->StopTree();
}

void ADecayZombieAIController::OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
{
    // We are only interested in players for now
    if (Actor && Actor->IsA(ADecayPlayerCharacter::StaticClass()))
    {
        if (Stimulus.WasSuccessfullySensed())
        {
            // Player detected
            UE_LOG(LogTemp, Verbose, TEXT("Player detected at %s"), *Actor->GetActorLocation().ToString());
            BlackboardComponent->SetValueAsObject(TEXT("TargetActor"), Actor);
            BlackboardComponent->SetValueAsBool(TEXT("HasLineOfSight"), Stimulus.Type == UAISense::GetSenseID<UAISense_Sight>());
            BlackboardComponent->SetValueAsVector(TEXT("LastKnownLocation"), Actor->GetActorLocation());
        }
        else
        {            
            // Player lost
            UE_LOG(LogTemp, Verbose, TEXT("Player lost"));
            // Don't clear TargetActor immediately. The Behavior Tree should handle this.
            // It might want to search the LastKnownLocation.
            BlackboardComponent->SetValueAsBool(TEXT("HasLineOfSight"), false);
        }
    }

    // Handle hearing stimuli (e.g., gunshots)
    if (Stimulus.Type == UAISense::GetSenseID<UAISense_Hearing>() && Stimulus.WasSuccessfullySensed())
    {
        UE_LOG(LogTemp, Verbose, TEXT("Heard a noise at %s"), *Stimulus.StimulusLocation.ToString());
        // If we don't have a primary target, go investigate the sound
        if (BlackboardComponent->GetValueAsObject(TEXT("TargetActor")) == nullptr)
        {
            BlackboardComponent->SetValueAsVector(TEXT("TargetLocation"), Stimulus.StimulusLocation);
        }
    }
}
