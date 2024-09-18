#include "/opt/homebrew/include/SDL2/SDL.h"
#include <stdio.h>
#include <math.h>

// Définition de la carte (un simple tableau 2D)
int map[10][10] = {
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 1, 0, 1, 1, 1, 0, 0, 1},
    {1, 0, 1, 0, 0, 0, 1, 0, 0, 1},
    {1, 0, 1, 1, 1, 0, 1, 1, 0, 1},
    {1, 0, 0, 0, 1, 0, 0, 0, 0, 1},
    {1, 1, 1, 0, 1, 1, 1, 0, 1, 1},
    {1, 0, 0, 0, 0, 0, 1, 0, 0, 1},
    {1, 0, 1, 1, 1, 0, 0, 0, 0, 1},
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1}
};

void renderScene(SDL_Renderer *renderer, double posX, double posY, double dirX, double dirY, double planeX, double planeY) {
    int x;
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
}

int main(int argc, char *argv[]) {
    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_Rect floorRect;
    double posX, posY, dirX, dirY, planeX, planeY;
    SDL_Event event;
    int quit = 0;
    double moveSpeed = 0.5;  // Ajustement de la vitesse de déplacement
    double rotSpeed = 0.1;  // Ajustement de la vitesse de rotation

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

    // Position initiale du joueur
    posX = 2.5;
    posY = 2.5;
    dirX = -1.0;
    dirY = 0.0;
    planeX = 0.0;
    planeY = 0.66;

    while (!quit) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                quit = 1;
            }

            if (event.type == SDL_KEYDOWN) {
                double oldDirX, oldPlaneX;

                switch(event.key.keysym.sym) {
                    case SDLK_RIGHT:  // Tourner à droite
                        oldDirX = dirX;
                        dirX = dirX * cos(-rotSpeed) - dirY * sin(-rotSpeed);
                        dirY = oldDirX * sin(-rotSpeed) + dirY * cos(-rotSpeed);
                        oldPlaneX = planeX;
                        planeX = planeX * cos(-rotSpeed) - planeY * sin(-rotSpeed);
                        planeY = oldPlaneX * sin(-rotSpeed) + planeY * cos(-rotSpeed);
                        break;
                    case SDLK_LEFT:  // Tourner à gauche
                        oldDirX = dirX;
                        dirX = dirX * cos(rotSpeed) - dirY * sin(rotSpeed);
                        dirY = oldDirX * sin(rotSpeed) + dirY * cos(rotSpeed);
                        oldPlaneX = planeX;
                        planeX = planeX * cos(rotSpeed) - planeY * sin(rotSpeed);
                        planeY = oldPlaneX * sin(rotSpeed) + planeY * cos(rotSpeed);
                        break;
                    case SDLK_z:  // Avancer
                        // Gestion des collisions avec une marge de 0.1 pour éviter de "coller" les murs
                        if(map[(int)(posX + dirX * moveSpeed * 2)][(int)posY] == 0) posX += dirX * moveSpeed;
                        if(map[(int)posX][(int)(posY + dirY * moveSpeed * 2)] == 0) posY += dirY * moveSpeed;
                        break;
                    case SDLK_s:  // Reculer
                        // Gestion des collisions avec une marge de 0.1 pour reculer
                        if(map[(int)(posX - dirX * moveSpeed * 2)][(int)posY] == 0) posX -= dirX * moveSpeed;
                        if(map[(int)posX][(int)(posY - dirY * moveSpeed * 2)] == 0) posY -= dirY * moveSpeed;
                        break;
                    case SDLK_q:  // Déplacer à gauche
                        // Gestion des collisions pour déplacement à gauche
                        if(map[(int)(posX - dirY * moveSpeed * 2)][(int)posY] == 0) posX -= dirY * moveSpeed;
                        if(map[(int)posX][(int)(posY + dirX * moveSpeed * 2)] == 0) posY += dirX * moveSpeed;
                        break;
                    case SDLK_d:  // Déplacer à droite
                        // Gestion des collisions pour déplacement à droite
                        if(map[(int)(posX + dirY * moveSpeed * 2)][(int)posY] == 0) posX += dirY * moveSpeed;
                        if(map[(int)posX][(int)(posY - dirX * moveSpeed * 2)] == 0) posY -= dirX * moveSpeed;
                        break;
                }
            }
        }

        // Redessiner le sol et le plafond
        SDL_SetRenderDrawColor(renderer, 35, 165, 255, 255);  // Plafond bleu
        SDL_RenderClear(renderer);
        SDL_SetRenderDrawColor(renderer, 34, 139, 34, 255);  // Sol vert
        SDL_RenderFillRect(renderer, &floorRect);

        // Redessiner la scène avec la nouvelle position
        renderScene(renderer, posX, posY, dirX, dirY, planeX, planeY);

        SDL_RenderPresent(renderer);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
