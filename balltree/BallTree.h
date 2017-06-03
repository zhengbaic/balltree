#ifndef __BALL_TREE_H
#define __BALL_TREE_H
#include"Utility.h"
#define N0 20

class BallTree {
public:
	static int bid;
	int target_bid;
	int dimesion;//Î¬¶È
	int num; //ÊýÁ¿
	ball *root;
	BallTree();
	~BallTree();

	bool buildTree(
		int n,
		int d,
		float** data);

	void buildBall(ball *&node, int n, int d, float **data);

	bool storeTree(
		const char* index_path);

	bool restoreTree(
		const char* index_path);
	
	int mipSearch(
		int d,
		float* query);

	float eval(int d, float* query, float Max, ball* Root);

	// optional
	bool insertData(
		int d,
		float* data);

	// optional
	bool deleteData(
		int d,
		float* data);

	// optional
	bool buildQuadTree(
		int n,
		int d,
		float** data);
};

#endif
