// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Grid.h"
#include "Room.h"
#include "Components/ActorComponent.h"
#include "DungeonGenerator.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class GRAYZONE_API UDungeonGenerator : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UDungeonGenerator();
    ~UDungeonGenerator();

    TSharedPtr<const Room> GetRoom(int roomID) const;
    TSharedPtr<const Grid> GetGrid() const { return this->m_mainGrid; }

    void GenerateDungeon();
    void CleanDungeon(); //This will take care of cleaning the whole dungeon (either for creating another one or just when destroying this class).

protected:
    // Called when the game starts or when spawned
    virtual void BeginPlay() override;

    void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;

private:

    static const int ROOM_MAX_SIZE_RATIO = 2; //This is the minimum ratio that a room should have.

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Grid", meta = (AllowPrivateAccess = true))
    FIntPoint m_gridSizeForRooms;

    UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "Grid", meta = (AllowPrivateAccess = true))
    bool m_drawGrid;

    UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "Grid", meta = (AllowPrivateAccess = true))
    bool m_drawDungeonTiles;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Rooms", meta = (AllowPrivateAccess = true))
    FIntPoint m_maxRoomSize;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Rooms", meta = (AllowPrivateAccess = true))
    FIntPoint m_minRoomSize;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Rooms", meta = (AllowPrivateAccess = true))
    FIntPoint m_spawnRoomSize;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Rooms", meta = (AllowPrivateAccess = true))
    FIntPoint m_exitRoomSize;

    UPROPERTY(VisibleInstanceOnly, BlueprintReadonly, Category = "Rooms", meta = (AllowPrivateAccess = true))
    int64 m_dungeonSeed;

    UPROPERTY(EditAnywhere, BlueprintReadonly, Category = "Rooms", meta = (AllowPrivateAccess = true))
    int32 m_distanceBetweenRooms;

    TMap<int, TSharedPtr<Room>> m_rooms;
    TSharedPtr<Grid>            m_mainGrid;
    TSharedPtr<Room>            m_spawnRoom;          //This room that the player will start at.
    TSharedPtr<Room>            m_exitRoom;           //This is the room that will enable the player to exit this dungeon.
    FIntPoint                   m_gridBufferZoneSize; //This is used to determine how much of the grid we want to leave unused when we're creating normal m_rooms so that

    TSet<int> m_roomsIdsOnMainPath;                   //These are the IDs of all the rooms that are considered as "on the main path" from the starting point to the exit.

    bool  GeneratorConfigIsValid();                                       //Check whether the generator params are valid or not.
    void  SetRoomsFinalPosition();
    void  DebugDraw();                                                    //Take care of debug drawing all the m_tiles and m_rooms.

    void  GenerateRooms(FIntPoint topLeftPosition, int width, int depth); //This will help us generate random m_rooms using each time the number of m_tiles on the x axis and those on the y axis.
    void  GenerateExitAndSpawnRooms();                                    //Spawn both the exit and spawn m_rooms on the buffer zone.   
    void  GenerateValidPathsToAllRooms();                                 //This will make sure that there's an accessible path for all rooms in the dungeon.
};
