#include "invertfilter.h"

InvertFilter::InvertFilter(QScreen *outputScreen,
                           QObject *parent,
                           OpenGLRenderer::OpenGLRenderSpecs specs,
                           const QSurfaceFormat &surfaceFormat,
                           QOpenGLContext *sharedContext) :
    OpenGLRenderSurface(outputScreen,
                        parent,
                        specs,
                        surfaceFormat,
                        sharedContext),
    inputTextureID(0)
{
}

void InvertFilter::processFrame(GLuint textureID)
{
    inputTextureID = textureID;
    renderFrame();
}

void InvertFilter::renderFrame()
{
    updateStartTime();

    //Render to FBO
    if(!makeContextCurrent())
        return;

    initialize();

    glBindFramebuffer(GL_FRAMEBUFFER, fboID);
    glBindVertexArray(vaoID);

    glEnable(GL_DEPTH_TEST);
    glClearColor(0.0f,0.0f,0.0f,1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    updateUniforms();
    shader->bind();

    glActiveTexture(GL_TEXTURE0 + textureUnit);
    glBindTexture(GL_TEXTURE_2D, inputTextureID);

    glViewport(0,0,renderSpecs.frameType.width,renderSpecs.frameType.height);

    glDrawBuffers(1, &GL_outputColorAttachment);
    glDrawArrays(GL_TRIANGLES, 0, 6);

    glBindTexture(GL_TEXTURE_2D, 0);
    glActiveTexture(GL_TEXTURE0);

    shader->release();

    glBindVertexArray(0);
    glBindFramebuffer(GL_FRAMEBUFFER,0);

    doneContextCurrent();

    updateEndTime();

    //qDebug()<<QString("Render thread:")<<QThread::currentThreadId();
    emit renderedFrame(1000.0f/t_delta.count());

    emit frameReady(outputTextureID,
                    renderSpecs.frameType.width,
                    renderSpecs.frameType.height);
}

void InvertFilter::initializeShaderProgram()
{
    //Create debug triangle OpenGL program; add vertex and fragment shaders, link and bind
    shader = new QOpenGLShaderProgram();
    shader -> addShaderFromSourceFile(QOpenGLShader::Vertex, QString(":/GLSL/invertVertex.glsl"));
    shader -> addShaderFromSourceFile(QOpenGLShader::Fragment, QString(":/GLSL/invertFragment.glsl"));

    shader -> link();
    shader -> bind();

    //Get locations of vertex shader attributes
    vertexAttributeLocation = glGetAttribLocation(shader -> programId(), "vertex");
    texCoordAttributeLocation = glGetAttribLocation(shader -> programId(), "texCoord");

    textureUniformLocation = glGetUniformLocation(shader -> programId(), "texture");

    glGetUniformiv(shader->programId(), textureUniformLocation, &textureUnit);   //Set the value of the texture unit (GL_TEXTUREX) so it can be used in glActiveTexture

    shader->release();
}

void InvertFilter::initializeUniforms()
{
}

void InvertFilter::updateUniforms()
{
}
