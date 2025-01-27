#pragma once

#include "TileMapBase.h"

namespace AM
{
namespace Client
{
class SpriteData;

/**
 * Owns and manages the world's tile map state.
 * Tiles are conceptually organized into 16x16 chunks.
 *
 * Tile map data is streamed from the server at runtime.
 */
class TileMap : public TileMapBase
{
public:
    /**
     * Attempts to parse TileMap.bin and construct the tile map.
     *
     * Errors if TileMap.bin doesn't exist or it fails to parse.
     */
    TileMap(SpriteData& inSpriteData);

    /**
     * Sets the size of the map and resizes the tiles vector.
     */
    void setMapSize(unsigned int inMapXLengthChunks,
                    unsigned int inMapYLengthChunks);
};

} // End namespace Client
} // End namespace AM
