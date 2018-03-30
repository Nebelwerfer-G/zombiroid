#include <iostream>
#include <SDL2/SDL.h>

using namespace std;

/*
 * Window class
 * Class including everything about handling events of game window.
 * A window object contains height and width (px), window name,
 * and a boolean value for indicating the window is closed or not.
 * Window is instantiated using a SDL_Window object.
 * */
class Window {
private:
    int wheight = 800;
    int wwidth = 600;
    string wTitle = "Zombiroid";
    bool wClosed = false;

    SDL_Window *window = nullptr;


public:
    Window(const std::string &wt, int width, int height) {
        wTitle = wt, wwidth = width, wheight = height;
        if (!wInit()) {
            wClosed = true;
        }

    };
    ~Window() {
        SDL_DestroyWindow(window);
        SDL_Quit();
    };

    bool wInit() {
        if (SDL_Init(SDL_INIT_VIDEO)!=0) {
            cerr<<"SDL Initialization failed.\n";
            return -1;
        }

        window = SDL_CreateWindow(
                wTitle.c_str(),
                SDL_WINDOWPOS_CENTERED,  // Constant SDL_WINDOWPOS_CENTERED indicates starting points
                SDL_WINDOWPOS_CENTERED,  // which always make the window at center of display.
                wwidth, wheight,0
        );
        if (window == nullptr) {
            cerr<<"Window Initialization failed.\n";
            return -1;
        }
        return true;
    }

    inline bool isClosed() const{return wClosed;}

    void wEventsListener() {
        SDL_Event wEvent;

        if (SDL_PollEvent(&wEvent)) {
            switch (wEvent.type) {
                case SDL_QUIT:
                    wClosed = true;
                    break;

                default:
                    break;
            }
        }
    }
};

int main() {

    Window gameWindow("Zombiroid - Asteroid with zombies", 800, 600);

    while(!gameWindow.isClosed()) {
        gameWindow.wEventsListener();
    }

    return 0;
}