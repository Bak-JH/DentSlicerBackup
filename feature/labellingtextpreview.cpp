#include "feature/labellingtextpreview.h"
#include "glmodel.h"
#include <QTexture>
#include <QAbstractTexture>
#include <QPlaneMesh>
#include <QSphereMesh>
#include <cmath>
#include <QDebug>

using namespace Qt3DCore;
using namespace Qt3DRender;
using namespace Qt3DExtras;

LabellingTextPreview::LabellingTextPreview(Qt3DCore::QNode* parent)
    : Qt3DCore::QEntity(parent)
{
    /*
    texture = new QTexture2D(parent->parentNode());
    texture->setMinificationFilter(QAbstractTexture::Filter::Linear);//Linear
    texture->setMagnificationFilter(QAbstractTexture::Filter::Linear);
    texture->setFormat(QAbstractTexture::TextureFormat::RGBAFormat); //RGBAFormat
    */

    //planeMesh = new Qt3DExtras::QPlaneMesh(parent->parentNode());


    planeMesh = new Qt3DExtras::QExtrudedTextMesh(parent);
    planeMesh->setDepth(10.0f);
    planeMesh->setText("");


    planeMesh = new Qt3DExtras::QExtrudedTextMesh(parent);
    planeMesh->setDepth(0.2f);

    planeTransform = new Qt3DCore::QTransform(this);

    planeMaterial = new Qt3DExtras::QPhongAlphaMaterial();
    planeMaterial->setAmbient(QColor(255,255,0,255));
    planeMaterial->setDiffuse(QColor(255,0,0,255));
    planeMaterial->setSpecular(QColor(0,255,255,255));
    planeMaterial->setAlpha(0.5f);

    planeEntity = new Qt3DCore::QEntity(parent);
    planeEntity->addComponent(this->planeMesh);
    planeEntity->addComponent(this->planeTransform);
    planeEntity->addComponent(this->planeMaterial);

    scaleY = 12.0f/16.0f;
    planeSelected = false;
}

LabellingTextPreview::~LabellingTextPreview(){
    planeTransform->deleteLater();
    planeMaterial->deleteLater();
    planeEntity->deleteLater();
}


void LabellingTextPreview::setText(QString text, int contentWidth)
{
    this->contentWidth = contentWidth;
    this->text = text;

    planeMesh->setText(this->text);

    if (text == "Enter text" && contentWidth == 0)
        this->contentWidth = 109;

   // if (textureImage) {
   //     texture->removeTextureImage(textureImage);
  //  }

    width = this->contentWidth*2;// * this->fontSize;
    qDebug() << "%%%%%%%text : " << text << "contentWidth: " << contentWidth << "width : " << width;
    if (width < minimumWidth)
        width = minimumWidth;
}

void LabellingTextPreview::setFontName(QString fontName)
{
    this->fontName = fontName;
}

void LabellingTextPreview::setFontBold(bool isbold){
    if (isbold){
        //qDebug() << "set font bold";
        this->fontWeight = QFont::Bold;
    } else {
        this->fontWeight = QFont::DemiBold;
    }
}

void LabellingTextPreview::setFontSize(int fontSize)
{
    scaleY = fontSize/16.0f;
    this->fontSize = fontSize;
}

void LabellingTextPreview::setTranslation(const QVector3D& t)
{
    translation = t;
}

void LabellingTextPreview::setNormal(const QVector3D& n)
{
    normal = n;
}

void LabellingTextPreview::updateTransform()
{
    if (planeMesh == nullptr) {
        qDebug() << "planeMesh nullptr @@@@@@@@@@@@";
        return;
    }
    planeMesh->setDepth(12.0f/fontSize);

    //checking if it is shadow model is needed
    QFont font( this->fontName );
    font.setPointSize( this->fontSize );

    font.setWeight( this->fontWeight );
    font.setUnderline( FALSE );

    planeMesh->setFont( font );


    QVector3D ref = QVector3D(0, 0, 1);

    QVector3D tangent;
    if (normal.normalized() == QVector3D(0,0,-1)){
        tangent = QVector3D(1,0,0);
    } else if (normal == QVector3D(0,0,1)){
        tangent = QVector3D(-1,0,0);
    } else {
        tangent = QVector3D::crossProduct(normal.normalized(), ref);
    }
    tangent.normalize();

    QVector3D binormal;
    if (normal.normalized() == QVector3D(0,0,-1)){
        binormal = QVector3D(0,1,0);
    } else if (normal.normalized() == QVector3D(0,0,1)){
        binormal = QVector3D(0,1,0);
    } else {
        binormal = QVector3D::crossProduct(tangent, normal.normalized());
    }
    binormal.normalize();

    planeTransform->setTranslation(translation + normal - normal.normalized());
    qDebug() << "changed translation :" << translation + normal - normal.normalized();
    planeTransform->setRotation(QQuaternion::fromAxes(tangent, normal.normalized(), binormal) * QQuaternion::fromAxisAndAngle(QVector3D(0,1, 1), 180));
    planeTransform->setScale3D(QVector3D(ratioY,ratioY, ratioY) * scaleY);

    translation = translation + normal - (0.8*normal.normalized());
    qDebug() << "@@@@@@@@@@@@@@@ " << planeMesh->geometry()->attributes().at(0)->buffer()->data();
}

void LabellingTextPreview::deleteLabel() {
    qDebug() << "@@@@ Delete" << this;

    this->hideLabel();
    planeMesh->deleteLater();
}

void LabellingTextPreview::updateChange(QString text, int contentWidth, QString fontName,
                                        bool isbold,int fontSize,const QVector3D& t, const QVector3D& n) {
    if (this->text != text || this->contentWidth != contentWidth)
        this->setText(text, contentWidth);
    if (this->fontName != fontName)
        this->setFontName(fontName);
    this->setFontBold(isbold);
    if (this->fontSize != fontSize)
        this->setFontSize(fontSize);
    else
        qDebug() << "%%%%%%%%%%%%%%HAHA" << this->fontSize << fontSize;
    if (this->translation != t)
        this->setTranslation(t);
    if (n != QVector3D(0,0,0))
        this->setNormal(n);

    updateTransform();
}

void LabellingTextPreview::hideLabel() {
    qDebug() << "@@@@@@@@@@@@@@@@@ hidelabel @@@@@@" << this;
    planeMaterial->setAlpha(0.0f);
    planeMesh->setDepth(0.0f);

}


Qt3DRender::QGeometry * LabellingTextPreview::getGeometry() {
    return planeMesh->geometry();
}




