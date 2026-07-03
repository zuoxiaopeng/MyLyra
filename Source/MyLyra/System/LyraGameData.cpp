#include "LyraGameData.h"

#include "LyraAssetManager.h"

ULyraGameData::ULyraGameData()
{
}

const ULyraGameData& ULyraGameData::Get()
{
	return ULyraAssetManager::Get().GetGameData();
}
