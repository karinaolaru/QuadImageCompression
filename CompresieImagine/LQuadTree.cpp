#include "LQuadTree.h"

LQuadTree::Node* LQuadTree::mergeNodes(const std::vector<Node*>& nodes) {
	Node* uppestNode = nodes[nodes.size() - 1];
	for (int i = 0; i < nodes.size(); i++) {
		if (nodes[i]->info.upperLeftCorner.y < uppestNode->info.upperLeftCorner.y) {
			uppestNode = nodes[i];
		}
		else if (nodes[i]->info.upperLeftCorner.y == uppestNode->info.upperLeftCorner.y && nodes[i]->info.upperLeftCorner.x < uppestNode->info.upperLeftCorner.x) {
			uppestNode = nodes[i];
		}
	}

	//Node* newNode = new Node(nodes[VIER - 1]->level - 1, Node::Info(nodes[VIER - 1]->info.upperLeftCorner, nodes[VIER - 1]->info.edge * 2), nodes[VIER - 1]->code);
	Node* newNode = new Node(uppestNode->level - 1, Node::Info(uppestNode->info.upperLeftCorner, uppestNode->info.edge * 2), uppestNode->code);
	newNode->color = findAverage(nodes);
	return newNode;
}

/*LQuadTree::Node* LQuadTree::createLowerLeafNode(Node* it, int noLevelsToDelete) {
	std::vector<int> newCode(codeSize);
	for (int i = 0; i < newCode.size(); ++i) {
		newCode[i] = it->code[i];
	}
	Node* node = new Node(
		it->level,
		Node::Info(
			Point2D(it->info.upperLeftCorner.x, it->info.upperLeftCorner.y),
			it->info.edge
		),
		newCode
	);
	node->color = it->color;
	return node;
}*/

LQuadTree::Node* LQuadTree::constructChildNode(Node*& parent, std::vector<int>& code, int addX, int addY) {
	return new Node(
		parent->level + 1,
		Node::Info(Point2D(parent->info.upperLeftCorner.x + addX, parent->info.upperLeftCorner.y + addY), parent->info.edge / 2),
		code
	);
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

/*Pixel findAverage(const std::vector<LQuadTree::Node*>& nodes) {
	int sumR = 0, sumG = 0, sumB = 0;
	int pixels = nodes.size();
	for (const auto& node : nodes) {
		sumR += node->color.R;
		sumG += node->color.G;
		sumB += node->color.B;
	}
	return Pixel((unsigned char)(sumR / pixels), (unsigned char)(sumG / pixels), (unsigned char)(sumB / pixels));
}*/
Pixel findAverage(const std::vector<LQuadTree::Node*>& nodes) {
	int sumR = 0;
	int pixels = nodes.size();
	for (const auto& node : nodes) {
		sumR += node->color.c;
	}
	return Pixel(sumR / pixels);
}

LQuadTree::~LQuadTree() {
	for (auto& item : leafNodes) {
		delete item;
	}
}