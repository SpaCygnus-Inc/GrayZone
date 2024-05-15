// Fill out your copyright notice in the Description page of Project Settings.


#include "Map/DungeonDecoratorComponent.h"
#include "Global/GlobalStatics.h"

UDungeonDecoratorComponent::UDungeonDecoratorComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

    this->m_roomsToSpawn         = TMap<FIntPoint, FRoomsToSpawn>();
    this->m_corridorsToSpawn     = TArray<TSubclassOf<ACorridor>>();

    this->m_spawnedCorridors     = TArray<TObjectPtr<ACorridor>>();
    this->m_spawnedRooms         = TArray<TObjectPtr<ARoom>>();

    this->m_spawningRoomsToSpawn = FRoomsToSpawn();
    this->m_exitRoomsToSpawn     = FRoomsToSpawn();

    this->m_wallsToSpawn         = TMap<FWallType, float>();
    this->m_WallsByPackOf        = FIntPoint(4, 7);
}

void UDungeonDecoratorComponent::DecorateDungeon(TObjectPtr<UDungeonGeneratorComponent> const dungeonGenerator, TObjectPtr<AActor> const parentActor)
{
    this->PlaceCorridors(dungeonGenerator, parentActor);
    this->PlaceRooms(dungeonGenerator, parentActor);
}


void UDungeonDecoratorComponent::PlaceRooms(TObjectPtr<UDungeonGeneratorComponent> const dungeonGenerator, TObjectPtr<AActor> const parentActor)
{
    for (auto roomData : dungeonGenerator->GetAllRooms())
    {
        auto roomsCanBeSpawned = TOptional<FRoomsToSpawn>(); //The rooms with the right sizes that we're going to randomly choose from.
        auto roomRotation      = FRotator::ZeroRotator;      //The rotation of the room.

        if      (roomData->GetRoomType() == RoomType::SPAWN_ROOM)    roomsCanBeSpawned = this->m_spawningRoomsToSpawn;              //If this is a spawn room, then we should choose from the list of spawn rooms.
        else if (roomData->GetRoomType() == RoomType::EXIT_ROOM)     roomsCanBeSpawned = this->m_exitRoomsToSpawn;                  //If this is an exit room, then we should choose from the list of exit rooms.
        else if (this->m_roomsToSpawn.Contains(roomData->GetSize())) roomsCanBeSpawned = this->m_roomsToSpawn[roomData->GetSize()]; //If have a list of rooms with the specified size, we get that one.
        else if (this->m_roomsToSpawn.Contains(UGlobalStatics::SwapFIntPoints(roomData->GetSize())))                                //If we have a list of rooms where their size is the swapped size of the room data, we also get that one.
        {
            roomsCanBeSpawned = this->m_roomsToSpawn[UGlobalStatics::SwapFIntPoints(roomData->GetSize())];
            roomRotation = FRotator(0, 90.0f, 0);
        }

        //Then from that list of rooms, we choose one randomly to spawn.
        auto roomToSpawn = roomsCanBeSpawned.IsSet() && roomsCanBeSpawned.GetValue().RoomsWithChanceToSpawn.Num() > 0 ? UGlobalStatics::GetItemRandomlyUsingPercentage(roomsCanBeSpawned.GetValue().RoomsWithChanceToSpawn) : nullptr;

        //If we weren't able to spawn any room, we print an error and ignore this room data.
        if (roomToSpawn == nullptr) {
            UE_LOG(LogTemp, Error, TEXT("We weren't able to find a room with the size (%i,%i)."), roomData->GetDepth(), roomData->GetWidth());
            continue;
        }

        roomRotation = FMath::RandBool() ? roomRotation : FRotator(0, roomRotation.Yaw + 180.0f, 0); //We randomly choose whether we want to rotate this room 180 degrees or not (180 degrees so that it still be valid at the same position).
        auto spawnedRoom = this->GetWorld()->SpawnActor<ARoom>(roomToSpawn, roomData->GetRealCenterPos(), roomRotation); //Finally we spawn the room...
        spawnedRoom->AttachToActor(parentActor, FAttachmentTransformRules::KeepRelativeTransform);                       //Attach it to the parent actor...
        spawnedRoom->FinishPlacing(roomData, m_WallsByPackOf, this->m_wallsToSpawn);
        this->m_spawnedRooms.Add(spawnedRoom);                                                                           //And add it to the list of spawned rooms.
    }
}

void UDungeonDecoratorComponent::PlaceCorridors(TObjectPtr<UDungeonGeneratorComponent> const dungeonGenerator, TObjectPtr<AActor> const parentActor)
{
    auto corridorTiles = dungeonGenerator->GetGrid()->GetAllTilesWithDescription(TileDescription::CORRIDOR_TILE);
    
    //Note: in the future this logic will need to change so that we detect each time whether we want to spawn a normal corridor, one that turns right/left or one with a dead end.
    for (auto corridor : corridorTiles)
    {
        auto corridorTypeToSpawn = this->m_corridorsToSpawn[FMath::RandRange(0, this->m_corridorsToSpawn.Num() - 1)];
        auto spawnPos            = FVector(corridor->GetRealCenterPosition().X, corridor->GetRealCenterPosition().Y, 0.0f);
    
        //We spawn the corridor at the needed position and then attach it to the parent actor.
        auto spawnedCorridor = this->GetWorld()->SpawnActor<ACorridor>(corridorTypeToSpawn, spawnPos, FRotator::ZeroRotator);
        spawnedCorridor->AttachToActor(parentActor, FAttachmentTransformRules::KeepRelativeTransform);

        this->m_spawnedCorridors.Add(spawnedCorridor);
    }
}

void UDungeonDecoratorComponent::Clean()
{
    for (auto corridor : this->m_spawnedCorridors) corridor->Destroy();
    for (auto room : this->m_spawnedRooms)         { room->Clean(); room->Destroy(); }

    this->m_spawnedCorridors.Reset();
    this->m_spawnedRooms.Reset();
}

