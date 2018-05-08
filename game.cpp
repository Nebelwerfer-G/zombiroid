#include <iostream>
#include <SDL2/SDL.h>
#include <SDL2_image/SDL_image.h>
#include <random>
#include <thread>
#include <chrono>
#define RNG rand()%(wlength-eSpriteH)

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
    SDL_Rect startR, destR;
    SDL_Renderer* rend;
    SDL_Event pEvent;
    int orient = 1; // Sprite orientaton flag. Sprites are "facing right" by default, start from there, by clockwise, R=1, D=2, L=3, U=4.


public:
    Objects(const char* oTexFile, SDL_Renderer* tgtrender, int texh, int texw, int texx, int texy) {
        rend = tgtrender;
        objTex = Textures::texLoader(oTexFile,rend);
        startR.h = texh; destR.h = texh;
        startR.w = texw; destR.w = texw;
        startR.x = texx; startR.y = texy;
    }
    ~Objects(){};

    SDL_Texture *getObjTex() const { return objTex; };

    const SDL_Rect &getStartR() const { return startR; }

    const SDL_Rect &getDestR() const { return destR; }


    void enemyposUpdate() {
        enemyPosCalc();
    }

    void posRender() {
            SDL_RenderCopy(rend, objTex, NULL, &startR);
    }

    void playerPosCalc(SDL_Event onEvent, int movstep) {
            pEvent = onEvent;
            if (pEvent.type==SDL_KEYDOWN) {
                switch (pEvent.key.keysym.sym) {
                    case SDLK_w:
                        startR.y-=movstep;
                        break;
                    case SDLK_a:
                        startR.x-=movstep;
                        break;
                    case SDLK_d:
                        startR.x+=movstep;
                        break;
                    case SDLK_s:
                        startR.y+=movstep;
                        break;
                    default:
                        break;
                }
            }

    }

    void enemyPosCalc() {

    }

    bool collision(Objects p) {
        SDL_Rect pRect = p.getStartR();
        if (startR.x+startR.w < pRect.x || startR.x > pRect.x+pRect.w) return false;
        if (startR.y+startR.h < pRect.y || startR.y > pRect.y+pRect.h) return false;

        return true;
    }

};

class MainGame{
private:
    int wlength;
    string wTitle;
    bool gRun;
    const int pMovStep = 4;
    const float pFramespeed = pMovStep/60;

    SDL_Window *window;
    SDL_Texture *bkgnd;
    Objects* player;
    Objects* enemy[5];
    Objects* bullet[3];
    SDL_Renderer *mRenderer;

    const int pSpriteW = 62;
    const int pSpriteH = 40;
    const int eSpriteW = 48;
    const int eSpriteH = 44;

    int lives = 3;
    int score = 0;
public:
    MainGame() {};
    ~MainGame() {};


    void wEvents() {
        SDL_Event gameEvent;
        SDL_PollEvent(&gameEvent);
        switch (gameEvent.type) {
            case SDL_QUIT:
                gRun = false;
                break;
            default:
                break;
        }
        player->playerPosCalc(gameEvent, pMovStep);
    }

    void whenPlayerCollide() {
        player->~Objects();
        lives--;
        cout << "Zombie encountered, current life remaining: " << lives << endl;
        if (lives < 0) {
            cout << "You have ran out your luck today! Better luck next time!\nGame Over" << endl;
            this_thread::sleep_until(chrono::system_clock::now() + chrono::seconds(2));
            gRun = false;
        } else {
            player = new Objects("assets/player_idle.png", mRenderer, pSpriteH, pSpriteW, ((wlength-pSpriteW)/2), ((wlength-pSpriteH)/2));
        }
    }

    void whenBulletCollide() {

    }

    void gameInit(string title, int length) {
        wTitle = title;
        wlength = length;
        srand(time(0));
        if (SDL_Init(SDL_INIT_EVERYTHING) == 0) {
            cout << "SDL Initialized." << endl;

            window = SDL_CreateWindow(wTitle.c_str(),SDL_WINDOWPOS_CENTERED,SDL_WINDOWPOS_CENTERED,wlength,wlength,0);
            if (window) { cout << "Window Initialized and created." << endl; }

            mRenderer = SDL_CreateRenderer(window,-1,SDL_RENDERER_PRESENTVSYNC);
            if (mRenderer) {
                cout << "Renderer Initialized." << endl;
                SDL_SetRenderDrawColor(mRenderer,128,128,128,255);

            }

            bgRender();
            psRender();
            esRender();


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
        player = new Objects("assets/player_idle.png", mRenderer, pSpriteH, pSpriteW, ((wlength-pSpriteW)/2), ((wlength-pSpriteH)/2));
    }

    void esRender() {
        enemy[0] = new Objects("assets/zombie_idle.png", mRenderer, eSpriteH, eSpriteW, RNG, RNG);
        enemy[1] = new Objects("assets/zombie_idle.png", mRenderer, eSpriteH, eSpriteW, RNG, RNG);
        enemy[2] = new Objects("assets/zombie_idle.png", mRenderer, eSpriteH, eSpriteW, RNG, RNG);
        enemy[3] = new Objects("assets/zombie_idle.png", mRenderer, eSpriteH, eSpriteW, RNG, RNG);
        enemy[4] = new Objects("assets/zombie_idle.png", mRenderer, eSpriteH, eSpriteW, RNG, RNG);
        for (int i = 0; i < 5; ++i) {
            enemy[i]->enemyposUpdate();
        }
    }
    void gameRender() {
        SDL_RenderClear(mRenderer);
        if (bkgnd) SDL_RenderCopy(mRenderer,bkgnd,NULL,NULL);
        if (player->getObjTex()) {
            player->posRender();
        }
        for (int i = 0; i < 5; ++i) {
            if (enemy[i]->getObjTex()) enemy[i]->posRender();
            if (enemy[i]->collision(*player)) whenPlayerCollide();
        }
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
    int length = 800;
    game = new MainGame();


    Uint32 startcount;
    Uint32 elapsedcount;
    Uint32 FPS;
    const int dFPS = 60;
    const int dFrametime = 1000/dFPS;
    game->gameInit(gTitle,length);
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
        }
    }

    game->gameExit();


    return 0;
}