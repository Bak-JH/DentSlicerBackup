#ifdef _DEBUG

#include "DebugRenderObject.h"
using namespace Qt3DCore;

void Hix::Debug::DebugRenderObject::initialize(QEntity* root)
{
	_root = root;
}

void Hix::Debug::DebugRenderObject::addLine(const std::vector<QVector3D>& vertices, const QVector4D& color)
{
	_lines.emplace_back(vertices, _root, color);
}

void Hix::Debug::DebugRenderObject::clear()
{
	_lines.clear();
}





























#else
#endif
