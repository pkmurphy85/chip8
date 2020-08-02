#include <iostream>
#include "gameloop.h"
 
using namespace std;

int main(int argc, char* argv[])
{
    //cout << "Application started" << endl;
    Gameloop gameloop;
    //Chip8 chip8;
    //chip8.load_rom();
    int return_val = gameloop.execute();
    //cout << "Application ending " << return_val << endl;
    //return return_val;
    return 0;
}
