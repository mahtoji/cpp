#include <iostream>

#define RESET   "\033[0m"
#define RED     "\033[31m"
#define GREEN   "\033[32m"
#define BLUE    "\033[34m"

int main() {
    std::cout << RED << "This text is red." << RESET << std::endl;
    std::cout << GREEN << "This text is green." << RESET << std::endl;
    std::cout << BLUE << "This text is blue." << RESET << std::endl;
    return 0;
}

