﻿#include "BallTree.h"

extern int DIMENSION;
extern int SIZE_OF_POINT;
extern int BYTES_PER_BLOCK;
extern int BLOCKS_PER_PAGE;

// 全局变量
map<int, Point*> storage;

BallTree::BallTree() {
	dimesion = 0;
	num = 0;
	targetid = -1;
	root = NULL;
	targetball = NULL;
	Quadroot = NULL;
	numOfBlocks = 0;
	//initPage = false;
}

BallTree::~BallTree() {
	// 将更新的页面保存到硬盘
	if (page.pid != -1) {
		page.saveToDisk();
	}

	// delete掉，免得内存泄漏
	for (auto i : storage) {
		if (i.second != NULL) {
			delete i.second->data;
		}
	}
	storage.clear();
}

bool BallTree::buildTree(int n, int d, float **data) {
	static int id = 1;
	initConstants(d);  // 初始化Utility中的常量
	dimesion = d;
	num = n;
	block.init();
	page.init();

	printf("Building tree ...\n");
	Point* points = new Point[n];
	for (int i = 0; i < n; i++) {
		points[i].data = new float[d];
		points[i].id = id++;
		for (int j = 0; j < d; j++) {
			points[i].data[j] = data[i][j];
		}
	}
	root = new Ball();
	buildBall(root, n, d, points);
	printf("Building tree completed!\n");
	
	return true;
}

// 测试
int countTimes = 0;

void BallTree::buildBall(Ball* &node, int n, int d, Point *points) {
	static int bid = 0;
	static vector<Ball*> balls;

	float** data = new float*[n];
	for (int i = 0; i < n; i++) {
		data[i] = new float[d];
		for (int j = 0; j < d; j++) {
			data[i][j] = points[i].data[j];
		}
	}
	analyse(node, n, d, data);  // 得到圆心跟半径（Utility.cpp）

	// 测试
	if (countTimes < 100) {
		countTimes++;
		cout << node->radius << endl;
	}
	
	if (n <= N0) {
		// 叶子
		storage.insert(map<int, Point*>::value_type(bid, points));
		node->bid = bid;
		balls.push_back(node);
		node->datanum = n;
		bid++;
	}
	else {
		float* A = NULL;
		float* B = NULL;
		split(n, d, A, B, data);
		Point* leftdata;//A
		Point* rightdata;//B
		vector<Point> leftp;
		vector<Point> rightp;
		for (int i = 0; i < n; i++) {
			if (getDistanse(A, points[i].data, d) < getDistanse(B, points[i].data, d)) {//A
				leftp.push_back(points[i]);
			}
			else {
				rightp.push_back(points[i]);
			}
		}
		leftdata = vectorToPoint(leftp);
		rightdata = vectorToPoint(rightp);
		node->leftball = new Ball();
		node->rightball = new Ball();
		node->leftball->parent = node;
		node->rightball->parent = node;
		buildBall(node->leftball, leftp.size(), d, leftdata);
		buildBall(node->rightball, rightp.size(), d, rightdata);
	}
}

bool BallTree::buildQuadTree(int n, int d, float ** data)
{
	dimesion = d;
	num = n;
	printf("Building QuadTree ...\n");
	buildQuadBall(Quadroot, n, d, data);
	printf("Building QuadTree completed!\n");
	return true;
}

void BallTree::buildQuadBall(Quadball* &node, int n, int d, float **data) {
	node = new Quadball();
	quadAnalyse(node, n, d, data);
	if (n <= N0) {
		//叶子
	}
	else {
		float* A = NULL;
		float* B = NULL;
		float* C = NULL;
		float* D = NULL;
		quadSplit(n, d, A, B, C, D, data, node->center);
		float ** data1;//A
		float ** data2;//B
		float ** data3;//C
		float ** data4;//D
		vector<float*> d1;
		vector<float*> d2;
		vector<float*> d3;
		vector<float*> d4;
		for (int i = 0; i < n; i++) {
			float dis1 = getDistanse(A, data[i], d);
			float dis2 = getDistanse(B, data[i], d);
			float dis3 = getDistanse(C, data[i], d);
			float dis4 = getDistanse(D, data[i], d);
			if (dis1 == min(min(dis1, dis2), min(dis3, dis4))) {
				d1.push_back(data[i]);
			}
			else if (dis2 == min(min(dis1, dis2), min(dis3, dis4))) {
				d2.push_back(data[i]);
			}
			else if (dis3 == min(min(dis1, dis2), min(dis3, dis4))) {
				d3.push_back(data[i]);
			}
			else if (dis4 == min(min(dis1, dis2), min(dis3, dis4))) {
				d4.push_back(data[i]);
			}
		}
		data1 = vectorToFloat(d1);
		data2 = vectorToFloat(d2);
		data3 = vectorToFloat(d3);
		data4 = vectorToFloat(d4);
		buildQuadBall(node->ball1, d1.size(), d, data1);
		buildQuadBall(node->ball2, d2.size(), d, data2);
		buildQuadBall(node->ball3, d3.size(), d, data3);
		buildQuadBall(node->ball4, d4.size(), d, data4);
	}
}

