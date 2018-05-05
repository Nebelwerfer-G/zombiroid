#include <iostream>
#include <SDL2/SDL.h>
#include <SDL2_image/SDL_image.h>

using namespace std;

class MainGame{
private:
    int wheight;
    int wwidth;
    string wTitle;
    bool gRun;

    SDL_Window *window;
    SDL_Renderer *mRenderer;
    SDL_Texture *bkgnd, *pSprite, *eSprite;
    SDL_Surface *texTemp;

public:
    MainGame() {};
    ~MainGame() {};


    void events() {
        SDL_Event gameEvent;
        SDL_PollEvent(&gameEvent);
        switch (gameEvent.type) {
            case SDL_QUIT:
                gRun = false;
                break;
            default:
                break;
        }
    }

    void gameInit(string title, int height, int width) {
        wTitle = title;
        wwidth = width;
        wheight = height;
        if (SDL_Init(SDL_INIT_EVERYTHING) == 0) {
            cout << "SDL Initialized." << endl;

            window = SDL_CreateWindow(wTitle.c_str(),SDL_WINDOWPOS_CENTERED,SDL_WINDOWPOS_CENTERED,wwidth,wheight,0);
            if (window) { cout << "Window Initialized and created." << endl; }

            mRenderer = SDL_CreateRenderer(window,-1,SDL_RENDERER_PRESENTVSYNC);
            if (mRenderer) {
                cout << "Renderer Initialized." << endl;
                SDL_SetRenderDrawColor(mRenderer,128,128,128,255);

            }

            texTemp = IMG_Load("assets/bg.jpg");
            if (texTemp!=NULL) {
                bkgnd = SDL_CreateTextureFromSurface(mRenderer,texTemp);
                SDL_FreeSurface(texTemp);
            } else {
                SDL_SetRenderDrawColor(mRenderer,242,164,75,255);
                cerr << IMG_GetError() << endl;
            }


            gRun = true;
        } else { gRun = false; cerr << "Something is not right here." << endl; }


    }

    bool runIndicator() { return gRun; }

    void gameRender() {
        SDL_RenderClear(mRenderer);
        if (bkgnd) { SDL_RenderCopy(mRenderer,bkgnd,NULL,NULL); }
        SDL_RenderPresent(mRenderer);
    }

    void spriteUpdater() {}

    void gameExit() {
        SDL_DestroyRenderer(mRenderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        cout << "Game process ended." << endl;
    }

};



int main () {
    MainGame *game = nullptr;
    string gTitle = "Zombiroid";
    int height = 800;
    int width = 800;
    game = new MainGame();

    game->gameInit(gTitle,height,width);

    while (game->runIndicator()) {


        game->events();
        game->spriteUpdater();
        game->gameRender();

    }

    game->gameExit();


    return 0;
}