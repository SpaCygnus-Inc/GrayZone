// Fill out your copyright notice in the Description page of Project Settings.


#include "GrayZoneGameInstance.h"

UGrayZoneGameInstance::UGrayZoneGameInstance()
{
    this->LastUsedID = 0;
}

int UGrayZoneGameInstance::GetUniqueID()
{
    this->LastUsedID++;
    return this->LastUsedID;
}
