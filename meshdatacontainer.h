#ifndef MESHDATACONTAINER_H
#define MESHDATACONTAINER_H
#include <vector>
#include <array>
#include <qvector3d.h>
class MeshVertex;
class Mesh;
class MeshDataContainer;

class MeshDataType {
public:
	MeshDataType(MeshDataContainer* owner);
protected:
	MeshDataContainer* _owner = nullptr;
	virtual void markModified() = 0;

};

class MeshFace : private MeshDataType {
public:
	size_t idx()const;
	QVector3D fn()const;
	const std::array<std::vector<const MeshFace*>, 3> neighboring_faces()const;
	const std::array<const MeshVertex*, 3> mesh_vertex()const;

	void setFn(QVector3D normal);
	MeshFace(MeshDataContainer* owner);
	MeshFace();

private:
	size_t _idx;
	QVector3D _fn;
	std::array<std::vector<const MeshFace*>, 3> _neighboring_faces;
	std::array<const MeshVertex*, 3> _mesh_vertex{ nullptr, nullptr, nullptr };

	void markModified()override;
};

class MeshVertex : private MeshDataType {
public:
	size_t idx() const;
	QVector3D position() const;
	QVector3D vn() const;

	void calculateNormalFromFaces();
	void setPosition(QVector3D);
	MeshVertex();

	friend inline bool operator== (const MeshVertex& a, const MeshVertex& b) {
		return a.position == b.position;
	}

	friend inline bool operator!= (const MeshVertex& a, const MeshVertex& b) {
		return a.position != b.position;
	}

	MeshVertex(MeshDataContainer* container, QVector3D position);
	MeshVertex(MeshDataContainer* container);

private:
	size_t _idx;
	QVector3D _position;
	QVector3D _vn;
	std::vector<const MeshFace*> _connected_faces;
	void markModified()override;


};


class MeshDataContainer
{
public:
	enum MeshOpType {
		Delete = 0 //only index(size_t)
		, Modify   //range(size_t,size_t) or pointers
		, Append   //range(size_t,size_t) or pointers
	};
	enum MeshOpOperand {
		Face = 0
		,Vertex
	};
	enum MeshOpRange {
		Single = 0
		,Range
	};
	struct MeshOp {
		MeshOpType Type;
		MeshOpOperand Operand;
		//*ranges are (offset, count)
		std::variant<std::pair<size_t, size_t>, size_t, const MeshVertex*, const MeshFace*> Data;
	};

    MeshDataContainer();
	const MeshFace* face(size_t index) const;
	const MeshVertex* vertex(size_t index)const;
	const std::vector<const MeshVertex*>vertices() const;
	const std::vector<const MeshFace*>faces() const;
	void eraseFace(const MeshFace* face);
	void eraseVertex(const MeshVertex* vtx);
	void eraseFace(size_t index);
	void eraseVertex(size_t index);
	void appendFace(MeshFace face);
	void appendVertex(MeshVertex vertex);

	void markFaceModified(const MeshFace* face);
	void markVertexModified(const MeshVertex* face);
	
	

private:
	std::list<MeshVertex> _vertices;
	std::list<MeshFace> _faces;
	std::vector<std::list<MeshFace>::iterator> _renderOrderFaces;
	std::vector<std::list<MeshVertex>::iterator> _renderOrderVertices;
};

#endif // MESHDATACONTAINER_H
