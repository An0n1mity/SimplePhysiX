#include <stdio.h>
#include <stdlib.h>

#include "renderer.h"
#include "window.h"
#include "physics.h"
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

    // Creation des balles
    Ball *ball = Scene_addBall(scene, Vec2_set(4.0f, 1.0f));
    Ball *ball_2 = Scene_addBall(scene, Vec2_set(6.0f, 1.0f));
    Ball *ball_3 = Scene_addBall(scene, Vec2_set(5.0f, 2.0f));

    // Propriétés physique des balles
    ball->velocity = Vec2_set(0.f, 0.f);
    ball_2->velocity = Vec2_set(0.f, 0.f);
    ball_3->velocity = Vec2_set(0.f, 0.f);

    // Connexion des balles
    Ball_connect(ball, ball_2, 1);
    Ball_connect(ball_3, ball_2, 1);
    Ball_connect(ball, ball_3, 1);

    float timeStep = 1.f / 100.f;
    float accumulator = 0.f;

    Timer_start(timer);
    int quitLoop = 0;

    Vec2 cursor_position;
    BallQuery neareast_cursor_ball;
    int x, y;

    while (!quitLoop)
    {

        SDL_Event evt;
        int mouseClick = 0;
        Timer_update(timer);

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
                case SDL_SCANCODE_ESCAPE:
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

            if(mouseClick){
              Ball *ball = Scene_addBall(scene, Vec2_set(mousePos.x, mousePos.y));
              Ball *neareast_ball = Scene_addBall(scene, Vec2_set(neareast_cursor_ball.ball->position.x, neareast_cursor_ball.ball->position.y));
              Ball_connect(ball, neareast_cursor_ball.ball, 1);
            }
        }

        if (quitLoop)
            break;

        // Clear the previous frame
        Renderer_clear(renderer);

        // Get the mouse position
        Camera_viewToWorld(camera, mouseX, mouseY, &mousePos);

        // Recupere la position du curseur dans le monde
        cursor_position.x = mousePos.x;
        cursor_position.y = mousePos.y;

        // Recupere la balle la plus proche du curseur
        neareast_cursor_ball = Scene_getNearestBall(scene, cursor_position);
        // Transforme les coordonnées de la balle de metres vers pixels
        Camera_worldToView(camera, neareast_cursor_ball.ball->position, &x, &y);

        // Update the physics engine
        accumulator += Timer_getDelta(timer);
        while (accumulator >= timeStep)
        {
            Scene_updateBalls(scene, timeStep);
            accumulator -= timeStep;
        }

        // Render the scene
        Renderer_drawLine(renderer, mouseX, mouseY, x, y, Color_set(255, 255, 255, 255));
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
    return EXIT_FAILURE;
}
