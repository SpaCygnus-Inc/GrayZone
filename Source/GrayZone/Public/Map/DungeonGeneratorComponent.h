// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Grid.h"
#include "RoomData.h"
#include "Components/ActorComponent.h"
#include "DungeonGeneratorComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class GRAYZONE_API UDungeonGeneratorComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UDungeonGeneratorComponent();
    ~UDungeonGeneratorComponent();

    TOptional<TSharedRef<RoomData>>    GetRoom(int roomID) const;
    TArray<TSharedRef<RoomData const>> GetAllRooms()       const;
    TSharedRef<const Grid>             GetGrid()           const { return this->m_mainGrid.ToSharedRef(); }

    void GenerateDungeon();
    void Clean(); //This will take care of cleaning the whole dungeon (either for creating another one or just when destroying this class).

protected:
    // Called when the game starts or when spawned
    virtual void BeginPlay() override;

    void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;

private:

    UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "Grid", meta = (AllowPrivateAccess = true))
    bool m_drawGrid;

    UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "Grid", meta = (AllowPrivateAccess = true))
    bool m_drawDungeonTiles;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Grid", meta = (AllowPrivateAccess = true))
    FIntPoint m_numberOfRooms; //The number of rooms will be between these two numbers (x = min & y = max). 

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Grid", meta = (AllowPrivateAccess = true))
    TMap<FIntPoint, float> m_roomsSizesPossibleWithChanceToAppear; //Specify all the diferent rooms sizes that will be able to spawnalong with the chance that they wil lactually appear.

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Rooms", meta = (AllowPrivateAccess = true))
    FIntPoint m_spawnRoomSize;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Rooms", meta = (AllowPrivateAccess = true))
    FIntPoint m_exitRoomSize;

    UPROPERTY(EditAnywhere, BlueprintReadonly, Category = "Rooms", meta = (AllowPrivateAccess = true))
    FIntPoint m_distanceBetweenRooms; //The distance between rooms will always be between these two specified values (x = min & y = max).

    TMap<int, TSharedPtr<RoomData>> m_rooms;
    TSharedPtr<Grid>                m_mainGrid;
    TSharedPtr<RoomData>            m_spawnRoom;               //This room that the player will start at.
    TSharedPtr<RoomData>            m_exitRoom;                //This is the room that will enable the player to exit this dungeon.
    int                             m_BiggestRoomSizePossible; //We store here the highest size possible for a room (we take the highest depth or the highest width depending on which has a higher value).

    TSet<int> m_roomsIdsOnMainPath;                                       //These are the IDs of all the rooms that are considered as "on the main path" from the starting point to the exit.

    bool  GeneratorConfigIsValid();                                       //Check whether the generator params are valid or not.
    void  AssignTiles();
    void  DebugDraw();                                                    //Take care of debug drawing all the m_tiles and m_rooms.

    //Generate the number of specified rooms, each time choosing the size randomly from m_roomsSizesPossibleWithChanceToAppear.
    //Returns the limits of the dungeon that's actually used (minX, minY, maxX, maxY).
    FIntVector4 GenerateRooms(FIntPoint downLeftPosition, int columnsCount, int rowsCount, int totalRoomsCount);
    void  GenerateExitAndSpawnRooms(FIntPoint minPos, FIntPoint maxPos); //Spawn both the exit and spawn rooms on the edges of the used dungeon.  
    void  GenerateValidPathsToAllRooms();                                //This will make sure that there's an accessible path for all rooms in the dungeon.

    FIntPoint GetRoomSize(float value);                                   //Using the specified value (which should always between 0-100) we get a room size.
};
