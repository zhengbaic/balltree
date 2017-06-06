#include "BallTree.h"

// 全局变量
map<int, point*> storage;
vector<ball*> balls;

static int id = 1;

BallTree::BallTree() {
	numOfBlock = 0;
	dimesion = 0;
	num = 0;
	pid = -1;
	root = NULL;
	target = NULL;
	Quadroot = NULL;
	numOfBlocks = 0;
	target_bid = -1;
}

BallTree::~BallTree() {
	// 将更新的页面保存到硬盘
	savePage(pid);
}

int BallTree::getNumOfBlock() {
	return numOfBlock;
}

void BallTree::setNumOfBlock(int num) {
	numOfBlock = num;
}

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
	root = new ball();
	buildBall(root, n, d, points);
	printf("Building tree completed!\n");
	
	// displayTree();
	return true;
}

void BallTree::buildBall(ball* &node, int n, int d, point *points) {
	static int bid = 0;

	float** data = new float*[n];
	for (int i = 0; i < n; i++) {
		data[i] = new float[d];
		for (int j = 0; j < d; j++) {
			data[i][j] = points[i].data[j];
		}
	}
	Analyse(node, n, d, data);  // 得到圆心跟半径（Utility.cpp）
	if (n <= N0) {
		// 叶子
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
		for (int i = 0; i < n; i++) {
			if (getDistanse(A, points[i].data, d) < getDistanse(B, points[i].data, d)) {//A
				leftp.push_back(points[i]);
			}
			else {
				rightp.push_back(points[i]);
			}
		}
		leftdata = VectorToPoint(leftp);
		rightdata = VectorToPoint(rightp);
		node->leftball = new ball();
		node->rightball = new ball();
		node->leftball->parent = node;
		node->rightball->parent = node;
		buildBall(node->leftball, leftp.size(), d, leftdata);
		buildBall(node->rightball, rightp.size(), d, rightdata);
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
	numOfBlock = readF(root, index_path);
	printf("Restoring tree completed!\n");
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

float BallTree::eval(int d, float* query, float Max, ball* Root) {
	float temp = 0.0f;
	if (Root->leftball == NULL && Root->rightball == NULL) {
		for (int i = 0; i <= sizeof(storage[Root->bid]) / sizeof(float) / d; i++) {
			//temp = getInnerproduct(d, query, storage[Root->bid][i]);
			if (Max < temp) {
				Max = temp;
				target = Root;
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

void BallTree::loadPage(const int pid) {
	if (pid < 0) {
		return;
	}
	if (pid == this->pid) {
		return;
	}

	// 先保存当前在内存中的页
	savePage(pid);

	FILE *fp;
	char filename[L];
	sprintf(filename, "page%d.bin", pid);
	fp = fopen(filename, "rb");
	if (fp != NULL) {
		fread(page, SIZE_OF_POINT(dimesion), POINTS_PER_PAGE, fp);
		this->pid = pid;
	}
}

void BallTree::loadBlock(const int bid) {
	int pos = getBlockPosInPage(bid);

	// 从硬盘中加载page
	loadPage(pid);

	// 从page中读取block
	memcpy(block, page + pos, BYTES_PER_BLOCK(dimesion));
}

bool BallTree::insertData(int d, float* data) {
	if (data == NULL) {
		return false;
	}

	int id = mipSearch(dimesion, data);
	if (id == 0 || target == NULL) {
		//return;
	}

	if (target->datanum < N0) {
		float distance = getDistanse(data, target->CircleCenter, dimesion);
		if (distance > target->radius) {
			target->radius = distance;
		}
		// 把新增加的point放入page里面
		point p;
		p.id = ++num;
		p.data = new float[dimesion];
		memcpy(p.data, data, sizeof(float) * dimesion);
		int pos = getBlockPosInPage(target->bid);
		page[pos + target->datanum] = p;
		target->datanum++;
	} else {
		// 创建新的point数组来buildBall
		point *points = new point[N0 + 1];
		// 将原来节点上的points放到新的point数组里面
		loadBlock(target->bid);
		memcpy(points, block, SIZE_OF_POINT(dimesion) * N0);
		// 用要插入的data来创建新的point
		point p;
		p.id = ++num;
		p.data = new float[dimesion];
		memcpy(p.data, data, sizeof(float) * dimesion);
		// 将新point放进新points数组里面
		points[N0] = p;
		// 开始buildBall创建新的节点temp
		ball *temp = NULL;
		buildBall(temp, N0 + 1, dimesion, points);
		temp->bid = -1;
		temp->leftball->bid = target->bid;
		temp->rightball->bid = numOfBlocks++;
		// 将原来的节点替换为temp
		replaceBall(target, temp);
		// 如果需要新开一页
		if (numOfBlocks % BLOCKS_PER_PAGE == 0) {

			savePage(numOfBlocks / BLOCKS_PER_PAGE);
		}
	}

	return true;
}

bool BallTree::deleteData(int d, float* data) {
	return true;
}

void BallTree::replaceBall(ball *oldBall, ball *newBall) {
	if (oldBall == NULL || newBall == NULL) {
		return;
	}

	if (root == oldBall) {
		delete root;
		root = newBall;
		return;
	}

	traverseAndReplace(root, oldBall, newBall);
}

int BallTree::getBlockPosInPage(const int bid) {
	int pid = bid / BLOCKS_PER_PAGE;
	int leftBlocksInPages = bid % BLOCKS_PER_PAGE;
	int pos = leftBlocksInPages * BYTES_PER_BLOCK(dimesion);  // 第一个float所在的位置
	return pos;
}

ball* BallTree::getRoot() {
	return root;
}

void BallTree::savePage(int pid) {
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
	for (int i = 0; i < POINTS_PER_PAGE; i++) {
		dataFile.write((char*)&page[i].id, sizeof 4);
		dataFile.write((char*)page[i].data, sizeof 4.0f * 50);
	}
}