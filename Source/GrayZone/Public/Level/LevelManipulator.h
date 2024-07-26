// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/StaticMeshActor.h"
#include "Map/DungeonGeneratorComponent.h"
#include "Map/DungeonDecoratorComponent.h"
#include "Player/PlayerCharacter.h"
#include "Player/TargetCamera.h"
#include "LevelManipulator.generated.h"

UCLASS()
class GRAYZONE_API ALevelManipulator : public AStaticMeshActor
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

    UPROPERTY(VisibleInstanceOnly, BlueprintReadonly, Category = "Dungeon", meta = (AllowPrivateAccess = true))
    int64 m_dungeonSeed;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dungeon", meta = (AllowPrivateAccess = true))
    TObjectPtr<UDungeonGeneratorComponent> m_dungeonGenerator;
    
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dungeon", meta = (AllowPrivateAccess = true))
    TObjectPtr<UDungeonDecoratorComponent> m_dungeonDecorator;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Player", meta = (AllowPrivateAccess = true))
    TSubclassOf<APlayerCharacter> m_playerCharacter;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Player", meta = (AllowPrivateAccess = true))
    TSubclassOf<ATargetCamera> m_cameraType;

    UFUNCTION(CallInEditor)
    void GenerateLevel();

    TObjectPtr<APlayerCharacter> m_spawnedPlayer;
    TObjectPtr<ATargetCamera>    m_camera; //The camera that will be following the player.

    void SpawnOrEnablePlayer(FVector spawnPos);
    void CleanLevel();
};
