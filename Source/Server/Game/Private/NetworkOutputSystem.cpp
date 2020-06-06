#include "NetworkOutputSystem.h"
#include "Game.h"
#include "World.h"
#include "Network.h"
#include "MessageUtil.h"
#include "NetworkHelpers.h"
#include "Debug.h"

namespace AM
{
namespace Server
{

NetworkOutputSystem::NetworkOutputSystem(Game& inGame, World& inWorld, Network& inNetwork)
: game(inGame)
, world(inWorld)
, network(inNetwork)
, builder(BUILDER_BUFFER_SIZE)
{
}

void NetworkOutputSystem::sendClientUpdates()
{
    /* Process network output from all entities with a ClientComponent. */
    for (size_t entityID = 0; entityID < MAX_ENTITIES; ++entityID) {
        if ((world.componentFlags[entityID] & ComponentFlag::Client)) {
            // Prep the builder for a new message.
            builder.Clear();

            // Create the vector of entity data.
            std::vector<flatbuffers::Offset<fb::Entity>> entityVector;
            for (EntityID entityID = 0; entityID < MAX_ENTITIES; ++entityID) {
                // Only send updates for entities that changed.
                if (world.entityIsDirty[entityID]) {
                    DebugInfo("Broadcasting: %u", entityID);
                    entityVector.push_back(serializeEntity(entityID));
                    world.entityIsDirty[entityID] = false;
                }
            }
            auto serializedEntities = builder.CreateVector(entityVector);

            // Build an EntityUpdate.
            flatbuffers::Offset<fb::EntityUpdate> entityUpdate = fb::CreateEntityUpdate(builder,
                serializedEntities);

            // Build a Message.
            fb::MessageBuilder messageBuilder(builder);
            messageBuilder.add_tickTimestamp(game.getCurrentTick());
            messageBuilder.add_content_type(fb::MessageContent::EntityUpdate);
            messageBuilder.add_content(entityUpdate.Union());
            flatbuffers::Offset<fb::Message> message = messageBuilder.Finish();
            builder.Finish(message);

            // Send the message to all connected clients.
            network.send(world.clients[entityID].networkID,
                NetworkHelpers::constructMessage(builder.GetSize(), builder.GetBufferPointer()));
        }
    }
}

flatbuffers::Offset<AM::fb::Entity> NetworkOutputSystem::serializeEntity(
EntityID entityID)
{
    /* Fill the message with the latest PositionComponent, NetworkOutputComponent,
       and InputComponent data. */
    // Translate the inputs to fb's enum.
    InputComponent& input = world.inputs[entityID];

    fb::InputState fbInputStates[Input::Type::NumTypes];
    std::array<Input::State, Input::NumTypes>& playerInputStates =
        world.inputs[entityID].inputStates;

    for (uint8_t i = 0; i < Input::Type::NumTypes; ++i) {
        // Translate the Input::State enum to fb::InputState.
        fbInputStates[i] = MessageUtil::convertToFbInputState(playerInputStates[i]);
    }

    flatbuffers::Offset<flatbuffers::Vector<fb::InputState>> inputVector =
        builder.CreateVector(fbInputStates, Input::Type::NumTypes);

    // Build the InputComponent.
    flatbuffers::Offset<fb::InputComponent> inputComponent = fb::CreateInputComponent(
        builder, inputVector);

    // Build the PositionComponent.
    PositionComponent& position = world.positions[entityID];
    flatbuffers::Offset<fb::PositionComponent> positionComponent =
        fb::CreatePositionComponent(builder, position.x, position.y);

    // Build the MovementComponent.
    MovementComponent& movement = world.movements[entityID];
    flatbuffers::Offset<fb::MovementComponent> movementComponent =
        fb::CreateMovementComponent(builder, movement.velX, movement.velY,
            movement.maxVelX, movement.maxVelY);
    DebugInfo("Sending: (%f, %f), (%f, %f)", position.x, position.y, movement.velX,
        movement.velY);

    // Build the Entity.
    auto entityName = builder.CreateString(world.entityNames[entityID]);
    fb::EntityBuilder entityBuilder(builder);
    entityBuilder.add_id(entityID);
    entityBuilder.add_name(entityName);

    // Mark the components that we're sending.
    entityBuilder.add_flags(
        ComponentFlag::Position & ComponentFlag::Movement & ComponentFlag::Input);
    entityBuilder.add_positionComponent(positionComponent);
    entityBuilder.add_movementComponent(movementComponent);
    entityBuilder.add_inputComponent(inputComponent);

    return entityBuilder.Finish();
}

} // namespace Server
} // namespace AM
