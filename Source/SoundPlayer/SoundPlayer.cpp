#include <cassert>
#include "SoundPlayer.h"
#include "bass.h"
#include "Global/Rendering.h"
#include "Global/Variables.h"
#include "Global/Geometry.h"
#include "Game/Game.h"

#define FADE_TIME_STREAMS 2000
#define FADE_TIME_SAMPLES 800
#define MAX_PLAYBACKS_PER_SAMPLE 10

std::string SoundPlayer::pathToData = "";
SoundPlayer * SoundPlayer::sound_player = NULL;

SoundPlayer::SoundPlayer() 
: streams_volume(0)
, samples_volume(0)
, old_stream(0)
, current_stream(0)
{
}

SoundPlayer::~SoundPlayer() {
	printDebugString("Destroying SoundPlayer...\n");
	BASS_Free();
}

SoundPlayer * SoundPlayer::get() 
{
	if(sound_player == NULL)
		sound_player = new SoundPlayer();

	return sound_player;
}

void SoundPlayer::load()
{
	// Try to initiate Bass, if not, initiate Bass without sound 
	if(!BASS_Init(1, 44100, 0, 0, NULL))
		BASS_Init(0, 44100, 0, 0, NULL);

	if( pathToData.empty( ) )
		pathToData = Global::data_locator.getPath("sound");
	bool ok = xmlParseFile( pathToData + "sounds.xml" );
	assert(ok);

	setStreamsVolume( Game::configOptions.music );
	setSamplesVolume( Game::configOptions.sfx );
}

void SoundPlayer::onStartElement (const std::string &elem, MKeyValue &atts) 
{
	if (elem == "ambient") 
	{
		const std::string &name = atts["name"];
		const std::string &file = atts["file"];

		Sound &sound = sounds[ name ];
		assert( sound.type == Sound::NONE );
		sound.filename = pathToData + file;
		sound.type = Sound::STREAM;
		sound.handle = BASS_StreamCreateFile(FALSE, sound.filename.c_str(), 0, 0, BASS_SAMPLE_LOOP); // Loop by default
		if( sound.handle == 0 )
			threatError();
	}
	else if (elem == "sample") 
	{
		const std::string &name = atts["name"];
		const std::string &file = atts["file"];

		Sound &sound = sounds[ name ];
		assert( sound.type == Sound::NONE );
		sound.filename = pathToData + file;
		sound.type = Sound::SAMPLE;
		sound.handle = BASS_SampleLoad(FALSE, sound.filename.c_str(), 0, 0, MAX_PLAYBACKS_PER_SAMPLE, 0); // Not loop by default
		if( sound.handle == 0 )
			threatError();
	}
}

// ---------------------------------------------------------------------------------------

void SoundPlayer::setStreamsVolume( int pstreams_volume ) {
	streams_volume = clamp( pstreams_volume, 0, 100 );
	BASS_SetConfig( BASS_CONFIG_GVOL_STREAM, streams_volume * 100); // BASS_CONFIG_GVOL_STREAM goes from 0 to 10000 
}

void SoundPlayer::setSamplesVolume( int psamples_volume ) {
	samples_volume = clamp( psamples_volume, 0, 100 );
	BASS_SetConfig( BASS_CONFIG_GVOL_SAMPLE, samples_volume * 100); // BASS_CONFIG_GVOL_STREAM goes from 0 to 10000 
}

// ---------------------------------------------------------------------------------------

DWORD SoundPlayer::playStream( const char *name, bool loop ) {
	const Sound *stream = getSound( name, Sound::STREAM );
	if( stream == NULL )
		return 0;

	// Get a Channel
	DWORD channel = 0;

	// No hay nada sonando ahora
	if( current_stream == 0 ) {
		assert( old_stream == 0 );
		channel = current_stream = stream->handle;
		BASS_ChannelSlideAttribute( channel, BASS_ATTRIB_VOL, 1.0f, FADE_TIME_STREAMS );
	}
	// Hay algo sonando
	else {
		old_stream = current_stream;
		channel = current_stream = stream->handle;
		BASS_ChannelSlideAttribute( old_stream, BASS_ATTRIB_VOL, 0.0f, FADE_TIME_STREAMS );
		BASS_ChannelSlideAttribute( channel, BASS_ATTRIB_VOL, 1.0f, FADE_TIME_STREAMS );
		old_stream = 0;
	}

	// Error
	if( channel == 0 ) {
		threatError();
		return 0;
	}

	// Loop by default, if it must not loop
	if( !loop ) {
		BASS_CHANNELINFO channel_info;
		BASS_ChannelGetInfo( channel, &channel_info );
		BASS_ChannelFlags(channel, 0, BASS_SAMPLE_LOOP); // remove the LOOP flag
	}

	// Play Channel
	BASS_ChannelPlay( channel, TRUE );
	return channel;
}

