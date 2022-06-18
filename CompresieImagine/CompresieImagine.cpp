#include <iostream>
#include <fstream>
#include "Image.h"

int main(){
    Image image("D:\\FACULTATE\\ANUL I\\SEMESTRUL II\\Structuri de date\\CompresieImagine\\blue_jay_256x256.jpg");

    for (int i = 0; i <= image.getMaxLevel(); i++) {
        Image* com = image.compress(i);
        std::string path = std::string("D:\\FACULTATE\\ANUL I\\SEMESTRUL II\\Structuri de date\\CompresieImagine\\BlueJayCompressed\\test") + std::to_string(i) + std::string(".png");
        com->save(path);
        delete com;
    }

    return 0;
}
