#include "BallTree.h"

map<int, float**> storage;
vector<ball*> balls;

BallTree::BallTree() {
	dimesion = 0;
	num = 0;
	target_bid = -1;
	root = NULL;
}

BallTree::~BallTree() {}

bool BallTree::buildTree(int n, int d, float **data) {
	dimesion = d;
	num = n;
	printf("Building tree ...\n");
	buildBall(root, n, d, data);
	printf("Building tree completed!\n");
	displayTree();

	return true;
}

void BallTree::buildBall(ball* &node, int n, int d, float **data) {
	static int bid = 0;

	node = new ball();
	Analyse(node, n, d, data);  // 得到圆心跟半径（Utility.cpp）
	if (n <= N0) {
		//叶子
		storage.insert(map<int, float**>::value_type(bid, data));
		node->bid = bid;
		balls.push_back(node);
		node->datanum = n;
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

void BallTree::displayTree() {
	queue<ball*> tree;
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
	ball *node = search(d, point);
	if (node == NULL) {
		return false;
	}

	if (node->datanum < N0) {
		float distance = getDistanse(point, node->CircleCenter, dimesion);
		if (distance > node->radius) {
			node->radius = distance;
		}
		appendPointIntoBlock(point, node->bid);
		node->datanum++;
	} else {
		splitToTwoCircles(node, point);
		delete node;
		// 从数据所在的页中读取出来
		// 可以在搜索的时候就保存页面
		buildBall(node, N0 + 1, dimesion, );
	}

	return true;
}

bool BallTree::deleteData(int d, float* data) {
	return true;
}
