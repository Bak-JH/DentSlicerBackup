#include "stlexporter.h"

STLexporter::STLexporter()
{

}

void STLexporter::exportSTL(Mesh* mesh, QString outfilename){

    qDebug() << "export STL";
    ofstream outfile(outfilename.toStdString().c_str(), ios::out);

    writeHeader(outfile);

    for (MeshFace mf : mesh->faces){
        writeFace(outfile, mesh, mf);
    }

    writeFooter(outfile);

    outfile.close();

    return;
}

void STLexporter::writeFace(ofstream& outfile, Mesh* mesh, MeshFace mf){

    outfile << "facet normal "<< mf.fn.x() <<" "<< mf.fn.y()<<" "<< mf.fn.z() << "\n";
    outfile << "    outer loop\n";
    MeshVertex mv1 = mesh->idx2MV(mf.mesh_vertex[0]);
    MeshVertex mv2 = mesh->idx2MV(mf.mesh_vertex[1]);
    MeshVertex mv3 = mesh->idx2MV(mf.mesh_vertex[2]);
    outfile << "        vertex "<< mv1.position.x()<<" "<< mv1.position.y()<<" "<< mv1.position.z()<<"\n";
    outfile << "        vertex "<< mv2.position.x()<<" "<< mv2.position.y()<<" "<< mv2.position.z()<<"\n";
    outfile << "        vertex "<< mv3.position.x()<<" "<< mv3.position.y()<<" "<< mv3.position.z()<<"\n";
    outfile << "    endloop\n";
    outfile << "endfacet\n";
}

void STLexporter::writeHeader(ofstream& outfile){
    outfile << "solid diridiri\n";
}

void STLexporter::writeFooter(ofstream& outfile){
    outfile << "endsolid diridiri";
}