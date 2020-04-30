#include "Game.h"
#include "Network.h"
#include "Debug.h"
#include <iostream>
#include <iomanip>

extern bool exitRequested;

namespace AM
{
namespace Client
{

Game::Game(Network& inNetwork, std::shared_ptr<SDL2pp::Texture>& inSprites)
: world()
, network(inNetwork)
, playerInputSystem(*this, world, network)
, networkMovementSystem(*this, world, network)
, movementSystem(world)
, builder(BUILDER_BUFFER_SIZE)
, timeSinceTick(0.0f)
, currentTick(0)
, sprites(inSprites)
{
    Debug::registerCurrentTickPtr(&currentTick);
}

void Game::connect()
{
    while (!(network.connect())) {
        std::cerr << "Network failed to connect. Retrying." << std::endl;
    }

    // Wait for the player's ID from the server.
    BinaryBufferPtr responseBuffer = network.receive();
    while (responseBuffer == nullptr) {
        responseBuffer = network.receive();
        SDL_Delay(10);
    }

    // Get our info from the connection response.
    const fb::Message* message = fb::GetMessage(responseBuffer->data());
    if (message->content_type() != fb::MessageContent::ConnectionResponse) {
        std::cerr << "Expected ConnectionResponse but got something else." << std::endl;
    }
    auto connectionResponse = static_cast<const fb::ConnectionResponse*>(message->content());
    currentTick = connectionResponse->currentTick();
    EntityID player = connectionResponse->entityID();

    // Set up our player.
    SDL2pp::Rect textureRect(0, 32, 16, 16);
    SDL2pp::Rect worldRect(connectionResponse->x(), connectionResponse->y(), 64, 64);

    world.AddEntity("Player", player);
    world.positions[player].x = connectionResponse->x();
    world.positions[player].y = connectionResponse->y();
    world.movements[player].maxVelX = 250;
    world.movements[player].maxVelY = 250;
    world.sprites[player].texturePtr = sprites;
    world.sprites[player].posInTexture = textureRect;
    world.sprites[player].posInWorld = worldRect;
    world.AttachComponent(player, ComponentFlag::Input);
    world.AttachComponent(player, ComponentFlag::Movement);
    world.AttachComponent(player, ComponentFlag::Position);
    world.AttachComponent(player, ComponentFlag::Sprite);
    world.registerPlayerID(player);
}

void Game::tick(float deltaSeconds)
{
    timeSinceTick += deltaSeconds;
    if (timeSinceTick < GAME_TICK_INTERVAL_S) {
        // It's not yet time to process the game tick.
        return;
    }
    currentTick++;

    /* Run all systems. */
    // Process all waiting user input events.
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            exitRequested = true;
        }
        else if (event.type == SDL_WINDOWEVENT) {
//            switch(event.type) {
//                case SDL_WINDOWEVENT_SHOWN:
//                case SDL_WINDOWEVENT_EXPOSED:
//                case SDL_WINDOWEVENT_MOVED:
//                case SDL_WINDOWEVENT_MAXIMIZED:
//                case SDL_WINDOWEVENT_RESTORED:
//                case SDL_WINDOWEVENT_FOCUS_GAINED:
//                // Window was messed with, we've probably lost sync with the server.
//                // TODO: Handle the far-out-of-sync client.
//            }
        }
        else {
            // Assume it's a key or mouse event.
            playerInputSystem.processInputEvent(event);
        }
    }
    // Send updates to the server.
    playerInputSystem.sendInputState();

    movementSystem.processMovements(timeSinceTick);

    // Process network movement after normal movement to sync with server.
    // (The server processes movement before sending updates.)
    networkMovementSystem.processServerMovements();

    timeSinceTick = 0;
}

World& Game::getWorld()
{
    return world;
}

Uint32 Game::getCurrentTick()
{
    return currentTick;
}

} // namespace Client
} // namespace AM