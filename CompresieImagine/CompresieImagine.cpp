#include <iostream>
#include "LQuadTree.h"
#include <fstream>

int main(){
    std::ifstream f("date.txt");
    int n;
    f >> n;
    std::vector<std::vector<float>> mat(n);
    for (int i = 0; i < n; i++) {
        mat[i].resize(n);
        for (int j = 0; j < n; j++) {
            f >> mat[i][j];
        }
    }
    LQuadTree image(mat);
    image.showListItems();

    std::vector<std::vector<float>>* compressed = image.compress();
    for (size_t i = 0; i < compressed->size(); i++)
    {
        for (size_t j = 0; j < (*compressed)[0].size(); j++)
        {
            std::cout << (*compressed)[i][j] << " ";
        }
        std::cout << std::endl;
    }

    return 0;
}
