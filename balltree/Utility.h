#ifndef __UTILITY_H
#define __UTILITY_H

#include <stdio.h>
#include <math.h>
#include <vector>
#include <map>
#include <set>
#include <stack>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <sstream>
using namespace std;

#define L                     20
#define N0                    20
#define BYTES_PER_PAGE        65536

struct Ball {
	Ball* parent;
	Ball* leftball;
	Ball* rightball;
	int bid;
	int pid;
	int datanum;
	float* center;
	float radius;

	Ball() {
		parent = leftball = rightball = NULL;
		center = NULL;
		datanum = -1;
		radius = 0;
		pid = bid = -1;
	}

	~Ball() {
		// clear();
	}
	void removeRecursively(Ball *ball) {
		if (ball == NULL) {
			return;
		}

		ball->datanum = -1;
		ball->radius = 0;
		ball->pid = ball->bid = -1;
		if (ball->center != NULL) {
			delete[] ball->center;
		}
		removeRecursively(ball->leftball);
		removeRecursively(ball->rightball);
	}

	void clear() {
		// 使父节点指向NULL
		if (parent != NULL) {
			if (parent->leftball == this) {
				parent->leftball = NULL;
			} else {
				parent->rightball = NULL;
			}
		}

		// 递归删除子节点
		removeRecursively(this);
	}
};

struct Quadball {
	Quadball* ball1;
	Quadball* ball2;
	Quadball* ball3;
	Quadball* ball4;
	float* center;
	float radius;

	Quadball() {
		radius = -1;
		center = NULL;
		ball1 = ball2 = ball3 = ball4 = NULL;
	}

	~Quadball() {
		// clear();
	}

	void removeRecursively(Quadball *ball) {
		if (ball == NULL) {
			return;
		}

		ball->radius = -1;
		if (ball->center != NULL) {
			delete[] ball->center;
		}
		removeRecursively(ball->ball1);
		removeRecursively(ball->ball2);
		removeRecursively(ball->ball3);
		removeRecursively(ball->ball4);
	}

	void clear() {
		removeRecursively(this);
	}
};

struct Point {
	int id;
	float *data;

	Point() {
		id = -1;
		data = NULL;
	}

	~Point() {
		// clear();
	}

	void clear() {
		id = -1;
		if (data != NULL) {
			delete[] data;
		}
	}
};

struct Block {
	// 成员变量
	int bid;
	int pid;
	Point *points;
	// 成员函数
	Block();
	~Block();
	void clear();
	void init();
};

struct Page {
	// 成员变量
	int pid;
	Block *blocks;
	// 成员函数
	Page();
	~Page();
	void clear();
	void init();  // 为一整张页开好空间
	void saveToDisk();
	void loadFromDisk(const int pid);
};

void initConstants(const int d);

bool read_data(int n, int d, float** &data, const char* file_name);

float getDistanse(float* a, float *b, int d);

void analyse(Ball *node, int n, int d, float **data);

void quadAnalyse(Quadball *node, int n, int d, float **data);

float* findFurestPoint(float** data, float * po, int n, int d);

void findFurestPoints(float** data, int n, int d, set<float*> s);

void split(int n, int d, float* &a, float* &b, float** data);

void quadSplit(int n, int de, float* &a, float* &b, float* &c, float* &d, float** data, float* & mean);

Point* vectorToPoint(vector<Point> v);

float** vectorToFloat(vector<float*> v);

void openF(Ball* root, map<int, Point*> storage, const char* index_path);

int readF(Ball* &root, const char* index_path);

float getMax(int d, float* query, Ball* Root);

void output(Ball* root);

void outputfloat2(float** f, int n, int d);

void displayCenter(float* f, int d);

float getLength(int d, float* query);

float getInnerproduct(int d, float * query, float * vec);

#endif
