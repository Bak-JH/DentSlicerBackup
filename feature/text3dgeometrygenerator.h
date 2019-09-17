#ifndef TEXT3DGEOMETRYGENERATOR_H
#define TEXT3DGEOMETRYGENERATOR_H

#include <QVector3D>
#include <QFont>
#include <QString>
#include <QPainterPath>
#include <QTransform>
#include <QTextLayout>

#include "DentEngine/src/mesh.h"

using namespace ClipperLib;
using namespace Hix::Engine3D;

class Text3D
{
public:
	Text3D() {}
	virtual ~Text3D() {}
	void generateText3D(QFont font, QString text, Hix::Engine3D::Mesh* targetMesh);
	void generateTextFace(const PolyTree& tree, Hix::Engine3D::Mesh* targetMesh);
	std::vector<p2t::Point*> toPointVector(const std::vector<QVector2D> floatPath);
	std::vector<p2t::Point*> toPointVector(const ClipperLib::Path intPath);
private:
	std::unordered_map<PolyNode*, std::vector<QVector2D>> _floatPathMap;
};
#endif // TEXT3DGEOMETRYGENERATOR_H
