#pragma once

#ifndef SPLICE_FUNCTION_H
#define SPLICE_FUNCTION_H

#include <set>
#include <vector>
#include <queue>
 
using namespace std;

class SimpleFunction {
	public:
		SimpleFunction(const float &t0);
		SimpleFunction(const float &t0, const float &v0, const float &t1, const float &v1);
		float eval(const float &t0) const;
		float x0;
		float x1;
		float m;
		float b;
};

bool operator==(const float &v, const SimpleFunction &v2); 
bool operator<(const SimpleFunction &v1, const SimpleFunction &v2);

typedef  set<SimpleFunction> FunctionSet;

class SpliceFunction : public FunctionSet
{
public:
	SpliceFunction(void);
	~SpliceFunction(void);

	void add(const float &t0, const float &v0);
	void add(queue<float> * rr);
	float eval(const float &t0) const;
private:
	bool first;
	float lastT0;
	float lastV0;

	float tMin;
	float vMin;
	float tMax;
	float vMax;
};


#endif