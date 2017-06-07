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
	static int PAGES_LIMIT;  // ���ֻ�ܼ��ض���ҳ���ڴ���
	Ball *root;
	Ball *targetball;
	Quadball *quadroot;
	string index_path;
	map<int, Page> pages;
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
	void loadBlock(const int pid, const int offset);

	// @author painterdrown
	void setPagesLimit(const int limit);
};

#endif
