// Fill out your copyright notice in the Description page of Project Settings.


#include "Map/DungeonGeneratorComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Templates/SharedPointer.h"
#include "Global/GlobalStatics.h"

// Sets default values for this component's properties
UDungeonGeneratorComponent::UDungeonGeneratorComponent()
{
    //We don't need this to tick every frame.
	PrimaryComponentTick.bCanEverTick = false;

    //Intitialize debugging toggle to false.
    this->m_drawGrid = false;
    this->m_drawDungeonTiles = false;
    //Initialize our different shared pointers.
    this->m_rooms     = TMap<int, TSharedPtr<RoomData>>();
    this->m_mainGrid  = nullptr;
    this->m_spawnRoom = nullptr;
    this->m_exitRoom  = nullptr;
    //Initialize the rest.
    this->m_roomsSizesPossibleWithChanceToAppear = TMap<FIntPoint, float>();
    this->m_roomsIdsOnMainPath                   = TSet<int>();
    //Initialize the properties that can be modified in the editor to some random values (well not really random but yeh...).
    this->m_spawnRoomSize           = FIntPoint(10, 10);
    this->m_exitRoomSize            = FIntPoint(15, 10);
    this->m_numberOfRooms           = FIntPoint(6, 10);
    this->m_distanceBetweenRooms    = FIntPoint(1, 2);
    this->m_BiggestRoomSizePossible = 0;
}


// Called when the game starts
void UDungeonGeneratorComponent::BeginPlay()
{
    Super::BeginPlay();

    if (!this->GeneratorConfigIsValid()) return;

    for (auto entry : this->m_roomsSizesPossibleWithChanceToAppear) 
    {
        auto higherSize                 = entry.Key.X >= entry.Key.Y ? entry.Key.X : entry.Key.Y;
        this->m_BiggestRoomSizePossible = this->m_BiggestRoomSizePossible >= higherSize ? this->m_BiggestRoomSizePossible : higherSize;
    }
}

void UDungeonGeneratorComponent::GenerateDungeon()
{
    //We get the number of rooms that we want to generate, and how we want to divide them between the different rows.
    auto numberOfRooms = FMath::RandRange(this->m_numberOfRooms.X, this->m_numberOfRooms.Y);
    auto columnsCount  = FMath::RoundToInt(numberOfRooms * 0.3f);
    auto rowsCount     = numberOfRooms / columnsCount;

    //We get the grid buffer zone using the biggest m_size between the spawn and exit room and add to it the distance between room so that we have enough space to create those m_rooms.
    auto gridBufferZoneSize = FIntPoint(FMath::Max(this->m_spawnRoomSize.X, this->m_exitRoomSize.X) + this->m_distanceBetweenRooms.Y * columnsCount, FMath::Max(this->m_spawnRoomSize.Y, this->m_exitRoomSize.Y) + this->m_distanceBetweenRooms.Y * rowsCount);
    
    //We create a new grid, using a calculated depth and width.
    auto depth         = gridBufferZoneSize.X * 2 + (this->m_BiggestRoomSizePossible * rowsCount);
    auto width         = gridBufferZoneSize.Y * 2 + (this->m_BiggestRoomSizePossible * columnsCount);
    this->m_mainGrid   = MakeShared<Grid>(width, depth);

    auto usedMapLimits = this->GenerateRooms(FIntPoint(gridBufferZoneSize.X, gridBufferZoneSize.Y), columnsCount, rowsCount, numberOfRooms); //We generate the different rooms...
    this->GenerateExitAndSpawnRooms(FIntPoint(usedMapLimits.X, usedMapLimits.Y), FIntPoint(usedMapLimits.Z, usedMapLimits.W)); //Then we generate the spawn and exit rooms on the edges of the used dungeon...
    this->AssignTiles();                                                             //And finally we make sure that each tile is populated with the right description.

    this->DebugDraw(); //We refresh the debug mode in case it's enabled.
}

FIntPoint UDungeonGeneratorComponent::GetRoomSize(float value)
{
    auto resultSize = UGlobalStatics::GetItemRandomlyUsingPercentage<FIntPoint>(this->m_roomsSizesPossibleWithChanceToAppear); //We get a room size randomly...
    if (FMath::RandBool()) resultSize = UGlobalStatics::SwapFIntPoints(resultSize);                                            //And then we randomly choose whether to rotate the room or not.

    return resultSize;
}

