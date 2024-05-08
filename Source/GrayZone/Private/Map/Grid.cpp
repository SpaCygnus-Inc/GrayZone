// Fill out your copyright notice in the Description page of Project Settings.


#include "Map/Grid.h"
#include "Global/GlobalStatics.h"

Grid::Grid(int width, int depth, int tileSize)
{
    this->m_width = width;
    this->m_depth = depth;

    this->m_singleTileSize          = tileSize;
    this->m_corridorsTilesGenerated = false;
    this->m_tiles                   = TMap<FIntPoint, TSharedPtr<Tile>>();

    //We generate the grid.
    for (int x = 0; x < this->m_depth; x ++) 
    {
        for (int y = 0; y < this->m_width; y ++)
        {
            auto position = FIntPoint(x, y);
            this->m_tiles.Add(position, MakeShared<Tile>(position, this->m_singleTileSize));
        }
    }
}

FIntPoint Grid::GetHalfGridPos() const
{
    return FIntPoint(FMath::RoundToInt32(this->m_width * 0.5), FMath::RoundToInt32(this->m_depth * 0.5));
}

bool Grid::RoomIsInsideGrid(Room& room) const
{
    return room.GetPosition().X >= 0 && room.GetPosition().X + room.GetDepth() <= this->GetDepth()
        && room.GetPosition().Y >= 0 && room.GetPosition().Y + room.GetWidth() <= this->GetWidth();
}

void Grid::SetRoomTiles(Room& room)
{
    if (!this->AllowChangeTilesTypes()) return;

    for (int i = 0; i < room.GetWidth(); i++)
    {
        for (int j = 0; j < room.GetDepth(); j++)
        {
            auto currentTile = this->m_tiles[FIntPoint(room.GetPosition().X + j, room.GetPosition().Y + i)];
            currentTile->ResetAndSetTileType(TileDescription::ROOM_TILE, room.GetID());
        }
    }
}

void Grid::CleanRoomTiles(Room& room)
{
    if (!this->AllowChangeTilesTypes()) return;

    for (int i = 0; i < room.GetWidth(); i++)
    {
        for (int j = 0; j < room.GetDepth(); j++)
        {
            auto currentTile = this->m_tiles[FIntPoint(room.GetPosition().X + j, room.GetPosition().Y + i)];
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

TSharedPtr<Tile> Grid::GetRoomCenterTile(TSharedRef<Room> const room) const
{
    auto centerPos = room.Get().GetCenterPosition();
    auto approxCenterTile = FIntPoint(FMath::RoundToInt32(centerPos.X), FMath::RoundToInt32(centerPos.Y));

    return this->m_tiles[approxCenterTile];
}

TArray<TSharedPtr<Tile>> Grid::GetTileNeighbors(TSharedPtr<Tile> tile) const
{
    auto resultArray = TArray<TSharedPtr<Tile>>();

    for (auto direction : Grid::m_tileNeighborsDirections)
    {
        auto neighborPos = tile.Get()->GetPosition() + direction;
        if (this->m_tiles.Contains(neighborPos)) resultArray.Add(TSharedPtr<Tile>(this->m_tiles[neighborPos]));
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

TSharedPtr<Tile> Grid::GetTileAtPos(int x, int y) const
{
    return this->GetTileAtPos(FIntPoint(x, y));
}

TSharedPtr<Tile> Grid::GetTileAtPos(FIntPoint position) const
{
    return this->m_tiles[position];
}

TSet<int> Grid::GenerateDoorsFromPathAndReturnAffectedRooms(TArray<TSharedPtr<Tile>> path)
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
        if (!(path[i].Get()->GetTileDescription() & TileDescription::ROOM_TILE)) continue;

        if (path[i-1].Get()->GetTileDescription() & TileDescription::CORRIDOR_TILE || path[i + 1].Get()->GetTileDescription() & TileDescription::CORRIDOR_TILE)
            path[i].Get()->AddTileDescription(TileDescription::DOOR_TILE);

        roomsIDs.Add(path[i].Get()->GetTileTypeID());
    }

    return roomsIDs;
}

void Grid::GenerateWallsForSpecifiedRoom(TSharedRef<Room> room)
{
    auto roomPosition = room.Get().GetPosition();

    //We check both the up and down walls...
    for (int x = roomPosition.X; x < roomPosition.X + room.Get().GetDepth(); x++)
    {
        auto currentTile = m_tiles[FIntPoint(x, roomPosition.Y)];
        if (!(currentTile.Get()->GetTileDescription() & TileDescription::DOOR_TILE))
            currentTile.Get()->AddTileDescription(TileDescription::WALL_TILE);

        currentTile = m_tiles[FIntPoint(x, roomPosition.Y + room.Get().GetWidth() - 1)];
        if (!(currentTile.Get()->GetTileDescription() & TileDescription::DOOR_TILE))
            currentTile.Get()->AddTileDescription(TileDescription::WALL_TILE);
    }

    //Then we check the right and left.
    for (int y = roomPosition.Y; y < roomPosition.Y + room.Get().GetWidth(); y++)
    {
        auto currentTile = m_tiles[FIntPoint(roomPosition.X, y)];
        if (!(currentTile.Get()->GetTileDescription() & TileDescription::DOOR_TILE))
            currentTile.Get()->AddTileDescription(TileDescription::WALL_TILE);

        currentTile = m_tiles[FIntPoint(roomPosition.X + room.Get().GetDepth() - 1, y)];
        if (!(currentTile.Get()->GetTileDescription() & TileDescription::DOOR_TILE))
            currentTile.Get()->AddTileDescription(TileDescription::WALL_TILE);
    }

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
