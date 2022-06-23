#include "Image.h"
#include <memory>

int compareVectors(std::vector<int> v1, std::vector<int> v2) {
	for (int i = 0; i < v1.size(); i++) {
		if (v1[i] > v2[i]) {
			return -1;
		}
		else if (v1[i] < v2[i]) {
			return 1;
		}
	}
	return 0;
}

Image::Image(const std::vector<std::vector<Pixel>>& matrix) {
	construct(matrix);
}

Image::Image(std::vector<std::vector<Pixel>*>* matrix, std::vector<Node*>* nodes) {
	pixelMatrix.resize(matrix->size(), std::vector<Pixel>(matrix->size()));
	for (int i = 0; i < matrix->size(); i++) {
		for (int j = 0; j < matrix->size(); j++) {
			pixelMatrix[i][j] = (*(*matrix)[i])[j];
		}
	}
	for (int i = 0; i < nodes->size(); i++) {
		this->leafNodes.emplace_back(new Node(*(*nodes)[i]));
	}
	this->codeSize = log2(matrix->size());
	int currMaxLevel = 0;
	for (int i = 0; i < nodes->size(); i++) {
		if ((*nodes)[i]->level > currMaxLevel) currMaxLevel = (*nodes)[i]->level;
	}
	this->maxLevel = currMaxLevel;
}



/*Image::Image(std::string path) {
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
}*/

void Image::construct() {
	float size = log2(pixelMatrix.size());
	if (pixelMatrix.size() < 8 || size != (int)size || pixelMatrix.size() != pixelMatrix[0].size()) {
		return;
	}
	codeSize = (int)size;
	std::vector<int> firstCode(codeSize, 0);
	leafNodes.emplace_back(new Node(0, Node::Info(Point2D(0, 0), pixelMatrix.size()), firstCode));
	divide();
	std::cout << "Image has been loaded\n";
}

void Image::construct(const std::vector<std::vector<Pixel>>& pixelMatrix) {
	this->pixelMatrix = pixelMatrix;
	construct();
}

void Image::divide() {
	struct DivNode {
		Pixel color;
		int depth = 0;
		std::shared_ptr<DivNode> parent;
		std::vector<int> code;

		DivNode(){}
		DivNode(std::vector<int> m_code, Pixel m_color, int m_depth, std::shared_ptr<DivNode> m_parent) {
			color = m_color;
			depth = m_depth;
			parent = m_parent;
			code = m_code;
		}
		DivNode(Pixel m_color, int m_depth, std::shared_ptr<DivNode> m_parent) {
			color = m_color;
			depth = m_depth;
			parent = m_parent;
		}
	};

	std::shared_ptr<DivNode> firstNode = std::make_shared<DivNode>(std::vector<int>(codeSize, 0), pixelMatrix[0][0], codeSize, nullptr);
	std::vector<std::shared_ptr<DivNode>> nodes(pixelMatrix.size(), nullptr);
	int edge = pow(2, codeSize - 1);
	for (int i = 0; i < edge; i++) {
		nodes[i] = firstNode;
	}
	std::shared_ptr<DivNode> currNode;
	int xt, yt, depth = 0;
	for (int row = 0; row < edge * 2; row++) {
		for (int col = 0; col < edge * 2; col++) {
			currNode = nodes[col / 2];
			if (currNode->color != pixelMatrix[row][col]) {
				xt = col; yt = row; depth = 0;
				while (xt % 2 == 0 && yt % 2 == 0 && depth < codeSize) {
					xt /= 2; yt /= 2; depth++;
				}
				if (depth != 0) {
					std::shared_ptr<DivNode> newActiveNode = std::make_shared<DivNode>(pixelMatrix[row][col],depth,currNode);
					int end = pow(2, depth - 1);
					for (int j = col / 2; j < col / 2 + end; j++) {
						nodes[j] = newActiveNode;
					}
				}
				insert(calculateCode(row, col, codeSize));
			}
			int power = pow(2, currNode->depth);
			int T = 0;
			while ((row + 1) % power == 0 && (col + 1) % power == 0) {
				T = currNode->depth;
				currNode = currNode->parent;
				if (currNode == nullptr)
					break;
				power = pow(2, currNode->depth);
			}
			power = pow(2, T - 1);
			for (int i = col / 2; i >= col / 2 - power + 1; i--) {
				nodes[i] = currNode;
			}
		}
	}
}

/*bool Image::shallDivide(Node*& parent) {
	Pixel firstElem = pixelMatrix[parent->info.upperLeftCorner.y][parent->info.upperLeftCorner.x];
	for (int i = parent->info.upperLeftCorner.y; i < parent->info.upperLeftCorner.y + parent->info.edge; i++) {
		for (int j = parent->info.upperLeftCorner.x; j < parent->info.upperLeftCorner.x + parent->info.edge; j++) {
			if (pixelMatrix[i][j] != firstElem) {
				return true;
			}
		}
	}
	return false;
}*/

std::vector<LQuadTree::Node*>* Image::createLeavesCompressedImage(int noLevelsToDelete) {
	auto nodes = new std::vector<Node*>();
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

void Image::insert(std::vector<int> code) {
	std::list<Node*>::iterator it;
	while (true) {
		for (it = leafNodes.begin(); it != leafNodes.end(); ++it) {
			if (compareVectors((*it)->code, code) == -1) break;
		}
		--it;
		if (compareVectors((*it)->code, code) == 0) {
			return;
		}
		
		std::vector<std::vector<int>>* codes = (*it)->split();
		std::vector<Node*> children({ constructChildNode(*it, (*codes)[0]), constructChildNode(*it, (*codes)[1], (*it)->info.edge / 2),
								constructChildNode(*it, (*codes)[2], 0, (*it)->info.edge / 2), constructChildNode(*it, (*codes)[3], (*it)->info.edge / 2, (*it)->info.edge / 2) });
		leafNodes.insert(it, children[0]);
		leafNodes.insert(it, children[1]);
		leafNodes.insert(it, children[2]);
		leafNodes.insert(it, children[3]);
		leafNodes.remove(*it);
	}
}

std::vector<int> Image::base2Transformation(int nr, int codeSize) {
	std::vector<int> newNr(codeSize, 0);
	int i = 1, r;
	while (nr > 0) {
		r = nr % 2;
		newNr[newNr.size() - i] = r;
		nr /= 2;
		i++;
	}
	return newNr;
}

std::vector<int> Image::calculateCode(int row, int col, int codeSize) {
	std::vector<int> code(codeSize);
	std::vector<int> rowBase2 = base2Transformation(row, codeSize);
	std::vector<int> colBase2 = base2Transformation(col, codeSize);
	for (int i = 0; i < rowBase2.size(); i++) {
		code[i] = colBase2[i] + rowBase2[i] * 2;
	}
	return code;
}

std::pair<std::vector<std::vector<Pixel>*>*, std::vector<LQuadTree::Node*>*> Image::compressMatrix(int noLevelsToDelete) {
	auto compressedImage = new std::vector<std::vector<Pixel>*>(pixelMatrix.size());
	for (int i = 0; i < compressedImage->size(); ++i) {
		(*compressedImage)[i] = new std::vector<Pixel>(pixelMatrix.size());
	}
	std::vector<Node*>* nodes = createLeavesCompressedImage(noLevelsToDelete);

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

/* void Image::save(std::string path) {
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
}*/
