// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Map/Room.h"
#include "Map/Corridor.h"
#include "Map/DungeonGeneratorComponent.h"
#include "DungeonDecoratorComponent.generated.h"


USTRUCT(BlueprintType)
struct FRoomsToSpawn
{
    GENERATED_BODY()

    UPROPERTY(EditDefaultsOnly)
    TMap<TSubclassOf<ARoom>, float> RoomsWithChanceToSpawn;

    FRoomsToSpawn() { this->RoomsWithChanceToSpawn = TMap<TSubclassOf<ARoom>, float>(); }
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class GRAYZONE_API UDungeonDecoratorComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UDungeonDecoratorComponent();

    void DecorateDungeon(TObjectPtr<UDungeonGeneratorComponent> const dungeonGenerator, TObjectPtr<AActor> const parentActor);
    void Clean();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

private:	

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
    TMap<FIntPoint, FRoomsToSpawn> m_roomsToSpawn; 

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
    TArray<TSubclassOf<ACorridor>>                   m_corridorsToSpawn;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
    FRoomsToSpawn m_spawningRoomsToSpawn;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
    FRoomsToSpawn m_exitRoomsToSpawn;

    TArray<TObjectPtr<ACorridor>> m_spawnedCorridors; //All the corridors instances currently spawned in the world.
    TArray<TObjectPtr<ARoom>>     m_spawnedRooms;     //All the rooms instances currently spawned in the world.

    void PlaceRooms(TObjectPtr<UDungeonGeneratorComponent> const dungeonGenerator, TObjectPtr<AActor> const parentActor);
    void PlaceCorridors(TObjectPtr<UDungeonGeneratorComponent> const dungeonGenerator, TObjectPtr<AActor> const parentActor);
};
