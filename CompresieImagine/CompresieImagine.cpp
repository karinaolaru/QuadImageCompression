#include <iostream>
#include "Image.h"
#include <fstream>

int main(){
    std::ifstream f("date.txt");
    Image image;
    f >> image;
    Image* compressed = image.compress();
    std::cout << *compressed;

    return 0;
}
