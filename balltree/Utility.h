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
#include <string>
using namespace std;

#define L                     256
#define N0                    20
#define BYTES_PER_PAGE        65536

struct Ball {
	Ball* parent;
	Ball* leftball;
	Ball* rightball;
	int bid;
	int pid;  // ��Ӧ�����ݿ�����һҳ
	int offset;  // ��Ӧ�����ݿ���ҳ�еĵڼ���
	int datanum;
	float* center;
	float radius;
	// ��Ա����
	Ball();
	~Ball();
	void removeRecursively(Ball * ball);
	void clear();
};

struct Quadball {
	Quadball* ball1;
	Quadball* ball2;
	Quadball* ball3;
	Quadball* ball4;
	float* center;
	float radius;
	// ��Ա����
	Quadball();
	~Quadball();
	void removeRecursively(Quadball *ball);
	void clear();
};

struct Point {
	int id;
	float *data;
	// ��Ա����
	Point();
	~Point();
	void clear();
};

struct Block {
	// ��Ա����
	Point *points;
	// ��Ա����
	Block();
	~Block();
	void clear();
};

struct Page {
	// ��Ա����
	Block *blocks;
	// ��Ա����
	Page();
	~Page();
	void clear();
	void saveToDisk(const int pid, const string index_path);
	void loadFromDisk(const int pid, const string index_path);
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

void openF(Ball* root, map<int, Point*> storage, const char* index_path, int dim);

int readF(Ball* &root, const char* index_path, int &dim);

float getMax(int d, float* query, Ball* Root);

void output(Ball* root);

void outputfloat2(float** f, int n, int d);

void displayCenter(float* f, int d);

float getLength(int d, float* query);

float getInnerproduct(int d, float * query, float * vec);

#endif
