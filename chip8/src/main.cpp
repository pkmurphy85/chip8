#include <iostream>
#include "gameloop.h"

 
using namespace std;

int main(int argc, char* argv[])
{
    cout << "Application started" << endl;
    Gameloop gameloop;
    int return_val = gameloop.execute();
    cout << "Application ending " << return_val << endl;
    return return_val;
}
