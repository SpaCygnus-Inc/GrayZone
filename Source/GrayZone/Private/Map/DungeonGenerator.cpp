// Fill out your copyright notice in the Description page of Project Settings.


#include "Map/DungeonGenerator.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Templates/SharedPointer.h"
#include "Global/GlobalStatics.h"

// Sets default values for this component's properties
UDungeonGenerator::UDungeonGenerator()
{
    //We don't need this to tick every frame.
	PrimaryComponentTick.bCanEverTick = false;

    //Intitialize debugging toggle to false.
    this->m_drawGrid = false;
    this->m_drawDungeonTiles = false;
    //Initialize our different shared pointers.
    m_rooms = TMap<int, TSharedPtr<Room>>();
    m_mainGrid = nullptr;
    m_spawnRoom = nullptr;
    m_exitRoom = nullptr;
    //Initialize the rest.
    m_gridBufferZoneSize = FIntPoint(0, 0);
    m_dungeonSeed = 0;
    //Initialize the properties that can be modified in the editor to some random values (well not really random but yeh...).
    m_gridSizeForRooms = FIntPoint(80, 80);
    m_maxRoomSize = FIntPoint(30, 30);
    m_minRoomSize = FIntPoint(11, 11);
    m_spawnRoomSize = FIntPoint(10, 10);
    m_exitRoomSize = FIntPoint(15, 10);
    m_distanceBetweenRooms = 4;
}


// Called when the game starts
void UDungeonGenerator::BeginPlay()
{
    Super::BeginPlay();

    if (!this->GeneratorConfigIsValid()) return;

    //We get the grid buffer zone using the biggest m_size between the spawn and exit room and add to it the distance between room so that we have enough space to create those m_rooms.
    this->m_gridBufferZoneSize = FIntPoint(FMath::Max(this->m_spawnRoomSize.X, this->m_exitRoomSize.X) + this->m_distanceBetweenRooms, FMath::Max(this->m_spawnRoomSize.Y, this->m_exitRoomSize.Y) + this->m_distanceBetweenRooms);
    auto depth = this->m_gridBufferZoneSize.X * 2 + this->m_gridSizeForRooms.X;
    auto width = this->m_gridBufferZoneSize.Y * 2 + this->m_gridSizeForRooms.Y;
    this->m_mainGrid = MakeShared<Grid>(width, depth, UGrayZoneGameInstance::TILE_SIZE);
}

void UDungeonGenerator::GenerateDungeon()
{
    //We use the current ticks as a seed for creating a map. 
    this->m_dungeonSeed = FDateTime::Now().GetTicks();
    FMath::RandInit(this->m_dungeonSeed);

    this->GenerateRooms(FIntPoint(this->m_gridBufferZoneSize.X, this->m_gridBufferZoneSize.Y), this->m_gridSizeForRooms.X, this->m_gridSizeForRooms.Y); //First we create rooms that have soem distance between them...
    this->GenerateExitAndSpawnRooms(); //Then on the edges of the map we create both the spawn and exit room...
    this->SetRoomsFinalPosition();     //We also want to move randomly each room a bit on the sides just to add some more variety to the map and assign each room its tiles...

    
    this->m_mainGrid->GenerateCorridorsTiles(this->GetWorld()); //Now that we have the room tiles, we take care of assigning the corridors tiles...
    this->GenerateValidPathsToAllRooms();                       //Finally we make sure that all rooms have a valid path and create doors and walls tiles for rooms.

    this->DebugDraw(); //We refresh the debug mode in case it's enabled.
}

void UDungeonGenerator::GenerateRooms(FIntPoint topLeftPosition, int width, int depth)
{
    //We make sure to cut the m_width and m_depth while also making sure that the room respect the minimum m_size allowed.
    //We multiply by 2 because we at least want to get a size that can allow two rooms that are respecting the minimum size.
    auto newWidth = FMath::RandRange(this->m_minRoomSize.Y * 2, width) - this->m_minRoomSize.Y;
    auto newDepth = FMath::RandRange(this->m_minRoomSize.X * 2, depth) - this->m_minRoomSize.X;

    //The real m_width and m_depth of the room.
    auto widthWithoutBufferSpace = width - this->m_distanceBetweenRooms;
    auto depthWithoutBufferSpace = depth - this->m_distanceBetweenRooms;

    auto respectedRatio = (float)width / (float)depth <= UDungeonGenerator::ROOM_MAX_SIZE_RATIO && (float)depth / (float)width <= UDungeonGenerator::ROOM_MAX_SIZE_RATIO;

    auto createOtherRooms = widthWithoutBufferSpace > this->m_maxRoomSize.Y || depthWithoutBufferSpace > this->m_maxRoomSize.X //If the m_width or the m_depth is still above the max then we should create a new room.                                                                                                       
        || ((widthWithoutBufferSpace >= this->m_minRoomSize.Y * 2 || depthWithoutBufferSpace >= this->m_minRoomSize.X * 2)     //If the m_width and the m_depth are both already at the minimum then this room shouldn't be cut.
            && (!respectedRatio || FMath::RandBool()));                                                                                             //Finally we just add a random factor.

    if (createOtherRooms)
    {                                                                         //We check whether the ratio rate is currently respected or not.
        auto verticalCut = depthWithoutBufferSpace < this->m_minRoomSize.X * 2 || (!respectedRatio && width > depth);                                           //If we already have the minimum m_depth then we should cut the room vertically (the m_width will get smaller).
        if (!verticalCut) verticalCut = (widthWithoutBufferSpace < this->m_minRoomSize.Y * 2 || (!respectedRatio && width < depth)) ? false : FMath::RandBool(); //If we already have the minimum m_width we should cut the room horizontally (the m_depth get smaller).

        //We always create two m_rooms one from the starting m_position m_position and the other from the m_position + remaining m_width/m_depth.
        if (verticalCut)
        {
            this->GenerateRooms(topLeftPosition, newWidth, depth);
            this->GenerateRooms(FIntPoint(topLeftPosition.X, topLeftPosition.Y + newWidth), width - newWidth, depth);
            return;
        }
        else
        {
            this->GenerateRooms(topLeftPosition, width, newDepth);
            this->GenerateRooms(FIntPoint(topLeftPosition.X + newDepth, topLeftPosition.Y), width, depth - newDepth);
            return;
        }
    }

    //Finally we create the room and add it to the map.
    auto room = MakeShared<Room>(topLeftPosition, widthWithoutBufferSpace, depthWithoutBufferSpace, this->GetWorld());
    this->m_rooms.Add(room->GetID(), room);
}

