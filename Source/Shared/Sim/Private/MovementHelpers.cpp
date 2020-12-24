#include "MovementHelpers.h"
#include "Position.h"
#include "PreviousPosition.h"
#include "Movement.h"
#include "Ignore.h"

namespace AM
{
void MovementHelpers::moveEntity(Position& position, Movement& movement,
                                 Input::StateArr& inputStates,
                                 double deltaSeconds)
{
    // Update the velocity.
    updateVelocity(movement, inputStates, deltaSeconds);

    // Update the position.
    position.x += (deltaSeconds * movement.velX);
    position.y += (deltaSeconds * movement.velY);
}

Position MovementHelpers::interpolatePosition(PreviousPosition& previousPos,
                                              Position& position, double alpha)
{
    float interpX = (position.x * alpha) + (previousPos.x * (1.0 - alpha));
    float interpY = (position.y * alpha) + (previousPos.y * (1.0 - alpha));
    return {interpX, interpY};
}

void MovementHelpers::updateVelocity(Movement& movement,
                                     Input::StateArr& inputStates,
                                     double deltaSeconds)
{
    // TODO: Ignoring while velocity is constant for testing.
    ignore(deltaSeconds);

    static constexpr double VELOCITY = 300.145;
    // Handle up/down (favors up).
    if (inputStates[Input::Up] == Input::Pressed) {
        movement.velY = -VELOCITY;
        //        movement.velY -= (acceleration * deltaSeconds);
        //
        //        if (movement.velY < -(movement.maxVelY)) {
        //            movement.velY = -(movement.maxVelY);
        //        }
    }
    else if (inputStates[Input::Down] == Input::Pressed) {
        movement.velY = VELOCITY;
        //        movement.velY += (acceleration * deltaSeconds);
        //
        //        if (movement.velY > movement.maxVelY) {
        //            movement.velY = movement.maxVelY;
        //        }
    }
    else {
        //        // Slow the entity down.
        //        if (movement.velY > 0) {
        //            movement.velY -= (acceleration * deltaSeconds);
        //        }
        //        else if (movement.velY < 0) {
        //            movement.velY += (acceleration * deltaSeconds);
        //        }
        movement.velY = 0;
    }

    // Handle left/right (favors right).
    if (inputStates[Input::Left] == Input::Pressed) {
        movement.velX = -VELOCITY;
        //        movement.velX -= (acceleration * deltaSeconds);
        //
        //        if (movement.velX < -(movement.maxVelX)) {
        //            movement.velX = -(movement.maxVelX);
        //        }
    }
    else if (inputStates[Input::Right] == Input::Pressed) {
        movement.velX = VELOCITY;
        //        movement.velX += (acceleration * deltaSeconds);
        //
        //        if (movement.velX > movement.maxVelX) {
        //            movement.velX = movement.maxVelX;
        //        }
    }
    else {
        //        // Slow the entity down.
        //        if (movement.velX > 0) {
        //            movement.velX -= (acceleration * deltaSeconds);
        //        }
        //        else if (movement.velX < 0) {
        //            movement.velX += (acceleration * deltaSeconds);
        //        }
        movement.velX = 0;
    }
}

} // namespace AM
