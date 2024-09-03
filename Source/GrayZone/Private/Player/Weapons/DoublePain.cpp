// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/Weapons/DoublePain.h"

ADoublePain::ADoublePain()
{
    this->m_leftPistol = this->CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("pistol_l"));
    this->m_leftPistol->SetupAttachment(this->RootComponent);

    this->m_rightPistol = this->CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("pistol_r"));
    this->m_rightPistol->SetupAttachment(this->RootComponent);

    m_fireRightArm = true;
}

void ADoublePain::InitializeWeapon(TObjectPtr<USkeletalMeshComponent> const playerMesh, FString rightHandIk, FString leftHandIk)
{
    auto socketLocation = playerMesh->GetSocketLocation(FName(rightHandIk));
    auto socketRotation = playerMesh->GetSocketRotation(FName(rightHandIk));

    this->m_rightPistol->AttachToComponent(playerMesh, FAttachmentTransformRules::SnapToTargetNotIncludingScale, FName(rightHandIk));
    this->m_rightPistol->SetRelativeRotation(this->m_rotationR);

    this->m_leftPistol->AttachToComponent(playerMesh , FAttachmentTransformRules::SnapToTargetNotIncludingScale, FName(leftHandIk));
    this->m_leftPistol->SetRelativeRotation(this->m_rotationL);
}

void ADoublePain::StartAttackAnim(TObjectPtr<UAnimInstance> const playerAnim)
{
    this->m_weaponState = EWeaponState::ATTACK;

    if (this->m_fireRightArm) playerAnim->Montage_Play(this->m_attackMontage);
    else                      playerAnim->Montage_Play(this->m_attack2Montage);

    this->m_fireRightArm = !this->m_fireRightArm;
}

void ADoublePain::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    this->m_rightPistol->SetRelativeLocation(this->m_relativePositionR);
    this->m_leftPistol->SetRelativeLocation(this->m_relativePositionL);
}