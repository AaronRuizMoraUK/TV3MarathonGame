#include "ActionPSManager.h"
#include "XMLParser/XMLParser.h"
#include "DomainGenerator.h"

using namespace std;

const int 	NUM_AVOID				= 	0;
const int 	NUM_BOUNCE				= 	1;
const int 	NUM_DAMPING				= 	2;
const int 	NUM_ROTDAMPING			= 	3;
const int 	NUM_EXPLOSION			= 	4;
const int 	NUM_FOLLOW				= 	5;
const int 	NUM_GRAVITATE			= 	6;
const int 	NUM_GRAVITY				= 	7;
const int 	NUM_JET					= 	8;
const int 	NUM_KILLOLD				= 	9;
const int 	NUM_MATCHVELOCITY		= 	10;
const int 	NUM_MATCHROTVELOCITY	= 	11;
const int 	NUM_MOVE				= 	12;
const int 	NUM_ORBITLINE			= 	13;
const int 	NUM_ORBITPOINT			= 	14;
const int 	NUM_RANDOMACCEL			= 	15;
const int 	NUM_RANDOMDISPLACE		= 	16;
const int 	NUM_RANDOMVELOCITY		= 	17;
const int 	NUM_RANDOMROTVELOCITY	= 	18;
const int 	NUM_RESTORE				= 	19;
const int 	NUM_SINK				= 	20;
const int 	NUM_SINKVELOCITY		= 	21;
const int 	NUM_SORT				= 	22;
const int 	NUM_SOURCE				= 	23;
const int 	NUM_SPEEDLIMIT			= 	24;
const int 	NUM_TARGETSIZE			= 	25;
const int 	NUM_TARGETVELOCITY		= 	26;
const int 	NUM_TARGETROTVELOCITY	= 	27;
const int 	NUM_VERTEX				= 	28;
const int 	NUM_VORTEX				= 	29;

const int 	NUM_SIZE				= 	30;
const int 	NUM_MASS				= 	31;
const int 	NUM_ROTVELOCITY			= 	32;
const int 	NUM_UPVEC				= 	33;
const int 	NUM_VELOCITY			= 	34;
const int 	NUM_COLOR				= 	35;

const int 	NUM_KILLOLDALPHA		= 	100;
const int 	NUM_ALPHASTART			= 	101;
const int 	NUM_FUNCTIONCOLOR		= 	102;
const int 	NUM_FUNCTIONALPHA		= 	103;
const int 	NUM_FUNCTIONSIZE		= 	104;
const int 	NUM_AIRTUBE				= 	105;

const string 	STR_AVOID		= 	"AVOID";
const string 	STR_BOUNCE		= 	"BOUNCE";
const string 	STR_DAMPING		= 	"DAMPING";
const string 	STR_ROTDAMPING	= 	"ROTDAMPING";
const string 	STR_EXPLOSION	= 	"EXPLOSION";
const string 	STR_FOLLOW		= 	"FOLLOW";
const string 	STR_GRAVITATE	= 	"GRAVITATE";
const string 	STR_GRAVITY		= 	"GRAVITY";
const string 	STR_JET			= 	"JET";
const string 	STR_KILLOLD		= 	"KILLOLD";
const string 	STR_MATCHVELOCITY		= 	"MATCHVELOCITY";
const string 	STR_MATCHROTVELOCITY	= 	"MATCHROTVELOCITY";
const string 	STR_MOVE				= 	"MOVE";
const string 	STR_ORBITLINE			= 	"ORBITLINE";
const string 	STR_ORBITPOINT			= 	"ORBITPOINT";
const string 	STR_RANDOMACCEL			= 	"RANDOMACCEL";
const string 	STR_RANDOMDISPLACE		= 	"RANDOMDISPLACE";
const string 	STR_RANDOMVELOCITY		= 	"RANDOMVELOCITY";
const string 	STR_RANDOMROTVELOCITY	= 	"RANDOMROTVELOCITY";
const string 	STR_RESTORE				= 	"RESTORE";
const string 	STR_SINK				= 	"SINK";
const string 	STR_SINKVELOCITY		= 	"SINKVELOCITY";
const string 	STR_SORT				= 	"SORT";
const string 	STR_SOURCE				= 	"SOURCE";
const string 	STR_SPEEDLIMIT			= 	"SPEEDLIMIT";
const string 	STR_TARGETSIZE			= 	"TARGETSIZE";
const string 	STR_TARGETVELOCITY		= 	"TARGETVELOCITY";
const string 	STR_TARGETROTVELOCITY	= 	"TARGETROTVELOCITY";
const string 	STR_VERTEX				= 	"VERTEX";
const string 	STR_VORTEX				= 	"VORTEX";

