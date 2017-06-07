#include "Utility.h"

int DIMENSION;
int SIZE_OF_POINT;
int BYTES_PER_BLOCK;
int BLOCKS_PER_PAGE;

// =========== Block Struct ===========

Block::Block() {
	points = new Point[N0];
}

Block::~Block() {
	// clear();
}

void Block::clear() {
	if (points != NULL) {
		delete[] points;
	}
}

// =========== Page Struct ===========

Page::Page() {
	blocks = new Block[BLOCKS_PER_PAGE];
}

Page::~Page() {
	// clear();
}

void Page::clear() {
	if (blocks != NULL) {
		delete[] blocks;
	}
}

void Page::saveToDisk(const int pid, const string index_path) {
	if (pid == -1) {
		return;
	}

	stringstream stream;
	stream << pid;
	string temp;
	stream >> temp;
	string fileName = index_path + "/page" + temp + ".bin";
	ofstream dataFile;
	dataFile.open(fileName, ios_base::out | ios_base::binary);
	if (!dataFile.is_open()) {
		exit(EXIT_FAILURE);
	}
	for (int i = 0; i < BLOCKS_PER_PAGE; ++i) {
		for (int j = 0; j < N0; ++j) {
			dataFile.write((char*)&(blocks[i].points[j].id), sizeof(int));
			dataFile.write((char*)blocks[i].points[j].data, sizeof(float) * DIMENSION);
		}
	}
}

void Page::loadFromDisk(const int pid, const string index_path) {
	if (pid == -1) {
		return;
	}

	FILE *fp;
	char filename[L];
	sprintf(filename, "/page%d.bin", pid);
	string filepath = index_path + filename;
	fp = fopen(filepath.c_str(), "rb");
	if (fp == NULL) {
		return;
	}
	for (int i = 0; i < BLOCKS_PER_PAGE; ++i) {
		for (int j = 0; j < N0; ++j) {
			fread(&blocks[i].points[j].id, sizeof(int), 1, fp);
			fread(blocks[i].points[j].data, sizeof(float), DIMENSION, fp);
		}
	}
}

// ===================================

void initConstants(const int d) {
	DIMENSION = d;
	SIZE_OF_POINT = 4 + 4 * DIMENSION;
	BYTES_PER_BLOCK = N0 * SIZE_OF_POINT;
	BLOCKS_PER_PAGE = BYTES_PER_PAGE / BYTES_PER_BLOCK;
}

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

void analyse(Ball *node, int n, int d, float **data) {
	float* mean = new float[d] {.0f};
	for (int i = 0; i < n; i++) {
		for (int j = 0; j < d; j++) {
			mean[j] += data[i][j];
		}
	}
	for (int i = 0; i < d; i++) {
		mean[i] /= n;
	}

	node->center = mean;

	float r = 0;
	for (int i = 0; i < n; i++) {
		r = max(r, getDistanse(mean, data[i], d));
	}

	node->radius = sqrt(r);
}

void quadAnalyse(Quadball *node, int n, int d, float **data) {
	float* mean = new float[d] {.0f};
	for (int i = 0; i < n; i++) {
		for (int j = 0; j < d; j++) {
			mean[j] += data[i][j];
		}
	}
	for (int i = 0; i < d; i++) {
		mean[i] /= n;
	}

	node->center = mean;

	float r = 0;
	for (int i = 0; i < n; i++) {
		r = max(r, getDistanse(mean, data[i], d));
	}

	node->radius = sqrt(r);
}

