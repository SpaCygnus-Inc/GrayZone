// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CharactersComponents/AttacksComponent.h"
#include "WeaponBase.generated.h"

UENUM()
enum EWeaponState
{
    IDLE,
    ATTACK,
    SPECIAL_ATTACK,
    DODGE
};
 
/* The base weapon class that will be inherited by all the player weapons. */
UCLASS(Abstract)
class GRAYZONE_API AWeaponBase : public AActor
{
	GENERATED_BODY()
	
public:	

	AWeaponBase();

    virtual void Tick(float DeltaTime) override;

    virtual void StartAttackAnim(TObjectPtr<UAnimInstance> const playerAnim);                             //Will take care of starting the attack animation.
    UFUNCTION(BlueprintCallable) virtual void Attack()        PURE_VIRTUAL(AWeaponBase::Attack, );        //This where the actual logic of the attack will happen.

    virtual void StartSpecialAttackAnim() { this->m_weaponState = EWeaponState::SPECIAL_ATTACK; }         //Will take care of starting the special attack animation.
    UFUNCTION(BlueprintCallable) virtual void SpecialAttack() PURE_VIRTUAL(AWeaponBase::SpecialAttack, ); //This where the actual logic of the attack will happen.

    virtual void StartDodge() { this->m_weaponState = EWeaponState::DODGE; }                              //Will take care of starting the dodge animation.
    UFUNCTION(BlueprintCallable) virtual void Dodge()         PURE_VIRTUAL(AWeaponBase::Dodge, );         //This where the actual logic of the attack will happen.

    virtual void ResetWeapon() { this->m_weaponState = EWeaponState::IDLE; }
    
    //Make sure to attach the weapon to the player and initialize everything else.
    virtual void InitializeWeapon(TObjectPtr<USkeletalMeshComponent> const playerMesh, FString const rightHandIk, FString const leftHandIk);

    /**
    * Take care of either hiding or showing the weapon (also enable/disable collision and ticking).
    */
    void WeaponVisibility(bool visible);
    EWeaponState inline GetState() const { return this->m_weaponState; }

protected:

	virtual void BeginPlay() override;

    UPROPERTY()
    TObjectPtr<UAttacksComponent> m_attacksComponent;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
    TObjectPtr<UAnimMontage> m_attackMontage; 

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
    TObjectPtr<UAnimMontage> m_specialAttackMontage;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
    TEnumAsByte<EWeaponState> m_weaponState;
    
};
