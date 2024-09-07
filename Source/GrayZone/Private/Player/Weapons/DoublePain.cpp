// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/Weapons/DoublePain.h"

ADoublePain::ADoublePain() : AWeaponBase()
{
    this->m_leftPistol = this->CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("pistol_l"));
    this->m_leftPistol->SetupAttachment(this->RootComponent);

    this->m_rightPistol = this->CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("pistol_r"));
    this->m_rightPistol->SetupAttachment(this->RootComponent);

    m_fireRightArm = true;
}

void ADoublePain::InitializeWeapon(TObjectPtr<USkeletalMeshComponent> const playerMesh, FString rightHandIk, FString leftHandIk)
{
    Super::InitializeWeapon(playerMesh, rightHandIk, leftHandIk);

    this->m_rightPistol->AttachToComponent(playerMesh, FAttachmentTransformRules::SnapToTargetNotIncludingScale, FName(rightHandIk));
    this->m_leftPistol->AttachToComponent(playerMesh , FAttachmentTransformRules::SnapToTargetNotIncludingScale, FName(leftHandIk));


    #if WITH_EDITOR

    this->m_rightPistol->SetRelativeRotation(this->m_rotationR);
    this->m_rightPistol->SetRelativeLocation(this->m_relativePositionR);

    this->m_leftPistol->SetRelativeRotation(this->m_rotationL);
    this->m_leftPistol->SetRelativeLocation(this->m_relativePositionL);

    #endif
}

void ADoublePain::StartAttackAnim(TObjectPtr<UAnimInstance> const playerAnim)
{
    //We set all the attack params.
    auto attackInfo       = FAttackInfo();
    attackInfo.AttackAnim = this->m_fireRightArm ? this->m_attackMontage : this->m_attack2Montage; //We use the montage with the right arm.
    attackInfo.Damage     = 0;

    //We change the weapon state and start the attack.
    this->m_weaponState = EWeaponState::ATTACK;
    this->m_attacksComponent->BasicProjectileAttack(attackInfo);

    this->m_fireRightArm = !this->m_fireRightArm;
}

void ADoublePain::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}