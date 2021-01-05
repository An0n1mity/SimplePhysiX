#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#include "scene.h"
#include "physics.h"
#include "camera.h"

int Scene_doubleCapacity(Scene *scene);
int cmp(const void *a, const void *b);
struct array_index
{
  float value;
  int index;
};

Scene *Scene_new(Renderer *renderer)
{
    Camera *camera = NULL;
    Scene *scene = NULL;
    int capacity = 1 << 10;

    int width  = Renderer_getWidth(renderer);
    int height = Renderer_getHeight(renderer);

    scene = (Scene *)calloc(1, sizeof(Scene));
    if (!scene) goto ERROR_LABEL;

    camera = Camera_new(width, height);
    if (!camera) goto ERROR_LABEL;

    scene->m_balls = (Ball *)calloc(capacity, sizeof(Ball));
    if (!scene->m_balls) goto ERROR_LABEL;

    scene->m_camera = camera;
    scene->m_renderer = renderer;
    scene->m_nbBalls = 0;
    scene->m_capacity = capacity;

    return scene;

ERROR_LABEL:
    printf("ERROR - Scene_new()\n");
    return NULL;
}

void Scene_free(Scene *scene)
{
    if (!scene) return;

    Camera_free(scene->m_camera);

    if (scene->m_balls)
    {
        free(scene->m_balls);
    }

    memset(scene, 0, sizeof(Scene));
    free(scene);
}

Renderer *Scene_getRenderer(Scene *scene)
{
    return scene->m_renderer;
}

Camera *Scene_getCamera(Scene *scene)
{
    return scene->m_camera;
}

int Scene_doubleCapacity(Scene *scene)
{
    Ball *newBalls = NULL;
    int newCapacity = scene->m_capacity << 1;

    newBalls = (Ball *)realloc(scene->m_balls, newCapacity * sizeof(Ball));
    if (!newBalls) goto ERROR_LABEL;

    scene->m_balls = newBalls;
    scene->m_capacity = newCapacity;

    return EXIT_SUCCESS;

    ERROR_LABEL:
    printf("ERROR - Scene_doubleCapacity()\n");
    return EXIT_FAILURE;
}

Ball *Scene_addBall(Scene *scene, Vec2 position)
{
    Ball *ball = NULL;

    if (scene->m_nbBalls >= scene->m_capacity)
    {
        int exitStatus = Scene_doubleCapacity(scene);
        if (exitStatus == EXIT_FAILURE) goto ERROR_LABEL;
    }

    ball = &scene->m_balls[scene->m_nbBalls];
    scene->m_nbBalls++;

    *ball = Ball_set(position);

    return ball;

    ERROR_LABEL:
    printf("ERROR - Scene_addBall()\n");
    return NULL;
}

void Scene_removeBall(Scene *scene, Ball *ball)
{
    int nbBalls = Scene_getNbBalls(scene);
    Ball *balls = Scene_getBalls(scene);
    int index = (int)(ball - balls);
    int nbSprings = 0;

    if (index < 0 || index >= nbBalls)
        return;

    // Remove the springs
    nbSprings = ball->nbSprings;
    for (int i = 0; i < nbSprings; ++i)
    {
        Ball_deconnect(ball, ball->springs[i].other);
    }

    Ball *lastBall = &balls[nbBalls - 1];
    if (ball != lastBall)
    {
        // Copy the last ball
        *ball = *lastBall;

        // Update the springs
        ball->nbSprings = 0;
        nbSprings = lastBall->nbSprings;
        for (int i = 0; i < nbSprings; ++i)
        {
            Ball *other = lastBall->springs[i].other;
            float length = lastBall->springs[i].length;
            Ball_deconnect(lastBall, other);
            Ball_connect(ball, other, length);
        }
    }

    // Decrease the number of balls
    scene->m_nbBalls--;
}

int Scene_getNbBalls(Scene *scene)
{
    return scene->m_nbBalls;
}

Ball *Scene_getBalls(Scene *scene)
{
    return scene->m_balls;
}

BallQuery Scene_getNearestBall(Scene *scene, Vec2 position)
{
    int nbBalls = Scene_getNbBalls(scene); // Le nombre de balles sur la scene
    Ball *balls = Scene_getBalls(scene); // Tableau contenant les balles presentent sur la scene
    BallQuery query = { 0 };
    float minimal_distance = 1000.f, distance_from_cursor;

    // TODO - Compléter la fonction

    // Pour chaque balles dans la scene
    for(int i = 0; i < nbBalls; i++)
    {
        distance_from_cursor = Vec2_distance(balls[i].position, position);

        if(distance_from_cursor < minimal_distance)
        {
            minimal_distance = distance_from_cursor;
            query.ball = &balls[i]; // Pointe vers la balle la plus proche du curseur dans le tableau de balles
            query.distance = distance_from_cursor; // Stocke la distance de la balle par rapport au curseur
        }
    }

    return query;
}

int Scene_getNearestBalls(Scene *scene, Vec2 position, BallQuery *queries, int nbQueries)
{
    int nbBalls = Scene_getNbBalls(scene);
    Ball *balls = Scene_getBalls(scene);
    struct array_index array[nbBalls];

    // TODO - Compléter la fonction

    // Tableau contenant la distance de chaque balle par rapport au curseur
    float *distances_from_cursor = malloc(nbBalls * sizeof(float));

    for (int i = 0; i < nbBalls; i++)
    {
        //Calcul la distance de chaque balle par rapport au curseur
        distances_from_cursor[i] = Vec2_distance(position, balls[i].position);
    }

   for(int i=0; i < nbBalls; i++){
       array[i].value = distances_from_cursor[i];
       array[i].index = i ;
   }

   qsort(array, nbBalls, sizeof(array[0]), cmp);

   for(int i = 0; i < nbQueries; i++){
       queries[i].ball = &balls[array[i].index];
       printf("%d\n", array[0].index);
   }
return EXIT_SUCCESS;
}

// Pour la fonction qsort
int cmp(const void *a, const void *b)
{
    struct array_index *a1 = (struct array_index *)a;
    struct array_index *a2 = (struct array_index *)b;
    if ((*a1).value < (*a2).value)
        return -1;
    else if ((*a1).value > (*a2).value)
        return 1;
    else
        return 0;
}

void Scene_updateBalls(Scene *scene, float timeStep)
{
    int nbBalls = Scene_getNbBalls(scene);
    Ball *balls = Scene_getBalls(scene);

    for (int i = 0; i < nbBalls; i++)
    {
        Ball_updateVelocity(&balls[i], timeStep);
    }

    for (int i = 0; i < nbBalls; i++)
    {
        Ball_updatePosition(&balls[i], timeStep);
    }
}

void Scene_renderBalls(Scene *scene)
{
    int nbBalls = Scene_getNbBalls(scene);
    Ball *balls = Scene_getBalls(scene);
    Camera *camera = Scene_getCamera(scene);
    Renderer *renderer = Scene_getRenderer(scene);

    for (int i = 0; i < nbBalls; i++)
    {
        Ball *ball = &balls[i];
        int nbSprings = ball->nbSprings;
        int x1, y1;
        Camera_worldToView(camera, ball->position, &x1, &y1);

        for (int j = 0; j < nbSprings; j++)
        {
            Ball *other = ball->springs[j].other;
            int x2, y2;

            Camera_worldToView(camera, other->position, &x2, &y2);
            Renderer_drawLine(renderer, x1, y1, x2, y2, Color_set(105, 220, 255, 255));
        }

        Renderer_drawPoint(renderer, x1, y1, Color_set(255, 128, 128, 255));
    }
}
