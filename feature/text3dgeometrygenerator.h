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
	void generateText3DGeometry(QVector3D** vertices, int* verticesSize,
		unsigned int** indices, int* indicesSize,
		QFont font, QString text, float depth,
		Hix::Engine3D::Mesh* mesh,
		//const QVector3D* originalVertices,
		//const int originalVerticesCount,
		const QVector3D normalVector,
		const QMatrix4x4& transform,
		const QMatrix4x4& normalTransform);
	std::unordered_map<PolyNode*, std::vector<p2t::Triangle*>> Text3D::triangluateBFS(PolyTree& tree);
	std::vector<p2t::Point*> toPointVector(std::vector<QVector2D> floatPath);
private:
	std::vector<PolyNode*> nodes;
	std::unordered_map<PolyNode*, std::vector<QVector2D>> floatPathMap;
	std::vector<QVector2D> floatPath;
};
#endif // TEXT3DGEOMETRYGENERATOR_H