void UDungeonGenerator::SetRoomsFinalPosition()
{
    auto halfDistanceBtwRooms = this->m_distanceBetweenRooms * 0.5;

    for (auto entry : this->m_rooms)
    {
        //We don't move the spawn and exit rooms.
        if (entry.Value != this->m_spawnRoom && entry.Value != this->m_exitRoom)
        {
            //We push randomly a littly bit the rooms to make the dungeon a bit less monotone while making sure that their new positions isn't out of of the grid.
            auto newPosX = FMath::RandRange(FMath::Max(0, entry.Value->GetPosition().X - halfDistanceBtwRooms), FMath::Min(entry.Value->GetPosition().X + halfDistanceBtwRooms, this->m_mainGrid->GetDepth() - entry.Value->GetDepth() - 1));
            auto newPosY = FMath::RandRange(FMath::Max(0, entry.Value->GetPosition().Y - halfDistanceBtwRooms), FMath::Min(entry.Value->GetPosition().Y + halfDistanceBtwRooms, this->m_mainGrid->GetWidth() - entry.Value->GetWidth() - 1));
            entry.Value->SetPosition(FIntPoint(newPosX, newPosY));
        }

        this->m_mainGrid->SetRoomTiles(entry.Value.ToSharedRef().Get()); //This is the final m_position of the room and so we want to finally assign its m_tiles.
    }
}

void UDungeonGenerator::GenerateExitAndSpawnRooms()
{
    auto halfDistanceBtwRooms = FMath::RoundToInt32(this->m_distanceBetweenRooms * 0.5f);

    //The exit room is actually generated on the upper half of the map or on the left/right starting from half the m_depth.
    auto exitRoomStartingPos = FMath::RandBool() ? FIntPoint(this->m_mainGrid->GetDepth() - this->m_gridBufferZoneSize.X - halfDistanceBtwRooms, FMath::RandRange(this->m_gridBufferZoneSize.Y, this->m_mainGrid->GetWidth() - (this->m_gridBufferZoneSize.Y + this->m_exitRoomSize.Y + halfDistanceBtwRooms)))
        : FIntPoint(FMath::RandRange(this->m_mainGrid->GetHalfGridPos().X, this->m_mainGrid->GetDepth() - this->m_gridBufferZoneSize.X - this->m_exitRoomSize.Y - halfDistanceBtwRooms), FMath::RandBool() ? this->m_gridBufferZoneSize.Y - this->m_exitRoomSize.Y - halfDistanceBtwRooms : this->m_mainGrid->GetWidth() - m_gridBufferZoneSize.Y - halfDistanceBtwRooms);
    //The spawn room is generated on the bottom half of the map.
    auto spawnRoomStartingPos = FIntPoint(this->m_gridBufferZoneSize.X - this->m_spawnRoomSize.X - halfDistanceBtwRooms, FMath::RandRange(m_gridBufferZoneSize.Y, this->m_mainGrid->GetWidth() - (this->m_gridBufferZoneSize.Y + this->m_spawnRoomSize.Y + halfDistanceBtwRooms)));

    m_exitRoom = MakeShared<Room>(exitRoomStartingPos, this->m_exitRoomSize.Y, this->m_exitRoomSize.X, this->GetWorld());
    m_spawnRoom = MakeShared<Room>(spawnRoomStartingPos, this->m_spawnRoomSize.Y, this->m_spawnRoomSize.X, this->GetWorld());

    this->m_rooms.Add(this->m_exitRoom->GetID(), m_exitRoom);
    this->m_rooms.Add(this->m_spawnRoom->GetID(), m_spawnRoom);
}

