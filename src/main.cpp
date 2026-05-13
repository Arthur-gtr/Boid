#include <exception>
#include <iostream>

#include "boid.hpp"

int main(int ac, char **argv)
{
    if (ac != 3){
        std::cerr << "./usage [SFML] [CPU-2D, CPU-3D]" << std::endl;
        return EXIT_FAILURE;
    }
    std::cout << "Start boid\n"; 
    Boid Boid(argv[1], argv[2], 500);
    try 
    {
        std::cout << "Run  Boid\n";
        Boid.runBoid(); 
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
    return EXIT_SUCCESS;
}