#ifndef IMAGERENDER_H
#define IMAGERENDER_H

#include <QOpenGLBuffer>
#include <QOpenGLFunctions>
#include <QOpenGLShader>
#include <QOpenGLShaderProgram>
#include <QOpenGLTexture>
#include <QQuickFramebufferObject>
#include <QMutex>
#include <QOpenGLVertexArrayObject>
static const char vertexShader[] =
    "attribute vec4 rawVertex;"
    "attribute vec2 rawTexture;"
    "varying   vec2 currentTexture;"
    "void main()"
    "{"
    "    currentTexture = rawTexture;"
    "    gl_Position    = rawVertex;"
    "}";

static const char fragmentShader[] =
    "varying vec2      currentTexture;"
    "uniform sampler2D colorTexture;"
    "uniform bool      enabledPremultiply;"
    "uniform bool      enabledSwapRedBlue;"
    "void main()"
    "{"
    "    vec4 textureColor = texture2D( colorTexture, currentTexture );"
    "    if ( textureColor.w < 0.001 ) { discard; }"
    "    if ( enabledPremultiply )"
    "    {"
    "        textureColor.x *= textureColor.w;"
    "        textureColor.y *= textureColor.w;"
    "        textureColor.z *= textureColor.w;"
    "    }"
    "    if ( enabledSwapRedBlue )"
    "    {"
    "        float buf      = textureColor.x;"
    "        textureColor.x = textureColor.z;"
    "        textureColor.z = buf;"
    "    }"
    "    gl_FragColor = textureColor;"
    "}";

// ImageRender
class ImageRender: public QOpenGLFunctions
{
public:
    ImageRender() = default;
    ~ImageRender() = default;
    bool init();
    void render();
    void setImage(const QImage& image);
    void resize(int w, int h);
private:
    QByteArray getGlslData(const char *rawData);
    QSharedPointer< QOpenGLVertexArrayObject > generateVAOData();
private:
    QMutex mutex_;
    QImage buffer_;

    bool includesTransparentData_    = false;
    bool includesPremultipliedData_  = false;
    bool includesBGRData_            = false;

    int enabledPremultiplyLocation_ = -1;
    int enabledSwapRedBlueLocation_ = -1;

    QSharedPointer< QOpenGLShaderProgram >     program_;
    QSharedPointer< QOpenGLTexture >           imageTexture_;
    QSharedPointer< QOpenGLVertexArrayObject > imageVAO_;
};
#endif // IMAGERENDER_H
