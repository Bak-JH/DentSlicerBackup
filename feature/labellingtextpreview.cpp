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

    scaleY = 12.0f/16.0f;
    planeSelected = false;
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
    //scaleY = fontSize*scaleY/this->fontSize;
    qDebug() << "%%%%%%%fontsize" << fontSize;
    scaleY = fontSize/16.0f;
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
    planeMesh->setDepth(0.2f);

    //checking if it is shadow model is needed
    QFont font( this->fontName );
    qDebug() << "labellingtextpreview" << this->fontSize;
    font.setPointSize( this->fontSize );

    font.setWeight( this->fontWeight );
    font.setUnderline( FALSE );

    planeMesh->setFont( font );


    QVector3D ref = QVector3D(0, 0, 1);
    qDebug() << "labelling text preview fontsize : " << this->fontSize;
    float meshScale = 0.2f/14.0f * ((float)this->fontSize); // 14.0 pixel font generated and
    qDebug() << "meshScale" << meshScale << "content width:" << this->contentWidth;
    float length = this->text.length();
    float callibration = (this->contentWidth/2.0f * scaleY) * meshScale * 0.545f *2.38f * 0.105f;
    //QVector3D ref = QVector3D(1,0,0);
    auto tangent = QVector3D::crossProduct(normal, ref).normalized();
    //auto tangent = ref;
    auto binormal = QVector3D::crossProduct(tangent, normal).normalized();

    qDebug() << translation << " cal:"<<  callibration << scaleY << binormal << this->contentWidth;
    qDebug() << "@@@@"<<this<<translation + normal * 0.5f + tangent * callibration - binormal * (callibration / length * 12.0f);
    planeTransform->setTranslation(translation);
    //planeTransform->setTranslation(translation + tangent * (this->contentWidth/150 * this->fontSize) - binormal * (6.0f/this->fontSize*meshScale));// + normal * 0.5f + tangent*this->contentWidth/64/2 - binormal);// - binormal);
    //planeTransform->setTranslation(translation + normal * 0.5f + tangent * callibration - binormal * (callibration / this->contentWidth * 18.2f));
    //planeTransform->setRotation(QQuaternion::fromAxes(tangent, normal, binormal) * QQuaternion::fromAxisAndAngle(QVector3D(0, 1, 0), 180));
    planeTransform->setRotation(QQuaternion::fromAxes(tangent, normal, binormal) * QQuaternion::fromAxisAndAngle(QVector3D(0, 1, 1), 180));
    //planeTransform->setScale3D(QVector3D(width / minimumWidth, 2.0f, ratioY) * scaleY);
    //planeTransform->setScale3D(QVector3D(0.545f*2.38f, 0.545f *2.38f, ratioY) * scaleY  * meshScale);
    planeTransform->setScale3D(QVector3D(1,1,1)*meshScale);
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
    /*
    delete planeMaterial;
    planeMaterial = new Qt3DExtras::QPhongAlphaMaterial();
    planeMaterial->setAmbient(QColor(0,0,0,0));
    planeMaterial->setDiffuse(QColor(0,0,0,0));
    planeMaterial->setSpecular(QColor(0,0,0,0));
    planeMaterial->setAlpha(0.0f);
    planeEntity->addComponent(this->planeMaterial);
    */
    planeMaterial->setAlpha(0.0f);
    planeMesh->setDepth(0.0f);

}
