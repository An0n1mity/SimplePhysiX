
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "physics.h"

void Ball_updateVelocity(Ball *ball, float timeStep)
{
  //Données de la balle
  float mass = ball->mass;
  float friction = ball->friction;
  Vec2 velocity = ball->velocity;
  Vec2 position = ball->position;

  //Calcul des forces appliqués
  Vec2 gravity = Vec2_set(0, -9.81);
  Vec2 weight = Vec2_scale(gravity, mass);
  Vec2 Friction = Vec2_scale(velocity, friction * -1);
  Vec2 somme_forces = Vec2_add(weight, Friction);

  //Calcul de l'acceleration de la balle
  Vec2 acceleration = Vec2_scale(somme_forces, 1/mass);

  //Calcul de la vitesse de la balle
  Vec2 Velocity = Vec2_add(velocity, Vec2_scale(acceleration, timeStep));

  //Update de la balle
  ball->velocity = Velocity;

}

void Ball_updatePosition(Ball *ball, float timeStep)
{
  Ball_updateVelocity(ball, timeStep);

  //Données de la balle
  Vec2 velocity = ball->velocity;
  Vec2 position = ball->position;

  //Calcul de la position de la balle
  Vec2 Position = Vec2_add(position, Vec2_scale(velocity, timeStep));

  if (Position.y < 0){
    Position.y = 0;
  }

  //Update de la balle
  ball->position = Position;
}
