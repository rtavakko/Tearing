#include "videothread.h"

VideoThread::VideoThread(OpenGLRenderSurface *source) :
    QThread(),
    renderer(source),
    outputInitialized(false)
{
}

OpenGLDisplay *VideoThread::getDisplay()
{
    return display;
}

void VideoThread::resizeOutput(unsigned int w, unsigned int h)
{
    if(!outputInitialized)
        return;

    display->resize(w,h);
}

void VideoThread::updateSpecs(OpenGLRenderer::OpenGLRenderSpecs specs)
{
    if(!outputInitialized)
        return;

    display->updateSpecs(specs);
}

void VideoThread::setFrame(GLuint texID, unsigned int width, unsigned int height)
{
    if(!outputInitialized)
        return;

    display->setFrame(texID,width,height);
}

void VideoThread::run()
{
    createOutput();
    QThread::run();
}

bool VideoThread::createOutput()
{
    if(outputInitialized)
        return true;

    display = new OpenGLDisplay(nullptr,
                                renderer->getSpecs(),
                                renderer->getOpenGLFormat(),
                                renderer->getOpenGLContext());
    display->show();
    outputInitialized = true;
}