DWORD SoundPlayer::playSample( const char *name, bool loop ) {
	const Sound *sample = getSound( name, Sound::SAMPLE );
	if( sample == NULL )
		return 0;

	// Get a Channel
	DWORD channel = BASS_SampleGetChannel( sample->handle, FALSE );

	// Error que no sea NO FREE CHANNEL
	if( channel == 0 && BASS_ErrorGetCode() != BASS_ERROR_NOCHAN ) {
		threatError();
		return 0;
	}

	// No loop by default, if it must loop
	if( loop ) {
		BASS_CHANNELINFO channel_info;
		BASS_ChannelGetInfo( channel, &channel_info );
		BASS_ChannelFlags(channel, BASS_SAMPLE_LOOP, BASS_SAMPLE_LOOP); // set the LOOP flag
	}

	// Play Channel
	BASS_ChannelPlay( channel, TRUE );
	return channel;
}

void SoundPlayer::stopStream( bool fading ) {
	if( old_stream != 0 ) {
		BASS_ChannelStop( old_stream );
		old_stream = 0;
	}

	if( current_stream == 0 )
		return;

	if( fading )
		BASS_ChannelSlideAttribute( current_stream, BASS_ATTRIB_VOL, 0, FADE_TIME_STREAMS );
	else
		BASS_ChannelStop( current_stream );
	current_stream = 0;
}

void SoundPlayer::stopSample( DWORD channel, bool fading ) {
	if( channel == 0 )
		return;

	if( fading )
		BASS_ChannelSlideAttribute( channel, BASS_ATTRIB_VOL, 0, FADE_TIME_SAMPLES );
	else
		BASS_ChannelStop( channel );
}

void SoundPlayer::stopAllSamples( const char *name , bool fading ) {
	Sounds::const_iterator it = sounds.find( name );
	if( it == sounds.end( ) )
		return;
	const Sound &sound = it->second;
	assert( sound.type == Sound::SAMPLE );

	stopAllSamples( sound.handle, fading );
}

void SoundPlayer::stopAllSamples( DWORD handle, bool fading ) {
	HCHANNEL *channels = new HCHANNEL[ MAX_PLAYBACKS_PER_SAMPLE ];         // allocate channels array
	DWORD num_channels = BASS_SampleGetChannels( handle, channels ); // get the channels

	for( DWORD i=0; i<num_channels; ++i ) // go through them all and...
		stopSample( channels[i], fading );  // stop them

	delete [] channels; // free the channels array
}

void SoundPlayer::stopAllSamples( bool fading ) {
	Sounds::iterator it = sounds.begin( );
	while( it != sounds.end( ) ) {
		Sound &sound = it->second;
		if( sound.type == Sound::SAMPLE )
			stopAllSamples( sound.handle, fading );
		++it;
	}
}

void SoundPlayer::stopAll( bool fading ) {
	if( !fading ) {
		BASS_Stop( );
		old_stream = 0;
		current_stream = 0;
		BASS_Start( );
	}
	else {
		stopStream( fading );
		stopAllSamples( fading );
	}
}

const SoundPlayer::Sound *SoundPlayer::getSound( const char *name, Sound::Type sound_type ) {
	assert( sound_type != Sound::NONE );

	Sounds::const_iterator it = sounds.find( name );
	if( it == sounds.end( ) ) {
		warn( "%s not found: %s", (sound_type==Sound::STREAM) ? "Ambient" : "Sample", name );
		return NULL;
	}

	const Sound &sound = it->second;
	if( sound.type != sound_type ) {
		warn( "%s not found: %s", (sound_type==Sound::STREAM) ? "Ambient" : "Sample", name );
		return NULL;
	}

	return &sound;
}

// ---------------------------------------------------------------------------------------

