// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/PlayerCharacter.h"

FString const APlayerCharacter::RIGHT_HAND_IK = TEXT("item_r");
FString const APlayerCharacter::LEFT_HAND_IK  = TEXT("item_l");

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

    //Initialize the arrow component.
    this->m_arrow = CreateDefaultSubobject<UArrowComponent>(TEXT("ForwardDirection"));
    this->m_arrow->SetupAttachment(this->RootComponent);

    //Initialize velocities and movement directions.
    m_forwardVector = m_rightVector = FVector::ZeroVector;
    m_forwardVelocity = m_rightVelocity = FVector::ZeroVector;
}

// Called when the game starts or when spawned
void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

    this->EquipWeapon(this->m_equippedWeaponType);
}

// Called every frame
void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

    if (this->IsMoving())
    {
        auto velocity        = this->GetCurrentVelocity();
        auto desiredRotation = velocity.Rotation(); //We get the rotation of the character using the current movement direction.

        FHitResult hitResult;
        this->AddActorWorldOffset(velocity * DeltaTime, true, &hitResult); //We try to move the character toward that direction.

        //If the player hit a collider then we check whether we should do a "slide" or not (usually happen when the player is moving perpendicularly).
        if (hitResult.IsValidBlockingHit())
        {
            velocity = FVector::VectorPlaneProject(velocity * DeltaTime, hitResult.Normal);
            this->AddActorWorldOffset(velocity, true);
        }

        this->SetActorRotation(desiredRotation); //Finally we apply the rotation.
    }
}

void APlayerCharacter::Initialize(FVector forwardVec, FVector rightVec)
{
    this->m_forwardVector = forwardVec.GetSafeNormal();
    this->m_rightVector   = rightVec.GetSafeNormal();
}

// Called to bind functionality to input
void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

    PlayerInputComponent->BindAxis("MoveForward", this, &APlayerCharacter::MoveForward);
    PlayerInputComponent->BindAxis("MoveRight", this, &APlayerCharacter::MoveRight);
    PlayerInputComponent->BindAction("Attack", IE_Pressed, this, &APlayerCharacter::Attack);
}

void APlayerCharacter::MoveForward(float value)
{
    this->m_forwardVelocity = m_forwardVector * FMath::Clamp(value, -1, 1);
}

void APlayerCharacter::MoveRight(float value)
{
    this->m_rightVelocity = m_rightVector * FMath::Clamp(value, -1, 1);
}

void APlayerCharacter::Attack()
{
    if (m_equippedWeapon == nullptr) return;

    this->m_attacking = true;
    this->m_equippedWeapon->StartAttackAnim(this->m_mesh->GetAnimInstance());
}

void APlayerCharacter::AttackEnded()
{
    this->m_equippedWeapon->ResetWeapon();
    this->m_attacking = false;
}

void APlayerCharacter::EquipWeapon(EWeaponType weaponType)
{
    if (weaponType > this->m_allWeaponTypes.Num())
    {
        UE_LOG(LogTemp, Fatal, TEXT(""));
        return;
    }

    if (!this->m_equippedWeapon.IsNull()) this->m_equippedWeapon->Destroy(); //If there's already a weapon equipped, we destroy it...
    if (weaponType > 0) //Then we spawn the specified weapon and initialize it...
    {
        FActorSpawnParameters params;
        params.Owner = this;
        this->m_equippedWeapon = this->GetWorld()->SpawnActor<AWeaponBase>(this->m_allWeaponTypes[weaponType - 1], this->GetActorLocation(), FRotator::ZeroRotator, params);
        
        this->m_equippedWeapon->InitializeWeapon(this->m_mesh, APlayerCharacter::RIGHT_HAND_IK, APlayerCharacter::LEFT_HAND_IK);
    }



    this->m_equippedWeaponType = weaponType; //And finally change the equipped weapon type variable to the right one.
}

