#include "Utility.h"

int DIMENSION;
int SIZE_OF_POINT;
int BYTES_PER_BLOCK;
int BLOCKS_PER_PAGE;

// =========== Block Struct ===========

Block::Block() {
	bid = pid = -1;
	initialized = false;
	points = NULL;
	init();
}

Block::~Block() {
	// clear();
}

void Block::clear() {
	bid = pid = -1;
	if (points != NULL) {
		delete[] points;
	}
}

void Block::init() {
	points = new Point[N0];
	for (int i = 0; i < N0; ++i) {
		points[i].data = new float[DIMENSION] {0.0f};
	}
	initialized = true;
}

// ====================================

// =========== Page Struct ===========

Page::Page() {
	initialized = false;
	pid = -1;
	blocks = NULL;
}

Page::~Page() {
	// clear();
}

void Page::clear() {
	pid = -1;
	if (blocks != NULL) {
		delete[] blocks;
	}
}

void Page::init() {
	blocks = new Block[BLOCKS_PER_PAGE];
	for (int i = 0; i < BLOCKS_PER_PAGE; ++i) {
		blocks[i].points = new Point[N0];
		for (int j = 0; j < DIMENSION; ++j) {
			blocks[i].points->data = new float[DIMENSION] {0.0f};
		}
	}
	initialized = true;
}

void Page::saveToDisk() {
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

void Page::loadFromDisk(const int pid) {
	if (pid == -1) {
		return;
	}

	if (!initialized) {
		init();
	}

	// ���Ѿ����غ��˵ģ�ֱ�ӷ���
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

void openF(Ball* root, map<int, Point*> storage, const char* index_path) {
	int testCount = 0;

	// �������ļ�
	ofstream outFile;
	string tempFileName = index_path;
	tempFileName += "/index.bin";
	outFile.open(tempFileName, ios_base::out | ios_base::binary);
	
	// �����ļ�
	ofstream dataFile;
	map<int, Point*>::iterator mapIter;
	int pageId = 0;							// ҳ��
	int count = 0;							// ÿҳ������
	stringstream stream;
	stream << pageId;
	string temp;
	stream >> temp;
	string fileName = "/page" + temp + ".bin";
	tempFileName = index_path;
	tempFileName += fileName;
	dataFile.open(tempFileName, ios_base::out | ios_base::binary);

	// ��һ���������
	int zeroIndex = -1;
	float *zeroPoint = NULL;
	zeroPoint = new float[50];
	for (int i = 0; i < 50; i++) {
		zeroPoint[i] = 0.0f;
	}

	if (!outFile.is_open() || !dataFile.is_open()) {
		exit(EXIT_FAILURE);
	}

	stack<Ball*> Stack;
	while (root || !Stack.empty()) {

		while (root) {
			Stack.push(root);
			outFile.write((char*)&root->bid, sizeof 4);
			outFile.write((char*)root->center, sizeof 4.0f * 50);
			outFile.write((char*)&root->radius, sizeof 4.0f);
			outFile.write((char*)&root->datanum, sizeof 4);
			outFile.write((char*)&root->leftball, sizeof 4);
			outFile.write((char*)&root->rightball, sizeof 4);
			outFile.write((char*)&root->parent, sizeof 4);
			
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
					string fileName = "/page" + temp + ".bin";
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
					for (int i = 0; i < root->datanum; i++) {
						dataFile.write((char*)&mapIter->second[i].id, sizeof 4);
						dataFile.write((char*)mapIter->second[i].data, sizeof 4.0f * 50);
						// ����
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

int readF(Ball* &root, const char* index_path) {
	ifstream inFile;
	int numOfBlock = 0;
	string tempFileName = index_path;
	tempFileName += "/index.bin";
	inFile.open(tempFileName, ios_base::in | ios_base::binary);
	if (!inFile.is_open()) {
		exit(EXIT_FAILURE);
	}
	//inFile.read((char*)&root, sizeof root + 1);
	root = new Ball;
	root->center = new float[50];
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
	while (root || !Stack.empty()) {

		while (root) {
			Stack.push(root);
			// ����blockNum
			numOfBlock++;
			if (flag) {
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
			root = root->leftball;
		}
		root = Stack.top();
		Stack.pop();
		root = root->rightball;
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
			cout << "�˴������" << endl;
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
			cout << "Բ�ģ� [";
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
	cout << "Բ�ģ�[";
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