const string 	STR_SIZE				= 	"SIZE";
const string 	STR_MASS				= 	"MASS";
const string 	STR_ROTVELOCITY			= 	"ROTVELOCITY";
const string 	STR_UPVEC				= 	"UPVEC";
const string 	STR_VELOCITY			= 	"VELOCITY";
const string 	STR_COLOR				= 	"COLOR";

const string 	STR_KILLOLDALPHA		= 	"KILLOLDALPHA";
const string	STR_ALPHASTART			=	"ALPHASTART";
const string	STR_FUNCTIONCOLOR		=	"FUNCTIONCOLOR";
const string	STR_FUNCTIONALPHA		= 	"FUNCTIONALPHA";
const string	STR_FUNCTIONSIZE		= 	"FUNCTIONSIZE";
const string	STR_AIRTUBE				=   "AIRTUBE";

ActionPSManager::ActionPSManager(void) {
	nameDomain[STR_AVOID] =	NUM_AVOID;
	nameDomain[STR_BOUNCE] =	NUM_BOUNCE;
	nameDomain[STR_DAMPING] =	NUM_DAMPING;
	nameDomain[STR_ROTDAMPING] =	NUM_ROTDAMPING;
	nameDomain[STR_EXPLOSION] =	NUM_EXPLOSION;
	nameDomain[STR_FOLLOW] =	NUM_FOLLOW;
	nameDomain[STR_GRAVITATE] =	NUM_GRAVITATE;
	nameDomain[STR_GRAVITY] =	NUM_GRAVITY;
	nameDomain[STR_JET] =	NUM_JET;
	nameDomain[STR_KILLOLD] =	NUM_KILLOLD;
	nameDomain[STR_MATCHVELOCITY] =	NUM_MATCHVELOCITY;
	nameDomain[STR_MATCHROTVELOCITY] =	NUM_MATCHROTVELOCITY;
	nameDomain[STR_MOVE] =	NUM_MOVE;
	nameDomain[STR_ORBITLINE] =	NUM_ORBITLINE;
	nameDomain[STR_ORBITPOINT] =	NUM_ORBITPOINT;
	nameDomain[STR_RANDOMACCEL] =	NUM_RANDOMACCEL;
	nameDomain[STR_RANDOMDISPLACE] =	NUM_RANDOMDISPLACE;
	nameDomain[STR_RANDOMVELOCITY] =	NUM_RANDOMVELOCITY;
	nameDomain[STR_RANDOMROTVELOCITY] =	NUM_RANDOMROTVELOCITY;
	nameDomain[STR_RESTORE] =	NUM_RESTORE;
	nameDomain[STR_SINK] =	NUM_SINK;
	nameDomain[STR_SINKVELOCITY] =	NUM_SINKVELOCITY;
	nameDomain[STR_SORT] =	NUM_SORT;
	nameDomain[STR_SOURCE] =	NUM_SOURCE;
	nameDomain[STR_SPEEDLIMIT] =	NUM_SPEEDLIMIT;
	nameDomain[STR_TARGETSIZE] =	NUM_TARGETSIZE;
	nameDomain[STR_TARGETVELOCITY] =	NUM_TARGETVELOCITY;
	nameDomain[STR_TARGETROTVELOCITY] =	NUM_TARGETROTVELOCITY;
	nameDomain[STR_VERTEX] =	NUM_VERTEX;
	nameDomain[STR_VORTEX] =	NUM_VORTEX;

	nameDomain[STR_SIZE] = NUM_SIZE;
	nameDomain[STR_MASS] = NUM_MASS;
	nameDomain[STR_ROTVELOCITY] = NUM_ROTVELOCITY;
	nameDomain[STR_UPVEC] = NUM_UPVEC;
	nameDomain[STR_VELOCITY] = NUM_VELOCITY;
	nameDomain[STR_COLOR] = NUM_COLOR;

	nameDomain[STR_KILLOLDALPHA] = NUM_KILLOLDALPHA;
	nameDomain[STR_ALPHASTART] = NUM_ALPHASTART;
	nameDomain[STR_FUNCTIONCOLOR] = NUM_FUNCTIONCOLOR;
	nameDomain[STR_FUNCTIONALPHA] = NUM_FUNCTIONALPHA;
	nameDomain[STR_FUNCTIONSIZE] = 	NUM_FUNCTIONSIZE;
	nameDomain[STR_AIRTUBE] = NUM_AIRTUBE;
}

