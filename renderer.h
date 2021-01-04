
#ifndef _RENDERER_H_
#define _RENDERER_H_

/// @file renderer.h
/// @defgroup Renderer
/// @{

#include <stdint.h>
#ifdef _WIN32
#include <SDL.h>
#else
#include <SDL2/SDL.h>
#endif

/// @brief Structure représentant la moteur de rendu.
typedef struct Renderer_s
{
    /// @brief Le moteur de rendu SDL.
    SDL_Renderer *m_rendererSDL;

    /// @brief La largeur en pixels de l'image rendue.
    int m_width;
    
    /// @brief La hauteur en pixels de l'image rendue.
    int m_height;
} Renderer;

/// @brief Structure représentant une couleur au format RGBA.
typedef struct Color_s
{
    /// @brief L'intensité de la composante rouge entre 0 et 255.
    uint8_t r;

    /// @brief L'intensité de la composante verte entre 0 et 255.
    uint8_t g;

    /// @brief L'intensité de la composante bleue entre 0 et 255.
    uint8_t b;

    /// @brief La composante alpha (transparence) de la couleur.
    /// La couleur est parfaitement transparante quand elle vaut 0 et elle est opaque quand elle vaut 255.
    uint8_t a;
} Color;

/// @brief Construit une couleur à partir de ses composantes [R,V,B,A] chacune entre 0 et 255.
/// @param[in] r l'intensité du rouge.
/// @param[in] g l'intensité du vert.
/// @param[in] b l'intensité du bleu.
/// @param[in] a la composante alpha (transparence) de la couleur.
/// La couleur est parfaitement transparante quand elle vaut 0 et elle est opaque quand elle vaut 255.
/// @return La couleur spécifiée.
Color Color_set(int r, int g, int b, int a);

/// @brief Met à jour le moteur de rendu.
/// Cette fonction permet de d'afficher à l'écran la dernière image rendue.
/// @param[in,out] renderer le moteur de rendu.
void Renderer_update(Renderer* renderer);

/// @brief Efface le rendu précédent. Le rendu devient entièrement noir.
/// @param[in,out] renderer le moteur de rendu.
void Renderer_clear(Renderer* renderer);

/// @brief Remplit le rendu avec une couleur donnée.
/// @param[in,out] renderer le moteur de rendu.
/// @param[in] color la couleur de remplissage.
void Renderer_fill(Renderer* renderer, Color color);

/// @brief Dessine un segment spécifié par deux points sur le rendu.
/// @param[in,out] renderer le moteur de rendu.
/// @param[in] x1 abscisse du premier point (en pixels).
/// @param[in] y1 ordonnée du premier point (en pixels).
/// @param[in] x2 abscisse du second point (en pixels).
/// @param[in] y2 ordonnée du second point (en pixels).
/// @param[in] color la couleur de la ligne.
void Renderer_drawLine(Renderer *renderer, int x1, int y1, int x2, int y2, Color color);

/// @brief Dessine un point sur le rendu.
/// @param[in,out] renderer le moteur de rendu.
/// @param[in] x abscisse du point (en pixels).
/// @param[in] y ordonnée du point (en pixels).
/// @param[in] color la couleur du point.
void Renderer_drawPoint(Renderer *renderer, int x, int y, Color color);

/// @brief Renvoie la largeur du moteur de rendu.
/// @param[in,out] renderer le moteur de rendu.
/// @return La largeur du moteur de rendu.
int Renderer_getWidth(Renderer *renderer);

/// @brief Renvoie la hauteur du moteur de rendu.
/// @param[in,out] renderer le moteur de rendu.
/// @return La hauteur du moteur de rendu.
int Renderer_getHeight(Renderer *renderer);

#endif
