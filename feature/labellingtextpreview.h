#ifndef LABELLINGTEXTPREVIEW_H
#define LABELLINGTEXTPREVIEW_H

#include <QEntity>
#include <Qt3DCore/QTransform>
#include <Qt3DExtras>
#include <QPaintedTextureImage>
#include <QPainter>
#include <QPlaneMesh>
#include <QFont>
#include <QNormalDiffuseMapAlphaMaterial>

class LabellingTextPreview : public Qt3DCore::QEntity
{
    Q_OBJECT

    class TextureImage : public Qt3DRender::QPaintedTextureImage {
        friend class LabellingTextPreview;
    public:
        TextureImage(int w, int h, float scale, QString text, QString fontName, int fontWeight)
            : scale(scale), text(text), fontName(fontName), fontWeight(fontWeight)
        {
            setSize(QSize(w * scale, h * scale));

        }

    protected:
        void paint(QPainter *painter)
        {
            int w = painter->device()->width();
            int h = painter->device()->height();

            painter->eraseRect(0, 0, w, h);
            painter->fillRect(0, 0, w, h, QColor(255, 255, 255, 128));
            painter->setPen(QColor(0, 0, 0));
            painter->setFont(QFont(fontName, fontSize * scale, fontWeight, false));
            painter->drawText(QRectF(0, 0, w, h), text, Qt::AlignHCenter | Qt::AlignVCenter);
        }

    private:
        float scale;
        QString text;
        QString fontName;
        int fontSize = 12;
        int fontWeight = QFont::Normal;
    };

public:
    LabellingTextPreview(Qt3DCore::QNode* parent = nullptr);

    void setText(QString text, int contentWidth);
    void setFontName(QString fontName);
    void setFontBold(bool isbold);
    void setFontSize(int fontSize);
    void setTranslation(const QVector3D& t);
    void setNormal(const QVector3D& n);

    void updateTransform();
    void deleteLabel();

    void updateChange(QString text, int contentWidth, QString fontName,
                      bool isbold,int fontSize,const QVector3D& t, const QVector3D& n);
    void hideLabel();
    Qt3DRender::QGeometry* getGeometry();

    Qt3DCore::QTransform* planeTransform = nullptr;
    QVector3D translation;
    QVector3D normal;
    QString text;
    QString fontName = "Arial";
    int fontWeight = QFont::Normal;
    int fontSize = 12;
    int contentWidth = 30;

    float minimumScaleX = 1.0f;
    float minimumWidth = 64;
    float width = 64;
    float scaleY = 5.0f;
    //float scaleY = 0.5f;
    float ratioY = 16.0f / 64;

    bool planeSelected;

private:
    Qt3DCore::QNode* labelParent;
    //Qt3DExtras::QPlaneMesh* planeMesh = nullptr;
    Qt3DExtras::QExtrudedTextMesh* planeMesh = nullptr;
    Qt3DCore::QEntity* planeEntity = nullptr;
    //Qt3DExtras::QNormalDiffuseMapAlphaMaterial* planeMaterial = nullptr;
    Qt3DExtras::QPhongAlphaMaterial* planeMaterial = nullptr;

    Qt3DRender::QAbstractTexture* texture = nullptr;
    TextureImage* textureImage = nullptr;
};

#endif // LABELLINGTEXTPREVIEW_H
