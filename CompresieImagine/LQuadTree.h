#pragma once

#include <iostream>
#include <vector>
#include <list>
#include <stack>
#include <cmath>

#define VIER 4 

struct Point2D {
	int x, y;

	Point2D() {}
	Point2D(int x,int y): x(x), y(y) {}
	Point2D(const Point2D& point) { this->x = point.x; this->y = point.y; }
};

struct Pixel {
	unsigned char R, G, B;

	Pixel() : R(0), G(0), B(0) {}

	Pixel(const Pixel& ixel) {
		this->R = ixel.R;
		this->G = ixel.G;
		this->B = ixel.B;
	}

	Pixel(unsigned char r, unsigned char g, unsigned char b) {
		this->R = r;
		this->G = g;
		this->B = b;
	}

	Pixel(unsigned char* p) {
		this->R = p[2];
		this->G = p[1];
		this->B = p[0];
	}

	bool operator==(const Pixel& ixel) {
		if (this->R != ixel.R || this->G != ixel.G || this->B != ixel.B)
			return false;
		return true;
	}

	bool operator!=(const Pixel& ixel) {
		return !(*this == ixel);
	}

	friend std::ostream& operator<<(std::ostream& os, Pixel& pixel) {
		os << "( " << (int)pixel.R << ", " << (int)pixel.G << ", " << (int)pixel.B << " )";
		return os;
	}
};

/*struct Pixel {
	int c;

	Pixel() : c(0) {}

	Pixel(const Pixel& ixel) {
		this->c = ixel.c;
	}

	Pixel(int c) {
		this->c = c;
	}

	bool operator==(const Pixel& ixel) {
		if (this->c != ixel.c)
			return false;
		return true;
	}

	bool operator!=(const Pixel& ixel) {
		return !(*this == ixel);
	}

	friend std::ostream& operator<<(std::ostream& os, Pixel& pixel) {
		os << (int)pixel.c << " ";
		return os;
	}
};*/

class LQuadTree {
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
		Pixel color;
		
		Node(int level, Info info, std::vector<int> code) : level(level), info(info), code(std::move(code)), color(Pixel()) {}
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

	Node* mergeNodes(const std::vector<Node*>&);
	Node* createLowerLeafNode(Node*, int, int);
	Node* constructChildNode(Node*&, std::vector<int>&, int = 0, int = 0);

	virtual ~LQuadTree();
};

Pixel findAverage(const std::vector<LQuadTree::Node*>&);