float* findFurestPoint(float** data, float * po, int n, int d) {
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

void findFurestPoints(float** data, int n, int d, set<float*> s) {
	for (int i = 0; i < n; i++) {
		s.insert(findFurestPoint(data, data[i], n, d));
	}
}

void split(int n , int d, float* &a, float* &b, float** data) {
	float* randomPoint = data[0];
	a = new float[d] {.0f};
	b = new float[d] {.0f};
	a = findFurestPoint(data, randomPoint, n, d);
	b = findFurestPoint(data, a, n, d);
}

void quadSplit(int n, int de, float* &a, float* &b, float* &c, float* &d, float** data, float* & mean) {
	float* randomPoint = data[0];
	a = new float[de] {.0f};
	b = new float[de] {.0f};
	c = new float[de] {.0f};
	d = new float[de] {.0f};

	a = findFurestPoint(data, randomPoint, n, de);
	b = findFurestPoint(data, a, n, de);

	float max = 0;
	for (int i = 0; i < n; i++) {
		float cur = getDistanse(a, data[i], de) + getDistanse(b, data[i], de);
		if (max < cur) {
			max = cur;
			c = data[i];
		}
	}
	d = findFurestPoint(data, c, n, de);
}

Point* vectorToPoint(vector<Point> v) {
	int size = v.size();
	Point *res = new Point[size];
	for (int i = 0; i < size; i++) {
		res[i].data = v[i].data;
		res[i].id = v[i].id;
	}
	return res;
}

float** vectorToFloat(vector<float*> v) {
	int size = v.size();
	float **res = new float*[size];
	for (int i = 0; i < size; i++) {
		res[i] = v[i];
	}
	return res;
}

void openF(Ball* root, map<int, Point*> storage, const char* index_path, int dim) {
	int testCount = 0;
	
	// 索引树文件
	ofstream outFile;
	string tempFileName = index_path;
	tempFileName += "/index.bin";
	outFile.open(tempFileName, ios_base::out | ios_base::binary);
	
	// 数据文件
	ofstream dataFile;
	map<int, Point*>::iterator mapIter;
	int pageId = 0;							// 页号
	int count = 0;							// 每页的数量
	stringstream stream;
	stream << pageId;
	string temp;
	stream >> temp;
	string fileName = "/page" + temp + ".bin";
	tempFileName = index_path;
	tempFileName += fileName;
	dataFile.open(tempFileName, ios_base::out | ios_base::binary);

	// 做一个点填补空项
	int zeroIndex = -1;
	float *zeroPoint = NULL;
	zeroPoint = new float[50];
	for (int i = 0; i < 50; i++) {
		zeroPoint[i] = 0.0f;
	}

	if (!outFile.is_open() || !dataFile.is_open()) {
		exit(EXIT_FAILURE);
	}
	outFile.write((char*)&dim, sizeof 4);
	stack<Ball*> Stack;
	while (root || !Stack.empty()) {

		while (root) {
			Stack.push(root);
			outFile.write((char*)&pageId, sizeof 4);
			outFile.write((char*)&root->bid, sizeof 4);
			outFile.write((char*)root->center, sizeof 4.0f * 50);
			outFile.write((char*)&root->radius, sizeof 4.0f);
			outFile.write((char*)&root->datanum, sizeof 4);
			outFile.write((char*)&root->leftball, sizeof 4);
			outFile.write((char*)&root->rightball, sizeof 4);
			outFile.write((char*)&root->parent, sizeof 4);
			
			// 如果是叶子结点则写入硬盘中
			if (root->leftball == NULL && root->rightball == NULL) {
				count++;  // 给每一页的数据块计数，一页的数据块不超过16个
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
					string fileName = "/page" + temp + ".bin";
					string tempFileName = index_path;
					tempFileName += fileName;
					dataFile.open(tempFileName, ios_base::out | ios_base::binary);
					if (!dataFile.is_open()) {
						exit(EXIT_FAILURE);
					}
				}
				root->pid = pageId;

				// 数据写入硬盘，不够位的用0点填补
				mapIter = storage.find(root->bid);
				if (mapIter != storage.end()) {
					dataFile.write((char*)&root->bid, sizeof 4);
					for (int i = 0; i < root->datanum; i++) {
						dataFile.write((char*)&mapIter->second[i].id, sizeof 4);
						dataFile.write((char*)mapIter->second[i].data, sizeof 4.0f * 50);
						// 测试
						/*testCount++;
						if (testCount <= 300) {
							cout << "index: " << mapIter->second[i].id << endl;
							cout << "point: [";
							for (int j = 0; j < 50; j++) {
								cout << mapIter->second[i].data[j] << " ";
							}
							cout << "]" << endl;
						}*/
					}

					for (int i = 0; i < 20 - root->datanum; i++) {
						dataFile.write((char*)&zeroIndex, sizeof zeroIndex);
						dataFile.write((char*)zeroPoint, sizeof 4.0f * 50);
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

int readF(Ball* &root, const char* index_path, int &dim) {
	ifstream inFile;
	int numOfBlock = 0;
	string tempFileName = index_path;
	tempFileName += "/index.bin";
	inFile.open(tempFileName, ios_base::in | ios_base::binary);
	if (!inFile.is_open()) {
		exit(EXIT_FAILURE);
	}
	inFile.read((char*)&dim, sizeof 4);
	root = new Ball();
	root->center = new float[50];
	inFile.read((char*)&root->pid, sizeof 4);
	inFile.read((char*)&root->bid, sizeof 4);
	inFile.read((char*)root->center, sizeof 4.0f * 50);
	inFile.read((char*)&root->radius, sizeof 4.0f);
	inFile.read((char*)&root->datanum, sizeof 4);
	inFile.read((char*)&root->leftball, sizeof 4);
	inFile.read((char*)&root->rightball, sizeof 4);
	inFile.read((char*)&root->parent, sizeof 4);

	Ball *head = root;
	stack<Ball*> Stack;
	bool flag = false;

	// 测试
	int countTimes = 0;

	while (root || !Stack.empty()) {

		while (root) {
			Stack.push(root);
			// 计数blockNum
			numOfBlock++;
			if (flag) {
				root->center = new float[50];
				inFile.read((char*)&root->pid, sizeof 4);
				inFile.read((char*)&root->bid, sizeof 4);
				inFile.read((char*)root->center, sizeof 4.0f * 50);
				inFile.read((char*)&root->radius, sizeof 4.0f);
				inFile.read((char*)&root->datanum, sizeof 4);
				inFile.read((char*)&root->leftball, sizeof 4);
				inFile.read((char*)&root->rightball, sizeof 4);
				inFile.read((char*)&root->parent, sizeof 4);
			}
			else {
				flag = true;
			}
			// 测试
			if (countTimes < 100) {
				countTimes++;
				cout << root->radius << endl;
			}
			
			if (root->leftball != NULL) {
				root->leftball = new Ball();
				if (root == head) {
					head->leftball = root->leftball;
				}
				Ball* parent = root;
				root = root->leftball;
				root->parent = parent;
			}
			else {
				root = NULL;
			}
		}
		root = Stack.top();
		Stack.pop();
		if (root->rightball != NULL) {
			root->rightball = new Ball();
			if (root == head) {
				head->rightball = root->rightball;
			}
			Ball* parent = root;
			root = root->rightball;
			root->parent = parent;
		}
		else {
			root = NULL;
		}
	}
	root = head;
	inFile.close();

	/*ifstream dataFile;
	tempFileName = index_path;
	tempFileName += "page0.bin";
	dataFile.open(tempFileName, ios_base::in | ios_base::binary);
	if (!dataFile.is_open()) {
		exit(EXIT_FAILURE);
	}
	

	cout << endl << endl << endl << endl;
	int count = 300;
	while (count--) {
		int index = 0;
		dataFile.read((char*)&index, sizeof 4);
		cout << "index: " << index << endl;
		float *point = new float[50];
		dataFile.read((char*)point, sizeof 4.0f * 50);
		if (point == NULL) {
			cout << "此处是零点" << endl;
		}
		else {
			cout << "point: ";
			for (int i = 0; i < 50; i++) {
				cout << point[i] << " ";
			}
			cout << endl;
		}
	}
	dataFile.close();*/

	return numOfBlock;
}

float getMax(int d, float* query, Ball* Root) {
	float Max = 0;
	for (int i = 0; i < d; i++) {
		Max += Root->center[i] * query[i];
	}
	Max += Root->radius * getLength(d, query);
	return Max;
}

void output(Ball* root) {
	stack<Ball*> Stack;
	while (root || !Stack.empty()) {
		while (root) {
			Stack.push(root);
			cout << root->bid << " ";
			cout << "圆心： [";
			for (int i = 0; i < 50; i++) {
				printf("%f ,", root->center[i]);
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
