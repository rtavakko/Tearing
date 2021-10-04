#ifndef TRIANGLERENDERER_H
#define TRIANGLERENDERER_H

#include <openglrendersurface.h>

class TriangleRenderer : public OpenGLRenderSurface
{
public:
    TriangleRenderer(QScreen* outputScreen,
                     QObject* parent,
                     OpenGLRenderer::OpenGLRenderSpecs specs,
                     const QSurfaceFormat& surfaceFormat,
                     QOpenGLContext* sharedContext);

public slots:
    virtual void renderFrame() override;

protected:

    virtual void initializeFBO() override;
    virtual void initializeShaderProgram() override;
    virtual void initializeVertexBuffers() override;
    virtual void initializeUniforms() override;

    virtual void updateUniforms() override;

    //For rendering a debug triangle
    GLuint depthrenderbuffer;

    GLint trianglePositionAttributeLocation;
    GLint triangleColorAttributeLocation;
    GLint triangleMatrixUniformLocation;

    float triangleAngle;
};

#endif // TRIANGLERENDERER_H
