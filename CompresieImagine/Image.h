#pragma once

#include <vector>
#include "LQuadTree.h"

class Image : public LQuadTree {
private:
	std::vector<std::vector<float>> pixelMatrix;

	void divide(Node*&);
	bool shallDivide(Node*&);
	std::pair<std::vector<std::vector<float>*>*, std::vector<Node*>*> compressMatrix(PERCENTAGE = PERCENTAGE::LOW);

public:
	Image() {};
	Image(const Image& image) {
		this->pixelMatrix = image.pixelMatrix;
		for (auto& node : image.leafNodes) {
			this->leafNodes.emplace_back(new Node(*node));
		}
	}
	Image(const std::vector<std::vector<float>>&);
	Image(std::vector<std::vector<float>*>*, std::vector<Node*>*);
	friend std::istream& operator>>(std::istream&, Image&);
	friend std::ostream& operator<<(std::ostream&, Image&);
	Image* compress(PERCENTAGE = PERCENTAGE::LOW);
	void construct(const std::vector<std::vector<float>>&);
};

