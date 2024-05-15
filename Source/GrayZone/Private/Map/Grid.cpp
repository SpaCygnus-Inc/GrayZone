// Fill out your copyright notice in the Description page of Project Settings.


#include "Map/Grid.h"
#include "Global/GlobalStatics.h"

Grid::Grid(int width, int depth, int tileSize)
{
    this->m_width = width;
    this->m_depth = depth;

    this->m_corridorsTilesGenerated = false;
    this->m_tiles                   = TMap<FIntPoint, TSharedPtr<Tile>>();

    //We generate the grid.
    for (int x = 0; x < this->m_depth; x ++) 
    {
        for (int y = 0; y < this->m_width; y ++)
        {
            auto position = FIntPoint(x, y);
            this->m_tiles.Add(position, MakeShared<Tile>(position));
        }
    }
}

FIntPoint Grid::GetHalfGridPos() const
{
    return FIntPoint(FMath::RoundToInt32(this->m_width * 0.5), FMath::RoundToInt32(this->m_depth * 0.5));
}

bool Grid::RoomIsInsideGrid(TSharedRef<RoomData> room) const
{
    return room->GetPosition().X >= 0 && room->GetPosition().X + room->GetDepth() <= this->GetDepth()
        && room->GetPosition().Y >= 0 && room->GetPosition().Y + room->GetWidth() <= this->GetWidth();
}

void Grid::SetRoomTiles(TSharedRef<RoomData> room)
{
    if (!this->AllowChangeTilesTypes()) return;

    for (int i = 0; i < room->GetWidth(); i++)
    {
        for (int j = 0; j < room->GetDepth(); j++)
        {
            auto currentTile = this->m_tiles[FIntPoint(room->GetPosition().X + j, room->GetPosition().Y + i)];
            currentTile->ResetAndSetTileType(TileDescription::ROOM_TILE, room->GetID());
        }
    }
}

void Grid::CleanRoomTiles(TSharedRef<RoomData> room)
{
    if (!this->AllowChangeTilesTypes()) return;

    for (int i = 0; i < room->GetWidth(); i++)
    {
        for (int j = 0; j < room->GetDepth(); j++)
        {
            auto currentTile = this->m_tiles[FIntPoint(room->GetPosition().X + j, room->GetPosition().Y + i)];
            currentTile->ResetAndSetTileType(TileDescription::NONE_TILE);
        }
    }
}

void Grid::GenerateCorridorsTiles(const UWorld* inWorld)
{
    if (!this->AllowChangeTilesTypes()) return;

    this->GenerateCorridordsOnAxis(true, inWorld);
    this->GenerateCorridordsOnAxis(false, inWorld);
    
    this->m_corridorsTilesGenerated = true;
}

void Grid::GenerateCorridordsOnAxis(bool horizontal, const UWorld* inWorld)
{
    //We generate corridors either horizontally or vertically.
    auto column = horizontal ? this->m_width : this->m_depth;
    auto row    = horizontal ? this->m_depth : this->m_width;

    for (int i = 0; i < row; i++)
    {
        //We want to start generating corridors starting from the first room and ending with the last one on that row.
        auto startIndex      = 0;
        auto endIndex        = column - 1;
        auto checkingEnd     = true;
        auto startGenerating = false;

        do {
            auto currentIndex = checkingEnd ? endIndex : startIndex;
            auto currentPos = FIntPoint(horizontal ? i : currentIndex, horizontal ? currentIndex : i);
            auto currentTile = this->m_tiles[currentPos];
            
            //Once we find a the last tile that's used for a room, we save that index as the last tile to check.
            if (checkingEnd) 
            {
                if (currentTile->GetTileDescription() & TileDescription::ROOM_TILE) checkingEnd = false;
                else endIndex--;
            }
            else 
            {
                //Once we find the first tile that's used for a room on this row, we start generating corridors on each tile that isn't used for anything.
                if      (startGenerating && currentTile->GetTileDescription()  & TileDescription::NONE_TILE) currentTile->ResetAndSetTileType(TileDescription::CORRIDOR_TILE, UGlobalStatics::GetGameInstance(inWorld).GetUniqueID());
                else if (!startGenerating && currentTile->GetTileDescription() & TileDescription::ROOM_TILE) startGenerating = true;

                startIndex++;
            }
        }while(startIndex < endIndex);
    }
}

TSharedRef<Tile> Grid::GetRoomCenterTile(TSharedRef<RoomData> const room) const
{
    auto centerPos = room->GetCenterPosition();
    auto approxCenterTile = FIntPoint(FMath::RoundToInt32(centerPos.X), FMath::RoundToInt32(centerPos.Y));

    return this->m_tiles[approxCenterTile].ToSharedRef();
}

TArray<TSharedRef<Tile>> Grid::GetTileNeighbors(TSharedPtr<Tile> tile) const
{
    auto resultArray = TArray<TSharedRef<Tile>>();

    for (auto direction : Grid::m_tileNeighborsDirections)
    {
        auto neighborPos = tile->GetPosition() + direction;
        if (this->m_tiles.Contains(neighborPos)) resultArray.Add(this->m_tiles[neighborPos].ToSharedRef());
    }

    return resultArray;
}

bool Grid::AllowChangeTilesTypes() const
{
    if (this->m_corridorsTilesGenerated)
    {
        UE_LOG(LogTemp, Error, TEXT("We're trying to change a title type after the corridors were generated."));
        return false;
    }

    return true;
}

