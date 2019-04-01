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
    labelParent = parent;
    qDebug() << "new labellingTextPreview ^ ^ ^ ^ ^ ^" << parent << parent->parentNode();
    /*
    texture = new QTexture2D(parent->parentNode());
    texture->setMinificationFilter(QAbstractTexture::Filter::Linear);//Linear
    texture->setMagnificationFilter(QAbstractTexture::Filter::Linear);
    texture->setFormat(QAbstractTexture::TextureFormat::RGBAFormat); //RGBAFormat
    */

    //planeMesh = new Qt3DExtras::QPlaneMesh(parent->parentNode());
    qDebug() << "parent:" << parent << "parent's parent:" << parent->parentNode() << "parent*3"<<parent->parentNode()->parentNode();


    planeMesh = new Qt3DExtras::QExtrudedTextMesh(parent->parentNode());
    planeMesh->setDepth(10.0f);
    planeMesh->setText("");


    planeMesh = new Qt3DExtras::QExtrudedTextMesh(parent->parentNode());
    planeMesh->setDepth(1.2f);
    //planeMesh->setText("asdfasdfasdf");

    planeTransform = new Qt3DCore::QTransform(this);

    planeMaterial = new Qt3DExtras::QPhongAlphaMaterial();
    planeMaterial->setAmbient(QColor(255,255,0,255));
    planeMaterial->setDiffuse(QColor(255,0,0,255));
    planeMaterial->setSpecular(QColor(0,255,255,255));
    planeMaterial->setAlpha(0.5f);

    planeEntity = new Qt3DCore::QEntity(parent->parentNode());
    planeEntity->addComponent(this->planeMesh);
    planeEntity->addComponent(this->planeTransform);
    planeEntity->addComponent(this->planeMaterial);

    planeSelected = false;
}
void LabellingTextPreview::setText(QString text, int contentWidth)
{
    this->contentWidth = contentWidth;
    this->text = text;

    planeMesh->setText(this->text);

   // if (textureImage) {
   //     texture->removeTextureImage(textureImage);
  //  }

    width = this->contentWidth*2;// * this->fontSize;
    qDebug() << "%%%%%%%text : " << text << "contentWidth: " << contentWidth << "width : " << width;
    if (width < minimumWidth)
        width = minimumWidth;

  //  textureImage = new TextureImage(width, this->fontSize/(1+1.1*log(this->fontSize/12)), 8.0f, text, fontName, fontWeight);
  //  textureImage->update();
 //   texture->addTextureImage(textureImage);
    //planeMaterial->setDiffuse(texture);
    //updateTransform();
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
    scaleY = fontSize*scaleY/this->fontSize;

    this->fontSize = fontSize;
}

void LabellingTextPreview::setTranslation(const QVector3D& t)
{
    translation = t;

    //updateTransform();
}

void LabellingTextPreview::setNormal(const QVector3D& n)
{
    normal = n;

    //normal.setZ(-normal.z());
    normal.setZ(normal.z());
    normal.normalize();

    //updateTransform();
}

void LabellingTextPreview::updateTransform()
{
    if (planeMesh == nullptr) {
        qDebug() << "planeMesh nullptr @@@@@@@@@@@@";
        return;
    }
    planeMesh->setDepth(10.0f);

    //checking if it is shadow model is needed
    QFont font( this->fontName );
    font.setPointSize( this->fontSize );

    font.setWeight( this->fontWeight );
    font.setUnderline( FALSE );

    planeMesh->setFont( font );


    QVector3D ref = QVector3D(0, 0, 1);
    float meshScale = 0.04f;
    float length = this->text.length();
    float callibration = (length/2.0f * this->fontSize) * meshScale / 1.0f;
    //QVector3D ref = QVector3D(1,0,0);
    auto tangent = QVector3D::crossProduct(normal, ref);
    //auto tangent = ref;
    tangent.normalize();
    auto binormal = QVector3D::crossProduct(tangent, normal);
    binormal.normalize();

    qDebug() << translation << " cal:"<<  callibration << this->text.length();
    qDebug() << "@@@@"<<this<<translation + normal * 0.5f + tangent * callibration - binormal * (callibration / length* 10.0f);
    //planeTransform->setTranslation(translation + normal * 0.5f);
    planeTransform->setTranslation(translation + normal * 0.5f + tangent * callibration - binormal * (callibration / length * 10.0f));
    //planeTransform->setRotation(QQuaternion::fromAxes(tangent, normal, binormal) * QQuaternion::fromAxisAndAngle(QVector3D(0, 1, 0), 180));
    planeTransform->setRotation(QQuaternion::fromAxes(tangent, normal, binormal) * QQuaternion::fromAxisAndAngle(QVector3D(0,1, 1), 180));
    //planeTransform->setScale3D(QVector3D(width / minimumWidth, 2.0f, ratioY) * scaleY);
    planeTransform->setScale3D(QVector3D(callibration*0.90f, callibration /1.2f, ratioY) * scaleY * meshScale);
}

void LabellingTextPreview::deleteLabel() {
    qDebug() << "@@@@ Delete" << this;
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
    if (this->translation != t)
        this->setTranslation(t);
    if (n != QVector3D(0,0,0))
        this->setNormal(n);

    updateTransform();
}

void LabellingTextPreview::hideLabel() {
    planeMesh->setDepth(0.0f);
}
