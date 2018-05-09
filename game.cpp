#include <iostream>
#include <SDL2/SDL.h>
#include <SDL2_image/SDL_image.h>
#include <thread>
#define RNG rand()%(wlength-eSpriteH)
#define eBoundR wlength-eSpriteH
#define ENEMY_MAX 3
#define BULLET_MAX 5

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
        } else {
            cerr << SDL_GetError() << endl;
            exit(2);
        }
    }
};

class Objects{
private:
    SDL_Texture* objTex;
    SDL_Rect texR;
    SDL_Renderer* rend;
    SDL_Event pEvent;

public:
    Objects(const char* oTexFile, SDL_Renderer* tgtrender, int texh, int texw, int texx, int texy) {
        rend = tgtrender;
        objTex = Textures::texLoader(oTexFile,rend);
        if (objTex==nullptr) {
            cerr<<"no textures."<<endl;
        }
        texR.h = texh; texR.w = texw;
        texR.x = texx; texR.y = texy;
    }
    ~Objects(){};

    SDL_Texture *getObjTex() const { return objTex; };

    const SDL_Rect &getStartR() const { return texR; }


    void enemyposUpdate(int enemyspd) {
        objPosCalc(true, enemyspd);
    }

    void bulletposUpdate(int bulletspd) {
        objPosCalc(false, bulletspd);
    }

    void posRender() {
            SDL_RenderCopy(rend, objTex, NULL, &texR);
    }

    void playerPosCalc(SDL_Event onEvent, int movstep, int windowlength) {
            pEvent = onEvent;
            if (pEvent.type==SDL_KEYDOWN) {
                switch (pEvent.key.keysym.sym) {
                    case SDLK_UP:
                        if (texR.y>4) texR.y-=movstep;
                        break;
                    case SDLK_DOWN:
                        if (texR.y<windowlength-texR.h) texR.y+=movstep;
                        break;
                    default:
                        break;
                }
            }

    }

    void objPosCalc(bool LRflag, int movstep) {
        if (LRflag) { texR.x-=movstep; }
        else { texR.x+=movstep; }
    }

    bool collision(Objects p) {
        SDL_Rect pRect = p.getStartR();
        if (texR.x+texR.w < pRect.x || texR.x > pRect.x+pRect.w) return false;
        if (texR.y+texR.h < pRect.y || texR.y > pRect.y+pRect.h) return false;

        return true;
    }

};

class MainGame{
private:
    int wlength;
    string wTitle;
    bool gRun;
    const int pMovStep = 7;
    const float pFramespeed = pMovStep/60;

    SDL_Window *window;
    SDL_Texture *bkgnd;
    Objects* player;
    Objects* enemy[ENEMY_MAX];
    Objects* bullet[BULLET_MAX];
    SDL_Renderer *mRenderer;

    const int pSpriteW = 52;
    const int pSpriteH = 31;
    const int eSpriteW = 39;
    const int eSpriteH = 34;

    int lives = true;
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
        player->playerPosCalc(gameEvent, pMovStep, wlength);
        if (gameEvent.type==SDL_KEYDOWN) {
            if (gameEvent.key.keysym.sym==SDLK_SPACE) {
                playerShoot();
            }
        }
    }

    void whenPlayerCollide() {
        player->~Objects();
        lives = false;
        cout << "You have ran out your luck today! Better luck next time!\nGame Over, Your kill count is: "<< score << endl;
        this_thread::sleep_until(chrono::system_clock::now() + chrono::seconds(2));
        gRun = false;
    }

    void playerShoot() {
        for (int i = 0; i < BULLET_MAX; ++i) {
            if (bullet[i] == NULL) {
                bullet[i] = new Objects("assets/bullet.png",mRenderer,6,6,(player->getStartR().x+player->getStartR().w),(player->getStartR().y+21));
                break;
            }
        }

    }

    void whenBulletCollide(int bulletIndex, int enemyIndex) {
        score++;
        whenBulletFade(bulletIndex);
        enemy[enemyIndex]->~Objects();
        enemy[enemyIndex] = NULL;
        enemy[enemyIndex] = new Objects("assets/zombie_idle.png", mRenderer, eSpriteH, eSpriteW, eBoundR, RNG);

    }

    void whenBulletFade(int bulletIndex) {
        bullet[bulletIndex]->~Objects();
        bullet[bulletIndex] = NULL;
    }

    void gameInit(string title, int length) {
        for ( auto &i : enemy ) i = NULL;
        for ( auto &i : bullet ) i = NULL;
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
        player = new Objects("assets/player_idle.png", mRenderer, pSpriteH, pSpriteW, 0, ((wlength-pSpriteH)/2));
    }

    void esRender() {
        for (int i = 0; i < ENEMY_MAX; ++i) {
            enemy[i] = new Objects("assets/zombie_idle.png", mRenderer, eSpriteH, eSpriteW, eBoundR, RNG);
        }
    }
    void gameRender() {
        SDL_RenderClear(mRenderer);
        if (bkgnd) SDL_RenderCopy(mRenderer,bkgnd,NULL,NULL);
        if (player->getObjTex()) {
            player->posRender();
        }
        for (auto &i : enemy) {
            i->enemyposUpdate(1);
            if (i->getObjTex()) i->posRender();
            if (i->collision(*player) || i->getStartR().x<0) whenPlayerCollide();
        }
        for (int j = 0; j < BULLET_MAX; ++j) {
            if (bullet[j]!=NULL && bullet[j]->getObjTex()) {
                bullet[j]->bulletposUpdate(4);
                bullet[j]->posRender();
            }
            for (int i = 0; i < ENEMY_MAX; ++i) {
                if (bullet[j]!=NULL && bullet[j]->collision(*enemy[i])) whenBulletCollide(j, i);
                if (bullet[j]!=NULL && bullet[j]->getStartR().x>wlength) whenBulletFade(j);
            }
        }
        SDL_RenderPresent(mRenderer);
    }

    void spriteUpdater() {}

    void gameExit() {
        SDL_DestroyTexture(bkgnd);
        SDL_DestroyRenderer(mRenderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        cout << "Game process ended." << endl;
    }

};



int main () {
    MainGame *game = nullptr;
    string gTitle = "Zombiroid";
    int length = 400;
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