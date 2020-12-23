#pragma once
#include <Qt3dCore>
namespace Hix
{
	namespace Render
	{
		//Color code is sent to geometry shader via unsigned int array uniform.
		//Actual color values are stored in form of look up table in form of vec3 array uniform.
		//MeshColorCodes enum is used to set color in GLModel, its unsigned values are sent to the shader in GPU
		namespace Colors
		{
			//RGBA
			const QVector4D Default =				QVector4D(0.278f, 0.670f, 0.706f, 1.0f);
			const QVector4D Selected =				QVector4D(0.510f, 0.816f, 0.490f, 1.0f);
			const QVector4D ShellOffset =			QVector4D(0.510f, 0.816f, 0.490f, 1.0f);
			const QVector4D SelectedFace =			QVector4D(0.901f, 0.843f, 0.133f, 1.0f);
			const QVector4D Infill =				QVector4D(0.969f, 0.945f,0.184f,1.0f);
			const QVector4D Support =				QVector4D(0.259f, 0.749f,0.8f, 1.0f);
			const QVector4D Raft =					QVector4D(0.431f, 0.929f,0.196f, 1.0f);
			const QVector4D OutOfBound =			QVector4D(0.1f	, 0.1f, 0.1f, 1.0f);
			const QVector4D SupportHighlighted =	QVector4D(0.3108f,0.8988f, 0.96f, 1.0f);
#ifdef _DEBUG
			const QVector4D DebugRed =				QVector4D(1.0f, 0.0f, 0.00f, 1.0f);
			const QVector4D DebugOrange =			QVector4D(1.0f, 0.5f, 0.00f, 1.0f);
#endif

		}
	}
}


