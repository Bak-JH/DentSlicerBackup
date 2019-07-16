#pragma once
#include <Qt3dCore>
namespace Hix
{
	namespace Render
	{
		//Color code is sent to geometry shader via unsigned int array uniform.
		//Actual color values are stored in form of look up table in form of vec3 array uniform.
		//MeshColorCodes enum is used to set color in GLModel, its unsigned values are sent to the shader in GPU

		enum MeshColorCodes
		{
			Default = 0,
			Selected = 1,
			SelectedFace = 2,
			Infill = 3,
			Support = 4,
			Raft = 5,
			OutOfBound = 6
		};

		namespace Colors
		{
			


			const QVector3D Default = QVector3D(0.278f, 0.670f, 0.706f);
			const QVector3D Selected = QVector3D(0.510f, 0.816f, 0.490f);
			const QVector3D SelectedFace = QVector3D(0.901f, 0.843f, 0.133f);
			const QVector3D Infill = QVector3D(0.969f,0.945f,0.184f);
			const QVector3D Support = QVector3D(0.259f,0.749f,0.8f);
			const QVector3D Raft = QVector3D(0.431f,0.929f,0.196f);
			const QVector3D OutOfBound = QVector3D(0.1, 0.1f, 0.1f);

		}
	}
}


