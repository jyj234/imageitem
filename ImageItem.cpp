
// Qt lib import
#include <QDebug>
#include <QMutex>
#include <QPainter>
#include <QThread>
#include <QOpenGLFunctions>
#include <QOpenGLFramebufferObjectFormat>
#include <QOpenGLShaderProgram>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLTexture>
#include <QOpenGLBuffer>
#include <QQuickOpenGLUtils>
#include "ImageItem.h"
#include "ImageRender.h"

class ImageItemRenderer: public QQuickFramebufferObject::Renderer
{
public:
    ImageItemRenderer();
    ~ImageItemRenderer() = default;
    void render() override;
    QOpenGLFramebufferObject* createFramebufferObject(const QSize& size) override;
    void synchronize(QQuickFramebufferObject*) override;

private:
    ImageRender m_render;
};
ImageItemRenderer::ImageItemRenderer()
{
    m_render.init();
}
void ImageItemRenderer::render()
{
    m_render.render();
    QQuickOpenGLUtils::resetOpenGLState();
}

QOpenGLFramebufferObject* ImageItemRenderer::createFramebufferObject(const QSize& size)
{
    QOpenGLFramebufferObjectFormat format;
    format.setAttachment(QOpenGLFramebufferObject::CombinedDepthStencil);
    format.setSamples(4);
    m_render.resize(size.width(), size.height());
    return new QOpenGLFramebufferObject(size, QOpenGLFramebufferObject::NoAttachment);
}
void ImageItemRenderer::synchronize(QQuickFramebufferObject* item)
{
    static int cnt = 0;
    //qDebug() << "VideoItemRender::synchronize" << cnt++;
    ImageItem* pItem = qobject_cast<ImageItem*>(item);
    if (pItem)
    {
        m_render.setImage(pItem->getImage());
    }
}
ImageItem::ImageItem()
{
}

ImageItem::~ImageItem()
{
}

void ImageItem::setImage(const QImage &image)
{
    if ( !image.isNull() &&
        ( image.format() != QImage::Format_Grayscale8 ) &&
        ( image.format() != QImage::Format_RGB888 ) &&
        ( image.format() != QImage::Format_RGBA8888 ) &&
        ( image.format() != QImage::Format_RGB32 ) &&
        ( image.format() != QImage::Format_ARGB32 ) &&
        ( image.format() != QImage::Format_ARGB32_Premultiplied ) )
    {
        qDebug() << "ImageItem::setImage: unsupported image format: " << image.format();
        return;
    }
    auto newImage = image.scaled(this->size().toSize(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    m_image = newImage;
    update();

}
const QImage& ImageItem::getImage()
{
    return m_image;
}
void ImageItem::clearImage(bool istransparent)
{
    QImage img(this->size().toSize(),QImage::Format_ARGB32);
    img.fill(QColor(0,0,0,255 * !istransparent));
    setImage(img);
}
QQuickFramebufferObject::Renderer *ImageItem::createRenderer() const
{
    return new ImageItemRenderer;
}
