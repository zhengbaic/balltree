#ifndef __BALL_TREE_H
#define __BALL_TREE_H

#include "Utility.h"
#include <vector>
#include <map>
#include <queue>
#include <cstdio>
#include <string>
using namespace std;

class BallTree {
private:
	int dimesion;  //维度
	int num;  // 记录的总数量
	int numOfBlocks;  // 用于生成新的Block
	int targetid;
	Ball *root;
	Ball *targetball;
	Quadball *Quadroot;
	Page page;
	Block block;
	string index_path;
public:
	BallTree();

	~BallTree();

	// @author bytrain
	bool buildTree(int n, int d, float** data);

	// @author bytrain
	void buildBall(Ball *&node, int n, int d, Point *p);

	// @author bytrain
	bool buildQuadTree(int n, int d, float** data);

	// @author bytrain
	void buildQuadBall(Quadball *&node, int n, int d, float **data);

	// @author sysuzzy
	bool storeTree(const char* index_path);

	// @author sysuzzy
	bool restoreTree(const char* index_path);

	// 搜索，并且返回查询向量的id
	// @author zhijian
	int mipSearch(int d, float* query);

	// @author 123zzj123
	float eval(int d, float* query, float Max, Ball* Root);

	// @author: painterdrown
	bool insertData(int d, float* data);

	// @author painterdrown
	bool deleteData(int d, float* data);

	// 按树的层次遍历并且输出
	// @author painterdrown
	void displayTree();

	// 根据bid加载相应的数据到私有成员block中
	// @author painterdrown
	void loadBlock(Ball *ball);
};

#endif
