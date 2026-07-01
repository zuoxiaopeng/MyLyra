#include "LyraGameData.h"

#include "LyraAssetManager.h"

const ULyraGameData& ULyraGameData::Get()
{
	return ULyraAssetManager::Get().GetGameData();
}
