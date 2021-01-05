#include <stdio.h>
#include <stdlib.h>

#include "renderer.h"
#include "window.h"
#include "physics_bonus.h"
#include "timer.h"
#include "camera.h"
#include "scene.h"

int main(int argc, char *argv[])
{
    Window *window = NULL;
    Renderer *renderer = NULL;
    Timer *timer = NULL;
    Scene *scene = NULL;
    int exitStatus;

    exitStatus = SDL_init();
    if (exitStatus == EXIT_FAILURE) goto ERROR_LABEL;

    window = Window_new(WINDOW_WIDTH, WINDOW_HEIGHT);
    if (!window) goto ERROR_LABEL;

    renderer = Window_getRenderer(window);

    timer = Timer_new();
    if (!timer) goto ERROR_LABEL;

    scene = Scene_new(renderer);
    if (!scene) goto ERROR_LABEL;

    Camera *camera = Scene_getCamera(scene);
    int mouseX = 0;
    int mouseY = 0;
    Vec2 mousePos = Vec2_set(0.f, 0.f);

    //************************************Creation des la plateforme de base********************************
    Ball *ball = Scene_addBall(scene, Vec2_set(7.5f, 1.0f));
    Ball *ball_2 = Scene_addBall(scene, Vec2_set(8.0f, 1.86f));
    Ball *ball_3 = Scene_addBall(scene, Vec2_set(8.5f, 1.0f));

    // Connexion des balles
    Ball_connect(ball, ball_2, 1);
    Ball_connect(ball_3, ball_2, 1);
    Ball_connect(ball, ball_3, 1);

    ball->is_static = true;
    ball_2->is_static = true;
    ball_3->is_static = true;


    //********************************************************************************************************

    float timeStep = 1.f / 100.f;
    float accumulator = 0.f;

    Timer_start(timer);
    int quitLoop = 0;

    Vec2 cursor_position;
    BallQuery nearest_cursor_balls[2];
    int x, y, nbBalls;
    float Score=0;

    while (!quitLoop)
    {

        SDL_Event evt;
        int mouseClick = 0;
        Timer_update(timer);

        //************************************************ Obtenir le score ************************************
        nbBalls = Scene_getNbBalls(scene);
        // On recupere les balles de la scene dans un tableau
        Ball *scene_balls = Scene_getBalls(scene);
        // On cherche la balle ayant l'ordonné le plus élevé
        for (int i = 0; i < nbBalls; i++) {
            if(scene_balls[i].position.y > Score){
                Score = scene_balls[i].position.y ;
            }
        }
        //Afficher le Score
        printf("Score : %f\n", Score);

        while (SDL_PollEvent(&evt))
        {
            SDL_Scancode scanCode;
            SDL_MouseButtonEvent mouseButton;

            switch (evt.type)
            {
            case SDL_QUIT:
                quitLoop = 1;
                break;

            case SDL_KEYDOWN:
                scanCode = evt.key.keysym.scancode;
                if (evt.key.repeat)
                    break;

                switch (scanCode)
                {
                //************************************ Mouvement de caméra*******************************************
                case SDL_SCANCODE_UP:
                    Camera_move(camera, Vec2_set(0.f, 1.f));
                    break;
                case SDL_SCANCODE_DOWN:
                    Camera_move(camera, Vec2_set(0.f, -1.f));
                    break;
                case SDL_SCANCODE_RIGHT:
                    Camera_move(camera, Vec2_set(1.f, 0.f));
                    break;
                case SDL_SCANCODE_LEFT:
                    Camera_move(camera, Vec2_set(-1.f, 0.f));
                    break;

                case SDL_SCANCODE_ESCAPE:
                case SDL_SCANCODE_BACKSPACE:
                    //*******************Recupere toutes les balles de la scene et les retirés ***********************
                    // Recupere le nombre de balles presentent sur la scene
                    nbBalls = Scene_getNbBalls(scene);
                    // Recupere le tableau de ces balles
                    Ball *balls_to_remove = Scene_getBalls(scene);
                        // Retire toute les balles de la scene
                        for (int i = 0; i < nbBalls; i++) {
                            Scene_removeBall(scene, &balls_to_remove[0]);
                        }
                        // Rajoute les balles de base
                        Ball *ball = Scene_addBall(scene, Vec2_set(7.5f, 1.0f));
                        Ball *ball_2 = Scene_addBall(scene, Vec2_set(8.0f, 1.86f));
                        Ball *ball_3 = Scene_addBall(scene, Vec2_set(8.5f, 1.0f));

                        // Connexion des balles
                        Ball_connect(ball, ball_2, 1);
                        Ball_connect(ball_3, ball_2, 1);
                        Ball_connect(ball, ball_3, 1);

                        ball->is_static = true;
                        ball_2->is_static = true;
                        ball_3->is_static = true;

                    break;
                case SDL_SCANCODE_RETURN:
                    quitLoop = 1;
                    break;
                }

                break;

            case SDL_KEYUP:
                scanCode = evt.key.keysym.scancode;

                if (evt.key.repeat)
                    break;

                break;

            case SDL_MOUSEMOTION:
                mouseX = evt.motion.x;
                mouseY = evt.motion.y;
                break;

            case SDL_MOUSEBUTTONDOWN:
                mouseButton = evt.button;

                if (mouseButton.button == SDL_BUTTON_LEFT)
                    mouseClick = 1;
                break;
            }

            // Action du click gauche
            if(mouseClick)
            {
                // On crée une balle au niveau du curseur
                Ball *ball = Scene_addBall(scene, Vec2_set(mousePos.x, mousePos.y));

                // Si trop de ressort relié à la balle la plus proche
                for (int i = 0; i < 2; i++) {
                    // Si trop de ressort attaché à la balle
                    if(Ball_connect(ball, nearest_cursor_balls[i].ball, Vec2_distance(cursor_position, nearest_cursor_balls[i].ball->position)))
                        // Retirer la nouvelle balle
                        Scene_removeBall(scene, ball);
                }
            }
        }

        if (quitLoop)
            break;

        // Clear the previous frame
        Renderer_clear(renderer);

        // Get the mouse position
        Camera_viewToWorld(camera, mouseX, mouseY, &mousePos);

        // Recupère la position du curseur dans le monde
        cursor_position.x = mousePos.x;
        cursor_position.y = mousePos.y;

        // Récupère les n balles les plus proche du curseur
        Scene_getNearestBalls(scene, cursor_position, nearest_cursor_balls, 2);

        for (int i = 0; i < 2; i++) {
            // Transforme les coordonnées des balles les plus proches en pixels
            Camera_worldToView(camera, nearest_cursor_balls[i].ball->position, &x, &y);
            // Dessine une ligne entre le curseur et les balles les plus proches
            Renderer_drawLine(renderer, mouseX, mouseY, x, y, Color_set(255, 221, 51, 255));
        }

        // Update the physics engine
        accumulator += Timer_getDelta(timer);
        while (accumulator >= timeStep)
        {
            Scene_updateBalls(scene, timeStep);
            accumulator -= timeStep;
            }

        // Render the scene
        Scene_renderBalls(scene);
        Renderer_update(renderer);
    }

    // Free the memory
    Scene_free(scene);
    scene = NULL;
    camera = NULL;
    Window_free(window);
    window = NULL;
    Scene_free(scene);
    scene = NULL;
    Timer_free(timer);
    timer = NULL;

    SDL_quit();

    return EXIT_SUCCESS;

ERROR_LABEL:
    printf("ERROR - main()\n");
    Window_free(window);
    Scene_free(scene);
    Timer_free(timer);

    SDL_quit();
}
