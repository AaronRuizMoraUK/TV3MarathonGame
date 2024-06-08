#ifndef MATERIAL_H
#define MATERIAL_H

#include <Windows.h>
#include <vector>
#include "Directx/Directx.h"

struct Texture;
struct Effect;
class CalModel;
//class ParticleEmitter;
//struct ShadowInformation;

/**
* Texture map struct
*/

struct TextureMap {
	const Texture *texture;
	const Texture *normalMap;
	std::string mapFilename;

	TextureMap()
		: texture( NULL )
		, normalMap( NULL )
		, mapFilename("")
	{
	}

};

/**
* Texture map vector
*/

class TextureMapVector : public std::vector<TextureMap *>
{
public:
	// Default constructor
	TextureMapVector() {}

	// Override copy constructor
	TextureMapVector( const TextureMapVector &source ) {
		copy(source);
	}

	// Destructor
	~TextureMapVector();

	// Override Operator '='
	void operator=(const TextureMapVector &source) {
		clear();
		copy(source);
	}

	// Override clear function
	void clear() {
		while( !empty() ) {
			TextureMap * textureMap = (*this)[size()-1];
			// TextureFactory is who destroies the texture!!
			if(textureMap)
				delete textureMap, textureMap=NULL;
			pop_back();
		}
	}

private:
	// Copy function
	void copy(const TextureMapVector &source) {
		for(unsigned int i=0; i<source.size(); ++i) {
			TextureMap *textureMap = new TextureMap( *(source[i]) );
			push_back(textureMap);
		}
	}
};

/**
* Material for object. This has the texture and shader.
*/

class Material
{
protected:
	struct AmbientColor {
		bool enabled;
		DWORD color;

		AmbientColor( ) : enabled( false ), color( 0xFF000000 ) {}
	};
	
	// Additional color for checking behaviours
	AmbientColor additionalColor;

	TextureMapVector textureMapVector;
	const Effect *shader;
	std::string shaderName;
	std::string techniqueName;

	D3DXMATRIX matWorldViewProj;
	D3DXMATRIX matWorld;

	float texture_speed;

public:
	~Material();

	// VERY VERY VERY OPTIONAL!!!
	Material(const std::string &filename, const std::string &pShaderName, const std::string &pTechniqueName);
	Material(const Texture *texture, const std::string &pShaderName, const std::string &pTechniqueName);

	void activate() const;
	void activateCal(const CalModel &calModel) const;
	//void activateParticle( const ParticleEmitter &particle ) const;
	//void activateShadow( const ShadowInformation *shadowInfs, int numShadowInformations ) const;

	const Effect & getShader() const;

	// TODO
	//void setTechnique(const std::string &technique);

	inline void setTextureSpeed( float atexture_speed ) { texture_speed = atexture_speed; }

	inline void setMatWorld(const D3DXMATRIX &pMatWorld) { 
		matWorld = pMatWorld;
	}

	inline void setMatWorldViewProj(const D3DXMATRIX &pMatWorldViewProj) { 
		matWorldViewProj = pMatWorldViewProj;
	}

	void changeTexture( const std::string &texture_name );

	void changeTextureAndNormal( const std::string &texture_name, const std::string &normalmap_texture );

	inline void enableAdditionalColor(DWORD color) {
#ifndef NDEBUG
		additionalColor.enabled=true;
		additionalColor.color=color;
#endif
	}

	inline void disableAdditionalColor() {
#ifndef NDEBUG
		additionalColor.enabled=false;
#endif
	}

};

class MaterialVector : public std::vector<Material *>
{
public:
	// Default constructor
	MaterialVector(){}

	// Override copy constructor
	MaterialVector(const MaterialVector &source) {
		copy(source);
	}

	// Destructor
	~MaterialVector();

	// Override Operator '='
	void operator=(const MaterialVector &source) {
		clear();
		copy(source);
	}

	// Override clear function
	void clear() {
		while( !empty() ) {
			Material * material = (*this)[size()-1];
			if(material)
				delete material, material=NULL;
			pop_back();
		}
	}

private:
	// Copy function
	void copy(const MaterialVector &source) {
		for(unsigned int i=0; i<source.size(); ++i) {
			Material *material = new Material( *(source[i]) );
			push_back(material);
		}
	}

};

#endif //MATERIAL_H
