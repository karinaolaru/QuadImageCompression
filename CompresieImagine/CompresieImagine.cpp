#include <iostream>
#include <fstream>
#include "Image.h"

int main(){
    Image image("D:\\FACULTATE\\ANUL I\\SEMESTRUL II\\Structuri de date\\CompresieImagine\\hibi_512x512.png");

    for (int i = 0; i <= image.getMaxLevel(); i++) {
        Image* com = image.compress(i);
        std::string path = std::string("D:\\FACULTATE\\ANUL I\\SEMESTRUL II\\Structuri de date\\CompresieImagine\\HibiCompressed\\test") + std::to_string(i) + std::string(".png");
        com->save(path);
        delete com;
    }

    return 0;
}
