#include "BallTree.h"

// 全局变量
map<int, point*> storage;
vector<ball*> balls;

static int id = 0;

BallTree::BallTree() {
	dimesion = 0;
	num = 0;
	target_bid = -1;
	root = NULL;
	Quadroot = NULL;
}

BallTree::~BallTree() {}

bool BallTree::buildTree(int n, int d, float **data) {
	dimesion = d;
	num = n;
	printf("Building tree ...\n");
	point* points = new point[n];
	for (int i = 0; i < n; i++) {
		points[i].data = new float[d];
		points[i].id = id++;
		for (int j = 0; j < d; j++) {
			points[i].data[j] = data[i][j];
		}
	}
	buildBall(root, n, d, points);
	printf("Building tree completed!\n");
	//displayTree();

	return true;
}

void BallTree::buildBall(ball* &node, int n, int d, point *points) {
	static int bid = 0;

	node = new ball();
	float** data;
	data = new float*[n];
	for (int i = 0; i < n; i++) {
		data[i] = new float[d];
		for (int j = 0; j < d; j++) {
			data[i][j] = points[i].data[j];
		}
	}
	Analyse(node, n, d, data);  // 得到圆心跟半径（Utility.cpp）
	if (n <= N0) {
		//叶子
		storage.insert(map<int, point*>::value_type(bid, points));
		node->bid = bid;
		balls.push_back(node);
		node->datanum = n;
		bid++;
	}
	else {
		float* A = NULL;
		float* B = NULL;
		Split(n, d, A, B, data);
		point* leftdata;//A
		point* rightdata;//B
		vector<point> leftp;
		vector<point> rightp;
		vector<float*> leftd;
		vector<float*> rightd;
		for (int i = 0; i < n; i++) {
			if (getDistanse(A, points[n].data, d) < getDistanse(B, points[n].data, d)) {//A
				leftp.push_back(points[n]);
			}
			else {
				rightp.push_back(points[n]);
			}
		}
		leftdata = VectorToPoint(leftp);
		rightdata = VectorToPoint(rightp);

		buildBall(node->leftball, leftd.size(), d, leftdata);
		buildBall(node->rightball, rightd.size(), d, rightdata);
	}
}

bool BallTree::storeTree(const char* index_path) {
	openF(root, storage, index_path);
	return true;
}

bool BallTree::restoreTree(const char* index_path) {
	readF(root, index_path);
	return true;
}

bool BallTree::buildQuadTree(int n, int d, float ** data)
{
	dimesion = d;
	num = n;
	printf("Building QuadTree ...\n");
	buildQuadBall(Quadroot, n, d, data);
	printf("Building QuadTree completed!\n");
	//displayTree();
	return true;
}

void BallTree::buildQuadBall(Quadball* &node, int n, int d, float **data) {
	node = new Quadball();
	QuadAnalyse(node, n, d, data);
	if (n <= N0) {
		//叶子
	}
	else {
		float* A = NULL;
		float* B = NULL;
		float* C = NULL;
		float* D = NULL;
		QuadSplit(n, d, A, B, C, D, data, node->CircleCenter);
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
		data1 = VectorToFloat(d1);
		data2 = VectorToFloat(d2);
		data3 = VectorToFloat(d3);
		data4 = VectorToFloat(d4);
		buildQuadBall(node->ball1, d1.size(), d, data1);
		buildQuadBall(node->ball2, d2.size(), d, data1);
		buildQuadBall(node->ball3, d3.size(), d, data1);
		buildQuadBall(node->ball4, d4.size(), d, data1);
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
	float temp = 0.0f;
	if (Root->leftball == NULL && Root->rightball == NULL) {
		for (int i = 0; i <= sizeof(storage[Root->bid]) / sizeof(float) / d; i++) {
			//temp = getInnerproduct(d, query, storage[Root->bid][i]);
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

//void BallTree::displayTree() {
//	queue<ball*> tree;
//	int count = 0;
//	int leaf = 0;
//	printf("=========== DISPLAY THE INDEX TREE ===========\n");
//	if (root != NULL) tree.push(root);
//	while (!tree.empty()) {
//		auto node = tree.front();
//		tree.pop();
//		if (node->bid != -1) ++leaf;
//		printf("NODE #%d:\t\tbid=%d\t\tdatanum=%d\n", count++, node->bid, node->datanum);
//		if (node->leftball) tree.push(node->leftball);
//		if (node->rightball) tree.push(node->rightball);
//	}
//	printf("==============================================\n");
//	printf("TOTAL NODES: %d\n", count);
//	printf("TOTAL LEAF NODES: %d\n", leaf);
//}

void BallTree::loadPage(const int pid, float *page) {
	if (pid < 0) {
		return;
	}

	FILE *fp;
	char filename[L];
	sprintf(filename, "page%d.bin", pid);
	fp = fopen(filename, "rb");
	if (fp != NULL) {
		fread(page, sizeof(float), FLOATS_PER_PAGE, fp);
	}
}

void BallTree::loadBlock(const int bid) {
	int static curr_pid = -1;
	int static backup_pid = -1;

	int str_pos, end_pos, str_pid, end_pid;
	str_pos = bid * N0;  // 第一个float所在的位置
	end_pos = str_pos + N0;  // 最后一个float所在的位置的下一位
	str_pid = (str_pos * 4) % BYTES_PER_PAGE;
	end_pid = (end_pos * 4) % BYTES_PER_PAGE;

	// 从硬盘中加载page
	if (str_pid != curr_pid) {
		loadPage(str_pid, page);
		curr_pid = str_pid;
	}
	if (end_pid != str_pid) {
		if (end_pid != backup_pid) {
			loadPage(end_pid, page_backup);
			backup_pid = end_pid;
		}
	}

	// 从page中读取block
	int count = 0;
	if (str_pid == end_pid) {
		for (int i = str_pos; i < end_pos; ++i) {
			block[count++] = page[i];
		}
	} else {
		for (int i = str_pos; i < FLOATS_PER_PAGE; ++i) {
			block[count++] = page[i];
		}
		for (int i = 0; i < end_pos; ++i) {
			block[count++] = page[i];
		}
	}
}

bool BallTree::insertData(int d, float* point) {
	ball *node = root;  // 这里需要改
	if (node == NULL) {
		return false;
	}

	if (node->datanum < N0) {
		float distance = getDistanse(point, node->CircleCenter, dimesion);
		if (distance > node->radius) {
			node->radius = distance;
		}
		//appendPointIntoBlock(point, node->bid);
		node->datanum++;
	} else {
		//splitToTwoCircles(node, point);
		delete node;
		// 从数据所在的页中读取出来
		// 可以在搜索的时候就保存页面
		//buildBall(node, N0 + 1, dimesion, );
	}

	return true;
}

bool BallTree::deleteData(int d, float* data) {
	return true;
}
