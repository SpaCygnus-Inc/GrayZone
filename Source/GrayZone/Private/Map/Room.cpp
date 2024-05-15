// Fill out your copyright notice in the Description page of Project Settings.


#include "Map/Room.h"
#include "Global/GlobalStatics.h"

// Sets default values
ARoom::ARoom()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
    this->SetActorEnableCollision(false);

    m_roomData     = TOptional<TSharedRef<RoomData>>();
    m_spawnedWalls = TArray<TObjectPtr<AWall>>();
}

void ARoom::Clean()
{
    //When a room is destroyed, we make sure to destroy all the spawned walls.
    for (auto spawnedWall : m_spawnedWalls)
        spawnedWall->Destroy();

    this->m_spawnedWalls.Reset();
}

// Called when the game starts or when spawned
void ARoom::BeginPlay()
{
	Super::BeginPlay();
	
}

void ARoom::FinishPlacing(TSharedRef<RoomData> roomData, FIntPoint wallsByPackOf, TMap<FWallType, float> wallsThatCanSpawn)
{
    this->m_roomData = roomData;

    this->GenerateWalls(wallsByPackOf, wallsThatCanSpawn);
}

void ARoom::GenerateWalls(FIntPoint wallsByPackOf, TMap<FWallType, float> wallsThatCanSpawn)
{
    if (!this->RoomDataISSet()) return; //If room data isn't set for this room, we don't do anything. This should never happen, but just in case.

    auto currentWallsPack = FMath::RandRange(wallsByPackOf.X, wallsByPackOf.Y);                //How many walls of the same type we want it to be.
    auto currentWallType  = UGlobalStatics::GetItemRandomlyUsingPercentage(wallsThatCanSpawn); //The current type of walls that will be spawned.

    for (auto wallTile : this->m_roomData.GetValue()->GetWallTiles())
    {
        //If the weak ptr is valid then we lock it so that it doesn't get deleted while we're using it.
        if (wallTile.IsValid())
        {
            auto refWallTile = wallTile.Pin();

            //TO DO: in the future we want to also take care of the "DOOR_TILE" and put a door on that tile, but for now we just let that tile be empty.
            if (!(refWallTile->GetTileDescription() & TileDescription::WALL_TILE)) continue;
    
            //We spawn the wall with the correct rotation and all (this is just in case, depending on how the models will be done in the future, there's a big chance we would't need to rotate the walls).
            auto roomPos     = this->m_roomData.GetValue()->GetPosition();
            auto spawnedWall = this->SpawnAndGetWall(currentWallType, refWallTile, this->m_roomData.GetValue()->GetPosition(), FIntPoint(roomPos.X + this->m_roomData.GetValue()->GetDepth() - 1, roomPos.Y + this->m_roomData.GetValue()->GetWidth() - 1));
            spawnedWall->AttachToActor(this, FAttachmentTransformRules::KeepWorldTransform);
            this->m_spawnedWalls.Add(spawnedWall);

            currentWallsPack--;

            //When we have spawned the right amount of walls of this type, we generate another pack.
            if (currentWallsPack == 0)
            {
                currentWallsPack = FMath::RandRange(wallsByPackOf.X, wallsByPackOf.Y);
                currentWallType  = UGlobalStatics::GetItemRandomlyUsingPercentage(wallsThatCanSpawn);
            }
        }
        else {
            UE_LOG(LogTemp, Error, TEXT("A tile was destroyed while we were creating walls for a room."));
            continue;
        }
    }
}

TObjectPtr<AWall> ARoom::SpawnAndGetWall(FWallType wallType, TSharedPtr<Tile> wallTile, FIntPoint minRoomPos, FIntPoint maxRoomPos)
{
    auto wallToSpawn = wallType.NormalWall;

    //If the wall position is at any of the room edges, we make sure to return an edge wall.
    if ((wallTile->GetPosition() == this->m_roomData.GetValue()->GetPosition())
    || (wallTile->GetPosition() == FIntPoint(minRoomPos.X, maxRoomPos.Y))
    || (wallTile->GetPosition() == FIntPoint(maxRoomPos.X, maxRoomPos.Y))
    || (wallTile->GetPosition() == FIntPoint(maxRoomPos.X, minRoomPos.Y)))
        wallToSpawn = wallType.EdgeWall;

    //We rotate the wall according to the position that we want it to spawn at.
    auto rotation = FRotator::ZeroRotator;

    if (wallTile->GetPosition().X == maxRoomPos.X && wallTile->GetPosition().Y != maxRoomPos.Y)      rotation = FRotator(0, 90.0f, 0);
    else if (wallTile->GetPosition().X != maxRoomPos.X && wallTile->GetPosition().Y == minRoomPos.Y) rotation = FRotator(0, 180.0f, 0);
    else if (wallTile->GetPosition().X == minRoomPos.X && wallTile->GetPosition().Y != minRoomPos.Y) rotation = FRotator(0, 270.0f, 0);

    return this->GetWorld()->SpawnActor<AWall>(wallToSpawn, wallTile->GetRealCenterPosition(), rotation);
}

bool ARoom::RoomDataISSet()
{
    if (!this->m_roomData.IsSet())
    {
        UE_LOG(LogTemp, Error, TEXT("Room data wasn't set for some specific room."));
        return false;
    }

    return true;
}

