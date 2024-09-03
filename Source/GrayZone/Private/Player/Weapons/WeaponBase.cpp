// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/Weapons/WeaponBase.h"

// Sets default values
AWeaponBase::AWeaponBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

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
    this->m_weaponState = EWeaponState::ATTACK;
    playerAnim->Montage_Play(this->m_attackMontage);
}

void AWeaponBase::WeaponVisibility(bool visible)
{
    this->SetActorHiddenInGame(!visible);
    this->SetActorEnableCollision(visible);
    this->SetActorTickEnabled(visible);
}

