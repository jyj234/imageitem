#include "imagerender.h"

bool ImageRender::init()
{
    this->initializeOpenGLFunctions();

    QSharedPointer< QOpenGLShaderProgram > program( new QOpenGLShaderProgram );

    auto vertexData = getGlslData( vertexShader );
    if ( vertexData.isEmpty() )
    {
        qDebug() << "ImageRender: read glsl file error";
        return false;
    }

    auto fragmentData = getGlslData( fragmentShader );
    if ( fragmentData.isEmpty() )
    {
        qDebug() << "ImageRender: read glsl file error";
        return false;
    }

    if ( !program->addShaderFromSourceCode( QOpenGLShader::Vertex, vertexData ) )
    {
        qDebug() << "ImageRender: add vertex shader error";
        return false;
    }

    if ( !program->addShaderFromSourceCode( QOpenGLShader::Fragment, fragmentData ) )
    {
        qDebug() << "ImageRender: add fragment shader error";
        return false;
    }

    program->bindAttributeLocation( "rawVertex", 0 );
    program->bindAttributeLocation( "rawTexturePos", 1 );

    if ( !program->link() )
    {
        qDebug() << "ImageRender: add fragment shader error";
        return false;
    }

    enabledPremultiplyLocation_ = program->uniformLocation( "enabledPremultiply" );
    enabledSwapRedBlueLocation_ = program->uniformLocation( "enabledSwapRedBlue" );

    program_ = program;
    program_->setUniformValue( program_->uniformLocation( "colorTexture" ), 0 );

    imageVAO_ = generateVAOData();

    return true;
}
void ImageRender::resize(int w,int h) {
    glViewport(0, 0, w, h);
}
QByteArray ImageRender::getGlslData(const char *rawData)
{
    QByteArray result;
#ifndef IS_DESKTOP
    //result.append( QByteArrayLiteral( "precision highp float;\n" ) );
#endif
    result.append( rawData );
    return result;
}

