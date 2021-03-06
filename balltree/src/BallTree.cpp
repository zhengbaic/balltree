#include "../include/BallTree.h"

extern int DIMENSION;
extern int SIZE_OF_POINT;
extern int BYTES_PER_BLOCK;
extern int BLOCKS_PER_PAGE;

// 全局变量
map<int, Point*> storage;

int BallTree::PAGES_LIMIT = 500;

BallTree::BallTree() {
	dimesion = 0;
	num = 0;
	targetid = -1;
	root = NULL;
	tb = NULL;
	quadroot = NULL;
	numOfBlocks = 0;
}

BallTree::~BallTree() {
	// 如果有脏页，要保存索引文件
	if (!dirtyPages.empty()) {
		saveIndex(index_path.c_str(), root, dimesion, num);
	}

	for (auto i : pages) {
		// 如果是脏页
		if (dirtyPages.find(i.first) != dirtyPages.end()) {
			i.second.saveToDisk(i.first, index_path);
		}
	}
}

bool BallTree::buildTree(int n, int d, float **data) {
	static int id = 1;

	// 初始化
	dimesion = d;
	num = n;
	initConstants(d);  // 初始化Utility中的常量

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
	
	return true;
}

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
	
	if (n <= N0) {
		// 叶子
		storage[bid] = points;
		balls.push_back(node);
		node->datanum = n;
		node->bid = bid;
		node->offset = bid % BLOCKS_PER_PAGE;
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

		for (int i = 0; i < n; ++i) {
			delete[] data[i];
		}

		buildBall(node->leftball, leftp.size(), d, leftdata);
		buildBall(node->rightball, rightp.size(), d, rightdata);
	}
}

bool BallTree::buildQuadTree(int n, int d, float ** data) {
	dimesion = d;
	num = n;
	printf("Building QuadTree ...\n");
	buildQuadBall(quadroot, n, d, data);
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
	this->index_path = string(index_path);

	openF(root, storage, index_path, dimesion, num);

	// delete掉，免得内存泄漏
	for (auto i : storage) {
		if (i.second != NULL) {
			delete i.second->data;
		}
	}
	storage.clear();
	return true;
}

bool BallTree::restoreTree(const char* index_path) {
	numOfBlocks = readF(root, index_path, dimesion, num);

	// 准备阶段
	this->index_path = string(index_path);
	initConstants(dimesion);

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
	return targetid;
}

int BallTree::linearSearch(int d, float* query) {
	float temp = 0.0f;
	float Max = 0.0f;
	for (int i = 0; i < 316; i++) {
		preparePage(i);
		for (int j = 0; j < 16; j++) {
			for (int k = 0; k < 20; k++)
			{
				temp = getInnerproduct(d, query, pages[i].blocks[j].points[k].data);
				if (Max < temp) {
					Max = temp;
					targetid = pages[i].blocks[j].points[k].id;
				}
			}
		}
	}
	return targetid;
}

