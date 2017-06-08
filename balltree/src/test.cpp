#include <iostream>
#include <time.h>

#include "../include/BallTree.h"
#include "../include/Utility.h"

#define MNIST

#ifdef MNIST
char dataset[L] = "data/Mnist";
int n = 60000, d = 50;
int qn = 1000;
#endif

#ifdef Netflix
char dataset[L] = "data/Netflix";
int n = 17770, d = 50;
int qn = 1000;
#endif

int main() {
	char data_path[L], query_path[L];
	char index_path[L], output_path[L], output_linear_path[L];
	float** data = nullptr;
	float** query = nullptr;

	sprintf(data_path, "%s/src/dataset.txt", dataset);
	sprintf(query_path, "%s/src/query.txt", dataset);
	sprintf(index_path, "%s/index", dataset);
	sprintf(output_path, "%s/dst/answer.txt", dataset);
	sprintf(output_linear_path, "%s/dst/answer-linear.txt", dataset);

	read_data(n, d, data, data_path);
	read_data(qn, d, query, query_path);
	FILE* fout = fopen(output_path, "w");
	FILE* fout_linear = fopen(output_linear_path, "w");

	BallTree ball_tree1, ball_tree2, ball_tree3;
	clock_t start, end;

	// 建树
	printf("building tree ...\n");
	start = clock();
	ball_tree1.buildTree(n, d, data);
	end = clock();
	printf("building tree completed after %d ms!\n", (end - start) / 1000);

	// 保存树
	printf("storing tree ...\n");
	start = clock();
	ball_tree1.storeTree(index_path);
	end = clock();
	printf("storing tree completed after %d ms!\n", (end - start) / 1000);

	// 搜索树
	ball_tree2.restoreTree(index_path);
	printf("querying ...\n");
	start = clock();
	for (int i = 0; i < qn; i++) {
		int index = ball_tree2.mipSearch(d, query[i]);
		fprintf(fout, "%d\n", index);
	}
	fclose(fout);
	end = clock();
	printf("querying completed after %d ms!\n", (end - start) / 1000);

	// 线性搜索树（性能比较）
	ball_tree3.restoreTree(index_path);
	printf("linearly querying ...\n");
	start = clock();
	for (int i = 0; i < qn; i++) {
		int index = ball_tree2.linearSearch(d, query[i]);
		fprintf(fout_linear, "%d\n", index);
	}
	fclose(fout_linear);
	end = clock();
	printf("linearly querying completed after %d ms!\n", (end - start) / 1000);

	return 0;
}
