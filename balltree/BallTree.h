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
	int dimesion;  //ά��
	int num;  // ��¼��������
	int numOfBlocks;  // ���������µ�Block
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

	// ���������ҷ��ز�ѯ������id
	// @author zhijian
	int mipSearch(int d, float* query);

	// @author 123zzj123
	float eval(int d, float* query, float Max, Ball* Root);

	// @author: painterdrown
	bool insertData(int d, float* data);

	// @author painterdrown
	bool deleteData(int d, float* data);

	// �����Ĳ�α����������
	// @author painterdrown
	void displayTree();

	// ����bid������Ӧ�����ݵ�˽�г�Աblock��
	// @author painterdrown
	void loadBlock(Ball *ball);
};

#endif
