#include <exception>
#include <iostream>

#include "boid.hpp"

int main(void)
{
    std::cout << "Start boid\n"; 
    Boid Boid("SFML", "CPU-2D", 20);
    try 
    {
        std::cout << "Run  Boid\n";
        Boid.runBoid(); 
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
}