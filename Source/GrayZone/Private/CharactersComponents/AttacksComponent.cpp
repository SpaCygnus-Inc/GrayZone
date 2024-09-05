// Fill out your copyright notice in the Description page of Project Settings.


#include "CharactersComponents/AttacksComponent.h"

FName const UAttacksComponent::NOTIFY_ATTACK_END = "AttackEnd";

// Sets default values for this component's properties
UAttacksComponent::UAttacksComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UAttacksComponent::Initialize(TObjectPtr<UAnimInstance> const animInstance)
{
    this->m_animInstance = animInstance;

    this->m_animInstance->OnPlayMontageNotifyBegin.AddDynamic(this, &UAttacksComponent::OnMontageNotifyBegin);
}

void UAttacksComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}

bool UAttacksComponent::CanUseAttack()
{
    if (this->m_animInstance == nullptr) { UE_LOG(LogTemp, Error, TEXT("No anim instance is assigned to this AttacksComponent.")); return false; }

    return true;
}

void UAttacksComponent::BasicProjectileAttack(FAttackInfo attackInfo)
{
    if (!this->CanUseAttack()) return;

    this->m_animInstance->Montage_Play(attackInfo.AttackAnim);
}

void UAttacksComponent::OnMontageNotifyBegin(FName notifyName, const FBranchingPointNotifyPayload& branchingPointPayload)
{
    //Each time a specific notification is hit, we want to call its relevant delegate.
    if (notifyName == UAttacksComponent::NOTIFY_ATTACK_END) this->OnAttackEnd.Broadcast();
}
