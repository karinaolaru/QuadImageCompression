#include <iostream>
#include <fstream>
#include "Image.h"
#include <chrono>
#include <ctime>
#include <iomanip>

int main(){
    std::chrono::time_point<std::chrono::system_clock> start, end; 

    start = std::chrono::system_clock::now();
    Image image("D:\\FACULTATE\\ANUL I\\SEMESTRUL II\\Structuri de date\\CompresieImagine\\hibi_256x256.png");
    end = std::chrono::system_clock::now();

    std::chrono::duration<double> elapsed_seconds = end - start;
    std::time_t end_time = std::chrono::system_clock::to_time_t(end);

    std::cout << "Image loading took: " <<std::setprecision(3)<< elapsed_seconds.count() << "s\n";

    for (int i = 0; i <= image.getMaxLevel(); i++) {
        start = std::chrono::system_clock::now();
        Image* com = image.compress(i);
        end = std::chrono::system_clock::now();

        elapsed_seconds = end - start;
        end_time = std::chrono::system_clock::to_time_t(end);

        std::cout << "Image compression " <<i<<" took: "<< std::setprecision(3) << elapsed_seconds.count() << "s\n";

        std::string path = std::string("D:\\FACULTATE\\ANUL I\\SEMESTRUL II\\Structuri de date\\CompresieImagine\\HibiCompressed\\test") + std::to_string(i) + std::string(".png");
        com->save(path);
        delete com;
    }

    return 0;
}
