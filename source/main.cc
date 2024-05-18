#include <iostream>

int main(int argc, char** argv)
{
        for (int index = 0; index < argc; ++index) {
                std::cout << argv[index] << " ";
        }
        std::cout << std::endl;
}
