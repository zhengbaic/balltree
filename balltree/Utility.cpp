#include "Utility.h"
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

void QuadAnalyse(Quadball *node, int n, int d, float **data) {
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

void FindFurestPoints(float** data, int n, int d, set<float*> s) {
	for (int i = 0; i < n; i++) {
		s.insert(FindFurestPoint(data, data[i], n, d));
	}
}

void Split(int n , int d, float* &a, float* &b, float** data) {
	float* randomPoint = data[0];
	a = new float[d] {.0f};
	b = new float[d] {.0f};
	a = FindFurestPoint(data, randomPoint, n, d);
	b = FindFurestPoint(data, a, n, d);
}

void QuadSplit(int n, int de, float* &a, float* &b, float* &c, float* &d, float** data, float* & mean) {
	float* randomPoint = data[0];
	a = new float[de] {.0f};
	b = new float[de] {.0f};
	c = new float[de] {.0f};
	d = new float[de] {.0f};

	a = FindFurestPoint(data, randomPoint, n, de);
	b = FindFurestPoint(data, a, n, de);

	set<float*> table;
	FindFurestPoints(data,n, de, table);
	set<float*>::iterator iter;
	float min = 1000;
	for (iter = table.begin(); iter != table.end(); iter++) {
		float cur = getDistanse(*iter, a, de) - getDistanse(*iter, b, de);
		if (cur < min) {
			min = cur;
			c = *iter;
		}
	}

	d = FindFurestPoint(data, c, n, de);
}

point* VectorToPoint(vector<point> v) {
	int size = v.size();
	point *res = new point[size];
	for (int i = 0; i < size; i++) {
		res[i].data = v[i].data;
		res[i].id = v[i].id;
	}
	return res;
}

float** VectorToFloat(vector<float*> v) {
	int size = v.size();
	float **res = new float*[size];
	for (int i = 0; i < size; i++) {
		res[i] = v[i];
	}
	return res;
}

void openF(ball* root, map<int, point*> storage, const char* index_path) {
	// 索引树文件
	ofstream outFile;
	string tempFileName = index_path;
	tempFileName += "index.bin";
	outFile.open(tempFileName, ios_base::out | ios_base::binary);
	
	// 数据文件
	ofstream dataFile;
	map<int, point*>::iterator mapIter;
	int pageId = 0;							// 页号
	int count = 0;							// 每页的数量
	stringstream stream;
	stream << pageId;
	string temp;
	stream >> temp;
	string fileName = "page" + temp + ".bin";
	tempFileName = index_path;
	tempFileName += fileName;
	dataFile.open(tempFileName, ios_base::out | ios_base::binary);

	// 做一个点填补空项
	int zeroIndex = -1;
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
			
			// 如果是叶子结点则写入硬盘中
			if (root->leftball == NULL && root->rightball == NULL) {

				count++;
				// 分页并初始化
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

				// 数据写入硬盘，不够位的用0点填补
				mapIter = storage.find(root->bid);
				if (mapIter != storage.end()) {
					for (int i = 0; i < root->datanum; i++) {
						dataFile.write((char*)&mapIter->second[i].id, 4);
						dataFile.write((char*)&mapIter->second[i].data, 4 * 50);
					}

					for (int i = 0; i < 20 - root->datanum; i++) {
						dataFile.write((char*)&zeroIndex, sizeof zeroIndex);
						dataFile.write((char*)&zeroPoint, sizeof zeroPoint);
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

	ifstream dataFile;
	dataFile.open("page0.bin", ios_base::in | ios_base::binary);
	if (!dataFile.is_open()) {
		exit(EXIT_FAILURE);
	}
	

	cout << endl << endl << endl << endl;
	int count = 300;
	while (count--) {
		int index = 0;
		dataFile.read((char*)index, 4);
		float *point = new float[50];
		dataFile.read((char*)&point, 4 * 50);
		if (point == NULL) {
			cout << "此处是零点" << endl;
		}
		else {
			for (int i = 0; i < 50; i++) {
				cout << point[i] << " ";
			}
			cout << endl;
		}
	}
	dataFile.close();
}

void output(ball* root) {
	stack<ball*> Stack;
	while (root || !Stack.empty()) {

		while (root) {
			Stack.push(root);
			cout << root->bid << " ";
			cout << "圆心： [";
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

void outputfloat2(float** f, int n, int d) {
	for (int i = 0; i < n; i++) {
		for (int j = 0; j < d; j++) {
			printf("%f", f[i][j]);
		}
	}
}

void displayCenter(float* f, int d) {
	cout << "圆心：[";
	for (int i = 0; i < d; i++) {
		printf("%f ,", f[i]);
	}
	cout << "]" << endl;
}

float getMax(int d, float* query, ball* Root) {
	float Max = 0;
	for (int i = 0; i < d; i++) {
		Max += Root->CircleCenter[i] * query[i];
	}
	Max += Root->radius * getLength(d, query);
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