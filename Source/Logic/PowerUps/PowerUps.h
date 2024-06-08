#ifndef POWER_UPS_H
#define POWER_UPS_H

#include <vector>
#include "Directx/Directx.h"
#include "XMLParser/XMLParser.h"

class IntelligentObject;

// -----------------------------------------------------------------------------------------
class PowerUps : public std::vector<IntelligentObject *>, public CXMLParser {
protected:
	struct PowerUpBornPoint {
	private:
		D3DXVECTOR3 position;
		float       powerup_life_probability;
		float       powerup_score_probability;
		float       total_probability;

		// The first time we play we are going to calculate and save the transform, and no calculate again
		bool        initiated;
		D3DXMATRIX  transform;
		float       alpha;
		float       delta;

	public:
		const IntelligentObject *powerup_associated; // Powerup asociado a este PowerUpBornPoint

	public:
		PowerUpBornPoint( const D3DXVECTOR3 &aposition, float apowerup_life_probability, float apowerup_score_probability );

		const IntelligentObject &getPowerUpToCreate( ) const;
		const D3DXMATRIX &getTransform( );

		float getAlpha( ) const;
		float getDelta( ) const;
	};

	typedef std::vector< PowerUpBornPoint * > PowerUpBornPoints;
	PowerUpBornPoints powerup_born_points; // XML
	PowerUpBornPoints non_used_power_up_born_points;
	PowerUpBornPoints used_power_up_born_points;

	size_t max_powerups_same_time;

public:
	PowerUps( ) : max_powerups_same_time( 0 ) {}
	~PowerUps( );
	void clear( );

	void onStartElement( const std::string &elem, MKeyValue &atts );

	void add( int num_power_ups );
	iterator del( iterator it );

	void updateAnimation( float deltaTime );
	void updateAI( float deltaTime );
	void render( );

	inline size_t getNumPowerUpsBornPoints( ) const { return powerup_born_points.size( ); }
	inline size_t getNumNonUsedPowerUpsBornPoints( ) const { return non_used_power_up_born_points.size( ); }
	inline size_t getNumUsedPowerUpsBornPoints( ) const { return used_power_up_born_points.size( ); }
	inline size_t getMaxPowerUpsSameTime( ) const { return max_powerups_same_time; }

private:
	void resetPowerUpBornPoints( );
	void setPowerUpBornPointAsUsed( size_t powerup_born_idx, const IntelligentObject *powerup );
	void setPowerUpBornPointAsNotUsed( const IntelligentObject *powerup );
};

#endif //POWER_UPS_H