float BallTree::eval(int d, float* query, float Max, Ball* Root) {
	float temp = 0.0f;
	if (Root->leftball == NULL && Root->rightball == NULL) {
		preparePage(Root->pid);
		Block block = pages[Root->pid].blocks[Root->offset];
		for (int i = 0; i < Root->datanum; i++) {
			temp = getInnerproduct(d, query, pages[Root->pid].blocks[Root->offset].points[i].data);
			if (Max < temp) {
				Max = temp;
				tb = Root;
				targetid = pages[tb->pid].blocks[tb->offset].points[i].id;
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
		if (node->datanum <= 0) {
			++leaf;
			// printf("NODE #%d:\t\tbid=%d\t\tdatanum=%d\n", count++, node->bid, node->datanum);
		}
		if (node->leftball) tree.push(node->leftball);
		if (node->rightball) tree.push(node->rightball);
	}
	printf("==============================================\n");
	printf("TOTAL NODES: %d\n", count);
	printf("TOTAL LEAF NODES: %d\n", leaf);
}

//bool BallTree::insertData(int d, float* data) {
//	if (data == NULL) {
//		return false;
//	}
//
//	if (root == NULL) {
//		root = new Ball();
//		root->bid = 0;
//		root->pid = 0;
//		root->offset = 0;
//		root->datanum = 1;
//		root->radius = 0.0f;
//		root->leftball = root->rightball = root->parent = NULL;
//		memcpy(root->center, data, sizeof(float) * dimesion);
//	}
//
//	tb = root;
//	float dst = getDistanse(data, tb->center, dimesion);
//	if (dst > tb->radius) {
//		tb->radius = dst;
//	}
//	// 找到要插入的叶子节点
//	while (!(tb->leftball == NULL && tb->rightball == NULL)) {
//		float leftdst = getDistanse(data, tb->leftball->center, dimesion);
//		float rightdst = getDistanse(data, tb->rightball->center, dimesion);
//		if (leftdst < rightdst) {
//			tb = tb->leftball;
//			if (leftdst > tb->radius) {
//				tb->radius = leftdst;
//			}
//		} else {
//			tb = tb->rightball;
//			if (rightdst > tb->radius) {
//				tb->radius = rightdst;
//			}
//		}
//	}
//
//	preparePage(tb->pid);
//	num++;
//	tb->datanum++;
//	// 如果不用分裂
//	if (tb->datanum < N0) {
//		// 把新增加的point放入page里面
//		Point p;
//		p.id = num;
//		p.data = new float[dimesion];
//		memcpy(p.data, data, sizeof(float) * dimesion);
//		pages[tb->pid].blocks[tb->offset].points[tb->datanum] = p;
//		dirtyPages.insert(tb->pid);
//	} else {
//		// 创建新的point数组来buildBall
//		Point *points = new Point[N0 + 1];
//
//		// 将原来节点上的points放到新的point数组里面
//		for (int i = 0; i < tb->datanum; ++i) {
//			points[i].id = pages[tb->pid].blocks[tb->offset].points[i].id;
//			memcpy(points[i].data, pages[tb->pid].blocks[tb->offset].points[i].data, sizeof(float) * dimesion);
//		}
//
//		// 用要插入的data来创建新的point
//		points[N0].id = num;
//		points[N0].data = new float[dimesion];
//		memcpy(points[N0].data, data, sizeof(float) * dimesion);
//
//		// 开始buildBall创建新的节点temp
//		Ball *temp = new Ball();
//		buildBall(temp, N0 + 1, dimesion, points);
//		temp->pid = temp->bid = -1;
//		temp->datanum = -1;
//		temp->leftball->pid = tb->pid;
//		temp->leftball->bid = tb->bid;  // 分裂出来的左节点使用原理节点的block
//		temp->leftball->offset = tb->offset;
//		temp->rightball->bid = numOfBlocks++;  // 分裂出来的右节点要在所以blocks后面追加一个block（可能要开一张新的page）
//		temp->rightball->pid = numOfBlocks / BLOCKS_PER_PAGE;
//
//		// 将新节点与父节点连接起来
//		if (tb->parent->leftball == tb) {
//			tb->parent->leftball = temp;
//		} else {
//			tb->parent->rightball = temp;
//		}
//
//		// 获取分裂后左右两个节点的数据
//		auto it = storage.rbegin();
//		Point *leftpoints  = it->second;
//		++it;
//		Point *rightpoints = it->second;
//
//		// 把左节点的数据保存到page相应的block中
//		for (int i = 0; i < temp->leftball->datanum; ++i) {
//			pages[tb->pid].blocks[tb->offset].points[i].id = leftpoints[i].id;
//			memcpy(pages[tb->pid].blocks[tb->offset].points[i].data, leftpoints[i].data, sizeof(float) * dimesion);
//		}
//		dirtyPages.insert(tb->pid);
//
//		// 如果需要新开一页
//		if (numOfBlocks % BLOCKS_PER_PAGE == 0) {
//			temp->rightball->offset = 0;
//			int pageid = numOfBlocks / BLOCKS_PER_PAGE;
//			Page page;
//			pages[pageid] = page;
//			// 将右节点的数据放到page里面
//			for (int i = 0; i < temp->rightball->datanum; ++i) {
//				pages[pageid].blocks[0].points[i].id = rightpoints[i].id;
//				memcpy(pages[pageid].blocks[0].points[i].data, rightpoints[i].data, sizeof(float) * dimesion);
//			}
//			dirtyPages.insert(pageid);
//		} else {
//			// 加载最后一张page并且将右节点的数据放到里面
//			int pageid = numOfBlocks / BLOCKS_PER_PAGE;
//			int offset = numOfBlocks % BLOCKS_PER_PAGE;
//			temp->rightball->offset = offset;
//			for (int i = 0; i < temp->leftball->datanum; ++i) {
//				pages[pageid].blocks[offset].points[i].id = rightpoints[i].id;
//				memcpy(pages[pageid].blocks[offset].points[i].data, rightpoints[i].data, sizeof(float) * dimesion);
//			}
//			dirtyPages.insert(pageid);
//		}
//	}
//
//	return true;
//}

//bool BallTree::deleteData(int d, float* data) {
//	if (data == NULL) {
//		return false;
//	}
//	if (d != dimesion) {
//		return false;
//	}
//
//	// 找到要删除的点的id
//	int id = mipSearch(d, data);
//	if (id == -1 || tb == NULL) {
//		return false;
//	}
//
//	// 如果要删除数据所在的叶子只有它一个数据
//	if (tb->datanum == 1) {
//		page.loadFromDisk(tb->pid, index_path);
//		if (tb == root) {
//			root->clear();
//		} else if (tb->parent == root) {
//			if (root->leftball == tb) {
//				root = root->rightball;
//			} else {
//				root = root->leftball;
//			}
//		} else {
//			Ball *parent, *grandparent;
//			parent = tb->parent;
//			grandparent = parent->parent;
//			if (grandparent->leftball == parent) {
//				if (parent->leftball == tb) {
//					grandparent->leftball = parent->rightball;
//				} else {
//					grandparent->leftball = parent->leftball;
//				}
//			}
//			else {
//				if (parent->leftball == tb) {
//					grandparent->rightball = parent->rightball;
//				} else {
//					grandparent->rightball = parent->leftball;
//				}
//			}
//		}
//		tb->clear();
//	} else {
//		tb->datanum--;
//		page.loadFromDisk(tb->pid, index_path);
//		int pos = -1;
//		for (int i = 0; i < BLOCKS_PER_PAGE; ++i) {
//			if (page.blocks[i].bid == tb->bid) {
//				pos = i;
//				break;
//			}
//		}
//		for (int i = 0; i < N0; ++i) {
//			if (page.blocks[pos].points[i].id == id) {
//				page.blocks[pos].points[i].id = page.blocks[pos].points[tb->datanum - 1].id;
//				memcpy(page.blocks[pos].points[i].data, page.blocks[pos].points[tb->datanum - 1].data, sizeof(float) * dimesion);
//				break;
//			}
//		}
//	}
//
//	return true;
//}

void BallTree::preparePage(const int pid) {
	if (pid == -1) {
		return;
	}

	// 如果目标页不在内存中
	if (pages.find(pid) == pages.end()) {
		// 如果内存中的页数超出限制
		if (pages.size() == PAGES_LIMIT) {
			pages.begin()->second.clear();
			int pid = pages.begin()->first;
			pages.begin()->second.saveToDisk(pid, index_path);
			pages.erase(pages.begin());  // 删去第一页
		}
		Page temp;
		temp.loadFromDisk(pid, index_path);
		pages[pid] = temp;
	}
}

void BallTree::setPagesLimit(const int limit) {
	if (limit > 0) {
		PAGES_LIMIT = limit;
	}
}
