#include <stdio.h>
#include <SDL.h>
#include <SDL_mixer.h>

//http://collabedit.com/5wsts

// gcc /home/flavio/Documents/sdl/dodge_game/dodge.c `pkg-config --cflags --libs sdl2` -o /home/flavio/Documents/sdl/dodge_game/dodge && /home/flavio/Documents/sdl/dodge_game/dodge

typedef enum { false, true } bool;

typedef struct ctrl{
    bool left, right, up, down;
} Ctrl;

// recebe os parametros: (velocidadeX, velocidadeY, aceleraçãoX, aceleraçãoY, freqAcel)
typedef struct variacao{
    int vX, vY, aX, aY, fA;
} Variacao;

//dimensões da tela:
const int SCREEN_WIDTH = 700;
const int SCREEN_HEIGHT = 700;


SDL_Window* gWindow = NULL;
SDL_Renderer* gRenderer = NULL;


//Declarem todas as funções aqui em cima:
void collRectWall(SDL_Rect* rect, Variacao* varObj);
void velObj(int *pX, int *pY, int *vX, int *vY);
void acelObj(Variacao* varObj);
void doTheCtrl(Ctrl* ctrlObj, Variacao* varObj, int vHandObj[]);
bool init(); //init sdl
void closeWindow(SDL_Event* event, bool* running);
void controlObj(SDL_Event* event, Ctrl* ctrlObj);
void quit(); //close sdl
//----------------------

//modos:
void historia();

//fases:
void fase1();

//menu
int main (int argc, char* args[]) {

    //Start up SDL and create window
    if( !init() ) {
        printf( "Failed to initialize!\n" );
    }
    else {
        // carrega e toca a musica
        Mix_Music* musica = NULL;
        musica = Mix_LoadMUS("musica1.ogg");
        if (musica == NULL) {
            printf("Erro ao ler a musica, erro: %s", Mix_GetError());
        }
        Mix_PlayMusic(musica, -1);

        //coloquem cada modo dentro de uma função pra dps fazer o menu:
        historia();
    }

    quit();
    return 0;
}

//menu do modo historia:
void historia(){
    fase1();
}

//fases:
void fase1(){
    //declarem td aqui em cima:
    SDL_Event event;
    SDL_Rect rect = {100, 100, 50, 50}; //retangulo azul

    bool running = true; //mantem o laço principal rodando
    Variacao varRect = {0,0,0,1,2}; //variação do retangulo azul (velocidadeX, velocidadeY, aceleraçãoX, aceleraçãoY, freqAcel)
    int vHandRect[3] = {1,1,3}; //vetor de controle do retangulo azul (handlingX, handlingY, freqCtrl); "handling == dirigibilidade"
    Ctrl ctrlRect = {false, false, false, false};
    //-----------------------------------

    while(running){
        //verifica os eventos:
        while(SDL_PollEvent(&event)){
            //fechar janela
            closeWindow(&event, &running);
            //pega os comandos para o objeto:
            controlObj(&event, &ctrlRect);
        }
        //fundo verde:
        SDL_SetRenderDrawColor(gRenderer, 0, 255, 0, 255);
        SDL_RenderClear(gRenderer);


        //retangulo azul:
        SDL_SetRenderDrawColor(gRenderer, 0, 0, 255, 255);
        SDL_RenderFillRect(gRenderer, &rect);
        //atualiza a tela:
        SDL_RenderPresent(gRenderer);
        //-------------------------------------------------------- aqui a tela é atualizada

        //interacoes fisicas:
        collRectWall(&rect, &varRect);
        acelObj(&varRect);
        doTheCtrl(&ctrlRect, &varRect, vHandRect);
        velObj(&rect.x, &rect.y, &(varRect.vX), &(varRect.vY));

        //delay pra controlar a velocidade de atualização da tela:
        SDL_Delay(20);
    }
}
//-----------------

