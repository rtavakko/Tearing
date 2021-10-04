#ifndef INVERTFILTER_H
#define INVERTFILTER_H

#include <openglrendersurface.h>

class InvertFilter : public OpenGLRenderSurface
{
public:
    InvertFilter(QScreen* outputScreen,
                 QObject* parent,
                 OpenGLRenderer::OpenGLRenderSpecs specs,
                 const QSurfaceFormat& surfaceFormat,
                 QOpenGLContext* sharedContext);

public slots:
    virtual void processFrame(GLuint textureID);

    virtual void renderFrame() override;

protected:

    virtual void initializeShaderProgram() override;
    virtual void initializeUniforms() override;

    virtual void updateUniforms() override;

    //Input texture
    GLuint inputTextureID;
};

#endif // INVERTFILTER_H
