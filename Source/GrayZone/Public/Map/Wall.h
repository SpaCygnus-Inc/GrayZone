// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/StaticMeshActor.h"
#include "Wall.generated.h"


UENUM(BlueprintType)
enum WallDestructibility
{
    INDESTRUCTIBLE  = 0,
    HARD_TO_DESTROY = 1,
    EASY_TO_DESTROY = 2
};

/**
 * A wall that can be destroyed.
 */
UCLASS()
class GRAYZONE_API AWall : public AStaticMeshActor
{
	GENERATED_BODY()

public:

    AWall();

private:

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
    TEnumAsByte<WallDestructibility> m_canBeDestroyed;
	
};
