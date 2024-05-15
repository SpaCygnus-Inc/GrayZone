// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Map/Wall.h"
#include "Map/RoomData.h"
#include "Room.generated.h"

/* Each wall type has a normal wall that can be placed anywhere and an edge wall that will always be placed on the edges of a room. */
USTRUCT(BlueprintType)
struct FWallType
{ 
    GENERATED_BODY()

    UPROPERTY(EditDefaultsOnly)
    TSubclassOf<AWall> NormalWall;

    UPROPERTY(EditDefaultsOnly)
    TSubclassOf<AWall> EdgeWall;

    FWallType() { }

    bool operator==(const FWallType& other) 
    {
        return EdgeWall == other.EdgeWall && NormalWall == other.NormalWall;
    }

    bool operator !=(const FWallType& other)
    {
        return EdgeWall != other.EdgeWall || NormalWall != other.NormalWall;
    }

    friend inline uint32 GetTypeHash(const FWallType& wallType) { return HashCombine(GetTypeHash(wallType.NormalWall), GetTypeHash(wallType.EdgeWall)); }
};

UCLASS()
class GRAYZONE_API ARoom : public AStaticMeshActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ARoom();

    void FinishPlacing(TSharedRef<RoomData> roomData, FIntPoint wallsByPackOf, TMap<FWallType,float> wallsThatCanSpawn);
    void Clean();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:

    TOptional<TSharedRef<RoomData>> m_roomData;     //Data about all the information of this room.
    TArray<TObjectPtr<AWall>>       m_spawnedWalls; //All the walls that were spawned for this room.

    void  GenerateWalls(FIntPoint wallsByPackOf, TMap<FWallType, float> wallsThatCanSpawn);

    //We spawn and return the right wall with the correct rotation for the specified wall tile.
    TObjectPtr<AWall> SpawnAndGetWall(FWallType wallType, TSharedPtr<Tile> wallTile, FIntPoint minRoomPos, FIntPoint maxRoomPos);

    bool RoomDataISSet();

};
