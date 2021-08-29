#include "MessageHandler.h"
#include "Network.h"
#include "Deserialize.h"
#include "ConnectionResponse.h"
#include "EntityUpdate.h"
#include "EntityState.h"
#include "Log.h"

namespace AM
{
namespace Client
{
MessageHandler::MessageHandler(Network& inNetwork)
: network(inNetwork)
{
}

void MessageHandler::handleConnectionResponse(BinaryBuffer& messageRecBuffer,
                                              Uint16 messageSize)
{
    // Deserialize the message.
    std::unique_ptr<ConnectionResponse> connectionResponse
        = std::make_unique<ConnectionResponse>();
    Deserialize::fromBuffer(messageRecBuffer, messageSize,
                              *connectionResponse);

    // Save our player entity so we can determine which update messages are for
    // the player.
    network.setPlayerEntity(connectionResponse->entity);

    // Queue the message.
    if (!(connectionResponseQueue.enqueue(std::move(connectionResponse)))) {
        LOG_ERROR("Ran out of room in queue and memory allocation failed.");
    }
}

void MessageHandler::handleEntityUpdate(BinaryBuffer& messageRecBuffer,
                                        Uint16 messageSize)
{
    // Deserialize the message.
    std::shared_ptr<EntityUpdate> entityUpdate
        = std::make_shared<EntityUpdate>();
    Deserialize::fromBuffer(messageRecBuffer, messageSize, *entityUpdate);

    // Pull out the vector of entities.
    const std::vector<EntityState>& entities = entityUpdate->entityStates;

    // Iterate through the entities, checking if there's player or npc data.
    bool playerFound = false;
    bool npcFound = false;
    entt::entity playerEntity = network.getPlayerEntity();
    for (auto entityIt = entities.begin(); entityIt != entities.end();
         ++entityIt) {
        entt::entity entity = entityIt->entity;

        if (entity == playerEntity) {
            // Found the player.
            if (!(playerUpdateQueue.enqueue(entityUpdate))) {
                LOG_ERROR("Ran out of room in queue and memory allocation "
                          "failed.");
            }
            playerFound = true;
        }
        else if (!npcFound) {
            // Found a non-player (npc).
            // Queueing the message will let all npc updates within be
            // processed.
            if (!(npcUpdateQueue.enqueue(
                    {NpcUpdateType::Update, entityUpdate}))) {
                LOG_ERROR("Ran out of room in queue and memory allocation "
                          "failed.");
            }
            npcFound = true;
        }

        // If we found the player and an npc, we can stop looking.
        if (playerFound && npcFound) {
            break;
        }
    }

    // If we didn't find an NPC and queue an update message, push an
    // implicit confirmation to show that we've confirmed up to this tick.
    if (!npcFound) {
        if (!(npcUpdateQueue.enqueue({NpcUpdateType::ImplicitConfirmation,
                                      nullptr, entityUpdate->tickNum}))) {
            LOG_ERROR("Ran out of room in queue and memory allocation failed.");
        }
    }
}

} // End namespace Client
} // End namespace AM
