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
	int dimesion;//ά��

	int num; //����

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
	
	// ���������ҷ��ز�ѯ������id
	// @author zhijian
	int mipSearch(int d, float* query);

	// ���������ҷ��ز�ѯ������ڵ�Ҷ�ӽ��
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

	// �����Ĳ�α����������
	// @author painterdrown
	// void displayTree();

	// ����pid����Ӳ������Ӧ��page
	// @author painterdrown
	void loadPage(const int pid, float *page);

	// ����bid������Ӧ�����ݵ�˽�г�Աblock��
	// @author painterdrown
	void loadBlock(const int bid);
};

#endif
