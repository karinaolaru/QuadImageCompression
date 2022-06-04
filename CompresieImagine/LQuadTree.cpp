#include "LQuadTree.h"

LQuadTree::Node* LQuadTree::mergeNodes(const std::vector<Node*>& nodes) {
	Node* newNode = new Node(nodes[0]->level - 1, Node::Info(nodes[0]->info.upperLeftCorner, nodes[0]->info.edge * 2), std::move(nodes[0]->code));
	newNode->color = findAverage(nodes);
	return newNode;
}

LQuadTree::Node* LQuadTree::createLowerLeafNode(Node* it, PERCENTAGE percentage, int percPow) {
	std::vector<int> newCode(codeSize - percPow);
	for (int i = 0; i < newCode.size(); ++i) {
		newCode[i] = it->code[i];
	}
	Node* node = new Node(
		it->level,
		Node::Info(
			Point2D(it->info.upperLeftCorner.x / percentage, it->info.upperLeftCorner.y / percentage),
			it->info.edge / percentage
		),
		newCode
	);
	node->color = it->color;
	return node;
}

LQuadTree::Node* LQuadTree::constructChildNode(Node*& parent, std::vector<int>& code, int addX, int addY) {
	return new Node(
		parent->level + 1,
		Node::Info(Point2D(parent->info.upperLeftCorner.x + addX, parent->info.upperLeftCorner.y + addY), parent->info.edge / 2),
		code
	);
}

std::vector<LQuadTree::Node*>* LQuadTree::createLeavesCompressedImage(PERCENTAGE percentage) {
	auto nodes = new std::vector<Node*>();
	std::vector<int> currentIndexInLevel(codeSize + 1);
	std::vector<Node*> neighbours(VIER);

	int percPow = log2((int)percentage);
	for (const auto& it : this->leafNodes) {
		if (it->info.edge >= percentage) {
			nodes->emplace_back(createLowerLeafNode(it, percentage, percPow));
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
			Node* merged = mergeNodes(neighbours);
			nodes->emplace_back(createLowerLeafNode(merged, percentage, percPow));
			delete merged;
		}
	}
	return nodes;
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

LQuadTree::~LQuadTree() {
	for (auto& item : leafNodes) {
		delete item;
	}
}