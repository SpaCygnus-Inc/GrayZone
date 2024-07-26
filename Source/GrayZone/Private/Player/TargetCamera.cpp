// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/TargetCamera.h"
#include "Kismet/GameplayStatics.h"

FRotator const ATargetCamera::CAMERA_ROTATION = FRotator(-45.0f, -45.f, 0);

// Sets default values
ATargetCamera::ATargetCamera()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

    this->m_cameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComponent"));
    this->RootComponent = this->m_cameraComponent;
}

// Called when the game starts or when spawned
void ATargetCamera::BeginPlay()
{
	Super::BeginPlay();

    UGameplayStatics::GetPlayerController(this->GetWorld(), 0)->SetViewTarget(this); //We make sure that this camera is always set as the default one.
}

// Called every frame
void ATargetCamera::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

    if (this->m_target.IsNull())
    {
        UE_LOG(LogTemp, Warning, TEXT("No target was set for the camera to follow."));
        return;
    }

    auto desiredPosition = this->m_target->GetTargetLocation() + this->m_cameraOffset; //The final position will be the current target position plus the camera offset.

    if (!this->m_target->IsMoving() && this->GetActorLocation() == desiredPosition) return; //If the player isn't moving and the camera is already in the right position then no need to do anything else.

    auto nextPosition = FMath::VInterpTo(this->m_target->GetTargetLocation(), desiredPosition, DeltaTime, this->m_interpolationSpeed);
    nextPosition.Z = desiredPosition.Z;
    this->SetActorLocation(nextPosition);
}

void ATargetCamera::SetTarget(TObjectPtr<APlayerCharacter> target)
{
    this->m_target = target;

    this->SetActorLocation(this->m_target->GetActorLocation() + this->m_cameraOffset); //We initialize the camera position to be on the target.
    this->SetActorRotation(CAMERA_ROTATION);                                           //And then we set the initial camera rotation which will never change.
}
