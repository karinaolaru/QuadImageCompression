#include <iostream>
#include <fstream>
#include "Image.h"

int main(){
    /*Image image("D:\\FACULTATE\\ANUL I\\SEMESTRUL II\\Structuri de date\\CompresieImagine\\hibi_512x512.png");
    //Image image("C:\\Users\\Karina\\Downloads\\penguin_16x16.jpg");
    Image com = *image.compress();

    com.save("D:\\FACULTATE\\ANUL I\\SEMESTRUL II\\Structuri de date\\CompresieImagine\\test.png");*/
    std::ifstream f("date.txt");
    std::vector<std::vector<Pixel>> matrix;
    int n;
    f >> n;
    matrix.resize(n);
    for (int i = 0; i < n; i++)
    {
        matrix[i].resize(n);
        for (int j = 0; j < n; j++)
        {
            int x;
            f >> x;
            matrix[i][j] = Pixel(x);
        }
    }
    Image* img = new Image(matrix);
    img->showListItems();
    std::cout << *img;
    std::cout << "\n";
    Image* com = img->compress(2);
    com->showListItems();
    std::cout << *com;


    return 0;
}
