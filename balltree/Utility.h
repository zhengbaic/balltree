#ifndef __UTILITY_H
#define __UTILITY_H
#include<vector>
#define L 256
using namespace std;
class ball {
public:
	ball* leftball;
	ball* rightball;

	float* CircleCenter;//Ô²ÐÄ
	float radius;//°ë¾¶
	int bid;
	ball() {
		radius = 0;
		bid = -1;
		leftball = rightball = NULL;
	}
};

void Analyse(ball *node, int n, int d, float **data);
bool read_data(
	int n,
	int d,
	float** &data,
	const char* file_name);
void Split(int n, int d, float* &a, float* &b, float** data);
float getDistanse(float* a, float *b, int d);
float** VectorToFloat(vector<float*> v);
#endif
