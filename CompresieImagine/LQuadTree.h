#pragma once

#include <iostream>
#include <vector>
#include <list>
#include <stack>
#include <cmath>

#define VIER 4 

enum ERRORS {
	BAD_SIZE = 69
};

enum PERCENTAGE {
	LOW = 2, 
	MEDIUM = 4, 
	HIGH = 8
};

struct Point2D {
	int x, y;

	Point2D() {}
	Point2D(int x,int y): x(x), y(y) {}
	Point2D(const Point2D& point) { this->x = point.x; this->y = point.y; }
};

class LQuadTree
{
public:
	struct Node {
		int level;
		struct Info{
			Point2D upperLeftCorner;
			int edge;

			Info() {}
			Info(Point2D upperLeft,int edge): upperLeftCorner(upperLeft), edge(edge){}
			Info(const Info& inf) { this->upperLeftCorner = inf.upperLeftCorner; this->edge = inf.edge; }
		} info;
		std::vector<int> code;
		float color;
		
		Node(int level, Info info, std::vector<int> code) : level(level), info(info), code(std::move(code)), color(0) {}
		Node(const Node& node) {
			this->level = node.level;
			this->info = node.info;
			this->color = node.color;
			this->code = node.code;
		}
		std::vector<std::vector<int>>* split() {
			auto children = new std::vector<std::vector<int>>(VIER, std::vector<int>(this->code));
			for (int i = 1; i < VIER; i++){
				(*children)[i][this->level] += i;
			}
			return children;
		}
	};

	void showListItems();

protected:
	int codeSize;
	std::list<Node*> leafNodes;

	Node* mergeNodes(const std::vector<Node*>& nodes);
	Node* createLowerLeafNode(Node* it, PERCENTAGE percentage, int percPow);
	Node* constructChildNode(Node*&, std::vector<int>&, int = 0, int = 0);
	std::vector<Node*>* createLeavesCompressedImage(PERCENTAGE);

	virtual ~LQuadTree();
};

float findAverage(const std::vector<LQuadTree::Node*>&);