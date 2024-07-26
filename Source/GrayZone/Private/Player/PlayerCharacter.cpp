// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/PlayerCharacter.h"

// Sets default values
APlayerCharacter::APlayerCharacter()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

    //We make sure that the player will always be able to control this player.
    this->AutoPossessPlayer = EAutoReceiveInput::Player0;

    //Initialize the collider and set it as the root component.
    this->m_collider = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Collider"));
    this->RootComponent = this->m_collider;

    //Initialize the mesh.
    this->m_mesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Mesh"));
    this->m_mesh->SetupAttachment(this->RootComponent);

    //Initialize the arrow component
    this->m_arrow = CreateDefaultSubobject<UArrowComponent>(TEXT("ForwardDirection"));
    this->m_arrow->SetupAttachment(this->RootComponent);
}

// Called when the game starts or when spawned
void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);


    if (!this->m_currentVelocity.IsZero())
    {
        FVector newPosition = GetActorLocation() + (this->m_currentVelocity * DeltaTime);

        FRotator desiredRotation = this->m_currentVelocity.Rotation();
        //FRotator newRotation = FMath::RInterpConstantTo(this->GetActorRotation(), desiredRotation, DeltaTime, 100); 

        this->SetActorLocation(newPosition);
        this->SetActorRotation(desiredRotation);
    }
}

// Called to bind functionality to input
void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

    PlayerInputComponent->BindAxis("MoveForward", this, &APlayerCharacter::MoveForward);
    PlayerInputComponent->BindAxis("MoveRight", this, &APlayerCharacter::MoveRight);
}

void APlayerCharacter::MoveForward(float value)
{
    this->m_currentVelocity.X = FMath::Clamp(value, -1, 1) * this->m_moveSpeed;
}

void APlayerCharacter::MoveRight(float value)
{
    this->m_currentVelocity.Y = FMath::Clamp(value, -1, 1) * this->m_moveSpeed;
}