bool BallTree::storeTree(const char* index_path) {
	printf("Storing tree ...\n");
	openF(root, storage, index_path);
	printf("Storing tree completed!\n");
	return true;
}

bool BallTree::restoreTree(const char* index_path) {
	printf("Restoring tree ...\n");
	numOfBlocks = readF(root, index_path);
	printf("Restoring tree completed!\n");
	return true;
}

int BallTree::mipSearch(int d,float* query) {
	if (d != dimesion) {
		return -1;
	}

	float Max = 0;
	Max = eval(d, query, Max, root->leftball);
	if (Max < getMax(d,query, root->rightball)) {
		eval(d, query, Max, root->rightball);
	}
	return -1;  // 这里需要改！
}

float BallTree::eval(int d, float* query, float Max, Ball* Root) {
	float temp = 0.0f;
	if (Root->leftball == NULL && Root->rightball == NULL) {
		loadBlock(Root);
		for (int i = 0; i <= Root->datanum; i++) {
			temp = getInnerproduct(d, query, block.points[i].data);
			if (Max < temp) {
				Max = temp;
				targetball = Root;
				targetid = block.points[i].id;
			}
		}
		return Max;
	}
	Max = eval(d, query, Max, Root->leftball);
	if (Max < getMax(d, query, Root->rightball)) {
		Max = eval(d, query, Max, Root->rightball);
	}
	return Max;
}

void BallTree::displayTree() {
	queue<Ball*> tree;
	int count = 0;
	int leaf = 0;
	printf("=========== DISPLAY THE INDEX TREE ===========\n");
	if (root != NULL) tree.push(root);
	while (!tree.empty()) {
		auto node = tree.front();
		tree.pop();
		if (node->bid != -1) ++leaf;
		printf("NODE #%d:\t\tbid=%d\t\tdatanum=%d\n", count++, node->bid, node->datanum);
		if (node->leftball) tree.push(node->leftball);
		if (node->rightball) tree.push(node->rightball);
	}
	printf("==============================================\n");
	printf("TOTAL NODES: %d\n", count);
	printf("TOTAL LEAF NODES: %d\n", leaf);
}

