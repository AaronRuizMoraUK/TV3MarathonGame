
#include <vector>
#include "SpliceFunction.h"

using namespace std;

SimpleFunction::SimpleFunction(const float &t0) {
	x0 = t0;
	x1 = t0;
}

SimpleFunction::SimpleFunction(const float &t0, const float &v0, const float &t1, const float &v1) {
	x0 = t0;
	x1 = t1;
	m = (v1 - v0) / (t1 - t0);
	b = v1 - m * t1;
}

bool operator==(const float &v, const SimpleFunction &v2) {
	return (v >= v2.x0) && (v <= v2.x1);
}

bool operator!=(const SimpleFunction &v1, const SimpleFunction &v2) {
	return (v1.x0 < v2.x0) || (v1.x0 > v2.x1);
}

bool operator<(const SimpleFunction &v1, const SimpleFunction &v2) {
	// Check Equals
	if ((v1.x0 == v1.x1) || (v2.x0 == v2.x1)) {
		if (v1.x0 == v1.x1) {
			if (v1.x0 == v2)
				return false;
		} else {
			if (v2.x0 == v1)
				return false;
		} 
	}
	// Check Less
	return (v1.x0 < v2.x0);
}

float SimpleFunction::eval(const float &t0) const {
	return (t0 * m + b);
}

SpliceFunction::SpliceFunction()
{
	first = true;
}

SpliceFunction::~SpliceFunction(void)
{
}

void SpliceFunction::add(queue<float> * rr) {
	while (!(rr->empty())) {
		float t0 = (*rr).front();
		rr->pop();
		float v0 = rr->front();
		rr->pop();
		add(t0, v0);
	}
}

void SpliceFunction::add(const float &t0, const float &v0) {
	if (!first) {
		this->insert(SimpleFunction(lastT0, lastV0, t0, v0));
		if (t0 < tMin) {
			tMin = t0;
			vMin = v0;
		}

		if(t0 > tMax) {
			tMax = t0;
			vMax = v0;
		}
	} else {
		first = false;
		tMin = t0;
		vMin = v0;
		tMax = t0;
		vMax = v0;
	}
	lastT0 = t0;
	lastV0 = v0;
}

float SpliceFunction::eval(const float &t0) const {
	if (size() == 0)
		return vMin;

	// Check out of definition
	if (t0 < tMin)
		return vMin;
	else
		if (t0 > tMax)
			return vMax;

	FunctionSet::iterator it = find(SimpleFunction(t0));
	if (it != end()) {
		return it->eval(t0);
	} 
	return 0.0f;
}