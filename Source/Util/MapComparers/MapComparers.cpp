#include <cassert>
#include "MapComparers.h"

bool CompareFloatVector::operator()(const std::vector<float> &v1, const std::vector<float> &v2) const {
	assert(v1.size()==v2.size());

	// Compare v1 and v2 and return true if v1 < v2
	for(size_t i=0; i<v1.size(); ++i) {
		if( v1[i] != v2[i] )
			return v1[i]<v2[i];
	}

	return false;
}
