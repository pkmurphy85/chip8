#ifndef GAMELOOP_H
#define GAMELOOP_H

#include <SDL2/SDL.h> 
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_timer.h>
#include "chip8.h"

class Gameloop
{
    private:
        bool is_running;
        SDL_Window* window;
        SDL_Renderer* renderer;
        SDL_Texture* texture;
        Chip8 chip8;
        
        bool initialize(void);
        void process_event(SDL_Event* Event);
        void update(void);
        void render(void);
        void cleanup(void);

    public:
        Gameloop(void);
        ~Gameloop(void);
        int execute(void);
       


};

#endif // GAMELOOP_H