ActionPSManager::~ActionPSManager(void) {
	
}

void ActionPSManager::addSample(MKeyValue &atts) {
	(functions.top())->push(atts.getFloat("t", 1.0f));
	(functions.top())->push(atts.getFloat("v", 1.0f));
}

void ActionPSManager::enterAction(const string &action, MKeyValue &atts) {
	map<string, int>::iterator it = nameDomain.find(action);
	if (it != nameDomain.end()) {
		int code = it->second;
		switch(code) {
			case NUM_AVOID: {
					parametersF.push(atts.getFloat("magnitude", 0.0f));
					parametersF.push(atts.getFloat("epsilon", 0.0f));
					parametersF.push(atts.getFloat("look_ahead", 0.0f));
				} break;
			case NUM_BOUNCE: {
					parametersF.push(atts.getFloat("friction", 0.0f));
					parametersF.push(atts.getFloat("resilience", 0.0f));
					parametersF.push(atts.getFloat("cutoff", 0.0f));
				} break;
			case NUM_DAMPING: {
					parametersF.push(atts.getFloat("vlow", 0.0f));
					parametersF.push(atts.getFloat("vhigh", 0.0f));
				} break;
			case NUM_ROTDAMPING: {
					parametersF.push(atts.getFloat("vlow", 0.0f));
					parametersF.push(atts.getFloat("vhigh", 0.0f));
				} break;
			case NUM_EXPLOSION: {
					parametersF.push(atts.getFloat("radius", 0.0f));
					parametersF.push(atts.getFloat("magnitude", 0.0f));
					parametersF.push(atts.getFloat("sigma", 0.0f));
					parametersF.push(atts.getFloat("epsilon", 0.0f));
				} break;
			case NUM_FOLLOW: {
					parametersF.push(atts.getFloat("magnitude", 0.0f));
					parametersF.push(atts.getFloat("epsilon", 0.0f));
					parametersF.push(atts.getFloat("max_radius", 0.0f));
				} break;
			case NUM_GRAVITATE: {
					parametersF.push(atts.getFloat("magnitude", 0.0f));
					parametersF.push(atts.getFloat("epsilon", 0.0f));
					parametersF.push(atts.getFloat("max_radius", 0.0f));
				} break;
			case NUM_GRAVITY: {
				} break;
			case NUM_JET: {
				} break;
			case NUM_KILLOLD: {
					parametersF.push(atts.getFloat("age_limit", 0.0f));
				} break;
			case NUM_MATCHVELOCITY: {
					parametersF.push(atts.getFloat("magnitude", 0.0f));
					parametersF.push(atts.getFloat("epsilon", 0.0f));
					parametersF.push(atts.getFloat("max_radius", 0.0f));
				} break;
			case NUM_MATCHROTVELOCITY: {
					parametersF.push(atts.getFloat("magnitude", 0.0f));
					parametersF.push(atts.getFloat("epsilon", 0.0f));
					parametersF.push(atts.getFloat("max_radius", 0.0f));
				} break;
			case NUM_MOVE: {
					parametersB.push(atts.getBool("move_velocity", true));
					parametersB.push(atts.getBool("move_rotational_velocity", false));
				} break;
			case NUM_ORBITLINE: {
					parametersF.push(atts.getFloat("magnitude", 0.0f));
					parametersF.push(atts.getFloat("epsilon", 0.0f));
					parametersF.push(atts.getFloat("max_radius", 0.0f));
				} break;
			case NUM_ORBITPOINT: {
					parametersF.push(atts.getFloat("magnitude", 0.0f));
					parametersF.push(atts.getFloat("epsilon", 0.0f));
					parametersF.push(atts.getFloat("max_radius", 0.0f));
				} break;
			case NUM_RANDOMACCEL: {
				} break;
			case NUM_RANDOMDISPLACE: {
				} break;
			case NUM_RANDOMVELOCITY: {
				} break;
			case NUM_RANDOMROTVELOCITY: {
				} break;
			case NUM_RESTORE: {
					parametersF.push(atts.getFloat("time", 0.0f));
					parametersB.push(atts.getBool("vel", true));
					parametersB.push(atts.getBool("rvel", true));
				} break;
			case NUM_SINK: {
					parametersB.push(atts.getBool("kill_inside", false));
				} break;
			case NUM_SINKVELOCITY: {
					parametersB.push(atts.getBool("kill_inside", false));
				} break;
			case NUM_SORT: {
					parametersB.push(atts.getBool("front_to_back", true));
					parametersB.push(atts.getBool("clamp_negative", true));
				} break;
			case NUM_SOURCE: {
					parametersF.push(atts.getFloat("particle_rate", 0.0f));
				} break;
			case NUM_SPEEDLIMIT: {
					parametersF.push(atts.getFloat("min_speed", 0.0f));
					parametersF.push(atts.getFloat("max_speed", 0.0f));
				} break;
			case NUM_TARGETSIZE: {
					parametersF.push(atts.getFloat("scale", 0.0f));
				} break;
			case NUM_TARGETVELOCITY: {
					parametersF.push(atts.getFloat("scale", 0.0f));
				} break;
			case NUM_TARGETROTVELOCITY: {
					parametersF.push(atts.getFloat("scale", 0.0f));
				} break;
			case NUM_VERTEX: {
				} break;
			case NUM_VORTEX: {
					parametersF.push(atts.getFloat("tightnessExponent", 0.0f));
					parametersF.push(atts.getFloat("max_radius", 0.0f));
					parametersF.push(atts.getFloat("inSpeed", 0.0f));
					parametersF.push(atts.getFloat("upSpeed", 0.0f));
					parametersF.push(atts.getFloat("aroundSpeed", 0.0f));
				} break;
			case NUM_MASS: {
					parametersF.push(atts.getFloat("mass", 1.0f));
				} break;
			case NUM_KILLOLDALPHA: {
					parametersF.push(atts.getFloat("age_limit", 0.0f));
					parametersF.push(atts.getFloat("scale", 1.0f));
				} break;
			case NUM_ALPHASTART: {
					parametersF.push(atts.getFloat("age_limit", 0.0f));
					parametersF.push(atts.getFloat("scale", 1.0f));
					parametersF.push(atts.getFloat("limit", 0.0f));
				} break;
			case NUM_AIRTUBE: {
					parametersF.push(atts.getFloat("vRot", 1.0f));
					parametersF.push(atts.getFloat("vIn", 1.0f));
				} break;
		}
	}
}