FIntVector4 UDungeonGeneratorComponent::GenerateRooms(FIntPoint downLeftPosition, int columnsCount, int rowsCount, int totalRoomsCount)
{
    FIntPoint minPos(INT_MAX, INT_MAX);
    FIntPoint maxPos(0, 0);

    auto roomsByRows = TMap<int, TArray<TSharedPtr<RoomData>>>(); //The rooms that are on each row.

    for (int i = 0; i < rowsCount; i++)
    {
        auto xPos              = downLeftPosition.X + (i * this->m_BiggestRoomSizePossible);       //The default x position for the next rooms.
        auto currentCountCount = FMath::Min(columnsCount, totalRoomsCount - i * columnsCount); //If the remaining number of rooms is less than a row max, then we just get the remaining number of rooms.
        auto roomsInCurrentRow = TArray<TSharedPtr<RoomData>>();                                   //All the rooms that are on the current row.

        for (int j = 0; j < currentCountCount; j++)
        {
            auto currentYPos     = downLeftPosition.Y + (j * this->m_BiggestRoomSizePossible);
            auto currentXPos     = xPos;
            auto currentRoomSize = this->GetRoomSize(FMath::RandRange(0, 100)); //We get randomly the room size that we want to insert at this position.

            //When this raw already has some rooms, we always want the next one to be as close as possible to the last created one.
            if (!roomsInCurrentRow.IsEmpty())
            {
                auto distanceBtwRoomsOnY = FMath::RandRange(this->m_distanceBetweenRooms.X, this->m_distanceBetweenRooms.Y);
                auto roomOnTheLeftSide   = roomsInCurrentRow.Last();
                currentYPos              = roomOnTheLeftSide->GetPosition().Y + roomOnTheLeftSide->GetWidth() + distanceBtwRoomsOnY;
            }
            
            //When we already have other rows of rooms, we make sure that this room is as close as possible to the rooms under it without intersecting with them.
            if (!roomsByRows.IsEmpty())
            {
                //We start by setting the x position a bit above the room that's on the same column as this one.
                auto distanceBtwRoomsOnX = FMath::RandRange(this->m_distanceBetweenRooms.X, this->m_distanceBetweenRooms.Y);
                currentXPos              = roomsByRows[i - 1][j]->GetPosition().X + roomsByRows[i - 1][0]->GetDepth() + distanceBtwRoomsOnX;
            
                //Each time that we find a room that's actually intersecting with this one, we make sure to move the x position up a bit.
                for (int roomIndex = 0; roomIndex < columnsCount; roomIndex++)
                {
                    auto roomUnder = roomsByRows[i - 1][roomIndex];
               
                    if (!roomUnder->IntersectWithAnotherRoom(FIntPoint(currentXPos, currentYPos), currentRoomSize)) continue;
     
                    currentXPos = roomUnder->GetPosition().X + roomUnder->GetDepth() + distanceBtwRoomsOnX;
                }
            }

            //Finally we create the room...
            auto roomPos = FIntPoint(currentXPos, currentYPos);
            TSharedPtr<RoomData> currentRoom = MakeShared<RoomData>(roomPos, currentRoomSize.Y, currentRoomSize.X, this->m_mainGrid->GetTiles(roomPos, currentRoomSize), this->GetWorld());

            //And we store it accordingly.
            roomsInCurrentRow.Add(currentRoom);
            this->m_rooms.Add(currentRoom->GetID(), currentRoom);

            //We set the min and max positions of the rooms.
            minPos.X = minPos.X <= currentXPos ? minPos.X : currentXPos;
            minPos.Y = minPos.Y <= currentYPos ? minPos.Y : currentYPos;
            maxPos.X = maxPos.X >= currentXPos + currentRoomSize.X ? maxPos.X : currentXPos + currentRoomSize.X;
            maxPos.Y = maxPos.Y >= currentYPos + currentRoomSize.Y ? maxPos.Y : currentYPos + currentRoomSize.Y;
        }

        roomsByRows.Add(i, roomsInCurrentRow);
    }

    return FIntVector4(minPos.X, minPos.Y, maxPos.X, maxPos.Y);
}

void UDungeonGeneratorComponent::AssignTiles()
{
    for (auto entry : this->m_rooms) this->m_mainGrid->SetRoomTiles(entry.Value.ToSharedRef());

    this->m_mainGrid->GenerateCorridorsTiles(this->GetWorld()); //Now that we have the room tiles, we take care of assigning the corridors tiles...
    this->GenerateValidPathsToAllRooms();                       //Finally we make sure that all rooms have a valid path and create door and wall tiles for rooms.
}

void UDungeonGeneratorComponent::GenerateExitAndSpawnRooms(FIntPoint minPos, FIntPoint maxPos)
{
    auto distanceBtwRooms = FMath::RandRange(this->m_distanceBetweenRooms.X, this->m_distanceBetweenRooms.Y);

    //The exit room is actually generated on the upper half of the map or on the left/right starting from half the m_depth.
    auto exitRoomStartingPos = FMath::RandBool() ? FIntPoint(maxPos.X + distanceBtwRooms, FMath::RandRange(minPos.Y, maxPos.Y - m_exitRoomSize.Y))
         : FIntPoint(FMath::RandRange(maxPos.X - minPos.X / 2, maxPos.X - m_exitRoomSize.X), FMath::RandBool() ? minPos.Y - m_exitRoomSize.Y - distanceBtwRooms : maxPos.Y + distanceBtwRooms);

    //The spawn room is generated on the bottom half of the map.
    auto spawnRoomStartingPos = FIntPoint(minPos.X - m_spawnRoomSize.X - distanceBtwRooms, FMath::RandRange(minPos.Y, maxPos.X - m_spawnRoomSize.Y));

    m_exitRoom  = MakeShared<RoomData>(exitRoomStartingPos, this->m_exitRoomSize.Y, this->m_exitRoomSize.X, this->m_mainGrid->GetTiles(exitRoomStartingPos, this->m_exitRoomSize), this->GetWorld(), RoomType::EXIT_ROOM);
    m_spawnRoom = MakeShared<RoomData>(spawnRoomStartingPos, this->m_spawnRoomSize.Y, this->m_spawnRoomSize.X, this->m_mainGrid->GetTiles(spawnRoomStartingPos, this->m_spawnRoomSize), this->GetWorld(), RoomType::SPAWN_ROOM);

    this->m_rooms.Add(this->m_exitRoom->GetID(), m_exitRoom);
    this->m_rooms.Add(this->m_spawnRoom->GetID(), m_spawnRoom);
}

