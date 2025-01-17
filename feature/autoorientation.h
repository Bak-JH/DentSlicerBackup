#pragma once
#include "QMatrix4x4"
#include <math.h>
#include <algorithm>
#include <map>
#include <string>
#include <QTime>
#include "interfaces/Feature.h"
#include "interfaces/Mode.h"
namespace Hix
{
	namespace Engine3D
	{
		class Mesh;
	}
}
class rotateResult{
public:
    float phi;
    QVector3D v;
//    float R[16];
    QMatrix4x4 R;
};

class Orient {
public:
    Orient(float _val, QVector3D _label);
    float val;
    QVector3D label;
};

class Liste {
public:
    QVector3D orientation;
    float bottomA;
    float overhangA;
    float lineL;
    bool isActive=false;
};


namespace Hix
{

	namespace Features
	{
		class AutoOrientateMode : public Hix::Features::InstantMode
		{
		public:
			AutoOrientateMode();
			virtual ~AutoOrientateMode();
		private:
		};
	}
}

namespace autoorientation
{

    rotateResult* Tweak(const Hix::Engine3D::Mesh* mesh, bool bi_algorithmic,int CA,bool *appropriately_rotated);
    float approachvertex(const Hix::Engine3D::Mesh* mesh,float n[]);


};
