
#include "renderer.h"

#ifdef _WIN32
#include <SDL.h>
#else
#include <SDL2/SDL.h>
#endif

#include <stdio.h>
#include <stdlib.h>

int Renderer_getWidth(Renderer *renderer)
{
    return renderer->m_width;
}

int Renderer_getHeight(Renderer *renderer)
{
    return renderer->m_height;
}

void Renderer_clear(Renderer* renderer)
{
    SDL_Renderer* rendererSDL = renderer->m_rendererSDL;
    SDL_SetRenderDrawColor(rendererSDL, (Uint8)0, (Uint8)0, (Uint8)0, (Uint8)255);
    SDL_RenderClear(rendererSDL);
}

void Renderer_fill(Renderer *renderer, Color color)
{
    SDL_Rect rect;
    SDL_Renderer* rendererSDL = renderer->m_rendererSDL;

    rect.x = 0;
    rect.y = 0;
    rect.w = renderer->m_width;
    rect.h = renderer->m_height;

    SDL_SetRenderDrawBlendMode(rendererSDL, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(rendererSDL, (Uint8)color.r, (Uint8)color.g, (Uint8)color.b, (Uint8)color.a);
    SDL_RenderFillRect(rendererSDL, &rect);
}

void Renderer_drawPoint(Renderer *renderer, int x, int y, Color color)
{
    SDL_Renderer* rendererSDL = renderer->m_rendererSDL;

    SDL_SetRenderDrawBlendMode(rendererSDL, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(rendererSDL, (Uint8)color.r, (Uint8)color.g, (Uint8)color.b, (Uint8)color.a);
    SDL_RenderDrawLine(rendererSDL, x - 5, y, x + 5, y);
    SDL_RenderDrawLine(rendererSDL, x, y - 5, x, y + 5);
}

void Renderer_drawLine(Renderer *renderer, int x1, int y1, int x2, int y2, Color color)
{
    SDL_Renderer* rendererSDL = renderer->m_rendererSDL;

    SDL_SetRenderDrawBlendMode(rendererSDL, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(rendererSDL, (Uint8)color.r, (Uint8)color.g, (Uint8)color.b, (Uint8)color.a);
    SDL_RenderDrawLine(rendererSDL, x1, y1, x2, y2);
}

Color Color_set(int r, int g, int b, int a)
{
    Color color;
    color.r = (uint8_t)r;
    color.g = (uint8_t)g;
    color.b = (uint8_t)b;
    color.a = (uint8_t)a;

    return color;
}

void Renderer_update(Renderer* renderer)
{
    SDL_RenderPresent(renderer->m_rendererSDL);
}