#include <cassert>
#include "DomainGenerator.h"
#include "Expat/XMLParser.h"

using namespace std;

const int T_BLOB		 = 0;
const int T_BOX			 = 10;
const int T_CONE		 = 1;
const int T_CYLINDER	 = 2;
const int T_DISC		 = 3;
const int T_LINE		 = 4;
const int T_PLANE		 = 5;
const int T_POINT		 = 6;
const int T_RECTANGLE	 = 7;
const int T_SPHERE		 = 8;
const int T_TRIANGLE	 = 9;

const int T_DINAMIC_LINE = 11;

const string S_BLOB		 = "BLOB";
const string S_BOX		 = "BOX";
const string S_CONE	 	 = "CONE";
const string S_CYLINDER	 = "CYLINDER";
const string S_DISC		 = "DISC";
const string S_LINE		 = "LINE";
const string S_PLANE	 = "PLANE";
const string S_POINT	 = "POINT";
const string S_RECTANGLE = "RECTANGLE";
const string S_SPHERE	 = "SPHERE";
const string S_TRIANGLE	 = "TRIANGLE";

const string S_DINAMIC_LINE	 = "DINAMIC_LINE";

DomainGenerator::DomainGenerator(void) {
	nameDomain[S_BLOB] = T_BLOB;
	nameDomain[S_BOX] = T_BOX;
	nameDomain[S_CONE] = T_CONE;
	nameDomain[S_CYLINDER] = T_CYLINDER;
	nameDomain[S_DISC] = T_DISC;
	nameDomain[S_LINE] = T_LINE;
	nameDomain[S_PLANE] = T_PLANE;
	nameDomain[S_POINT] = T_POINT;
	nameDomain[S_RECTANGLE] = T_RECTANGLE;
	nameDomain[S_SPHERE] = T_SPHERE;
	nameDomain[S_TRIANGLE] = T_TRIANGLE;
	nameDomain[S_DINAMIC_LINE] = T_DINAMIC_LINE;
}

DomainGenerator::~DomainGenerator(void) {
	clear();
}

void DomainGenerator::clear(void) {
	while (!domainVolumes.empty())
		domainVolumes.pop();
	while (!toDelete.empty()) {
		PAPI::pDomain * aux = toDelete.top();
		toDelete.pop();
		delete aux;
	}
}

