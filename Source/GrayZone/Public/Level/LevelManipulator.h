// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Map/DungeonGeneratorComponent.h"
#include "LevelManipulator.generated.h"

UCLASS()
class GRAYZONE_API ALevelManipulator : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ALevelManipulator();

    // Called every frame
    virtual void Tick(float DeltaTime) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:

    UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
    TObjectPtr<UDungeonGeneratorComponent> m_dungeonGenerator;

    UFUNCTION(CallInEditor)
    void GenerateLevel();

    void CleanLevel();


};
