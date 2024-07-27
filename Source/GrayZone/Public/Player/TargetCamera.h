// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Camera/CameraComponent.h"
#include "Player/PlayerCharacter.h"
#include "TargetCamera.generated.h"

UCLASS()
class GRAYZONE_API ATargetCamera : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATargetCamera();

    void SetTarget(TObjectPtr<APlayerCharacter> target);

protected:

    virtual void BeginPlay() override;

private:

    FRotator static const CAMERA_ROTATION; //The rotation that the camera will always have.

    virtual void Tick(float DeltaTime) override;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = true))
    FVector m_cameraOffset;     //How farther/closer to the target position do we want the camera to be in.

    UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
    float m_interpolationSpeed; //How fast do we want the camera to follow the player ?

    UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
    float m_overshootFactor; //How far we want the camera to overshoot before it correct itself which will give a more organic feel to the camera (this is a multiplier that's applied to the next player position).

    UPROPERTY(VisibleInstanceOnly)
    TObjectPtr<APlayerCharacter> m_target; //The player that this camera will follow.

    UPROPERTY(VisibleAnywhere)
    TObjectPtr<UCameraComponent> m_cameraComponent;

    bool m_cameraFixingOvershoot;

};