void UDungeonGenerator::GenerateValidPathsToAllRooms()
{
    auto roomsIDsWithAPath   = TSet<int>();                                                                //Rooms that already have a path directing to them from the spawn room.
    auto spawnRoomCenterPos  = this->m_spawnRoom.Get()->GetCenterPosition();                               //The position of the spawn room center.
    auto spawnRoomCenterTile = this->m_mainGrid.Get()->GetRoomCenterTile(this->m_spawnRoom.ToSharedRef()); //Approximately the center tile of the room.

    //We sort the rooms from the farthest to the closest to the spawn room.
    this->m_rooms.ValueSort([spawnRoomCenterPos] (TSharedPtr<Room> first, TSharedPtr<Room> second) {
        return FVector2f::Distance(first.Get()->GetCenterPosition(), spawnRoomCenterPos) > FVector2f::Distance(second.Get()->GetCenterPosition(), spawnRoomCenterPos);
    });

    for (auto room : this->m_rooms)
    {
        if (roomsIDsWithAPath.Contains(room.Value.Get()->GetID()) || this->m_spawnRoom == room.Value) continue;       //If the room already has a path or is the spawn room itself, we ignore it.

        auto roomCenterTile        = this->m_mainGrid.Get()->GetRoomCenterTile(room.Value.ToSharedRef());             //We get the center tile of this room...
        auto pathToRoom            = UGlobalStatics::GetBestPath(*this, spawnRoomCenterTile, roomCenterTile);         //Then we create a path toward it from the spawn room...
        TSet<int> affectedRoomsIds = this->m_mainGrid.Get()->GenerateDoorsFromPathAndReturnAffectedRooms(pathToRoom); //Using that path, we generate doors for all the rooms that the path is passing through...

        //Finally we make sure to create walls around those rooms so that the next path will always try to use the already used tile doors.
        for (auto roomID : affectedRoomsIds)
        {      
            if (roomsIDsWithAPath.Contains(roomID)) continue;

            this->m_mainGrid.Get()->GenerateWallsForSpecifiedRoom(this->m_rooms[roomID].ToSharedRef());
            roomsIDsWithAPath.Add(roomID);
        }
    }
}

TSharedPtr<const Room> UDungeonGenerator::GetRoom(int roomID) const
{
    if (!this->m_rooms.Contains(roomID))
    {
        UE_LOG(LogTemp, Error, TEXT("There's no room with that ID."));
        return nullptr;
    }

    return this->m_rooms[roomID];
}

void UDungeonGenerator::CleanDungeon()
{
    this->m_rooms.Empty();
    this->m_spawnRoom.Reset();
    this->m_exitRoom.Reset();
    if (this->m_mainGrid != nullptr) this->m_mainGrid->ClearAllTiles();
}

bool UDungeonGenerator::GeneratorConfigIsValid()
{
    if (UGrayZoneGameInstance::TILE_SIZE == 0)
    {
        UE_LOG(LogTemp, Fatal, TEXT("Tile size can't be 0."));
        return false;
    }

    if (this->m_maxRoomSize.X > this->m_gridSizeForRooms.X || this->m_maxRoomSize.Y > this->m_gridSizeForRooms.Y)
    {
        UE_LOG(LogTemp, Fatal, TEXT("Maximum room size is bigger than the size of the grid."));
        return false;
    }

    if (this->m_minRoomSize.X > (this->m_maxRoomSize.X * 0.5 - this->m_distanceBetweenRooms) || this->m_minRoomSize.Y > (this->m_maxRoomSize.Y * 0.5 - this->m_distanceBetweenRooms))
    {
        UE_LOG(LogTemp, Fatal, TEXT("Minimum room size should always be at most half the maximum size minus the distance between rooms."));
        return false;
    }

    return true;
}

UDungeonGenerator::~UDungeonGenerator() { this->CleanDungeon(); }

void UDungeonGenerator::DebugDraw()
{
    FlushPersistentDebugLines(this->GetWorld()); //First we make sure to clean all debug lines.

    if (this->m_mainGrid == nullptr)
    {
        UE_LOG(LogTemp, Warning, TEXT("Main grid isn't yet built to call debug draw."));
        return;
    }

    //If the draw grid is enabled then we draw all the m_tiles.
    if (this->m_drawGrid) this->m_mainGrid->DebugDraw(this->GetWorld());

    //If the draw rooms and corridors is enabled then we draw all rooms and only the corridors m_tiles.
    if (this->m_drawDungeonTiles)
    {
        for (auto entry : this->m_rooms) entry.Value->DebugDraw(this->GetWorld());
        this->m_mainGrid->DebugDraw(this->GetWorld(), true);
    }
}

#if WITH_EDITOR

void UDungeonGenerator::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent)
{
    auto propertyName = PropertyChangedEvent.Property != NULL ? PropertyChangedEvent.Property->GetFName() : NAME_None;

    if (propertyName == GET_MEMBER_NAME_CHECKED(UDungeonGenerator, m_drawGrid) || propertyName == GET_MEMBER_NAME_CHECKED(UDungeonGenerator, m_drawDungeonTiles))
        this->DebugDraw();
}

#endif

