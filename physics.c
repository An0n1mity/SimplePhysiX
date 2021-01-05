
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "physics.h"

Ball Ball_set(Vec2 position)
{
    Ball ball = { 0 };

    ball.position = position;
    ball.velocity = Vec2_set(0.f, 0.f);
    ball.mass = 0.5f;
    ball.friction = 1.5f;
    ball.is_static = false;

    return ball;
}

int Ball_connect(Ball *ball1, Ball *ball2, float length)
{
    if ((ball1 == ball2) || (ball1->nbSprings >= MAX_EDGES)|| (ball2->nbSprings >= MAX_EDGES))
        return EXIT_FAILURE;

    ball1->springs[ball1->nbSprings].other = ball2;
    ball2->springs[ball2->nbSprings].other = ball1;
    ball1->springs[ball1->nbSprings].length = length;
    ball2->springs[ball2->nbSprings].length = length;

    ball1->nbSprings++;
    ball2->nbSprings++;

    return EXIT_SUCCESS;
}

int Ball_deconnect(Ball *ball1, Ball *ball2)
{
    int nbSprings1 = ball1->nbSprings;
    int nbSprings2 = ball2->nbSprings;
    int exitStatus = EXIT_SUCCESS;
    int i;

    // Remove ball2 from the egdes of ball1
    for (i = 0; i < nbSprings1; ++i)
    {
        if (ball1->springs[i].other == ball2)
        {
            ball1->springs[i] = ball1->springs[nbSprings1 - 1];
            ball1->nbSprings--;
            break;
        }
    }
    if (i >= nbSprings1)
    {
        exitStatus = EXIT_FAILURE;
    }

    // Remove ball1 from the egdes of ball2
    for (i = 0; i < nbSprings2; ++i)
    {
        if (ball2->springs[i].other == ball1)
        {
            ball2->springs[i] = ball2->springs[nbSprings2 - 1];
            ball2->nbSprings--;
            break;
        }
    }
    if (i >= nbSprings2)
    {
        exitStatus = EXIT_FAILURE;
    }

    return exitStatus;
}

void Ball_updateVelocity(Ball *ball, float timeStep)
{
  //Données de la balle
  float mass = ball->mass;
  float friction = ball->friction;
  Vec2 velocity = ball->velocity;
  Vec2 position = ball->position;
  Vec2 other;
  int nb_springs = ball->nbSprings;
  //***************************Calcul des forces appliqués****************************//
  // Le poids
  Vec2 gravity = Vec2_set(0, -9.81);
  Vec2 weight = Vec2_scale(gravity, mass);

  // La friction
  Vec2 Friction = Vec2_scale(velocity, friction * -1);
  Vec2 Rappel = Vec2_set(0.f, 0.f);

  // Le rappel en fonction du nombre de balles ratachés
  for (int i = 0; i < nb_springs; i++) {
    other = ball->springs[i].other->position;
    Vec2 vector_BalleAttach = Vec2_sub(other, position);
    float l = Vec2_norm(vector_BalleAttach);
    float l_0 = 1.f; // Longueur ressort au repos
    float k = 300; // Raideur du ressort
    Vec2 vector_I = Vec2_normalize(vector_BalleAttach);
    Rappel = Vec2_add(Rappel, Vec2_scale(vector_I, k*(l-l_0)));
  }

  // La somme des force
  Vec2 somme_forces = Vec2_add(weight, Friction);
  somme_forces = Vec2_add(somme_forces, Rappel);

  //*************************Calcule de la vitesse associé à la balle*******************//
  //Calcul de l'acceleration de la balle
  Vec2 acceleration = Vec2_scale(somme_forces, 1/mass);

  //Calcul de la vitesse de la balle
  Vec2 Velocity = Vec2_add(velocity, Vec2_scale(acceleration, timeStep));

  //Update de la balle
  if (ball->is_static)
    ball->velocity = Vec2_set(0.f, 0.f);
  else
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

  //Detection de colision avec le sol
  if(Position.y < 1){
  	ball->velocity.y *= -1;
  	Position.y = 1.f;
  }

  //Update de la balle
  ball->position = Position;
}
