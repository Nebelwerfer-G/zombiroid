#include <iostream>
#include <SDL2/SDL.h>
#include <SDL2_image/SDL_image.h>

using namespace std;

class Textures{
private:

public:
    static SDL_Texture* texLoader (const char* filename, SDL_Renderer* render) {
        SDL_Surface* tmp = IMG_Load(filename);
        if (tmp!=nullptr) {
            SDL_Texture* tex = SDL_CreateTextureFromSurface(render,tmp);
            SDL_FreeSurface(tmp);
            return tex;
        }
    }
};

class Objects{
private:
    SDL_Texture* objTex;
    SDL_Rect startR,destR;
    SDL_Renderer* rend;



public:
    Objects(const char* oTexFile, SDL_Renderer* tgtrender, int texh, int texw, int texx, int texy) {
        rend = tgtrender;
        objTex = Textures::texLoader(oTexFile,rend);
        startR.h = texh; destR.h = texh;
        startR.w = texw; destR.w = texw;
        startR.x = texx; startR.y = texy;
    };
    ~Objects(){};

    void posUpdate(int destx, int desty) {
        destR.x = destx;
        destR.y = desty;
    }

    void posRender() { SDL_RenderCopy(rend, objTex, &startR, &destR); }
};

class MainGame{
private:
    int wheight;
    int wwidth;
    string wTitle;
    bool gRun;
    const int pMovStep = 4;
    const float pFramespeed = pMovStep/60;

    SDL_Window *window;
    SDL_Texture *bkgnd, *pSprite, *eSprite;
    SDL_Surface *texTemp;
    SDL_Rect pSRect, eSRect;
    Objects* player;

public:
    static SDL_Renderer *mRenderer;
    MainGame() {};
    ~MainGame() {};


    void wEvents() {
        SDL_Event gameEvent;
        SDL_PollEvent(&gameEvent);
        switch (gameEvent.type) {
            case SDL_QUIT:
                gRun = false;
                break;
            case SDL_KEYDOWN:
                switch (gameEvent.key.keysym.sym) {
                    case SDLK_w:
                        pSRect.y-=pMovStep;
                        break;
                    case SDLK_a:
                        pSRect.x-=pMovStep;
                        break;
                    case SDLK_d:
                        pSRect.x+=pMovStep;
                        break;
                    case SDLK_s:
                        pSRect.y+=pMovStep;
                        break;
                    default:
                        break;
                }
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

            bgRender();
            psRender();


            gRun = true;
        } else { gRun = false; cerr << "Something is not right here." << endl; }


    }

    bool runIndicator() { return gRun; }

    void bgRender() {
        bkgnd = Textures::texLoader("assets/bg.jpg", mRenderer);
        if (bkgnd==NULL) {
            SDL_SetRenderDrawColor(mRenderer,242,164,75,255);
            cerr << IMG_GetError() << endl;
        }
    }

    void psRender() {
        player = new Objects("assets/player_idle.png", mRenderer, 40, 62, ((wwidth-62)/2), ((wheight-40)/2));
        player->posUpdate(((wwidth-62)/2), ((wheight-40)/2));
    }

    void gameRender() {
        SDL_RenderClear(mRenderer);
        if (bkgnd) { SDL_RenderCopy(mRenderer,bkgnd,NULL,NULL); }
        if (pSprite) { SDL_RenderCopy(mRenderer,pSprite,NULL,&pSRect); }
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


    Uint32 startcount;
    Uint32 elapsedcount;
    Uint32 FPS;
    const int dFPS = 60;
    const int dFrametime = 1000/dFPS;
    game->gameInit(gTitle,height,width);
    startcount = SDL_GetTicks();

    while (game->runIndicator()) {


        game->wEvents();
        game->spriteUpdater();
        game->gameRender();
        elapsedcount = SDL_GetTicks()-startcount;
        startcount = SDL_GetTicks();
        if (elapsedcount!=0) {
            FPS = 1000/elapsedcount;
            if (FPS>dFPS) {
                SDL_Delay(dFrametime-elapsedcount);
            }
            cout << "Current FPS: " << FPS << endl;
        }
    }

    game->gameExit();


    return 0;
}