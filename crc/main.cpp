// Paso 1 - un cuadrado que se mueve por una cuadrícula.
// En PC se mueve con las flechas del teclado; en el móvil, deslizando el dedo.
#include <SDL.h>

const int COLS = 8;   // casillas a lo ancho
const int ROWS = 12;  // casillas a lo alto
const int TILE = 64;  // tamaño de cada casilla (en píxeles lógicos)

int playerX = 3;      // posición del jugador en la cuadrícula
int playerY = 5;

// Mueve al jugador una casilla, sin dejarlo salir del tablero.
void move(int dx, int dy) {
    int nx = playerX + dx;
    int ny = playerY + dy;
    if (nx < 0 || nx >= COLS || ny < 0 || ny >= ROWS) return;
    playerX = nx;
    playerY = ny;
}

int main(int argc, char* argv[]) {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        SDL_Log("No se pudo iniciar SDL: %s", SDL_GetError());
        return 1;
    }

    SDL_Window* window = SDL_CreateWindow("Project Baba",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        COLS * TILE, ROWS * TILE, SDL_WINDOW_RESIZABLE);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_PRESENTVSYNC);

    // Dibujamos siempre en un "lienzo" de COLS*TILE x ROWS*TILE y SDL lo escala
    // a la pantalla real, sea cual sea su tamaño. Eso resuelve gran parte del port.
    SDL_RenderSetLogicalSize(renderer, COLS * TILE, ROWS * TILE);

    float startX = 0, startY = 0;   // dónde empezó el dedo
    bool running = true;

    while (running) {
        SDL_Event e;
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                running = false;
            } else if (e.type == SDL_KEYDOWN) {            // teclado (PC)
                switch (e.key.keysym.sym) {
                    case SDLK_LEFT:  move(-1, 0); break;
                    case SDLK_RIGHT: move(1, 0);  break;
                    case SDLK_UP:    move(0, -1); break;
                    case SDLK_DOWN:  move(0, 1);  break;
                }
            } else if (e.type == SDL_FINGERDOWN) {         // dedo (móvil)
                startX = e.tfinger.x;                      // coordenadas entre 0 y 1
                startY = e.tfinger.y;
            } else if (e.type == SDL_FINGERUP) {
                float dx = e.tfinger.x - startX;
                float dy = e.tfinger.y - startY;
                float adx = dx < 0 ? -dx : dx;
                float ady = dy < 0 ? -dy : dy;
                if (adx > 0.05f || ady > 0.05f) {          // ignorar toques sin deslizar
                    if (adx > ady) move(dx > 0 ? 1 : -1, 0);
                    else           move(0, dy > 0 ? 1 : -1);
                }
            }
        }

        // Fondo
        SDL_SetRenderDrawColor(renderer, 24, 24, 38, 255);
        SDL_RenderClear(renderer);

        // Líneas de la cuadrícula
        SDL_SetRenderDrawColor(renderer, 50, 50, 72, 255);
        for (int x = 0; x <= COLS; x++) SDL_RenderDrawLine(renderer, x * TILE, 0, x * TILE, ROWS * TILE);
        for (int y = 0; y <= ROWS; y++) SDL_RenderDrawLine(renderer, 0, y * TILE, COLS * TILE, y * TILE);

        // Jugador
        SDL_Rect r = { playerX * TILE + 6, playerY * TILE + 6, TILE - 12, TILE - 12 };
        SDL_SetRenderDrawColor(renderer, 240, 240, 240, 255);
        SDL_RenderFillRect(renderer, &r);

        SDL_RenderPresent(renderer);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}