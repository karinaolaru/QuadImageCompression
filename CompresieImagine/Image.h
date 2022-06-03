#pragma once

#include "LQuadTree.h"

class Image : public LQuadTree {
private:
	std::vector<std::vector<float>> pixelMatrix;

public:
	LQuadTree(const std::vector<std::vector<float>>&);
	friend std::istream& operator>>(std::istream&, LQuadTree&);
	std::vector<std::vector<float>*>* compress(PERCENTAGE = PERCENTAGE::LOW);
};

