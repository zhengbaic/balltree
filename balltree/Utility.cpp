#include "Utility.h"
#include<set>

bool read_data(int n, int d, float** &data, const char* file_name)
{
	FILE* fin = fopen(file_name, "r");
	if (!fin) {
		printf("%s doesn't exist!\n", file_name);
		return false;
	}

	int id;
	data = new float*[n];
	for (int i = 0; i < n; i++) {
		data[i] = new float[d];
		fscanf(fin, "%d", &id);
		for (int j = 0; j < d; j++) {
			fscanf(fin, "%f", &data[i][j]);
		}
	}

	printf("Finish reading %s\n", file_name);
	fclose(fin);

	return true;
}

float getDistanse(float* a, float *b, int d) {
	float r = 0;
	for (int i = 0; i < d; i++) {
		r += pow(a[i] - b[i], 2);
	}
	return r;
}

void Analyse(ball *node, int n, int d, float **data) {
	float* mean = new float[d] {.0f};
	for (int i = 0; i < n; i++) {
		for (int j = 0; j < d; j++) {
			mean[j] += data[i][j];
		}
	}
	for (int i = 0; i < d; i++) {
		mean[i] /= n;
	}

	node->CircleCenter = mean;

	float r = 0;
	for (int i = 0; i < n; i++) {
		r = max(r, getDistanse(mean, data[i], d));
	}

	node->radius = sqrt(r);
}

void QuadAnalyse(Quadball *node, int n, int d, float **data) {
	float* mean = new float[d] {.0f};
	for (int i = 0; i < n; i++) {
		for (int j = 0; j < d; j++) {
			mean[j] += data[i][j];
		}
	}
	for (int i = 0; i < d; i++) {
		mean[i] /= n;
	}

	node->CircleCenter = mean;

	float r = 0;
	for (int i = 0; i < n; i++) {
		r = max(r, getDistanse(mean, data[i], d));
	}

	node->radius = sqrt(r);
}

float* FindFurestPoint(float** data, float * po, int n, int d) {
	float* res = new float[d] {.0f};
	float max = 0;
	for (int i = 0; i < n; i++) {
		float cur = sqrt(getDistanse(po, data[i], d));
		if (max < cur) {
			max = cur;
			res = data[i];
		}
	}
	return res;
}
void FindFurestPoints(float** data, int n, int d, set<float*> s) {
	for (int i = 0; i < n; i++) {
		s.insert(FindFurestPoint(data, data[i], n, d));
	}
}
void Split(int n , int d, float* &a, float* &b, float** data) {
	float* randomPoint = data[0];
	a = new float[d] {.0f};
	b = new float[d] {.0f};
	a = FindFurestPoint(data, randomPoint, n, d);
	b = FindFurestPoint(data, a, n, d);
}

void QuadSplit(int n, int de, float* &a, float* &b, float* &c, float* &d, float** data, float* & mean) {
	float* randomPoint = data[0];
	a = new float[de] {.0f};
	b = new float[de] {.0f};
	c = new float[de] {.0f};
	d = new float[de] {.0f};

	a = FindFurestPoint(data, randomPoint, n, de);
	b = FindFurestPoint(data, a, n, de);

	set<float*> table;
	FindFurestPoints(data,n, de, table);
	set<float*>::iterator iter;
	float min = 1000;
	for (iter = table.begin(); iter != table.end(); iter++) {
		float cur = getDistanse(*iter, a, de) - getDistanse(*iter, b, de);
		if (cur < min) {
			min = cur;
			c = *iter;
		}
	}

	d = FindFurestPoint(data, c, n, de);
}

float** VectorToFloat(vector<float*> v) {
	int size = v.size();
	float **res = new float*[size];
	for (int i = 0; i < size; i++) {
		res[i] = v[i];
	}
	return res;
}

void outputfloat2(float** f, int n, int d) {
	for (int i = 0; i < n; i++) {
		for (int j = 0; j < d; j++) {
			printf("%f", f[i][j]);
		}
	}
}

void displayCenter(float* f, int d) {
	cout << "Ô²ÐÄ£º[";
	for (int i = 0; i < d; i++) {
		printf("%f ,", f[i]);
	}
	cout << "]" << endl;
}

float getMax(int d, float* query, ball* Root) {
	float Max = 0;
	for (int i = 0; i < d; i++) {
		Max += Root->CircleCenter[i] * query[i];
	}
	Max += Root->radius * getLength(d, query);
	return Max;
}

float getLength(int d, float* query) {
	float Length = 0;
	for (int i = 0; i < d; i++) {
		Length += pow(query[i], 2);
	}
	Length = sqrt(Length);
	return Length;
}

float getInnerproduct(int d, float * query, float * vec) {
	float Innerproduct = 0;
	for (int i = 0; i < d; i++) {
		Innerproduct += query[i] * vec[i];
	}
	return Innerproduct;
}
