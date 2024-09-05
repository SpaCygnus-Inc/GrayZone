// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/Weapons/WeaponBase.h"

// Sets default values
AWeaponBase::AWeaponBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

    this->m_attacksComponent = this->CreateDefaultSubobject<UAttacksComponent>(TEXT("Attacks"));
    this->m_attacksComponent->OnAttackEnd.AddUObject(this, &AWeaponBase::ResetWeapon);
}

void AWeaponBase::InitializeWeapon(TObjectPtr<USkeletalMeshComponent> const playerMesh, FString const rightHandIk, FString const leftHandIk)
{
    this->m_attacksComponent->Initialize(playerMesh->GetAnimInstance());
}

// Called when the game starts or when spawned
void AWeaponBase::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AWeaponBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AWeaponBase::StartAttackAnim(TObjectPtr<UAnimInstance> const playerAnim)
{
    //We set all the attack config.
    auto attackInfo       = FAttackInfo();
    attackInfo.AttackAnim = this->m_attackMontage;
    attackInfo.Damage     = 0;

    this->m_weaponState = EWeaponState::ATTACK;                              //We change the weapon state.
    this->m_attacksComponent->BasicProjectileAttack(attackInfo); //And then finally we play the attack.
}

void AWeaponBase::WeaponVisibility(bool visible)
{
    this->SetActorHiddenInGame(!visible);
    this->SetActorEnableCollision(visible);
    this->SetActorTickEnabled(visible);
}