void ActionPSManager::exitAction(const string &action, DomainGenerator &dom, PAPI::ParticleContext_t &particleEngine) {
	map<string, int>::iterator it = nameDomain.find(action);
	if (it != nameDomain.end()) {
		int code = it->second;
		switch(code) {
			case NUM_AVOID: {
					particleEngine.Avoid (getFloatValue(), getFloatValue(), getFloatValue(), *(getDomainValue(dom)));
				} break;
			case NUM_BOUNCE: {
					particleEngine.Bounce (getFloatValue(), getFloatValue(), getFloatValue(), *(getDomainValue(dom)));
				} break;
			case NUM_DAMPING: {
					particleEngine.Damping (getVectorValue(dom), getFloatValue(), getFloatValue());
				} break;
			case NUM_ROTDAMPING: {
					particleEngine.RotDamping (getVectorValue(dom), getFloatValue(), getFloatValue());
				} break;
			case NUM_EXPLOSION: {
				particleEngine.Explosion (getVectorValue(dom), getFloatValue(), 
										  getFloatValue(), getFloatValue(), getFloatValue());
				} break;
			case NUM_FOLLOW: {
					particleEngine.Follow (getFloatValue(), getFloatValue(), getFloatValue());
				} break;
			case NUM_GRAVITATE: {
					particleEngine.Gravitate (getFloatValue(), getFloatValue(), getFloatValue());
				} break;
			case NUM_GRAVITY: {
					particleEngine.Gravity (getVectorValue(dom));
				} break;
			case NUM_JET: {
					particleEngine.Jet (*(getDomainValue(dom)), *(getDomainValue(dom)));
				} break;
			case NUM_KILLOLD: {
					particleEngine.KillOld (getFloatValue());
				} break;
			case NUM_MATCHVELOCITY: {
					particleEngine.MatchVelocity (getFloatValue(), getFloatValue(), getFloatValue());
				} break;
			case NUM_MATCHROTVELOCITY: {
					particleEngine.MatchRotVelocity (getFloatValue(), getFloatValue(), getFloatValue());
				} break;
			case NUM_MOVE: {
					particleEngine.Move (getBoolValue(), getBoolValue());
				} break;
			case NUM_ORBITLINE: {
					particleEngine.OrbitLine (getVectorValue(dom), getVectorValue(dom), getFloatValue(), 
										  getFloatValue(), getFloatValue());
				} break;
			case NUM_ORBITPOINT: {
					particleEngine.OrbitPoint (getVectorValue(dom), getFloatValue(), getFloatValue(), getFloatValue());
				} break;
			case NUM_RANDOMACCEL: {
					particleEngine.RandomAccel (*(getDomainValue(dom)));
				} break;
			case NUM_RANDOMDISPLACE: {
					particleEngine.RandomDisplace (*(getDomainValue(dom)));
				} break;
			case NUM_RANDOMVELOCITY: {
					particleEngine.RandomVelocity (*(getDomainValue(dom)));
				} break;
			case NUM_RANDOMROTVELOCITY: {
					particleEngine.RandomRotVelocity (*(getDomainValue(dom)));
				} break;
			case NUM_RESTORE: {
					particleEngine.Restore (getFloatValue(), getBoolValue(), getBoolValue());
				} break;
			case NUM_SINK: {
					particleEngine.Sink (getBoolValue(), *(getDomainValue(dom)));
				} break;
			case NUM_SINKVELOCITY: {
					particleEngine.SinkVelocity (getBoolValue(), *(getDomainValue(dom)));
				} break;
			case NUM_SORT: {
					particleEngine.Sort (getVectorValue(dom), getVectorValue(dom), getBoolValue(), getBoolValue());
				} break;
			case NUM_SOURCE: {
					particleEngine.Source (getFloatValue(), *(getDomainValue(dom)));
				} break;
			case NUM_SPEEDLIMIT: {
					particleEngine.SpeedLimit (getFloatValue(), getFloatValue());
				} break;
			case NUM_TARGETSIZE: {
					particleEngine.TargetSize (getVectorValue(dom), getVectorValue(dom));
				} break;
			case NUM_TARGETVELOCITY: {
					particleEngine.TargetVelocity (getVectorValue(dom), getFloatValue());
				} break;
			case NUM_TARGETROTVELOCITY: {
					particleEngine.TargetRotVelocity (getVectorValue(dom), getFloatValue());
				} break;
			case NUM_VERTEX: {
					particleEngine.Vertex (getVectorValue(dom));
				} break;
			case NUM_VORTEX: {
					particleEngine.Vortex (getVectorValue(dom), getVectorValue(dom), getFloatValue(), 
						getFloatValue(), getFloatValue(), getFloatValue(), getFloatValue());
				} break;
			case NUM_SIZE: {
					particleEngine.Size(*(getDomainValue(dom)));
				} break;	
			case NUM_MASS: {
					particleEngine.Mass(getFloatValue());
				} break;	
			case NUM_ROTVELOCITY: {
					particleEngine.RotVelocity(*(getDomainValue(dom)));
				} break;
			case NUM_UPVEC: {
					particleEngine.UpVec(*(getDomainValue(dom)));
				} break;	
			case NUM_VELOCITY: {
					particleEngine.Velocity(*(getDomainValue(dom)));
				} break;
			case NUM_COLOR: {
					particleEngine.Color(*(getDomainValue(dom)), *(getDomainValue(dom)));
				} break;
			case NUM_KILLOLDALPHA: {
					particleEngine.KillOldAlpha(getFloatValue(), getFloatValue());
				} break;
			case NUM_ALPHASTART: {
					particleEngine.AlphaStart(getFloatValue(), getFloatValue(), getFloatValue());
				} break;
			case NUM_FUNCTIONCOLOR: {
					particleEngine.FunctionColor(getFunctionValue(), getFunctionValue(), getFunctionValue());
				} break;
			case NUM_FUNCTIONALPHA: {
					particleEngine.FunctionAlpha(getFunctionValue());
				} break;
			case NUM_FUNCTIONSIZE: {
					particleEngine.FunctionSize(getFunctionValue());
				} break;
			case NUM_AIRTUBE: {
				//particleEngine. (getVectorValue(dom), getVectorValue(dom), getBoolValue(), 
					particleEngine.AirTube(getVectorValue(dom), getFloatValue(), getFloatValue());
				} break;
		}
	}
}

float ActionPSManager::getFloatValue(void) {
	float aux;
	if (!parametersF.empty()) {
		aux = parametersF.top();
		parametersF.pop();
	} else {
		aux = 0.0f;
	}
	return aux;
}

bool ActionPSManager::getBoolValue(void) {
	bool aux;
	if (!parametersB.empty()) {
		aux = parametersB.top();
		parametersB.pop();
	} else {
		aux = false;
	}
	return aux;
}

PAPI::pVec ActionPSManager::getVectorValue(DomainGenerator &dom) {
	try {
		PAPI::PDPoint * aux = (PAPI::PDPoint *) dom.popDomain();
		PAPI::pVec v(aux->p.x(), aux->p.y(), aux->p.z());
		return v;
	} catch(...) {
		return PAPI::pVec(0.0f, 0.0f, 0.0f);
	}
}

PAPI::pDomain * ActionPSManager::getDomainValue(DomainGenerator &dom) {
	return dom.popDomain();
}

queue<float> * ActionPSManager::getFunctionValue(void) {
	if (functions.empty())
		return new queue<float>();
	
	queue<float> * aux = functions.top();
	functions.pop();
	return aux;
}