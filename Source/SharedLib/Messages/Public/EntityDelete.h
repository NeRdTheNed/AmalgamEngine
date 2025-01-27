#pragma once

#include "MessageType.h"
#include "entt/fwd.hpp"
#include "entt/entity/entity.hpp"

namespace AM
{
/**
 * Sent by the server to tell a client that an entity has left its area of
 * interest and must be deleted.
 */
struct EntityDelete {
    // The MessageType enum value that this message corresponds to.
    // Declares this struct as a message that the Network can send and receive.
    static constexpr MessageType MESSAGE_TYPE = MessageType::EntityDelete;

    /** The tick that this update corresponds to. */
    Uint32 tickNum{0};

    /** The entity that must be deleted. */
    entt::entity entity{entt::null};
};

template<typename S>
void serialize(S& serializer, EntityDelete& entityDelete)
{
    serializer.value4b(entityDelete.tickNum);
    serializer.value4b(entityDelete.entity);
}

} // End namespace AM
