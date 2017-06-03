#ifndef __UTILITY_H
#define __UTILITY_H

#include <vector>
#include <cstdio>
#include <cmath>
#include <algorithm>
#include <iostream>
#define L 20
using namespace std;

struct ball {
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
		leftball = rightball = NULL;
	}
};

void Analyse(ball *node, int n, int d, float **data);

bool read_data(int n, int d, float** &data, const char* file_name);

void Split(int n, int d, float* &a, float* &b, float** data);

float getDistanse(float* a, float *b, int d);

float** VectorToFloat(vector<float*> v);

void outputfloat2(float** f, int n, int d);

void displayCenter(float* f, int d);

void insertOnePointAndUpdateNode(ball *b, float *data) {
	
}

float getMax(int d, float* query, ball* Root);

float getLength(int d, float* query);

float getInnerproduct(int d, float * query, float * vec);

#endif