void SoundPlayer::threatError()
{
	int errCode = BASS_ErrorGetCode();
	if( errCode == BASS_OK )
		return;

	switch(errCode)
	{
	case BASS_ERROR_MEM:	// memory error
		printDebugString("BASS ERROR: memory error\n");
		break;
	case BASS_ERROR_FILEOPEN:	// can't open the file
		printDebugString("BASS ERROR: can't open the file\n");
		break;
	case BASS_ERROR_DRIVER:	// can't find a free/valid driver
		printDebugString("BASS ERROR: can't find a free/valid driver\n");
		break;
	case BASS_ERROR_BUFLOST:	// the sample buffer was lost
		printDebugString("BASS ERROR: the sample buffer was lost\n");
		break;
	case BASS_ERROR_HANDLE:	// invalid handle
		printDebugString("BASS ERROR: invalid handle\n");
		break;
	case BASS_ERROR_FORMAT:	// unsupported sample format
		printDebugString("BASS ERROR: unsupported sample format\n");
		break;
	case BASS_ERROR_POSITION:	// invalid playback position
		printDebugString("BASS ERROR: invalid playback position\n");
		break;
	case BASS_ERROR_INIT:	// BASS_Init has not been successfully called
		printDebugString("BASS ERROR: BASS_Init has not been successfully called\n");
		break;
	case BASS_ERROR_START:	// BASS_Start has not been successfully called
		printDebugString("BASS ERROR: BASS_Start has not been successfully called\n");
		break;
	case BASS_ERROR_SSL:	// SSL/HTTPS support isn't available
		printDebugString("BASS ERROR: SSL/HTTPS support isn't available\n");
		break;
	case BASS_ERROR_ALREADY:	// already initialized/paused/whatever
		printDebugString("BASS ERROR: already initialized/paused/whatever\n");
		break;
	case BASS_ERROR_NOTAUDIO:	// file does not contain audio
		printDebugString("BASS ERROR: file does not contain audio\n");
		break;
	case BASS_ERROR_NOCHAN:	// can't get a free channel
		printDebugString("BASS ERROR: can't get a free channel\n");
		break;
	case BASS_ERROR_ILLTYPE:	// an illegal type was specified
		printDebugString("BASS ERROR: an illegal type was specified\n");
		break;
	case BASS_ERROR_ILLPARAM:	// an illegal parameter was specified
		printDebugString("BASS ERROR: an illegal parameter was specified\n");
		break;
	case BASS_ERROR_NO3D:	// no 3D support
		printDebugString("BASS ERROR: no 3D support\n");
		break;
	case BASS_ERROR_NOEAX:	// no EAX support
		printDebugString("BASS ERROR: no EAX support\n");
		break;
	case BASS_ERROR_DEVICE:	// illegal device number
		printDebugString("BASS ERROR: illegal device number\n");
		break;
	case BASS_ERROR_NOPLAY:	// not playing
		printDebugString("BASS ERROR: not playing\n");
		break;
	case BASS_ERROR_FREQ:	// illegal sample rate
		printDebugString("BASS ERROR: illegal sample rate\n");
		break;
	case BASS_ERROR_NOTFILE:	// the stream is not a file stream
		printDebugString("BASS ERROR: the stream is not a file stream\n");
		break;
	case BASS_ERROR_NOHW:	// no hardware voices available
		printDebugString("BASS ERROR: no hardware voices available\n");
		break;
	case BASS_ERROR_EMPTY:	// the MOD music has no sequence data
		printDebugString("BASS ERROR: the MOD music has no sequence data\n");
		break;
	case BASS_ERROR_NONET:	// no internet connection could be opened
		printDebugString("BASS ERROR: no internet connection could be opened\n");
		break;
	case BASS_ERROR_CREATE:	// couldn't create the file
		printDebugString("BASS ERROR: couldn't create the file\n");
		break;
	case BASS_ERROR_NOFX:	// effects are not available
		printDebugString("BASS ERROR: effects are not available\n");
		break;
	case BASS_ERROR_NOTAVAIL:	// requested data is not available
		printDebugString("BASS ERROR: requested data is not available\n");
		break;
	case BASS_ERROR_DECODE:	// the channel is a decoding channel
		printDebugString("BASS ERROR: the channel is a decoding channel\n");
		break;
	case BASS_ERROR_DX:	// a sufficient DirectX version is not installed
		printDebugString("BASS ERROR: a sufficient DirectX version is not installed\n");
		break;
	case BASS_ERROR_TIMEOUT:	// connection timedout
		printDebugString("BASS ERROR: connection timedout\n");
		break;
	case BASS_ERROR_FILEFORM:	// unsupported file format
		printDebugString("BASS ERROR: unsupported file format\n");
		break;
	case BASS_ERROR_SPEAKER:	// unavailable speaker
		printDebugString("BASS ERROR: unavailable speaker\n");
		break;
	case BASS_ERROR_VERSION:	// invalid BASS version (used by add-ons)
		printDebugString("BASS ERROR: invalid BASS version (used by add-ons)\n");
		break;
	case BASS_ERROR_CODEC:	// codec is not available/supported
		printDebugString("BASS ERROR: codec is not available/supported\n");
		break;
	case BASS_ERROR_ENDED:	// the channel/file has ended
		printDebugString("BASS ERROR: the channel/file has ended\n");
		break;
	case BASS_ERROR_BUSY:	// the device is busy
		printDebugString("BASS ERROR: the device is busyd\n");
		break;
	case BASS_ERROR_UNSTREAMABLE:	// unstreamable file
		printDebugString("BASS ERROR: unstreamable file\n");
		break;
	default:	// some other mystery error
		printDebugString("BASS ERROR: some other mystery error\n");
		break;
	}

	assert( !"Bass Error" );
}

// ---------------------------------------------------------------------------------------

/*
// SE QUEDA PILLADO EL FICHERO DE SONIDO, NO TIENE SENTIDO RECARGAR
void SoundPlayer::reload( ) {
printDebugString( "Reloading sounds...\n" );
stopAll( );

Sounds::iterator it = sounds.begin( );
while( it != sounds.end( ) ) {
Sound &sound = it->second;

if( sound.type == Sound::STREAM ) {
BASS_StreamFree( sound.handle );
sound.handle = 0;
sound.handle = BASS_StreamCreateFile(FALSE, sound.filename.c_str(), 0, 0, BASS_SAMPLE_LOOP);
}
else {
BASS_SampleFree( sound.handle );
sound.handle = 0;
sound.handle = BASS_SampleLoad(FALSE, sound.filename.c_str(), 0, 0, MAX_PLAYBACKS_PER_SAMPLE, 0);
}

if( sound.handle == 0 )
threatError();

++it;
}
}
*/
