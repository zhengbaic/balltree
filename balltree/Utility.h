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
#define L 20

using namespace std;

struct point {
	int id;
	float *data;

	point() {
		data = NULL;
		id = -1;
	}
	~point() {
		//delete data;
	}
};

struct ball {
	ball* parent;
	ball* leftball;
	ball* rightball;
	int datanum;
	float* CircleCenter;
	float radius;
	int bid;

	ball() {
		datanum = -1;
		radius = 0;
		bid = -1;
		parent =  leftball = rightball = NULL;
	}
};

struct Quadball {
	Quadball* ball1;
	Quadball* ball2;
	Quadball* ball3;
	Quadball* ball4;
	float* CircleCenter;
	float radius;

	Quadball() {
		radius = -1;
		ball1 = ball2 = ball3 = ball4 = NULL;
	}
};

void Analyse(ball *node, int n, int d, float **data);

void QuadAnalyse(Quadball *node, int n, int d, float **data);

bool read_data(int n, int d, float** &data, const char* file_name);

void Split(int n, int d, float* &a, float* &b, float** data);

void QuadSplit(int n, int de, float* &a, float* &b, float* &c, float* &d, float** data, float* & meanW);

float getDistanse(float* a, float *b, int d);

point* VectorToPoint(vector<point> v);

float** VectorToFloat(vector<float*> v);

void openF(ball* root, map<int, point*> storage, const char* index_path);

int readF(ball* &root, const char* index_path);

void output(ball* root);

void outputfloat2(float** f, int n, int d);

void displayCenter(float* f, int d);

float getMax(int d, float* query, ball* Root);

float getLength(int d, float* query);

float getInnerproduct(int d, float * query, float * vec);

bool traverseAndReplace(ball *root, ball *oldBall, ball *newBall);

#endif
