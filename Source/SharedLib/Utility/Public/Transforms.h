#pragma once

#include "TilePosition.h"
#include "Position.h"
#include "BoundingBox.h"
#include "SharedConfig.h"
#include <SDL_rect.h>

namespace AM
{
struct ScreenPoint;
struct Sprite;
struct Camera;

/**
 * Static functions for transforming between world, screen, and model space.
 */
class Transforms
{
public:
    /**
     * Converts a position in world space to a point in screen space.
     *
     * @param zoomFactor  The camera's zoom factor.
     */
    static ScreenPoint worldToScreen(const Position& position,
                                     float zoomFactor);

    /**
     * Converts a Z coordinate in world space to a Y coordinate in screen space.
     *
     * @param zoomFactor  The camera's zoom factor.
     */
    static float worldZToScreenY(float zCoord, float zoomFactor);

    /**
     * Converts a point in screen space to a position in world space.
     */
    static Position screenToWorld(const ScreenPoint& screenPoint,
                                  const Camera& camera);

    /**
     * Converts a Y coordinate in screen space to a Z coordinate in world space.
     *
     * @param zoomFactor  The camera's zoom factor.
     */
    static float screenYToWorldZ(float yCoord, float zoomFactor);

    /**
     * Helper function, converts a camera-relative screen point to a tile
     * position.
     *
     * Mostly used for getting the tile that the mouse is over.
     */
    static TilePosition screenToTile(const ScreenPoint& screenPoint,
                                     const Camera& camera);

    /**
     * Converts a model-space bounding box to a world-space box, placed at the
     * given position.
     *
     * Mostly used to place a bounding box associated with a tile.
     */
    static BoundingBox modelToWorld(const BoundingBox& modelBounds,
                                    const Position& position);

    /**
     * Converts a model-space bounding box to a world-space box, centered on
     * the given position.
     *
     * Mostly used to center a bounding box on an entity's position.
     *
     * Note: This function takes care to center the bounds based on the
     *       sprite's "stage size". If you naively center based on the size of
     *       the box, you won't get the correct positioning.
     */
    static BoundingBox modelToWorldCentered(const BoundingBox& modelBounds,
                                            const Position& position);

    /**
     * Returns an entity's position, given that entity's BoundingBox and Sprite
     * components.
     *
     * Note: This function takes care to consider the sprite's "stage size".
     *       If you naively take the center of the bounds as the entity's
     *       position, it won't be correct.
     * Note: This isn't a transform between spaces, but it's related to
     *       modelToWorldCentered() so it's nice to have in this file.
     */
    template<typename T>
    static Position boundsToEntityPosition(const BoundingBox& boundingBox,
                                           const T& sprite)
    {
        // The box is centered on the entity's position by offsetting it by
        // half of the sprite's stage size. Remove this stage offset and the
        // model offset to find the X/Y position.
        // Note: This assumes that the sprite's stage is 1 tile large. When we
        //       add support for other sizes, this will need to be updated.
        Position position{};
        position.x = boundingBox.minX - sprite.modelBounds.minX
                     + (SharedConfig::TILE_WORLD_WIDTH / 2);
        position.y = boundingBox.minY - sprite.modelBounds.minY
                     + (SharedConfig::TILE_WORLD_WIDTH / 2);

        // The bottom of the stage is flush with the entity's position, so we
        // only need to remove the model offset.
        position.z = boundingBox.minZ - sprite.modelBounds.minZ;

        return position;
    }
};

} // End namespace AM