TSharedRef<Tile> Grid::GetTileAtPos(int x, int y) const
{
    return this->GetTileAtPos(FIntPoint(x, y));
}

TSharedRef<Tile> Grid::GetTileAtPos(FIntPoint position) const
{
    return this->m_tiles[position].ToSharedRef();
}

TMap<FIntPoint, TWeakPtr<Tile>> Grid::GetTiles(FIntPoint position, FIntPoint size)
{
    auto tilesToReturn = TMap<FIntPoint, TWeakPtr<Tile>>();

    for (int x = position.X; x < position.X + size.X; x++)
        for (int y = position.Y; y < position.Y + size.Y; y++) tilesToReturn.Add(FIntPoint(x, y), this->m_tiles[FIntPoint(x, y)]);

    return tilesToReturn;
}

TSet<int> Grid::GenerateDoorsFromPathAndReturnAffectedRooms(TArray<TSharedRef<Tile>> path)
{
    if (!this->m_corridorsTilesGenerated)
    {
        UE_LOG(LogTemp, Error, TEXT("We're trying to generate doors for rooms before even generating corridors. Corridors should always be generated first."));
        return TSet<int>();
    }

    auto roomsIDs = TSet<int>(); //We store all the rooms that this path will pass through.

    //We go through all the tiles that the path pass through.
    //Each time a tile is a corridor one directly before or after a room tile then it is a door.
    for (int i = 1; i < path.Num() - 1; i++)
    {
        if (!(path[i]->GetTileDescription() & TileDescription::ROOM_TILE)) continue;

        if (path[i - 1]->GetTileDescription() & TileDescription::CORRIDOR_TILE || (path[i - 1]->GetTileDescription() & TileDescription::ROOM_TILE && path[i - 1]->GetTileTypeID() != path[i]->GetTileTypeID())
            || (path[i + 1]->GetTileDescription() & TileDescription::CORRIDOR_TILE) || (path[i + 1]->GetTileDescription() & TileDescription::ROOM_TILE && path[i + 1]->GetTileTypeID() != path[i]->GetTileTypeID()))
        {
            path[i]->AddTileDescription(TileDescription::DOOR_TILE);
            roomsIDs.Add(path[i]->GetTileTypeID());
        }
    }

    return roomsIDs;
}

void Grid::GenerateWallsForSpecifiedRoom(TSharedRef<RoomData> room)
{
    auto roomPosition = room->GetPosition();

    //We check both the up and down walls...
    for (int x = roomPosition.X; x < roomPosition.X + room->GetDepth(); x++)
    {
        auto currentTile = m_tiles[FIntPoint(x, roomPosition.Y)];
        if (!(currentTile->GetTileDescription() & TileDescription::DOOR_TILE))
            currentTile->AddTileDescription(TileDescription::WALL_TILE);

        currentTile = m_tiles[FIntPoint(x, roomPosition.Y + room->GetWidth() - 1)];
        if (!(currentTile->GetTileDescription() & TileDescription::DOOR_TILE))
            currentTile->AddTileDescription(TileDescription::WALL_TILE);
    }

    //Then we check the right and left.
    for (int y = roomPosition.Y; y < roomPosition.Y + room->GetWidth(); y++)
    {
        auto currentTile = m_tiles[FIntPoint(roomPosition.X, y)];
        if (!(currentTile->GetTileDescription() & TileDescription::DOOR_TILE))
            currentTile->AddTileDescription(TileDescription::WALL_TILE);

        currentTile = m_tiles[FIntPoint(roomPosition.X + room->GetDepth() - 1, y)];
        if (!(currentTile->GetTileDescription() & TileDescription::DOOR_TILE))
            currentTile->AddTileDescription(TileDescription::WALL_TILE);
    }

}

TArray<TSharedRef<Tile>> Grid::GetAllTilesWithDescription(TileDescription description) const
{
    auto tilesToReturn = TArray<TSharedRef<Tile>>();

    for(auto entry : this->m_tiles)
        if (entry.Value->GetTileDescription() & description) tilesToReturn.Add(entry.Value.ToSharedRef());

    return tilesToReturn;
}

void Grid::ClearAllTiles()
{
    for (auto entry : this->m_tiles)
        entry.Value->ResetAndSetTileType(TileDescription::NONE_TILE);

    this->m_corridorsTilesGenerated = false;
}

Grid::~Grid()
{
    this->m_tiles.Empty();
}

void Grid::DebugDraw(const UWorld* inWorld, bool drawDetailedDungeonTiles)
{
    for (auto entry : this->m_tiles) {
         if (!drawDetailedDungeonTiles) { entry.Value->DebugDraw(inWorld, FColor::Red , 0); continue; }

         if      (entry.Value->GetTileDescription() & TileDescription::CORRIDOR_TILE) entry.Value->DebugDraw(inWorld, FColor::Blue, 0.1f);
         else if (entry.Value->GetTileDescription() & TileDescription::DOOR_TILE)     entry.Value->DebugDraw(inWorld, FColor::Green, 0.1f);
         else if (entry.Value->GetTileDescription() & TileDescription::WALL_TILE)     entry.Value->DebugDraw(inWorld, FColor::Red, 0.1f);
    }
}