void UDungeonGeneratorComponent::GenerateValidPathsToAllRooms()
{
    auto roomsIDsWithAPath   = TSet<int>();                                                                //Rooms that already have a path directing to them from the spawn room.
    auto spawnRoomCenterPos  = this->m_spawnRoom->GetCenterPosition();                               //The position of the spawn room center.
    auto spawnRoomCenterTile = this->m_mainGrid->GetRoomCenterTile(this->m_spawnRoom.ToSharedRef()); //Approximately the center tile of the room.

    //We sort the rooms from the farthest to the closest to the spawn room.
    this->m_rooms.ValueSort([spawnRoomCenterPos] (TSharedPtr<RoomData> first, TSharedPtr<RoomData> second) {
        return FVector2f::Distance(first->GetCenterPosition(), spawnRoomCenterPos) > FVector2f::Distance(second->GetCenterPosition(), spawnRoomCenterPos);
    });

    for (auto room : this->m_rooms)
    {
        if (roomsIDsWithAPath.Contains(room.Value->GetID()) || this->m_spawnRoom == room.Value) continue;       //If the room already has a path or is the spawn room itself, we ignore it.

        auto roomCenterTile        = this->m_mainGrid->GetRoomCenterTile(room.Value.ToSharedRef());             //We get the center tile of this room...
        auto pathToRoom            = UGlobalStatics::GetBestPath(*this, spawnRoomCenterTile, roomCenterTile);         //Then we create a path toward it from the spawn room...
        TSet<int> affectedRoomsIds = this->m_mainGrid->GenerateDoorsFromPathAndReturnAffectedRooms(pathToRoom); //Using that path, we generate doors for all the rooms that the path is passing through...

        //Finally we make sure to create walls around those rooms so that the next path will always try to use the already used tile doors.
        for (auto roomID : affectedRoomsIds)
        {      
            if (roomsIDsWithAPath.Contains(roomID)) continue;

            this->m_mainGrid->GenerateWallsForSpecifiedRoom(this->m_rooms[roomID].ToSharedRef());
            roomsIDsWithAPath.Add(roomID);
        }
    }
}

TOptional<TSharedRef<RoomData>> UDungeonGeneratorComponent::GetRoom(int roomID) const
{
    if (!this->m_rooms.Contains(roomID))
    {
        UE_LOG(LogTemp, Error, TEXT("There's no room with that ID."));
        return TOptional<TSharedRef<RoomData>>();
    }

    return this->m_rooms[roomID].ToSharedRef();
}

TArray<TSharedRef<RoomData>> UDungeonGeneratorComponent::GetAllRooms() const
{
    auto rooms = TArray<TSharedRef<RoomData>>();

    for (auto entry : this->m_rooms) rooms.Add(entry.Value.ToSharedRef());

    return rooms;
}

void UDungeonGeneratorComponent::Clean()
{
    this->m_rooms.Empty();
    this->m_spawnRoom.Reset();
    this->m_exitRoom.Reset();
    if (this->m_mainGrid != nullptr) this->m_mainGrid = nullptr;
}

bool UDungeonGeneratorComponent::GeneratorConfigIsValid()
{
    if (UGrayZoneGameInstance::TILE_SIZE == 0)
    {
        UE_LOG(LogTemp, Fatal, TEXT("Tile size can't be 0."));
        return false;
    }

    auto chanceSum = 0.0f;
    for (auto entry : this->m_roomsSizesPossibleWithChanceToAppear) chanceSum += entry.Value;
    if (chanceSum != 100)
    {
        UE_LOG(LogTemp, Fatal, TEXT("The sum of all roomsSizesPossibleWithChanceToAppear chance values should always be 100."));
        return false;
    }

    return true;
}

UDungeonGeneratorComponent::~UDungeonGeneratorComponent() { this->Clean(); }

void UDungeonGeneratorComponent::DebugDraw()
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

void UDungeonGeneratorComponent::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent)
{
    auto propertyName = PropertyChangedEvent.Property != NULL ? PropertyChangedEvent.Property->GetFName() : NAME_None;

    if (propertyName == GET_MEMBER_NAME_CHECKED(UDungeonGeneratorComponent, m_drawGrid) || propertyName == GET_MEMBER_NAME_CHECKED(UDungeonGeneratorComponent, m_drawDungeonTiles))
        this->DebugDraw();
}

#endif