void ImageRender::render()
{
    if ( !buffer_.isNull() )
    {
        mutex_.lock();
        auto buffer = buffer_;
        buffer_ = { };
        mutex_.unlock();

        /*if (imageTexture_ && (QSize(imageTexture_->width(), imageTexture_->height()) == buffer.size()))
        {
            // 根据图片类型选择合适的setData
            if ( buffer.format() == QImage::Format_Grayscale8 )
            {
                includesTransparentData_   = false;
                includesPremultipliedData_ = false;
                includesBGRData_           = false;
                imageTexture_->setData( 0, QOpenGLTexture::Luminance, QOpenGLTexture::UInt8, buffer.constBits() );
            }
            else if ( buffer.format() == QImage::Format_RGB888 )
            {
                includesTransparentData_   = false;
                includesPremultipliedData_ = false;
                includesBGRData_           = false;
                imageTexture_->setData( 0, QOpenGLTexture::RGB, QOpenGLTexture::UInt8, buffer.constBits() );
            }
            else if ( buffer.format() == QImage::Format_RGBA8888 )
            {
                includesTransparentData_   = true;
                includesPremultipliedData_ = false;
                includesBGRData_           = false;
                imageTexture_->setData( 0, QOpenGLTexture::RGBA, QOpenGLTexture::UInt8, buffer.constBits() );
            }
            else if ( buffer.format() == QImage::Format_RGB32 )
            {
                includesTransparentData_   = false;
                includesPremultipliedData_ = false;
                includesBGRData_           = true;
                imageTexture_->setData( 0, QOpenGLTexture::RGBA, QOpenGLTexture::UInt8, buffer.constBits() );
            }
            else if ( buffer.format() == QImage::Format_ARGB32 )
            {
                includesTransparentData_   = true;
                includesPremultipliedData_ = false;
                includesBGRData_           = true;
                imageTexture_->setData( 0, QOpenGLTexture::RGBA, QOpenGLTexture::UInt8, buffer.constBits() );
            }
            else if ( buffer.format() == QImage::Format_ARGB32_Premultiplied )
            {
                includesTransparentData_   = true;
                includesPremultipliedData_ = true;
                includesBGRData_           = true;
                imageTexture_->setData( 0, QOpenGLTexture::RGBA, QOpenGLTexture::UInt8, buffer.constBits() );
            }
            else
            {
                qDebug() << "ImageRender::render: unsupported image format:" << buffer;
                return;
            }
        }
        else*/
        {
            includesTransparentData_   = buffer.hasAlphaChannel();
            includesPremultipliedData_ = false;
            includesBGRData_           = false;

            imageTexture_.reset( new QOpenGLTexture( buffer ) );
            imageTexture_->setWrapMode( QOpenGLTexture::ClampToEdge );
            imageTexture_->setAutoMipMapGenerationEnabled( false );
            imageTexture_->setMinificationFilter( QOpenGLTexture::LinearMipMapLinear);
            imageTexture_->setMagnificationFilter( QOpenGLTexture::Linear );
        }
    }
    if ( !program_ || !imageTexture_ ) { return; }

    program_->bind();
    imageVAO_->bind();
    imageTexture_->bind();

    GLfloat clearColor[ 4 ];

    // 带透明数据时先清空老的数据，并且开启混合
    /*if (includesTransparentData_ || buffer_.isNull())
    {
        this->glGetFloatv( GL_COLOR_CLEAR_VALUE, clearColor );
        this->glClearColor( 0.0, 0.0, 0.0, 0.0 );
        this->glClear( GL_COLOR_BUFFER_BIT );
        this->glEnable( GL_BLEND );
        this->glBlendFunc( GL_ONE, GL_ONE_MINUS_SRC_ALPHA );

        program_->setUniformValue( enabledPremultiplyLocation_, !includesPremultipliedData_ );
    }
    else
    {
    }*/
        program_->setUniformValue( enabledPremultiplyLocation_, false );

    program_->setUniformValue( enabledSwapRedBlueLocation_, includesBGRData_ );

    this->glDrawArrays( GL_TRIANGLES, 0, 6 );

    /*if (includesTransparentData_)
    {
        this->glDisable( GL_BLEND );
        this->glClearColor( clearColor[ 0 ], clearColor[ 1 ], clearColor[ 2 ], clearColor[ 3 ] );
        this->glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
    }*/

    imageTexture_->release();
    imageVAO_->release();
    program_->release();
}
void ImageRender::setImage(const QImage& image)
{
    buffer_ = image;
}
QSharedPointer< QOpenGLVertexArrayObject > ImageRender::generateVAOData()
{
    struct VertexTextureVBO
    {
        float vertexX;
        float vertexY;
        float vertexZ;

        float textureX;
        float textureY;
    };

    QByteArray vboBuffer;
    vboBuffer.resize( 6 * static_cast< int >( sizeof( VertexTextureVBO ) ) );

    auto current = reinterpret_cast< VertexTextureVBO * >( vboBuffer.data() );

    // 绘制一个图片需要2个三角面片，这里生成对应的顶点和纹理坐标
    // 因为OpenGL的坐标系是左下角为原点, 所以这里纹理是颠倒映射的
    current->vertexX = -1.0f; current->vertexY = -1.0f; current->vertexZ = 0; current->textureX = 0; current->textureY = 0; ++current;
    current->vertexX = -1.0f; current->vertexY =  1.0f; current->vertexZ = 0; current->textureX = 0; current->textureY = 1; ++current;
    current->vertexX =  1.0f; current->vertexY =  1.0f; current->vertexZ = 0; current->textureX = 1; current->textureY = 1; ++current;
    current->vertexX =  1.0f; current->vertexY =  1.0f; current->vertexZ = 0; current->textureX = 1; current->textureY = 1; ++current;
    current->vertexX =  1.0f; current->vertexY = -1.0f; current->vertexZ = 0; current->textureX = 1; current->textureY = 0; ++current;
    current->vertexX = -1.0f; current->vertexY = -1.0f; current->vertexZ = 0; current->textureX = 0; current->textureY = 0; ++current;

    // 创建VBO和VAO
    auto vbo = new QOpenGLBuffer( QOpenGLBuffer::Type::VertexBuffer );

    QSharedPointer< QOpenGLVertexArrayObject > vao;
    vao.reset(
        new QOpenGLVertexArrayObject,
        [ vbo ](QOpenGLVertexArrayObject *vao)
        { if ( !qApp ) { return; } vao->destroy(); delete vao; vbo->destroy(); delete vbo; } );

    vao->create();
    vao->bind();

    vbo->create();
    vbo->bind();

    vbo->allocate( vboBuffer.size() );
    vbo->write( 0, vboBuffer.constData(), vboBuffer.size() );

    this->glEnableVertexAttribArray( 0 );
    this->glEnableVertexAttribArray( 1 );

    this->glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, sizeof( VertexTextureVBO ), nullptr );
    this->glVertexAttribPointer( 1, 2, GL_FLOAT, GL_FALSE, sizeof( VertexTextureVBO ), reinterpret_cast< void * >( 12 ) );

    vbo->release();
    vao->release();

    return vao;
}
