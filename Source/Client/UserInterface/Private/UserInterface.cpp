#include "UserInterface.h"
#include "World.h"
#include "SpriteData.h"
#include "Camera.h"
#include "Paths.h"
#include "SharedConfig.h"
#include "Transforms.h"
#include "Log.h"

namespace AM
{
namespace Client
{
UserInterface::UserInterface(World& inWorld, SpriteData& spriteData)
: tileHighlightSprite{}
, tileHighlightIndex{0, 0}
, world{inWorld}
{
    // Set up the tile highlight sprite.
    tileHighlightSprite = &(spriteData.get("test_8"));

    // Push the terrain sprites to cycle through.
    terrainSprites.push_back(&(spriteData.get("test_6")));
    terrainSprites.push_back(&(spriteData.get("test_8")));
    terrainSprites.push_back(&(spriteData.get("test_24")));
}

bool UserInterface::handleEvent(SDL_Event& event)
{
    switch (event.type) {
        // TODO: If the player moves through key presses but doesn't move the
        //       mouse, the highlight won't update. Decide how to handle it.
        case SDL_MOUSEMOTION:
            handleMouseMotion(event.motion);

            // Temporarily consuming mouse events until the sim has some use
            // for them.
            return true;
        case SDL_MOUSEBUTTONDOWN:
            handleMouseButtonDown(event.button);
            return true;
    }

    return false;
}

void UserInterface::handleMouseMotion(SDL_MouseMotionEvent& event)
{
    // Get the tile index that the mouse is hovering over.
    Camera& playerCamera = world.registry.get<Camera>(world.playerEntity);
    ScreenPoint screenPoint{static_cast<float>(event.x),
                            static_cast<float>(event.y)};
    TileIndex tileIndex = Transforms::screenToTile(screenPoint, playerCamera);

    // If the index is outside of the world bounds, ignore this event.
    if ((tileIndex.x < 0) || (tileIndex.y < 0)
        || (tileIndex.x >= static_cast<int>(SharedConfig::WORLD_WIDTH))
        || (tileIndex.y >= static_cast<int>(SharedConfig::WORLD_HEIGHT))) {
        return;
    }

    // Save the new index for the renderer to use.
    tileHighlightIndex = Transforms::screenToTile(screenPoint, playerCamera);
}

void UserInterface::handleMouseButtonDown(SDL_MouseButtonEvent& event)
{
    switch (event.button) {
        case SDL_BUTTON_LEFT:
            cycleTile(event.x, event.y);
            break;
    }
}

void UserInterface::cycleTile(int mouseX, int mouseY)
{
    // Find the tile index under the mouse's current position.
    Camera& playerCamera = world.registry.get<Camera>(world.playerEntity);
    ScreenPoint screenPoint{static_cast<float>(mouseX),
                            static_cast<float>(mouseY)};
    TileIndex tileIndex = Transforms::screenToTile(screenPoint, playerCamera);

    // If the index is outside of the world bounds, ignore this event.
    if ((tileIndex.x < 0) || (tileIndex.y < 0)
        || (tileIndex.x >= static_cast<int>(SharedConfig::WORLD_WIDTH))
        || (tileIndex.y >= static_cast<int>(SharedConfig::WORLD_HEIGHT))) {
        return;
    }

    // Determine which sprite the selected tile has.
    const Tile& tile = world.tileMap.get(tileIndex.x, tileIndex.y);

    unsigned int terrainSpriteIndex = 0;
    if (tile.spriteLayers[0].sprite->stringId == "test_6") {
        terrainSpriteIndex = 0;
    }
    else if (tile.spriteLayers[0].sprite->stringId == "test_8") {
        terrainSpriteIndex = 1;
    }
    else if (tile.spriteLayers[0].sprite->stringId == "test_24") {
        terrainSpriteIndex = 2;
    }

    // Set the tile to the next sprite.
    terrainSpriteIndex++;
    terrainSpriteIndex %= 3;
    world.tileMap.replaceSpriteLayer(tileIndex.x, tileIndex.y, 0,
                                     *(terrainSprites[terrainSpriteIndex]));
}

} // End namespace Client
} // End namespace AM