void DomainGenerator::pushDomain(MKeyValue &atts) {
	string stype = atts["type"];
	map<string, int>::iterator it = nameDomain.find(stype);

	assert(it != nameDomain.end());

	int type = it->second;

	switch (type) {
		case T_BLOB: {
				PAPI::pVec v(atts.getFloat("x0", 0.0f), atts.getFloat("y0", 0.0f), atts.getFloat("z0", 0.0f));
				float param = atts.getFloat("StandardDev", 1.0f);
				PAPI::pDomain * aux = new PAPI::PDBlob(v, param);
				domainVolumes.push(aux);
				toDelete.push(aux);
			}
			break;
		case T_BOX: {
				PAPI::pVec v0(atts.getFloat("x0", 0.0f), atts.getFloat("y0", 0.0f), atts.getFloat("z0", 0.0f));
				PAPI::pVec v1(atts.getFloat("x1", 0.0f), atts.getFloat("y1", 0.0f), atts.getFloat("z1", 0.0f));
				PAPI::pDomain * aux = new PAPI::PDBox(v0, v1);
				domainVolumes.push(aux);
				toDelete.push(aux);
			}
			break;
		case T_CONE:{
				PAPI::pVec v0(atts.getFloat("x0", 0.0f), atts.getFloat("y0", 0.0f), atts.getFloat("z0", 0.0f));
				PAPI::pVec v1(atts.getFloat("x1", 0.0f), atts.getFloat("y1", 0.0f), atts.getFloat("z1", 0.0f));
				float param0 = atts.getFloat("OuterRadius", 1.0f);
				float param1 = atts.getFloat("InnerRadius", 1.0f);
				PAPI::pDomain * aux = new PAPI::PDCone(v0, v1, param0, param1);
				domainVolumes.push(aux);
				toDelete.push(aux);
			}
			break;
		case T_CYLINDER:{
				PAPI::pVec v0(atts.getFloat("x0", 0.0f), atts.getFloat("y0", 0.0f), atts.getFloat("z0", 0.0f));
				PAPI::pVec v1(atts.getFloat("x1", 0.0f), atts.getFloat("y1", 0.0f), atts.getFloat("z1", 0.0f));
				float param0 = atts.getFloat("OuterRadius", 1.0f);
				float param1 = atts.getFloat("InnerRadius", 1.0f);
				PAPI::pDomain * aux = new PAPI::PDCylinder(v0, v1, param0, param1);
				domainVolumes.push(aux);
				toDelete.push(aux);
			}
			break;
		case T_DISC:{
				PAPI::pVec v0(atts.getFloat("x0", 0.0f), atts.getFloat("y0", 0.0f), atts.getFloat("z0", 0.0f));
				PAPI::pVec v1(atts.getFloat("normalx", 0.0f), 
							  atts.getFloat("normaly", 0.0f), 
							  atts.getFloat("normalz", 0.0f));
				float param0 = atts.getFloat("OuterRadius", 1.0f);
				float param1 = atts.getFloat("InnerRadius", 1.0f);
				float param2 = atts.getFloat("rangAngle", 2.0f * 3.1416f);
				float param3 = atts.getFloat("phase", 0.0f);
				PAPI::pDomain * aux = new PAPI::PDDisc(v0, v1, param0, param1, param2, param3);
				domainVolumes.push(aux);
				toDelete.push(aux);
			}
			break;
		case T_LINE: {
				PAPI::pVec v0(atts.getFloat("x0", 0.0f), atts.getFloat("y0", 0.0f), atts.getFloat("z0", 0.0f));
				PAPI::pVec v1(atts.getFloat("x1", 0.0f), atts.getFloat("y1", 0.0f), atts.getFloat("z1", 0.0f));
				PAPI::pDomain * aux = new PAPI::PDLine(v0, v1);
				domainVolumes.push(aux);
				toDelete.push(aux);
			}
			break;
		case T_PLANE:{
				PAPI::pVec v0(atts.getFloat("x0", 0.0f), atts.getFloat("y0", 0.0f), atts.getFloat("z0", 0.0f));
				PAPI::pVec v1(atts.getFloat("normalx", 0.0f), 
							  atts.getFloat("normaly", 0.0f), 
							  atts.getFloat("normalz", 0.0f));
				PAPI::pDomain * aux = new PAPI::PDPlane(v0, v1);
				domainVolumes.push(aux);
				toDelete.push(aux);
			}
			break;
		case T_POINT:{
				PAPI::pVec v0(atts.getFloat("x0", 0.0f), atts.getFloat("y0", 0.0f), atts.getFloat("z0", 0.0f));
				PAPI::pDomain * aux = new PAPI::PDPoint(v0);
				domainVolumes.push(aux);
				toDelete.push(aux);
			}
			break;
		case T_RECTANGLE:{
				PAPI::pVec v0(atts.getFloat("x0", 0.0f), atts.getFloat("y0", 0.0f), atts.getFloat("z0", 0.0f));
				PAPI::pVec v1(atts.getFloat("x1", 0.0f), atts.getFloat("y1", 0.0f), atts.getFloat("z1", 0.0f));
				PAPI::pVec v2(atts.getFloat("x2", 0.0f), atts.getFloat("y2", 0.0f), atts.getFloat("z2", 0.0f));
				PAPI::pDomain * aux = new PAPI::PDRectangle(v0, v1, v2);
				domainVolumes.push(aux);
				toDelete.push(aux);
			}
			break;
		case T_SPHERE:{
				PAPI::pVec v0(atts.getFloat("x0", 0.0f), atts.getFloat("y0", 0.0f), atts.getFloat("z0", 0.0f));
				float param0 = atts.getFloat("OuterRadius", 1.0f);
				float param1 = atts.getFloat("InnerRadius", 1.0f);
				PAPI::pDomain * aux = new PAPI::PDSphere(v0, param0, param1);
				domainVolumes.push(aux);
				toDelete.push(aux);
			}
			break;
		case T_TRIANGLE:{
				PAPI::pVec v0(atts.getFloat("x0", 0.0f), atts.getFloat("y0", 0.0f), atts.getFloat("z0", 0.0f));
				PAPI::pVec v1(atts.getFloat("x1", 0.0f), atts.getFloat("y1", 0.0f), atts.getFloat("z1", 0.0f));
				PAPI::pVec v2(atts.getFloat("x2", 0.0f), atts.getFloat("y2", 0.0f), atts.getFloat("z2", 0.0f));
				PAPI::pDomain * aux = new PAPI::PDTriangle(v0, v1, v2);
				domainVolumes.push(aux);
				toDelete.push(aux);
			}
			break;
		case T_DINAMIC_LINE: {
				PAPI::pVec v0(atts.getFloat("x0", 0.0f), atts.getFloat("y0", 0.0f), atts.getFloat("z0", 0.0f));
				PAPI::pVec v1(atts.getFloat("x1", 0.0f), atts.getFloat("y1", 0.0f), atts.getFloat("z1", 0.0f));
				string domainName = atts.getString("name", "");
				PAPI::pDomain * aux = new PAPI::PDDinamicLine(v0, v1);
				domainVolumes.push(aux);
				toDelete.push(aux);
			}
			break;
	}
}

PAPI::pDomain * DomainGenerator::popDomain(void) {
	PAPI::pDomain * aux;
	if (!domainVolumes.empty()) {
		aux = domainVolumes.top();
		domainVolumes.pop();
	} else {
		aux = new PAPI::PDPoint(PAPI::pVec(0.0f, 0.0f, 0.0f));
		toDelete.push(aux);
	}
	return aux;
}
