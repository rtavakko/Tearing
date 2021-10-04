#include "opengldisplay.h"

OpenGLDisplay::OpenGLDisplay(QScreen *outputScreen,
                             OpenGLRenderer::OpenGLRenderSpecs specs,
                             const QSurfaceFormat &surfaceFormat,
                             QOpenGLContext *sharedContext) :
    OpenGLRenderWindow(outputScreen,
                       specs,
                       surfaceFormat,
                       sharedContext),
    inputTextureID(0)
{
}

OpenGLDisplay::~OpenGLDisplay()
{

}

bool OpenGLDisplay::event(QEvent *event)
{
    switch (event->type())
    {
        case QEvent::UpdateRequest:
        renderFrame();
        return true;
        default:
        return QWindow::event(event);
    }
}

void OpenGLDisplay::exposeEvent(QExposeEvent *event)
{
    if(isExposed())
        renderFrame();
}

void OpenGLDisplay::resizeEvent(QResizeEvent *event)
{
    //TODO: FIX THIS; THE CONTEXT HAS TO BE ALREADY CREATED BY NOW SO NOT HAVING THIS SHOULDN'T CAUSE A CRASH
    if(!openGLContext)
        return;

    if(!makeContextCurrent())
        return;

    initialize();

    unsigned int w = event->size().width();
    unsigned int h = event->size().height();

    QWindow::resize(QSize(w,h));
    OpenGLRenderer::resize(w,h);

    doneContextCurrent();
}

void OpenGLDisplay::lockSize()
{
    setBaseSize(QSize(renderSpecs.frameType.width,renderSpecs.frameType.height));
}

void OpenGLDisplay::unlockSize()
{

}

void OpenGLDisplay::resize(unsigned int w, unsigned int h)
{
    if(!makeContextCurrent())
        return;

    initialize();

    QWindow::resize(QSize(w,h));
    OpenGLRenderer::resize(w,h);

    doneContextCurrent();
}

void OpenGLDisplay::updateSpecs(OpenGLRenderer::OpenGLRenderSpecs specs)
{
    if(!makeContextCurrent())
        return;

    initialize();

    OpenGLRenderer::updateSpecs(specs);

    swapSurfaceBuffers();
    doneContextCurrent();
}

void OpenGLDisplay::renderFrame()
{
    updateStartTime();

    if(!isExposed())
        return;

    if(!makeContextCurrent())
        return;

    initialize();

    //Render to FBO
    glBindFramebuffer(GL_FRAMEBUFFER,fboID);
    glBindVertexArray(vaoID);

    glClearColor(0.0f,0.0f,0.0f,1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glDisable(GL_DEPTH_TEST);

    shader->bind();

    //Update shader uniform values
    updateUniforms();

    glActiveTexture(GL_TEXTURE0 + textureUnit);
    glBindTexture(GL_TEXTURE_2D, inputTextureID);

    glViewport(0,0,renderSpecs.frameType.width,renderSpecs.frameType.height);

    glDrawBuffers(1, &GL_outputColorAttachment);
    glDrawArrays(GL_TRIANGLES, 0, 6);

    glBindTexture(GL_TEXTURE_2D, 0);
    glActiveTexture(GL_TEXTURE0);

    //Render to default FBO
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    glClearColor(0.0f,0.0f,0.0f,1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glDisable(GL_DEPTH_TEST);

    glActiveTexture(GL_TEXTURE0 + textureUnit);
    glBindTexture(GL_TEXTURE_2D, outputTextureID);

    glViewport(0,0,width(),height());
    glDrawArrays(GL_TRIANGLES, 0, 6);

    glBindTexture(GL_TEXTURE_2D, 0);
    glActiveTexture(GL_TEXTURE0);

    shader->release();

    glBindVertexArray(0);

    swapSurfaceBuffers();
    doneContextCurrent();

    updateEndTime();
    //qDebug()<<QString("Display thread:")<<QThread::currentThreadId();
    double actualFPS = 1000.0f/t_delta.count();

    emit renderedFrame(actualFPS);
}

void OpenGLDisplay::setFrame(GLuint texID, unsigned int width, unsigned int height)
{
    inputTextureID = texID;
    requestUpdate();
}

void OpenGLDisplay::renderLater()
{
    requestUpdate();
}