bool BallTree::insertData(int d, float* data) {
	if (data == NULL) {
		return false;
	}

	// 找到要插入的节点在哪
	int id = mipSearch(dimesion, data);
	if (id == -1 || targetball == NULL) {
		return false;
	}

	// 去看看是否需要修改父节点们的半径
	float distance = getDistanse(data, targetball->center, dimesion);
	if (distance > targetball->radius) {
		targetball->radius = distance;
		Ball *parent = targetball->parent;
		while (parent != NULL) {
			float distance = getDistanse(data, parent->center, dimesion);
			if (distance > parent->radius) {
				parent->radius = distance;
				parent = parent->parent;
			}
			else {
				break;
			}
		}
	}

	// 如果不用分裂
	if (targetball->datanum < N0) {
		// 把新增加的point放入page里面
		Point p;
		p.id = ++num;
		p.data = new float[dimesion];
		memcpy(p.data, data, sizeof(float) * dimesion);
		for (int i = 0; i < BLOCKS_PER_PAGE; ++i) {
			if (page.blocks[i].bid == targetball->bid) {
				page.blocks[i].points[targetball->datanum] = p;
				break;
			}
		}
	} else {
		// 创建新的point数组来buildBall
		Point *points = new Point[N0 + 1];

		// 将原来节点上的points放到新的point数组里面
		page.loadFromDisk(targetball->pid);
		loadBlock(targetball);
		for (int i = 0; i < targetball->datanum; ++i) {
			points[i].id = block.points[i].id;
			memcpy(points[i].data, block.points[i].data, sizeof(float) * dimesion);
		}

		// 用要插入的data来创建新的point
		points[N0].id = ++num;
		points[N0].data = new float[dimesion];
		memcpy(points[N0].data, data, sizeof(float) * dimesion);

		// 开始buildBall创建新的节点temp
		Ball *temp = NULL;
		buildBall(temp, N0 + 1, dimesion, points);
		temp->pid = temp->bid = -1;
		temp->datanum = -1;
		temp->leftball->pid = targetball->pid;
		temp->leftball->bid = targetball->bid;  // 分裂出来的左节点使用原理节点的block
		temp->rightball->bid = numOfBlocks++;  // 分裂出来的右节点要在所以blocks后面追加一个block（可能要开一张新的page）
		temp->rightball->pid = numOfBlocks / BLOCKS_PER_PAGE;

		// 将新节点与父节点连接起来
		if (targetball->parent->leftball == targetball) {
			targetball->parent->leftball = temp;
		} else {
			targetball->parent->rightball = temp;
		}

		// 获取分裂后左右两个节点的数据
		auto it = storage.rbegin();
		Point *leftpoints  = it->second;
		++it;
		Point *rightpoints = it->second;

		// 把左节点的数据保存到page相应的block中
		int pos = -1;
		for (int i = 0; i < BLOCKS_PER_PAGE; ++i) {
			if (page.blocks[i].bid == temp->leftball->bid) {
				pos = i;
				break;
			}
		}
		for (int i = 0; i < temp->leftball->datanum; ++i) {
			page.blocks[pos].points[i].id = leftpoints[i].id;
			memcpy(page.blocks[pos].points[i].data, leftpoints[i].data, sizeof(float) * dimesion);
		}

		// 如果需要新开一页
		if (numOfBlocks % BLOCKS_PER_PAGE == 0) {
			// 将右节点的数据放到page里面
			int pos = 0;
			for (int i = 0; i < temp->rightball->datanum; ++i) {
				page.blocks[pos].points[i].id = rightpoints[i].id;
				memcpy(page.blocks[pos].points[i].data, rightpoints[i].data, sizeof(float) * dimesion);
			}
			page.pid = numOfBlocks / BLOCKS_PER_PAGE;
		} else {
			// 加载最后一张page并且将右节点的数据放到里面
			page.loadFromDisk(numOfBlocks / BLOCKS_PER_PAGE);
			int pos = numOfBlocks % BLOCKS_PER_PAGE;
			for (int i = 0; i < temp->leftball->datanum; ++i) {
				page.blocks[pos].points[i].id = leftpoints[i].id;
				memcpy(page.blocks[pos].points[i].data, leftpoints[i].data, sizeof(float) * dimesion);
			}
		}
	}

	return true;
}

bool BallTree::deleteData(int d, float* data) {
	if (data == NULL) {
		return false;
	}
	if (d != dimesion) {
		return false;
	}

	// 找到要删除的点的id
	int id = mipSearch(d, data);
	if (id == -1 || targetball == NULL) {
		return false;
	}

	// 如果要删除数据所在的叶子只有它一个数据
	if (targetball->datanum == 1) {
		page.loadFromDisk(targetball->pid);
		if (targetball == root) {
			root->clear();
		} else if (targetball->parent == root) {
			if (root->leftball == targetball) {
				root = root->rightball;
			} else {
				root = root->leftball;
			}
		} else {
			Ball *parent, *grandparent;
			parent = targetball->parent;
			grandparent = parent->parent;
			if (grandparent->leftball == parent) {
				if (parent->leftball == targetball) {
					grandparent->leftball = parent->rightball;
				} else {
					grandparent->leftball = parent->leftball;
				}
			}
			else {
				if (parent->leftball == targetball) {
					grandparent->rightball = parent->rightball;
				} else {
					grandparent->rightball = parent->leftball;
				}
			}
		}
		targetball->clear();
	} else {
		targetball->datanum--;
		page.loadFromDisk(targetball->pid);
		int pos = -1;
		for (int i = 0; i < BLOCKS_PER_PAGE; ++i) {
			if (page.blocks[i].bid == targetball->bid) {
				pos = i;
				break;
			}
		}
		for (int i = 0; i < N0; ++i) {
			if (page.blocks[pos].points[i].id == id) {
				page.blocks[pos].points[i].id = page.blocks[pos].points[targetball->datanum - 1].id;
				memcpy(page.blocks[pos].points[i].data, page.blocks[pos].points[targetball->datanum - 1].data, sizeof(float) * dimesion);
				break;
			}
		}
	}

	return true;
}

void BallTree::loadBlock(Ball *ball) {
	if (ball == NULL) {
		return;
	}
	if (ball->pid == -1 || ball->bid == -1) {
		return;
	}

	page.loadFromDisk(ball->pid);
	for (int i = 0; i < BLOCKS_PER_PAGE; ++i) {
		if (page.blocks[i].bid == ball->bid) {
			for (int j = 0; j < N0; ++j) {
				block.points[j].id = page.blocks[i].points[j].id;
				memcpy(block.points[j].data, page.blocks[i].points[j].data, sizeof(float) * DIMENSION);
			}
		}
	}
}
