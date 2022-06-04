#include "Image.h"

Image::Image(const std::vector<std::vector<float>>& matrix) {
	construct(matrix);
}

Image::Image(std::vector<std::vector<float>*>* matrix, std::vector<Node*>* nodes) {
	pixelMatrix.resize(matrix->size(), std::vector<float>(matrix->size()));
	for (int i = 0; i < matrix->size(); i++) {
		for (int j = 0; j < matrix->size(); j++) {
			pixelMatrix[i][j] = (*(*matrix)[i])[j];
		}
	}
	for (int i = 0; i < nodes->size(); i++) {
		this->leafNodes.emplace_back(new Node(*(*nodes)[i]));
	}
}

void Image::construct(const std::vector<std::vector<float>>& pixelMatrix) {
	this->pixelMatrix = pixelMatrix;

	float size = log2(pixelMatrix.size());
	if (pixelMatrix.size() < 16 || size != (int)size || pixelMatrix.size() != pixelMatrix[0].size()) {
		return;
	}
	codeSize = (int)size;
	std::vector<int> firstCode(codeSize, 0);
	leafNodes.emplace_back(new Node(0, Node::Info(Point2D(0, 0), pixelMatrix.size()), firstCode));
	divide(leafNodes.front());
}

void Image::divide(Node*& parent) {
	if ((parent->info.edge == 1) || (shallDivide(parent) == false)) {
		parent->color = pixelMatrix[parent->info.upperLeftCorner.y][parent->info.upperLeftCorner.x];
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
	int firstElem = pixelMatrix[parent->info.upperLeftCorner.y][parent->info.upperLeftCorner.x];
	for (int i = parent->info.upperLeftCorner.y; i < parent->info.upperLeftCorner.y + parent->info.edge; i++) {
		for (int j = parent->info.upperLeftCorner.x; j < parent->info.upperLeftCorner.x + parent->info.edge; j++) {
			if (pixelMatrix[i][j] != firstElem) {
				return true;
			}
		}
	}
	return false;
}

std::pair<std::vector<std::vector<float>*>*, std::vector<LQuadTree::Node*>*> Image::compressMatrix(PERCENTAGE percentage) {
	int newMatrixSize = pixelMatrix.size() / percentage;
	auto compressedImage = new std::vector<std::vector<float>*>(newMatrixSize);
	for (int i = 0; i < compressedImage->size(); ++i) {
		(*compressedImage)[i] = new std::vector<float>(newMatrixSize);
	}
	std::vector<Node*>* nodes = createLeavesCompressedImage(percentage);

	for (auto it = nodes->begin(); it != nodes->end(); ++it) {
		for (int i = (*it)->info.upperLeftCorner.y; i < (*it)->info.upperLeftCorner.y + (*it)->info.edge; ++i) {
			for (int j = (*it)->info.upperLeftCorner.x; j < (*it)->info.upperLeftCorner.x + (*it)->info.edge; ++j) {
				(*(*compressedImage)[i])[j] = (*it)->color;
			}
		}
	}

	return { compressedImage, nodes };
}

Image* Image::compress(PERCENTAGE percentage) {
	auto compressRes = compressMatrix(percentage);
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

/*std::vector<LQuadTree::Node*>* Image::createLeavesCompressedImage(PERCENTAGE percentage) {
	auto nodes = new std::vector<Node*>();
	std::vector<int> currentIndexInLevel(codeSize + 1);
	std::vector<Node*> neighbours(VIER);

	for (const auto& it : this->leafNodes) {
		if (it->info.edge >= percentage) {
			nodes->emplace_back(new Node(*it));
			continue;
		}

		currentIndexInLevel[it->level]++;
		nodes->emplace_back(new Node(*it));
		for (int level = it->level; currentIndexInLevel[level] == VIER && level >= 0; --level) {
			currentIndexInLevel[level] = 0;
			for (int i = 0; i < VIER; i++) {
				neighbours[i] = nodes->back();
				nodes->pop_back();
			}
			nodes->emplace_back(mergeNodes(neighbours));
		}
	}
	return nodes;
}*/

std::istream& operator>>(std::istream& is, Image& image)
{
	int size;
	is >> size;
	image.pixelMatrix = std::vector<std::vector<float>>(size, std::vector<float>(size));
	for (size_t i = 0; i < size; ++i) {
		for (size_t j = 0; j < size; ++j) {
			is >> image.pixelMatrix[i][j];
		}
	}
	image.construct(image.pixelMatrix);
	return is;
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
