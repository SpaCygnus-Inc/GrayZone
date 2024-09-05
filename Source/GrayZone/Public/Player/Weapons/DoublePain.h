// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Player/Weapons/WeaponBase.h"
#include "Components/MeshComponent.h"
#include "DoublePain.generated.h"

/* This is the double pistol weapon which will be inspired from max payne. */
UCLASS()
class GRAYZONE_API ADoublePain : public AWeaponBase
{
	GENERATED_BODY()

public:

    ADoublePain();

    /** 
    * We override this so that we can toggle between the left and right arm. 
    */
    void StartAttackAnim(TObjectPtr<UAnimInstance> const playerAnim) override;

    /**
    * We override this so that we can attach each pistol on the right hand.
    */
    void InitializeWeapon(TObjectPtr<USkeletalMeshComponent> const playerMesh, FString const rightHandIk, FString const leftHandIk) override;

protected:

    virtual void Tick(float DeltaTime) override;

private:

    UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
    TObjectPtr<USkeletalMeshComponent> m_rightPistol;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
    TObjectPtr<USkeletalMeshComponent> m_leftPistol;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
    TObjectPtr<UAnimMontage> m_attack2Montage; //The first attack will be with the right arm and this one with the left one.

    bool m_fireRightArm; //This will help us toggle between the left and right arm when firing.


    //These are used just for now as we're using place holder assets, later these will get removed.
    #if WITH_EDITORONLY_DATA
    UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
    FRotator m_rotationR; //The relative rotation of the weapon on the right hand.

    UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
    FRotator m_rotationL; //The relative rotation of the weapon on the left hand.

    UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
    FVector m_relativePositionR; //The relative position of the weapon on the right hand.

    UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
    FVector m_relativePositionL; //The relative position of the weapon on the left hand.
    #endif
	
};
