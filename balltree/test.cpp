#include <iostream>

#include "BallTree.h"
#include "Utility.h"

#define Netflix

#ifdef Netflix
char dataset[L] = "Netflix";
int n = 200;
int	d = 50;
int qn = 1000;
#endif // Netflix

#ifdef MNIST
char dataset[L] = "Mnist";
int n = 600, d = 50;
int qn = 1000;
#endif

#ifdef YAHOO
char dataset[L] = "Yahoo";
int n = 624, d = 300;
int qn = 1000;
#endif

int main() {
	char data_path[L], query_path[L];
	char index_path[L], output_path[L];
	float** data = nullptr;
	float** query = nullptr;

	sprintf(data_path, "%s/src/dataset.txt", dataset);
	sprintf(query_path, "%s/src/query.txt", dataset);
	sprintf(index_path, "%s/index", dataset);
	sprintf(output_path, "%s/dst/answer.txt", dataset);

	if (!read_data(n, d, data, data_path)) {
		return 1;
	}

	BallTree ball_tree1;
	ball_tree1.buildTree(n, d, data);
	cout << endl << endl << endl << "����" << endl;
	// �鿴������Ľ�������ڵ���
	/*cout << endl << endl << endl << "����" << endl;
	output(ball_tree1.root);*/

	ball_tree1.storeTree(index_path);

	if (!read_data(qn, d, query, query_path));
	FILE* fout = fopen(output_path, "w");
	if (!fout) {
		printf("can't open %s!\n", output_path);
		return 1;
	}

	//BallTree ball_tree2;
	//ball_tree2.restoreTree(index_path);

	// �鿴��ȡ�ļ���Ľ�������ڵ���
	/*cout << endl << endl << endl << "�ļ���ȡ" << endl;
	output(ball_tree2.root);
	cout << "finish" << endl;*/


	//for (int i = 0; i < qn; i++) {
	//	int index = ball_tree2.mipSearch(d, query[i]);
	//	fprintf(fout, "%d\n", index);
	//}
	//fclose(fout);

	/*for (int i = 0; i < n; i++) {
		delete[] data[i];
	}

	for (int i = 0; i < qn; i++) {
		delete[] query[i];
	}*/
	return 0;
}