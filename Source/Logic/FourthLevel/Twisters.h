#ifndef TWISTERS_H
#define TWISTERS_H

#include <vector>

class IntelligentObject;

class Twisters : public std::vector<IntelligentObject *> {
public:
	~Twisters( );
	void clear( );

	void add( int num_dopaminas );
	iterator del( iterator it );

	void updateAI( float deltaTime );
};

#endif //TWISTERS_H
