#ifndef ALUCINACIONES_H
#define ALUCINACIONES_H

#include <vector>

class IntelligentObject;

class Alucinaciones : public std::vector<IntelligentObject *> {
public:
	~Alucinaciones( );
	void clear( );

	void add( int num_dopaminas );
	iterator del( iterator it );

	void updateAI( float deltaTime );
	void updateAnimation( float deltaTime );
	void render( );
};

#endif //ALUCINACIONES_H
