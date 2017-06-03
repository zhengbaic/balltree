#include<iostream>
#include<vector>
#include<map>
#include "BallTree.h"
#include "Utility.h"
using namespace std;

static int bid = 0;
map<int, float**> storage;
float getMax(int d, float* query, ball* Root);
float getLength(int d, float* query);
float getInnerproduct(int d, float * query, float * vec);

void outputfloat2(float** f, int n, int d) {
	for (int i = 0; i < n; i++) {
		for (int j = 0; j < d; j++) {
			printf("%f", f[i][j]);
		}
	}
}
void outputfloat(float* f, int d) {
	cout << "圆心： [";
	for (int i = 0; i < d; i++) {
		printf("%f ,", f[i]);
	}
	cout << "]"<<endl;
}

int BallTree::bid = 0;
BallTree::BallTree() {
	dimesion = 0;
	num = 0;
	root = NULL;
}
BallTree::~BallTree() {}

bool BallTree::buildTree(int n, int d, float **data) {
	dimesion = d;
	num = n;
	//outputfloat2(data, n, d);
	buildBall(root, n, d, data);

	return true;
}

void BallTree::buildBall(ball* &node, int n, int d, float **data) {
	node = new ball();
	Analyse(node, n, d, data);//得到圆心根半径（Utility.cpp）
	printf("半径 ： %f \n", node->radius);
	outputfloat(node->CircleCenter, d);
	if (n <= N0) {
		//叶子
		storage.insert(map<int, float**>::value_type(bid, data));
		node->bid = bid;
		bid++;
	}
	else {
		float* A = NULL;
		float* B = NULL;
		Split(n, d, A, B, data);
		float ** leftdata;//A
		float ** rightdata;//B
		vector<float*> leftd;
		vector<float*> rightd;
		for (int i = 0; i < n; i++) {
			if (getDistanse(A, data[i], d) < getDistanse(B, data[i], d)) {//A
				leftd.push_back(data[i]);
			}
			else {
				rightd.push_back(data[i]);
			}
		}
		leftdata = VectorToFloat(leftd);
		rightdata = VectorToFloat(rightd);

		buildBall(node->leftball, leftd.size(), d, leftdata);
		buildBall(node->rightball, rightd.size(), d, rightdata);
	}
}

int BallTree::mipSearch(int d,float* query) {
	float Max = 0;
	Max = eval(d, query, Max, root->leftball);
	if (Max < getMax(d,query, root->rightball)) {
		eval(d, query, Max, root->rightball);
	}
	return target_bid;
}

float BallTree::eval(int d, float* query, float Max,ball* Root) {
	float temp;
	if (Root->leftball == NULL && Root->rightball == NULL) {
		for (int i = 0; i <= sizeof(storage[Root->bid]) / sizeof(float) / d; i++) {
			temp = getInnerproduct(d, query, storage[Root->bid][i]);
			if (Max < temp) {
				Max = temp;
				target_bid = Root->bid;
			}
		}
		return Max;
	}
	Max = eval(d,query,Max, Root->leftball);
	if (Max < getMax(d, query, Root->rightball)) {
		Max = eval(d,query,Max, Root->rightball);
	}
	return Max;
}

float getMax(int d,float* query, ball* Root) {
	float Max = 0;
	for (int i = 0; i < d; i++) {
		Max += Root->CircleCenter[i] * query[i];
	}
	Max += Root->radius * getLength(d,query);
	return Max;
}

float getLength(int d, float* query) {
	float Length = 0;
	for (int i = 0; i < d; i++) {
		Length += pow(query[i], 2);
	}
	Length = sqrt(Length);
	return Length;
}

float getInnerproduct(int d, float * query, float * vec) {
	float Innerproduct = 0;
	for (int i = 0; i < d; i++) {
		Innerproduct += query[i] * vec[i];
	}
	return Innerproduct;
}