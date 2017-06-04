#include <cstdio>
#include <cmath>
#include <algorithm>
#include<vector>
#include<fstream>
#include<stack>
#include<iostream>
#include<map>
#include<sstream>
#include "Utility.h"
#include "BallTree.h"

using namespace std;

bool read_data(int n, int d, float** &data, const char* file_name) {
	FILE* fin = fopen(file_name, "r");
	if (!fin) {
		printf("%s doesn't exist!\n", file_name);
		return false;
	}

	int id;
	data = new float*[n];
	for (int i = 0; i < n; i++) {
		data[i] = new float[d];
		fscanf(fin, "%d", &id);
		for (int j = 0; j < d; j++) {
			fscanf(fin, "%f", &data[i][j]);
		}
	}

	printf("Finish reading %s\n", file_name);
	fclose(fin);

	return true;
}

float getDistanse(float* a, float *b, int d) {
	float r = 0;
	for (int i = 0; i < d; i++) {
		r += pow(a[i] - b[i], 2);
	}
	return r;
}

void Analyse(ball *node, int n, int d, float **data) {
	float* mean = new float[d] {.0f};
	for (int i = 0; i < n; i++) {
		for (int j = 0; j < d; j++) {
			mean[j] += data[i][j];
		}
	}
	for (int i = 0; i < d; i++) {
		mean[i] /= n;
	}

	node->CircleCenter = mean;

	float r = 0;
	for (int i = 0; i < n; i++) {
		r = max(r, getDistanse(mean, data[i], d));
	}

	node->radius = sqrt(r);
}

float* FindFurestPoint(float** data, float * po, int n, int d) {
	float* res = new float[d] {.0f};
	float max = 0;
	for (int i = 0; i < n; i++) {
		float cur = sqrt(getDistanse(po, data[i], d));
		if (max < cur) {
			max = cur;
			res = data[i];
		}
	}
	return res;
}

void Split(int n , int d, float* &a, float* &b, float** data) {
	float* randomPoint = data[0];
	a = new float[d] {.0f};
	b = new float[d] {.0f};
	a = FindFurestPoint(data, randomPoint, n, d);
	b = FindFurestPoint(data, a, n, d);
}

float** VectorToFloat(vector<float*> v) {
	int size = v.size();
	float **res = new float*[size];
	for (int i = 0; i < size; i++) {
		res[i] = v[i];
	}
	return res;
}

void openF(ball* root, map<int, float**> storage, const char* index_path) {
	// �������ļ�
	ofstream outFile;
	string tempFileName = index_path;
	tempFileName += "index.bin";
	outFile.open(tempFileName, ios_base::out | ios_base::binary);
	
	// �����ļ�
	ofstream dataFile;
	map<int, float**>::iterator mapIter;
	int pageId = 0;							// ҳ��
	int count = 0;							// ÿҳ������
	stringstream stream;
	stream << pageId;
	string temp;
	stream >> temp;
	string fileName = "page" + temp + ".bin";
	tempFileName = index_path;
	tempFileName += fileName;
	dataFile.open(tempFileName, ios_base::out | ios_base::binary);

	// ��һ���������
	float zeroPoint[50] = {0.0};
	for (int i = 0; i < 50; i++) {
		zeroPoint[i] = 0.0;
	}

	if (!outFile.is_open() || !dataFile.is_open()) {
		exit(EXIT_FAILURE);
	}

	stack<ball*> Stack;
	while (root || !Stack.empty()) {

		while (root) {
			Stack.push(root);
			outFile.write((char*)&root, sizeof root);
			
			// �����Ҷ�ӽ����д��Ӳ����
			if (root->leftball == NULL && root->rightball == NULL) {

				count++;
				// ��ҳ����ʼ��
				if (count >= 16) {
					dataFile.close();
					dataFile.clear();
					count = 0;
					pageId++;
					stringstream stream;
					stream << pageId;
					string temp;
					stream >> temp;
					string fileName = "page" + temp + ".bin";
					string tempFileName = index_path;
					tempFileName += fileName;
					dataFile.open(tempFileName, ios_base::out | ios_base::binary);
					if (!dataFile.is_open()) {
						exit(EXIT_FAILURE);
					}
				}

				// ����д��Ӳ�̣�����λ����0���
				mapIter = storage.find(root->bid);
				if (mapIter != storage.end()) {
					// dataFile.write((char*)&root->bid, sizeof root->bid);
					for (int i = 0; i < root->datanum; i++) {
						dataFile.write((char*)&mapIter->second[i], sizeof mapIter->second[0][0] * 50);
						/*����*/
						for (int j = 0; j < 50; j++) {
							cout << mapIter->second[i][j] << " ";
						}
						cout << endl;
					}
					for (int i = 0; i < 20 - root->datanum; i++) {
						dataFile.write((char*)&zeroPoint, sizeof zeroPoint);
						/*����*/
						/*for (int j = 0; j < 50; j++) {
							cout << zeroPoint[j] << " ";
						}
						cout << endl;*/
					}
				}
			}
			root = root->leftball;
		}
		root = Stack.top();
		Stack.pop();
		root = root->rightball;
	}
	outFile.close();
	dataFile.close();
}

void readF(ball* &root, const char* index_path) {
	ifstream inFile;
	string tempFileName = index_path;
	tempFileName += "index.bin";
	inFile.open(tempFileName, ios_base::in | ios_base::binary);
	if (!inFile.is_open()) {
		exit(EXIT_FAILURE);
	}
	inFile.read((char*)&root, sizeof root);
	ball *head = root;
	stack<ball*> Stack;
	bool flag = false;
	while (root || !Stack.empty()) {

		while (root) {
			Stack.push(root);
			if (flag) {
				inFile.read((char*)&root, sizeof root);
			}
			else {
				flag = true;
			}
			root = root->leftball;
		}
		root = Stack.top();
		Stack.pop();
		root = root->rightball;
	}
	root = head;
	inFile.close();

	/*ifstream dataFile;
	dataFile.open("page0.bin", ios_base::in | ios_base::binary);
	if (!dataFile.is_open()) {
		exit(EXIT_FAILURE);
	}
	

	cout << endl << endl << endl << endl;
	int count = 300;
	while (count--) {
		float *point = new float[50];
		dataFile.read((char*)&point, sizeof 4 * 50);
		if (point == NULL) {
			cout << "�˴������" << endl;
		}
		else {
			for (int i = 0; i < 50; i++) {
				cout << point[i] << " ";
			}
			cout << endl;
		}
		
	}
	dataFile.close();*/
}

void output(ball* root) {
	stack<ball*> Stack;
	while (root || !Stack.empty()) {

		while (root) {
			Stack.push(root);
			cout << root->bid << " ";
			cout << "Բ�ģ� [";
			for (int i = 0; i < 50; i++) {
				printf("%f ,", root->CircleCenter[i]);
			}
			cout << "]" << endl;
			cout << " " << root->radius << endl;
			root = root->leftball;
		}
		root = Stack.top();
		Stack.pop();
		root = root->rightball;
	}
}