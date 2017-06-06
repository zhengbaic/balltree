#ifndef __BALL_TREE_H
#define __BALL_TREE_H

#include "Utility.h"
#include <vector>
#include <map>
#include <queue>
#include <cstdio>
#define N0                    20
#define SIZE_OF_POINT(d)      4 + 4 * d
#define BYTES_PER_BLOCK(d)    N0 * SIZE_OF_POINT(d)
#define BLOCKS_PER_PAGE       16
#define POINTS_PER_PAGE       N0 * BLOCKS_PER_PAGE
using namespace std;

class BallTree {
private:
	int dimesion;//ά��

	int num; // ��¼��������

	int numOfBlocks;

	int pid;  // ��ǰʹ��ҳ��id
	int numOfBlock; // ����

	int target_bid;

	ball *root;

	ball *target;

	Quadball *Quadroot;

	point page[POINTS_PER_PAGE];

	point block[N0];
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
	void displayTree();

	// ����pid����Ӳ������Ӧ��page
	// @author painterdrown
	void loadPage(const int pid);

	// ����bid������Ӧ�����ݵ�˽�г�Աblock��
	// @author painterdrown
	void loadBlock(const int bid);

	// �滻ball�ڵ�
	// @author painterdrown
	void replaceBall(ball *oldBall, ball *newBall);

	// ����pid����page��Ӳ����
	// @author sysuzzy
	void savePage(const int pid);

	// ����bid����ÿ���page�е�λ��
	int getBlockPosInPage(const int bid);

	// д�뵥ҳ
	void savePage(int pid);

	ball* getRoot();

	int getNumOfBlock();

	void setNumOfBlock(int num);
};

#endif
