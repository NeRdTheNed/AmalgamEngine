#pragma once

#include "AssetCache.h"
#include "BoundingBox.h"
#include <SDL2/SDL_rect.h>
#include <memory>

namespace AM
{
namespace Client
{
/**
 * Represents all of the sprite data that the RenderSystem needs, except for
 * the world position.
 *
 * World position should be read from an associated Position component.
 */
struct Sprite {
public:
    /** The texture that contains this sprite. */
    TextureHandle texture{};

    /** UV position and size in texture. */
    SDL_Rect textureExtent{0, 0, 0, 0};

    /** Model-space bounding box. Defines the sprite's 3D volume. */
    BoundingBox modelBounds{0, 0, 0, 0, 0, 0};
};

} // namespace Client
} // namespace AM
