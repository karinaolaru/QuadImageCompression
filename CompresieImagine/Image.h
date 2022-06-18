#pragma once

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <vector>
#include "LQuadTree.h"

class Image : public LQuadTree {
private:
	std::vector<std::vector<Pixel>> pixelMatrix;
 	int maxLevel = 0;

	void divide(Node*&);
	bool shallDivide(Node*&);
	std::pair<std::vector<std::vector<Pixel>*>*, std::list<Node*>*> compressMatrix(int = 2);
	std::list<Node*>* createLeavesCompressedImage(int);

public:
	Image() {};
	Image(const Image& image) {
		this->pixelMatrix = image.pixelMatrix;
		for (auto& node : image.leafNodes) {
			this->leafNodes.emplace_back(new Node(*node));
		}
	}
	Image(const std::vector<std::vector<Pixel>>&);
	Image(std::vector<std::vector<Pixel>*>*, std::list<Node*>*);
	Image(std::string);

	int getMaxLevel() {
		return maxLevel;
	}

	friend std::ostream& operator<<(std::ostream&, Image&);
	Image* compress(int noLevelsToDelete = 2);
	void construct();
	void construct(const std::vector<std::vector<Pixel>>&);
	void save(std::string path);
};

