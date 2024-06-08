#ifndef DOPAMINAS_H
#define DOPAMINAS_H

#include <vector>

class IntelligentObject;

class Dopaminas : public std::vector<IntelligentObject *> {
public:
	~Dopaminas( );
	void clear( );

	void add( int num_dopaminas );
	iterator del( iterator it );

	void updateAI( float deltaTime );
	void render( );
};

#endif //DOPAMINAS_H
