#ifndef __BALL_TREE_H
#define __BALL_TREE_H

#include "Utility.h"
#include <vector>
#include <map>
#include <queue>
#include <cstdio>
#define N0                20
#define BYTES_PER_PAGE    64 * 1024
#define FLOATS_PER_BLOCK  N0 * 50
#define FLOATS_PER_PAGE   BYTES_PER_PAGE / 4
using namespace std;

class BallTree {
private:
	int dimesion;//维度

	int num; //数量

	int target_bid;

	ball *root;

	Quadball *Quadroot;

	float page[FLOATS_PER_BLOCK];

	float page_backup[FLOATS_PER_BLOCK];

	float block[FLOATS_PER_BLOCK];
public:
	BallTree();

	~BallTree();

	// @author bytrain
	bool buildTree(int n, int d, float** data);

	// @author bytrain
	void buildBall(ball *&node, int n, int d, point *p);

	// @author sysuzzy
	bool storeTree(const char* index_path);

	// @author sysuzzy
	bool restoreTree(const char* index_path);
	
	// 搜索，并且返回查询向量的id
	// @author zhijian
	int mipSearch(int d, float* query);

	// 搜索，并且返回查询结果所在的叶子结点
	// @author zhijian
	ball *search(int d, float* query);

	// @author: painterdrown
	bool insertData(int d, float* data);

	// @author bytrain
	bool buildQuadTree(int n, int d, float** data);

	// @author bytrain
	void buildQuadBall(Quadball *&node, int n, int d, float **data);

	// @author 123zzj123
	float eval(int d, float* query, float Max, ball* Root);

	// @author painterdrown
	bool deleteData(int d, float* data);

	// 按树的层次遍历并且输出
	// @author painterdrown
	// void displayTree();

	// 根据pid记载硬盘中相应的page
	// @author painterdrown
	void loadPage(const int pid, float *page);

	// 根据bid加载相应的数据到私有成员block中
	// @author painterdrown
	void loadBlock(const int bid);
};

#endif
