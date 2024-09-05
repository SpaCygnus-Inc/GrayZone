// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AttacksComponent.generated.h"

USTRUCT(BlueprintType)
struct FAttackInfo
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere)
    int Damage;

    UPROPERTY(EditAnywhere)
    TObjectPtr<UAnimMontage> AttackAnim;
};

DECLARE_MULTICAST_DELEGATE(FOnAttackEndDelegate);

/**
* A kind of dictionary of abstract attacks that will be used by most characters (Player/enemies/bosses) in the game. 
*/
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class GRAYZONE_API UAttacksComponent : public UActorComponent
{
	GENERATED_BODY()

public:	

	UAttacksComponent();

    FOnAttackEndDelegate OnAttackEnd;

    void Initialize(TObjectPtr<UAnimInstance> const animInstance);

    /**
    * Fire a basic projectile.
    */
    void BasicProjectileAttack(FAttackInfo attackInfo);

protected:

	virtual void BeginPlay() override;
 
	//virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
		
    FName static const NOTIFY_ATTACK_END;

    /**
    * Used to connect each montage noitification with its right delegate.
    */
    UFUNCTION() void OnMontageNotifyBegin(FName notifyName, const FBranchingPointNotifyPayload& branchingPointPayload);

    /**
    * This will check whether this current attack can be played or not (I.E checking if anim instance is valid).
    */
    bool CanUseAttack();

    TObjectPtr<UAnimInstance> m_animInstance; //The anim instance that will be used by all attacks.
};
