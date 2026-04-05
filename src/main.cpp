#include <exception>
#include <iostream>

#include "boid.hpp"

int main(void)
{
    Boid Boid(1);
    try 
    {
        Boid.runBoid(); 
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
    

}