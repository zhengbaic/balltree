#include<iostream>
#include<vector>
#include<map>
#include "BallTree.h"
#include "Utility.h"
using namespace std;

static int bid = 0;
map<int, float**> storage;

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

void BallTree::buildBall(ball* node, int n, int d, float **data) {
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