//permite que o usuário fecha a janela clicando no X: (flavio)
void closeWindow(SDL_Event* event, bool* running){
    if(event->type == SDL_QUIT){
        *running = false;
    }
}
//pega os comandos para o objeto:
void controlObj(SDL_Event* event, Ctrl* ctrlObj){

    if (event->type == SDL_KEYDOWN) {
        switch ((*event).key.keysym.sym) {
            case SDLK_LEFT:
                ctrlObj->left = true;
            break;
            case SDLK_RIGHT:
                ctrlObj->right = true;
            break;
            case SDLK_DOWN:
                ctrlObj->down = true;
            break;
            case SDLK_UP:
                ctrlObj->up = true;
            break;
        }
    } else if (event->type == SDL_KEYUP) {
        switch ((*event).key.keysym.sym) {
            case SDLK_LEFT:
                ctrlObj->left = false;
            break;
            case SDLK_RIGHT:
                ctrlObj->right = false;
            break;
            case SDLK_DOWN:
                ctrlObj->down = false;
            break;
            case SDLK_UP:
                ctrlObj->up = false;
            break;
        }
    }
}

//atualiza a velocidade de acordo com os comandos contidos na struct Ctrl e com a freqCtrl:
void doTheCtrl(Ctrl* ctrlObj, Variacao* varObj, int vHandObj[]){
    static int contFreqCtrl=0;
    contFreqCtrl++;
    if(contFreqCtrl == (vHandObj[2])){
        if(ctrlObj->left) varObj->vX -= vHandObj[0];
        if(ctrlObj->right) varObj->vX += vHandObj[0];
        if(ctrlObj->up) varObj->vY -= vHandObj[1];
        if(ctrlObj->down) varObj->vY += vHandObj[1];
        contFreqCtrl = 0;
    }
}

//detecta colisão entre retangulo e parede, e inverte valor da velocidade: (flavio)
void collRectWall(SDL_Rect* rect, Variacao* varObj){

    if((rect->x)<0){
        varObj->vX *= (-1);
        rect->x = 0;

    }
    else if((rect->x)+(rect->w) > SCREEN_WIDTH){
        varObj->vX *= (-1);
        rect->x = SCREEN_WIDTH - (rect->w);
    }

    if((rect->y)<0){
        varObj->vY *= (-1);
        rect->y = 0;
    }
    else if((rect->y)+(rect->h) > SCREEN_HEIGHT){
        varObj->vY *= (-1);
        rect->y = SCREEN_HEIGHT - (rect->h);
    }
}

//atualiza a posição do objeto de acordo com a velocidade: (flavio)
void velObj(int *pX, int *pY, int *vX, int *vY){
    *pX += *vX;
    *pY += *vY;
}

//atualiza a velocidade do personagem de acordo com a aceleração e com a freqAcel: (flavio)
void acelObj(Variacao* varObj){
    static int contFreqAcel=0;
    contFreqAcel++;
    if(contFreqAcel == (varObj->fA)){
        varObj->vX += varObj->aX;
        varObj->vY += varObj->aY;
        contFreqAcel = 0;
    }
}

//initialize sdl: (flavio)
bool init() {
    //Initialization flag
    bool success = true;

    //Initialize SDL
    if( SDL_Init( SDL_INIT_EVERYTHING ) < 0 ) {
        printf( "SDL could not initialize! SDL Error: %s\n", SDL_GetError() );
        success = false;
    }
    else {
        //Create window
        gWindow = SDL_CreateWindow( "Dodge Game",
                                    SDL_WINDOWPOS_UNDEFINED,
                                    SDL_WINDOWPOS_UNDEFINED,
                                    SCREEN_WIDTH,
                                    SCREEN_HEIGHT,
                                    SDL_WINDOW_RESIZABLE);

        if( gWindow == NULL ) {
            printf( "Window could not be created! SDL Error: %s\n", SDL_GetError() );
            success = false;
        }
        else {
            //set the renderer:
            gRenderer=SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED);
            if(gRenderer == NULL){
                printf( "Renderer could not be set! SDL Error: %s\n", SDL_GetError() );
                success = false;
            }
            // inicia o mixer para sons
            if( Mix_OpenAudio( 44100, MIX_DEFAULT_FORMAT, 2, 2048 ) < 0 ) {
                printf( "SDL_mixer could not initialize! SDL_mixer Error: %s\n", Mix_GetError() );
            }
        }
    }
    return success;
}

//close sdl: (flavio)
void quit() {
    SDL_DestroyRenderer(gRenderer);
    SDL_DestroyWindow(gWindow);
    gWindow = NULL;
    gRenderer = NULL;
    Mix_HaltMusic();
    Mix_Quit();
    SDL_Quit();
}
