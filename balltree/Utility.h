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

int DIMENSION;
int SIZE_OF_POINT;
int BYTES_PER_BLOCK;
int BLOCKS_PER_PAGE;

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

	Ball() {
		clear();
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

	Quadball() {
		clear();
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
		clear();
	}

	void clear() {
		id = -1;
		if (data != NULL) {
			delete[] data;
		}
	}
};

struct Block {
	int bid;
	int pid;
	bool initialized;
	Point *points;

	Block() {
		bid = pid = -1;
		initialized = false;
		points = NULL;
	}

	Block() {
		clear();
	}

	void clear() {
		bid = pid = -1;
		if (points != NULL) {
			delete[] points;
		}
 	}

	void init() {
		points = new Point[N0];
		for (int i = 0; i < N0; ++i) {
			points[i].data = new float[DIMENSION] {0.0f};
		}
		initialized = true;
	}

	void loadFromPage(Page *page, Ball *ball) {
		if (page == NULL || ball == NULL) {
			return;
		}

		if (!initialized) {
			init();
		}

		page->loadFromDisk(ball->pid);
		for (int i = 0; i < BLOCKS_PER_PAGE; ++i) {
			if (page->blocks[i].bid == ball->bid) {
				for (int j = 0; j < N0; ++j) {
					points[j].id = page->blocks[i].points[j].id;
					memcpy(points[j].data, page->blocks[i].points[j].data, sizeof(float) * DIMENSION);
				}
			}
		}
	}
};

struct Page {
	int pid;
	bool initialized;
	Block *blocks;

	Page() {
		initialized = false;
		pid = -1;
		blocks = NULL;
	}

	Page() {
		clear();
	}

	void clear() {
		pid = -1;
		if (blocks != NULL) {
			delete[] blocks;
		}
	}

	// 为一整张页开好空间
	void init() {
		blocks = new Block[BLOCKS_PER_PAGE];
		for (int i = 0; i < BLOCKS_PER_PAGE; ++i) {
			blocks[i].points = new Point[N0];
			for (int j = 0; j < DIMENSION; ++j) {
				blocks[i].points->data = new float[DIMENSION] {0.0f};
			}
		}
		initialized = true;
	}

	void saveToDisk() {
		stringstream stream;
		stream << pid;
		string temp;
		stream >> temp;
		string fileName = "page" + temp + ".bin";
		ofstream dataFile;
		dataFile.open(fileName, ios_base::out | ios_base::binary);
		if (!dataFile.is_open()) {
			exit(EXIT_FAILURE);
		}
		for (int i = 0; i < BLOCKS_PER_PAGE; ++i) {
			dataFile.write((char*)&blocks[i].bid, sizeof(int));
			for (int j = 0; j < N0; ++j) {
				dataFile.write((char*)blocks[i].points[j].data, sizeof(int) * DIMENSION);
			}
		}
	}

	void loadFromDisk(const int pid) {
		if (pid == -1) {
			return;
		}

		if (!initialized) {
			init();
		}

		// 早已经加载好了的，直接返回
		if (pid == this->pid) {
			return;
		}

		FILE *fp;
		char filename[L];
		sprintf(filename, "page%d.bin", pid);
		fp = fopen(filename, "rb");
		if (fp == NULL) {
			return;
		}
		for (int i = 0; i < BLOCKS_PER_PAGE; ++i) {
			blocks[i].pid = pid;
			fread(&blocks[i].bid, sizeof(int), 1, fp);
			for (int j = 0; j < N0; ++j) {
				fread(&blocks[i].points[j].id, sizeof(int), 1, fp);
				fread(blocks[i].points[j].data, sizeof(float), DIMENSION, fp);
			}
		}
		this->pid = pid;
	}
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
