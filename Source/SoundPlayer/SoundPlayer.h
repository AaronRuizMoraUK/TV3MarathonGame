#ifndef SOUND_PLAYER_H
#define SOUND_PLAYER_H

#include <Windows.h>
#include <map>
#include "Expat/XMLParser.h"

/**
* Manager of sounds.
*
* It is a singleton.
*/

class SoundPlayer : public CXMLParser
{
protected:
	static std::string pathToData;

	struct Sound {
		std::string filename;

		enum Type {
			NONE = 0
			, STREAM
			, SAMPLE
		};
		Type type;
		DWORD handle;

		Sound( )
			: filename( "" )
			, type( NONE )
			, handle( 0 )
		{}
	};

	typedef std::map<std::string, Sound> Sounds;
	Sounds sounds;

	int streams_volume;
	int samples_volume;

	DWORD old_stream;
	DWORD current_stream;

public:
	~SoundPlayer();

	static SoundPlayer * get();

	void load();
	void onStartElement (const std::string &elem, MKeyValue &atts);

	// Siempre se devuelve el channel reproducido por si se quiere almacenar
	DWORD playStream( const char *name, bool loop=true ); // Hace fade entre streams
	DWORD playSample( const char *name, bool loop=false );

	void stopStream( bool fading = false );
	void stopSample( DWORD channel, bool fading = false );
	void stopAllSamples( const char *name, bool fading = false );
	void stopAllSamples( bool fading = false );
	void stopAll( bool fading = false );
	
	int getStreamsVolume() const { return streams_volume; }
	int getSamplesVolume() const { return samples_volume; }
	void setStreamsVolume( int pstreams_volume );
	void setSamplesVolume( int psamples_volume );

private:
	static SoundPlayer * sound_player;
	SoundPlayer();
	
	void stopAllSamples( DWORD handle, bool fading = false );

	const Sound *getSound( const char *name, Sound::Type sound_type );

	void threatError();
};

#endif //SOUND_PLAYER_H
