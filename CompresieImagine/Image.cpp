#include "Image.h"

Image::Image(const std::vector<std::vector<Pixel>>& matrix) {
	construct(matrix);
}

Image::Image(std::vector<std::vector<Pixel>*>* matrix, std::list<Node*>* nodes) {
	pixelMatrix.resize(matrix->size(), std::vector<Pixel>(matrix->size()));
	for (int i = 0; i < matrix->size(); i++) {
		for (int j = 0; j < matrix->size(); j++) {
			pixelMatrix[i][j] = (*(*matrix)[i])[j];
		}
	}
	for (const auto& it : *nodes) {
		this->leafNodes.emplace_back(new Node(*it));
	}

	this->codeSize = log2(matrix->size());
	int currMaxLevel = 0;
	for (const auto& it : *nodes) {
		if (it->level > currMaxLevel) currMaxLevel = it->level;
	}
	this->maxLevel = currMaxLevel;
}

Image::Image(std::string path) {
	cv::Mat img = cv::imread(path, cv::IMREAD_COLOR);

	if (img.empty()) {
		return;
	}

	this->pixelMatrix.resize(img.rows, std::vector<Pixel>(img.cols));
	for (int i = 0; i < img.rows; i++) {
		for (int j = 0; j < img.cols; j++) {
			this->pixelMatrix[i][j] = Pixel(img.ptr(i, j));
		}
	}
	construct();
}

void Image::construct() {
	float size = log2(pixelMatrix.size());
	if (pixelMatrix.size() < 8 || size != (int)size || pixelMatrix.size() != pixelMatrix[0].size()) {
		return;
	}
	codeSize = (int)size;
	std::vector<int> firstCode(codeSize, 0);
	leafNodes.emplace_back(new Node(0, Node::Info(Point2D(0, 0), pixelMatrix.size()), firstCode));
	divide(leafNodes.front());
	std::cout << "Image has been loaded\n";
}

void Image::construct(const std::vector<std::vector<Pixel>>& pixelMatrix) {
	this->pixelMatrix = pixelMatrix;
	construct();
}

void Image::divide(Node*& parent) {
	if ((parent->info.edge == 1) || (shallDivide(parent) == false)) {
		parent->color = pixelMatrix[parent->info.upperLeftCorner.y][parent->info.upperLeftCorner.x];
		if (maxLevel < parent->level) {
			maxLevel = parent->level;
		}
		return;
	}

	std::vector<std::vector<int>>* codes = parent->split();
	std::vector<Node*> children({ constructChildNode(parent, (*codes)[0]), constructChildNode(parent, (*codes)[1], parent->info.edge / 2),
								  constructChildNode(parent, (*codes)[2], 0, parent->info.edge / 2), constructChildNode(parent, (*codes)[3], parent->info.edge / 2, parent->info.edge / 2) });
	leafNodes.remove(parent);
	delete codes;

	for (auto& child : children) {
		leafNodes.emplace_back(child);
		divide(child);
	}
}

bool Image::shallDivide(Node*& parent) {
	Pixel firstElem = pixelMatrix[parent->info.upperLeftCorner.y][parent->info.upperLeftCorner.x];
	for (int i = parent->info.upperLeftCorner.y; i < parent->info.upperLeftCorner.y + parent->info.edge; i++) {
		for (int j = parent->info.upperLeftCorner.x; j < parent->info.upperLeftCorner.x + parent->info.edge; j++) {
			if (pixelMatrix[i][j] != firstElem) {
				return true;
			}
		}
	}
	return false;
}

std::list<LQuadTree::Node*>* Image::createLeavesCompressedImage(int noLevelsToDelete) {
	auto nodes = new std::list<Node*>();
	std::vector<int> currentIndexInLevel(codeSize + 1);
	std::vector<Node*> neighbours(VIER);

	for (const auto& it : this->leafNodes) {
		nodes->emplace_back(new Node(*it));
		if (it->level <= maxLevel - noLevelsToDelete) {
			continue;
		}

		currentIndexInLevel[it->level]++;
		for (int level = it->level; currentIndexInLevel[level] == VIER && level > maxLevel - noLevelsToDelete; --level) {
			currentIndexInLevel[level] = 0;
			if (level - 1 > 0) {
				currentIndexInLevel[level - 1]++;
			}
			
			for (int i = 0; i < VIER; i++) {
				neighbours[i] = nodes->back();
				nodes->pop_back();
			}
			Node* merged = mergeNodes(neighbours);
			for (int i = 0; i < VIER; i++) {
				delete neighbours[i];
			}
			nodes->emplace_back(merged);
		}
	}
	return nodes;
}

std::pair<std::vector<std::vector<Pixel>*>*, std::list<LQuadTree::Node*>*> Image::compressMatrix(int noLevelsToDelete) {
	auto compressedImage = new std::vector<std::vector<Pixel>*>(pixelMatrix.size());
	for (int i = 0; i < compressedImage->size(); ++i) {
		(*compressedImage)[i] = new std::vector<Pixel>(pixelMatrix.size());
	}
	std::list<Node*>* nodes = createLeavesCompressedImage(noLevelsToDelete);

	for (auto it = nodes->begin(); it != nodes->end(); ++it) {
		for (int i = (*it)->info.upperLeftCorner.y; i < (*it)->info.upperLeftCorner.y + (*it)->info.edge; ++i) {
			for (int j = (*it)->info.upperLeftCorner.x; j < (*it)->info.upperLeftCorner.x + (*it)->info.edge; ++j) {
				if (i >= pixelMatrix.size() || j >= pixelMatrix.size()) {
					std::cout << "err";
				}
				(*(*compressedImage)[i])[j] = (*it)->color;
			}
		}
	}

	return { compressedImage, nodes };
}

Image* Image::compress(int noLevelsToDelete) {
	if (noLevelsToDelete > maxLevel) {
		return nullptr;
	}

	auto compressRes = compressMatrix(noLevelsToDelete);
	Image* newImage = new Image(compressRes.first, compressRes.second);
	for (int i = 0; i < compressRes.first->size(); i++) {
		delete (*compressRes.first)[i];
	}
	delete compressRes.first;

	for (auto it = compressRes.second->begin(); it != compressRes.second->end(); ++it) {
		delete *it;
	}
	delete compressRes.second;

	return newImage;
}

std::ostream& operator<<(std::ostream& os, Image& image)
{
	for (size_t i = 0; i < image.pixelMatrix.size(); ++i) {
		for (size_t j = 0; j < image.pixelMatrix.size(); ++j) {
			os << image.pixelMatrix[i][j] << " ";
		}
		os << "\n";
	}
	return os;
}

void Image::save(std::string path) {
	cv::Mat img(pixelMatrix.size(), pixelMatrix.size(), 16);
	unsigned char* p;
	for (size_t i = 0; i < img.rows; ++i) {
		for (size_t j = 0; j < img.cols; ++j) {
			p = img.ptr(i, j);
			p[2] = pixelMatrix[i][j].R;
			p[1] = pixelMatrix[i][j].G;
			p[0] = pixelMatrix[i][j].B;
		}
	}
	cv::imwrite(path, img);
}
