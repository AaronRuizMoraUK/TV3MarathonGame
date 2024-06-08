#ifndef DOPAMINAS_BRAIN4_H
#define DOPAMINAS_BRAIN4_H

#include <vector>

class IntelligentObject;

class DopaminasBrain4 : public std::vector<IntelligentObject *> {
public:
	~DopaminasBrain4( );
	void clear( );

	void add( int num_dopaminas, const char *dopamina_type );
	iterator del( iterator it );

	void updateAI( float deltaTime );
	void render( );
};

#endif //DOPAMINAS_BRAIN4_H
