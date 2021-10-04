#include "trianglerenderer.h"

TriangleRenderer::TriangleRenderer(QScreen *outputScreen,
                                   QObject *parent,
                                   OpenGLRenderSpecs specs,
                                   const QSurfaceFormat &surfaceFormat,
                                   QOpenGLContext *sharedContext) :
    OpenGLRenderSurface(outputScreen,
                        parent,
                        specs,
                        surfaceFormat,
                        sharedContext),
    depthrenderbuffer(0),
    trianglePositionAttributeLocation(0),
    triangleColorAttributeLocation(0),
    triangleMatrixUniformLocation(0),
    triangleAngle(0.0f)
{
}

void TriangleRenderer::renderFrame()
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

    glViewport(0, 0, renderSpecs.frameType.width, renderSpecs.frameType.height);

    glDrawArrays(GL_TRIANGLES, 0, 3);

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

void TriangleRenderer::initializeFBO()
{
    //Generate output FBO and texture
    glGenFramebuffers(1, &fboID);
    glBindFramebuffer(GL_FRAMEBUFFER, fboID);

    glGenTextures(1, &outputTextureID);

    glBindTexture(GL_TEXTURE_2D, outputTextureID);

    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    glTexImage2D(GL_TEXTURE_2D, 0, renderSpecs.frameType.internalFormat, renderSpecs.frameType.width, renderSpecs.frameType.height, 0, renderSpecs.frameType.format, renderSpecs.frameType.dataType, (const GLvoid*)(nullptr));
    glGenerateMipmap(GL_TEXTURE_2D);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_outputColorAttachment, GL_TEXTURE_2D, outputTextureID, 0);

    //Depth render buffer
    glGenRenderbuffers(1, &depthrenderbuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, depthrenderbuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, renderSpecs.frameType.width, renderSpecs.frameType.height);

    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthrenderbuffer);

    assert(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);

    glBindTexture(GL_TEXTURE_2D, 0);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void TriangleRenderer::initializeShaderProgram()
{
    //Create debug triangle OpenGL program; add vertex and fragment shaders, link and bind
    shader = new QOpenGLShaderProgram();
    shader -> addShaderFromSourceFile(QOpenGLShader::Vertex, QString(":/GLSL/triangleVertex.glsl"));
    shader -> addShaderFromSourceFile(QOpenGLShader::Fragment, QString(":/GLSL/triangleFragment.glsl"));

    shader -> link();
    shader -> bind();

    //Get locations of vertex shader attributes
    trianglePositionAttributeLocation = glGetAttribLocation(shader -> programId(), "positionAttribute");
    triangleColorAttributeLocation = glGetAttribLocation(shader -> programId(), "colorAttribute");

    triangleMatrixUniformLocation = glGetUniformLocation(shader -> programId(), "matrix");

    shader->release();
}

void TriangleRenderer::initializeVertexBuffers()
{
    static GLfloat triangleData[3][5] = {{0.0f, 0.707f, 1.0f, 0.0f, 0.0f},
                                         {-0.5f, -0.5f, 0.0f, 1.0f, 0.0f},
                                         {0.5f, -0.5f, 0.0f, 0.0f, 1.0f}
    };

    //Generate and bind triangle VAO
    glGenVertexArrays(1, &vaoID);
    glBindVertexArray(vaoID);

    //Generate and bind VBO
    glGenBuffers(1, &vboID);
    glBindBuffer(GL_ARRAY_BUFFER, vboID);
    glBufferData(GL_ARRAY_BUFFER, 15*sizeof(GLfloat), triangleData, GL_STATIC_DRAW);

    glEnableVertexAttribArray(trianglePositionAttributeLocation);
    glEnableVertexAttribArray(triangleColorAttributeLocation);

    glVertexAttribPointer(trianglePositionAttributeLocation, 2, GL_FLOAT, GL_TRUE, 5*sizeof(GLfloat), (const void*)(0));
    glVertexAttribPointer(triangleColorAttributeLocation, 3, GL_FLOAT, GL_TRUE, 5*sizeof(GLfloat), (const void*)(2*sizeof(GLfloat)));

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER,0);
}

void TriangleRenderer::initializeUniforms()
{

}

void TriangleRenderer::updateUniforms()
{
    shader->bind();

    triangleAngle += 1.0f;
    triangleAngle = (triangleAngle>360.0f)?(0.0f):(triangleAngle);

    QMatrix4x4 matrix;
    matrix.perspective(60.0f, 4.0f / 3.0f, 0.1f, 100.0f);
    matrix.translate(0.0f, 0.0f, -2.0f);
    matrix.rotate(triangleAngle, 0.0f, 1.0f, 0.0f);

    shader->setUniformValue("matrix", matrix);

    shader->release();
}
