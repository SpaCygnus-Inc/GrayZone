// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/ArrowComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Animation/AnimMontage.h"
#include "Player/Weapons/WeaponBase.h"
#include "PlayerCharacter.generated.h"

UENUM(BlueprintType)
enum EWeaponType
{
    NONE,
    DOUBLE_PAIN
};

UCLASS()
class GRAYZONE_API APlayerCharacter : public APawn
{
	GENERATED_BODY()

public:

	APlayerCharacter();

    virtual void Tick(float DeltaTime) override;

    virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
    
    UFUNCTION(BlueprintCallable) inline FVector GetCurrentVelocity() { return  this->GetMovingDirection() * this->m_moveSpeed; } ;

    UFUNCTION(BlueprintCallable) inline bool        IsAttacking()           const { return this->m_equippedWeapon != nullptr && this->m_equippedWeapon->GetState() != EWeaponState::IDLE && this->m_equippedWeapon->GetState() != EWeaponState::DODGE; }
    UFUNCTION(BlueprintCallable) inline EWeaponType GetEquippedWeaponType() const { return this->m_equippedWeaponType; }

    /* 
    * Get in which direction the player is currently moving. 
    */
    inline FVector GetMovingDirection() const { return  FVector(this->m_rightVelocity.X + this->m_forwardVelocity.X, this->m_rightVelocity.Y + this->m_forwardVelocity.Y, 0).GetSafeNormal(); }
    inline bool IsMoving()              const { return  !this->IsAttacking() && !this->GetMovingDirection().IsZero(); }

    /** 
    * We initialize the player with the forward and right vectors that will be mainly used for movement (Usually these are depened on the camera roation). 
    */
    void Initialize(FVector forwardVec, FVector rightVec);
    void EquipWeapon(EWeaponType weaponType);

protected:

	virtual void BeginPlay() override;

private:

    FString static const RIGHT_HAND_IK;
    FString static const LEFT_HAND_IK;
    
    UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
    TObjectPtr<UCapsuleComponent> m_collider;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
    TObjectPtr<USkeletalMeshComponent> m_mesh;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
    TObjectPtr<UArrowComponent> m_arrow;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
    float m_moveSpeed;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon", meta = (AllowPrivateAccess = true))
    TEnumAsByte<EWeaponType> m_equippedWeaponType;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon", meta = (AllowPrivateAccess = true))
    TArray<TSubclassOf<AWeaponBase>> m_allWeaponTypes;

    FVector m_rightVelocity;   //The current velocity of the player along the x axis.
    FVector m_forwardVelocity; //The current velocity of the player along the y axis.

    FVector3d m_rightVector;   //The right vector used for movement.
    FVector3d m_forwardVector; //The forward vector used for movement.

    TObjectPtr<AWeaponBase> m_equippedWeapon; //The weapon currently equipped and used by the player.

    void MoveForward(float value);
    void MoveRight(float value);

    /**
    * Rotate the player toward the mouse cursor.   
    */
    void RotateTowardCursor();

    void Attack();
    //void SpecialAttack();
    //void Dodge();

};
