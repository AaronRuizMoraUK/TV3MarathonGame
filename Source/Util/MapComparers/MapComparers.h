#ifndef MAP_COMPARERS_H
#define MAP_COMPARERS_H

#include <vector>

struct CompareFloatVector {
	bool operator ()(const std::vector<float> &v1, const std::vector<float> &v2) const;
};

#endif // MAP_COMPARERS_H
