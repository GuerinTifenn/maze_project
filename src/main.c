#include "/opt/homebrew/include/SDL2/SDL.h"
#include <stdio.h>
#include <math.h>

// Définition de la carte (un simple tableau 2D)
int map[5][5] = {
    {1, 1, 1, 1, 1},
    {1, 0, 0, 0, 1},
    {1, 0, 1, 0, 1},
    {1, 0, 0, 0, 1},
    {1, 1, 1, 1, 1}
};

int main(int argc, char *argv[]) {
    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_Rect floorRect;
    double posX, posY, dirX, dirY, planeX, planeY;
    int x;
    SDL_Event event;
    int quit = 0;

    (void)argc;
    (void)argv;

    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        printf("Erreur lors de l'initialisation de SDL: %s\n", SDL_GetError());
        return 1;
    }

    window = SDL_CreateWindow("Maze Project",
                              SDL_WINDOWPOS_CENTERED,
                              SDL_WINDOWPOS_CENTERED,
                              800, 600,
                              SDL_WINDOW_SHOWN);
    if (!window) {
        printf("Erreur lors de la création de la fenêtre: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_SOFTWARE);
    if (!renderer) {
        printf("Erreur lors de la création du renderer: %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    // Initialiser manuellement le rectangle pour le sol
    floorRect.x = 0;
    floorRect.y = 300;
    floorRect.w = 800;
    floorRect.h = 300;

    // Définir la couleur de fond pour le plafond (bleu) et le sol (vert foncé)
    SDL_SetRenderDrawColor(renderer, 35, 165, 255, 255);  // Plafond bleu
    SDL_RenderClear(renderer);

    SDL_SetRenderDrawColor(renderer, 34, 139, 34, 255);  // Sol vert foncé
    SDL_RenderFillRect(renderer, &floorRect);

    posX = 2.5;
    posY = 2.5;
    dirX = -1.0;
    dirY = 0.0;
    planeX = 0.0;
    planeY = 0.66;

    for (x = 0; x < 800; x++) {
        double cameraX, rayDirX, rayDirY;
        int mapX, mapY;
        double sideDistX, sideDistY;
        double deltaDistX, deltaDistY, perpWallDist;
        int stepX, stepY;
        int hit = 0;
        int side;
        int lineHeight, drawStart, drawEnd;

        cameraX = 2 * x / (double)800 - 1;
        rayDirX = dirX + planeX * cameraX;
        rayDirY = dirY + planeY * cameraX;

        mapX = (int)posX;
        mapY = (int)posY;

        deltaDistX = (rayDirY == 0) ? 0 : ((rayDirX == 0) ? 1 : fabs(1 / rayDirX));
        deltaDistY = (rayDirX == 0) ? 0 : ((rayDirY == 0) ? 1 : fabs(1 / rayDirY));

        if(rayDirX < 0) {
            stepX = -1;
            sideDistX = (posX - mapX) * deltaDistX;
        } else {
            stepX = 1;
            sideDistX = (mapX + 1.0 - posX) * deltaDistX;
        }

        if(rayDirY < 0) {
            stepY = -1;
            sideDistY = (posY - mapY) * deltaDistY;
        } else {
            stepY = 1;
            sideDistY = (mapY + 1.0 - posY) * deltaDistY;
        }

        while (hit == 0) {
            if(sideDistX < sideDistY) {
                sideDistX += deltaDistX;
                mapX += stepX;
                side = 0;
            } else {
                sideDistY += deltaDistY;
                mapY += stepY;
                side = 1;
            }
            if(map[mapX][mapY] > 0) hit = 1;
        }

        if(side == 0) perpWallDist = (mapX - posX + (1 - stepX) / 2) / rayDirX;
        else          perpWallDist = (mapY - posY + (1 - stepY) / 2) / rayDirY;

        lineHeight = (int)(600 / perpWallDist);

        drawStart = -lineHeight / 2 + 600 / 2;
        if(drawStart < 0) drawStart = 0;
        drawEnd = lineHeight / 2 + 600 / 2;
        if(drawEnd >= 600) drawEnd = 600 - 1;

        // Couleur des murs gris
        SDL_SetRenderDrawColor(renderer, 169, 169, 169, 255);  // Gris clair pour les murs
        if(side == 1) { SDL_SetRenderDrawColor(renderer, 105, 105, 105, 255); }  // Gris foncé si le mur est sur un côté

        SDL_RenderDrawLine(renderer, x, drawStart, x, drawEnd);
    }

    SDL_RenderPresent(renderer);

    while (!quit) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                quit = 1;
            }
        }

        SDL_RenderPresent(renderer);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
