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

//****************************** Creation des la plateforme de base ******************************//

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

//************************************************************************************************//

    float timeStep = 1.f / 100.f;
    float accumulator = 0.f;

    Timer_start(timer);
    int quitLoop = 0;

    Vec2 cursor_position;
    int x, y, nbBalls, n_nearest_ball = 2;
    float Score = 0, scene_balls_y = 0.f;
    Ball *scene_balls;
    BallQuery nearest_cursor_ball;

    while (!quitLoop)
    {

        BallQuery nearest_cursor_balls[n_nearest_ball];
        SDL_Event evt;
        int mouseClickLeft = 0;
        int mouseClickRight = 0;
        nbBalls = Scene_getNbBalls(scene);
        Timer_update(timer);

//*************************************** Obtenir le score ***************************************//

        // On recupere les balles de la scene dans un tableau
        scene_balls = Scene_getBalls(scene);

        // On cherche la balle ayant l'ordonné le plus élevé
        for (int i = 0; i < nbBalls; i++)
        {
            scene_balls_y = floor(100*scene_balls[i].position.y)/100;
            if(scene_balls_y > Score)
            {
                Score = scene_balls_y ;
                printf("Nouveau record : %.2f m (%d boules) \n", Score, nbBalls);
            }
        }

//************************************************************************************************//

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

//************************************* Mouvement de caméra **************************************//

                switch (scanCode)
                {
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

//************************************************************************************************//

                case SDL_SCANCODE_ESCAPE:
                quitLoop = 1;
                break;

//************************ Change le nombre de n balles les plus proches *************************//

                case SDL_SCANCODE_Q:
                    n_nearest_ball ++;
                    if(n_nearest_ball > nbBalls)
                        n_nearest_ball = nbBalls;
                    break;
                case SDL_SCANCODE_W:
                    n_nearest_ball --;
                    if(n_nearest_ball < 1)
                        n_nearest_ball = 1;
                    break;

//************************************************************************************************//

//*********** [RESTART GAME BONUS] Récupère toutes les balles de la scene et les retire **********//

                case SDL_SCANCODE_BACKSPACE:

                    // Récupère le nombre de balles présentent sur la scène
                    nbBalls = Scene_getNbBalls(scene);

                    // Récupère le tableau de ces balles
                    Ball *balls_to_remove = Scene_getBalls(scene);

                        // Retire toute les balles de la scène
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

                        printf("------- Nouvelle Partie -------\n");

                    break;

//************************************************************************************************//

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
                    mouseClickLeft = 1;
                if (mouseButton.button == SDL_BUTTON_RIGHT)
                    mouseClickRight = 1;
                break;
            }

//********************************* Ajout de balle dans la scene *********************************//

            if(mouseClickLeft)
            {
                // On crée une balle au niveau du curseur
                Ball *ball = Scene_addBall(scene, Vec2_set(mousePos.x, mousePos.y));

                // Si trop de ressort relié à la balle la plus proche
                for (int i = 0; i < n_nearest_ball; i++)
                {
                    // Si trop de ressort attaché à la balle
                    if(Ball_connect(ball, nearest_cursor_balls[i].ball, 1))
                        // Retirer la nouvelle balle
                        Scene_removeBall(scene, ball);
                }
            }
        }

//************************************************************************************************//

//************************************Retire la balle la plus proche******************************//
            if (mouseClickRight) {
                nearest_cursor_ball = Scene_getNearestBall(scene, cursor_position);
                Scene_removeBall(scene, nearest_cursor_ball.ball);
            }
//**************************************************************************************************//
        if (quitLoop)
            break;

        // Clear the previous frame
        Renderer_clear(renderer);

//************************************** Liaison des balles **************************************//

        // Get the mouse position
        Camera_viewToWorld(camera, mouseX, mouseY, &mousePos);

        // Recupère la position du curseur dans le monde
        cursor_position.x = mousePos.x;
        cursor_position.y = mousePos.y;

        // Récupère les n balles les plus proche du curseur

        Scene_getNearestBalls(scene, cursor_position, nearest_cursor_balls, n_nearest_ball);

        for (int i = 0; i < n_nearest_ball; i++)
        {
            // Transforme les coordonnées des balles les plus proches en pixels
            Camera_worldToView(camera, nearest_cursor_balls[i].ball->position, &x, &y);

            // Dessine une ligne entre le curseur et les balles les plus proches
            nearest_cursor_ball = Scene_getNearestBall(scene, cursor_position);
            if(Vec2_distance(cursor_position, nearest_cursor_ball.ball->position) < 1.f)
                Renderer_drawLine(renderer, mouseX, mouseY, x, y, Color_set(255, 221, 51, 255));
        }

//************************************************************************************************//

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
