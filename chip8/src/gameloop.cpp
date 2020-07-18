#include "gameloop.h"
#include <iostream>

Gameloop::Gameloop(void)
{
    is_running = true;
    window = NULL;
    renderer = NULL;
    texture = NULL;
}

Gameloop::~Gameloop(void)
{

}

int Gameloop::execute(void)
{
    if(initialize() == false)
    {
        return -1;
    }

    SDL_Event event;
    while(is_running)
    {
        while(SDL_PollEvent(&event))
        {
            process_event(&event);
        }

        update();
        render();
    }

    cleanup();

    return 0;
}

bool Gameloop::initialize(void)
{
    if(SDL_Init(SDL_INIT_EVERYTHING) < 0) 
    {
        std::cout << "SDL_Init() failed: " << SDL_GetError() << std::endl;
        return false;
    }
 
    window = SDL_CreateWindow("My Game Window",
                          SDL_WINDOWPOS_CENTERED,
                          SDL_WINDOWPOS_CENTERED,
                          640, 320,
                          SDL_WINDOW_SHOWN);
    if(window == NULL) 
    {
        std::cout << "SDL_CreateWindow failed: " << SDL_GetError() << std::endl;
        return false;
    }

   // Uint32 render_flags = SDL_RENDERER_ACCELERATED;
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, 0);
    if (renderer == NULL )
    {
        std::cout << "SDL_CreateRenderer failed: " << SDL_GetError() << std::endl;
        return false;
    }

    // make it black
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
    SDL_RenderPresent(renderer); 
    return true;
}

void Gameloop::process_event(SDL_Event* event)
{
    if(event->type == SDL_QUIT) 
    {
        is_running = false;
    }
}

void Gameloop::update(void)
{

}

void Gameloop::render(void)
{

}

void Gameloop::cleanup(void)
{
	SDL_DestroyTexture(texture); 
	SDL_DestroyRenderer(renderer); 
	SDL_DestroyWindow(window); 
    SDL_Quit();
}


