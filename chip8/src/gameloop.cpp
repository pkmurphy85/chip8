#include "gameloop.h"
#include <iostream>

Gameloop::Gameloop(void)
{
    is_running = true;
    window = nullptr;
    renderer = nullptr;
    texture = nullptr;
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

    if(renderer == nullptr)
    {
        std::cout << "RENDER NULL" << std::endl;
    }
    else
    {
        //std::cout << "ALL GOOD" << std::endl;
    }
    
    SDL_Event event;
    //render();
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

        if(window == nullptr) 
        {
            std::cout << "SDL_CreateWindow failed: " << std::endl;
            throw SDL_GetError();
        }

        // Uint32 render_flags = SDL_RENDERER_ACCELERATED;
        renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
        if(renderer == nullptr)
        {
            std::cerr << "SDL_CreateRenderer failed" << std::endl;
            throw SDL_GetError();
        }

        texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGB332, SDL_TEXTUREACCESS_STREAMING, VIDEO_WIDTH, VIDEO_HEIGHT);
        if(texture == nullptr)
        {
            std::cout << "SDL_CreateTexture failed" << std::endl;
            throw SDL_GetError();
        }

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
        std::cout << "Unknown in initalize" << std::endl;
    }

    return return_code;
    
}

void Gameloop::process_event(SDL_Event* event)
{
    uint8_t keys[16] = {0};
    switch(event->type)
    {
        case SDL_QUIT:
            is_running = false;
            break;
        
        case SDL_KEYDOWN:
            switch(event->key.keysym.sym)
            {
                case SDLK_ESCAPE:
                    is_running = false;
                    break;

                case SDLK_0:
                    keys[0] = 1;
                    break;

                case SDLK_1:
                    keys[1] = 1;
                    break;

                case SDLK_2:
                    keys[2] = 1;
                    break;

                case SDLK_3:
                    keys[3] = 1;
                    break;

                case SDLK_4:
                    keys[4] = 1;
                    break;

                case SDLK_5:
                    keys[5] = 1;
                    break;

                case SDLK_6:
                    keys[6] = 1;
                    break;                    

                case SDLK_7:
                    keys[7] = 1;
                    break;

                case SDLK_8:
                    keys[8] = 1;
                    break;

                case SDLK_9:
                    keys[9] = 1;
                    break;

                case SDLK_a:
                    keys[0xA] = 1;
                    break; 

                case SDLK_b:
                    keys[0xB] = 1;
                    break;

                case SDLK_c:
                    keys[0xC] = 1;
                    break;  

                case SDLK_d:
                    keys[0xD] = 1;
                    break;                 

                case SDLK_e:
                    keys[0xE] = 1;
                    break; 

                case SDLK_f:
                    keys[0xF] = 1;
                    break;   

                default:
                    break;
            }
            break;
        
        case SDL_KEYUP:
            switch(event->key.keysym.sym)
            {
                case SDLK_ESCAPE:
                    is_running = false;
                    break;

                case SDLK_0:
                    keys[0] = 0;
                    break;

                case SDLK_1:
                    keys[1] = 0;
                    break;

                case SDLK_2:
                    keys[2] = 0;
                    break;

                case SDLK_3:
                    keys[3] = 0;
                    break;

                case SDLK_4:
                    keys[4] = 0;
                    break;

                case SDLK_5:
                    keys[5] = 0;
                    break;

                case SDLK_6:
                    keys[6] = 0;
                    break;                    

                case SDLK_7:
                    keys[7] = 0;
                    break;

                case SDLK_8:
                    keys[8] = 0;
                    break;

                case SDLK_9:
                    keys[9] = 0;
                    break;

                case SDLK_a:
                    keys[0xA] = 0;
                    break; 

                case SDLK_b:
                    keys[0xB] = 0;
                    break;

                case SDLK_c:
                    keys[0xC] = 0;
                    break;  

                case SDLK_d:
                    keys[0xD] = 0;
                    break;                 

                case SDLK_e:
                    keys[0xE] = 0;
                    break; 

                case SDLK_f:
                    keys[0xF] = 0;
                    break;   

                default:
                    break;
            }
            break;


        default:
            break;

    }
    
    chip8.set_keys(keys);
}

void Gameloop::update(void)
{
    chip8.fetch();
    chip8.execute();
}

void Gameloop::render(void)
{
    /*std::cout << "RENDER " << SDL_GetError() << std::endl;
    if(renderer == nullptr)
    {   std::cerr << "Renderer is null" << std::endl;
    }
    else
    {
        std::cout << "Renderer is NOT null" << std::endl;
    }
    
    int error = SDL_SetRenderDrawColor(renderer, 100, 100, 100, 255);
    std::cerr << "ERROR1: " << SDL_GetError() << " " << error << std::endl;*/
    //std::cout << "RENDER" << std::endl;
    uint8_t *video = chip8.get_video();
    /*for(uint8_t j = 0; j<VIDEO_HEIGHT; j++)
    {
        for(uint8_t i = 0; i<VIDEO_WIDTH; i++)
        {
            if(video[i+j] == 0xff)
            {
                std::cout << 1;
            }
            else
            {
                std::cout << 0;
            }
            
        }
        std::cout << std::endl;
    }*/

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

    int error = SDL_UpdateTexture(texture, nullptr, video, sizeof(video[0]) * VIDEO_WIDTH);
    if(error < 0)
    {
        std::cerr << "Update Texture Error: " << std::endl;
        throw SDL_GetError();
    }
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, texture, nullptr, nullptr);

    /*SDL_Rect rect;
    rect.w = 10;
    rect.h = 10;
    rect.x = VIDEO_WIDTH/2;
    rect.y = VIDEO_HEIGHT/2;

    SDL_SetRenderDrawColor(renderer, 200, 0, 200, 255);
    SDL_RenderFillRect(renderer, &rect);*/

    SDL_RenderPresent(renderer);
    //std::cerr << "ERROR: " << SDL_GetError() << std::endl;
}

void Gameloop::cleanup(void)
{
	SDL_DestroyTexture(texture); 
	SDL_DestroyRenderer(renderer); 
	SDL_DestroyWindow(window); 
    SDL_Quit();
}


