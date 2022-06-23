#include <iostream>
#include <fstream>
#include "Image.h"
#include <chrono>
#include <ctime>
#include <iomanip>

/*int main() {
    std::chrono::time_point<std::chrono::system_clock> start, end; 

    start = std::chrono::system_clock::now();
    Image image("D:\\FACULTATE\\ANUL I\\SEMESTRUL II\\Structuri de date\\CompresieImagine\\hibi_1024x1024.png");
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
}*/

int main() {
    std::ifstream f("date.txt");
    std::vector<std::vector<Pixel>> matrix;
    int n, x;
    f >> n;
    matrix.resize(n);
    for (int i = 0; i < n; i++) {
        matrix[i].resize(n);
        for (int j = 0; j < n; j++) {
            f >> x;
            matrix[i][j] = Pixel(x);
        }
    }
    Image img(matrix);
    img.showListItems();
    std::cout << img << "\n";
    /*Image* com = img.compress();
    com->showListItems();
    std::cout << *com;*/

    return 0;
}
