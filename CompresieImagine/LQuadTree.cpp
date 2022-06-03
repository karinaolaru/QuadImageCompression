#include "LQuadTree.h"


LQuadTree::LQuadTree(const std::vector<std::vector<float>>& matrix) {
	construct(matrix);
}

void LQuadTree::construct(const std::vector<std::vector<float>>& pixelMatrix) {
	this->pixelMatrix = pixelMatrix;

	float size = log2(pixelMatrix.size());
	if (pixelMatrix.size() < 16 || size != (int)size || pixelMatrix.size() != pixelMatrix[0].size()) {
		return;
	}
	codeSize = (int)size;
	std::vector<int> firstCode(codeSize,0);
	leafNodes.emplace_back(new Node(0, Node::Info(Point2D(0, 0), pixelMatrix.size()), firstCode));
	divide(leafNodes.front());
}

void LQuadTree::divide(Node*& parent) {
	if ((parent->info.edge == 1) || (shallDivide(parent) == false)) {
		parent->color = pixelMatrix[parent->info.upperLeftCorner.y][parent->info.upperLeftCorner.x];
		return;
	}

	std::vector<std::vector<int>>* codes = parent->split();
	std::vector<Node*> children({ constructNode(parent, (*codes)[0]), constructNode(parent, (*codes)[1], parent->info.edge / 2),
								  constructNode(parent, (*codes)[2], 0, parent->info.edge / 2), constructNode(parent, (*codes)[3], parent->info.edge / 2, parent->info.edge / 2) });
	leafNodes.remove(parent);
	delete codes;

	for (auto& child : children) {
		leafNodes.emplace_back(child);
		divide(child);
	}
}

bool LQuadTree::shallDivide(Node* parent)
{
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

LQuadTree::Node* LQuadTree::constructNode(Node* parent, std::vector<int>& code, int addX, int addY) {
	return new Node(
		parent->level + 1,
		Node::Info(Point2D(parent->info.upperLeftCorner.x + addX, parent->info.upperLeftCorner.y + addY), parent->info.edge / 2),
		code
	);
}

std::vector<std::vector<float>>* LQuadTree::compress(PERCENTAGE percentage) {
	int newMatrixSize = pixelMatrix.size() / percentage;
	auto compressedImage = new std::vector<std::vector<float>>(newMatrixSize, std::vector<float>(newMatrixSize));
	std::vector<Node*>* nodes = createLeafsCompressedImage(percentage);
	
	int yAxis, xAxis, newEdge;
	for (auto it = nodes->begin(); it != nodes->end(); ++it) {
		yAxis = (*it)->info.upperLeftCorner.y / percentage;
		xAxis = (*it)->info.upperLeftCorner.x / percentage;
		newEdge = (*it)->info.edge / percentage;

		for (int i = yAxis; i < yAxis + newEdge; ++i) {
			for (int j = xAxis; j < xAxis + newEdge; ++j) {
				(*compressedImage)[i][j] = (*it)->color;
			}
		}
	}

	delete nodes;
	return compressedImage;
}

std::vector<LQuadTree::Node*>* LQuadTree::createLeafsCompressedImage(PERCENTAGE percentage) {
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
}

LQuadTree::Node* LQuadTree::mergeNodes(const std::vector<Node*>& nodes) {
	Node* newNode = new Node(nodes[0]->level - 1, Node::Info(nodes[0]->info.upperLeftCorner, nodes[0]->info.edge * 2), std::move(nodes[0]->code));
	newNode->color = findAverage(nodes);
	return newNode;
}

void LQuadTree::showListItems() {
	for (const auto& item : this->leafNodes) {
		std::cout << "level: " << item->level << ", {edge:" << item->info.edge << ", point: (" << item->info.upperLeftCorner.x << ", " << item->info.upperLeftCorner.y << ")}, code: ";
		for (int i = 0; i < this->codeSize; i++) {
			std::cout << item->code[i];
		}
		std::cout << "\n";
	}
}

float findAverage(const std::vector<LQuadTree::Node*>& nodes) {
	float sum = 0;
	for (const auto& node : nodes) {
		sum += node->color;
	}
	return sum / nodes.size();
}
