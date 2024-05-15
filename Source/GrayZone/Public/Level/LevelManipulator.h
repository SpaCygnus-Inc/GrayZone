// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/StaticMeshActor.h"
#include "Map/DungeonGeneratorComponent.h"
#include "Map/DungeonDecoratorComponent.h"
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

    UPROPERTY(VisibleInstanceOnly, BlueprintReadonly, meta = (AllowPrivateAccess = true))
    int64 m_dungeonSeed;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
    TObjectPtr<UDungeonGeneratorComponent> m_dungeonGenerator;
    
    UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
    TObjectPtr<UDungeonDecoratorComponent> m_dungeonDecorator;

    UFUNCTION(CallInEditor)
    void GenerateLevel();

    void CleanLevel();


};
