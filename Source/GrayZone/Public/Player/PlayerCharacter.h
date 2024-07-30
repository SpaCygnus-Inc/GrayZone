// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/ArrowComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "PlayerCharacter.generated.h"

UCLASS()
class GRAYZONE_API APlayerCharacter : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	APlayerCharacter();

    // Called every frame
    virtual void Tick(float DeltaTime) override;

    // Called to bind functionality to input
    virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
    
    UFUNCTION(BlueprintCallable)
    inline FVector GetCurrentVelocity() { return  this->GetMovingDirection() * this->m_moveSpeed; } ;

    inline FVector GetMovingDirection() { return  FVector(this->m_rightVelocity.X + this->m_forwardVelocity.X, this->m_rightVelocity.Y + this->m_forwardVelocity.Y, 0).GetSafeNormal(); }
    inline bool IsMoving()              { return !this->GetMovingDirection().IsZero(); }

    void Initialize(FVector forwardVec, FVector rightVec);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
    
    UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
    TObjectPtr<UCapsuleComponent> m_collider;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
    TObjectPtr<USkeletalMeshComponent> m_mesh;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
    TObjectPtr<UArrowComponent> m_arrow;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
    float m_moveSpeed;

    FVector m_rightVelocity;
    FVector m_forwardVelocity;

    FVector3d m_forward;         //The forward vector used for movement.
    FVector3d m_right;           //The right vector used for movement.

    void MoveForward(float value);
    void MoveRight(float value);

};
