#include "gameloop.h"
#include <iostream>

Gameloop::Gameloop(void)
{
    is_running = true;
    window = NULL;
    renderer = NULL;
    texture = NULL;
}

Gameloop::~Gameloop()
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
    bool return_code = true;
    try
    {
        if(SDL_Init(SDL_INIT_EVERYTHING) < 0) 
        {
            std::cout << "SDL_Init() failed: " << std::endl;
            throw SDL_GetError();
        }
 
        window = SDL_CreateWindow("My Game Window",
                            SDL_WINDOWPOS_CENTERED,
                            SDL_WINDOWPOS_CENTERED,
                            640, 320,
                            SDL_WINDOW_SHOWN);
        if(window == NULL) 
        {
            std::cout << "SDL_CreateWindow failed: " << std::endl;
            throw SDL_GetError();
        }

        // Uint32 render_flags = SDL_RENDERER_ACCELERATED;
        SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, 0);
        if (renderer == NULL )
        {
            std::cout << "SDL_CreateRenderer failed" << std::endl;
            throw SDL_GetError();
        }

        // make it black
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);
        SDL_RenderPresent(renderer); 

        chip8.initialize();
    }
    catch(const char* error)
    {
        return_code = false;
        std::cout << "Error in initalize: [" << error << "]" << std::endl;
    }
    catch(...)
    {
        return_code = false;
        std::cout << "Unhandled error" << std::endl;
    }

    return return_code;
    
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
    chip8.fetch();
    chip8.execute();
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


