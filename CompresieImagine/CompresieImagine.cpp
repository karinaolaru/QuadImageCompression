#include <iostream>
#include <fstream>
#include "Image.h"

int main(){
    Image image("D:\\FACULTATE\\ANUL I\\SEMESTRUL II\\Structuri de date\\CompresieImagine\\hibi_512x512.png");
    //Image image("C:\\Users\\Karina\\Downloads\\penguin_16x16.jpg");
    Image com = *image.compress();
    com.save("D:\\FACULTATE\\ANUL I\\SEMESTRUL II\\Structuri de date\\CompresieImagine\\test.png");

    return 0;
}
