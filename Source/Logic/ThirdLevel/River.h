#ifndef RIVER_H
#define RIVER_H

#include <vector>
#include "Directx/Directx.h"
#include "Expat/XMLParser.h"
#include "Util/Tracker/Tracker.h"

class AnimatedObject;

class River : public CXMLParser {
protected:
	typedef std::vector<AnimatedObject *> RiverElements;
	RiverElements river_elements;

	typedef std::vector<float> Speeds;
	Speeds speeds;
	size_t curr_speed_idx;

	std::string river_particles;
	D3DXVECTOR3 start_river_point;
	D3DXVECTOR3 end_river_point;
	TTracker start_river_tracker;
	TTracker end_river_tracker;

private:
	bool first_time_loading;

public:
	River( );
	~River( );

	void load( );
	void onStartElement (const std::string &elem, MKeyValue &atts);

	void init( );
	void destroy( );

	float getCurrentSpeed( ) const;
	void nextRiverSpeed( );
	void prevRiverSpeed( );

	void updateAnimation( float delta_time );
	void render( );

protected:
	void changeSpeed( );
};

#endif //RIVER_H
