#include "raindrop.h"
#include "random"

RainDrop::RainDrop(int n) : OctahedronBall (n)
{
    for (int i = 0; i < mVertices.size(); i++){
        mVertices [i].set_rgb(0,0,1);
    }
    std::random_device randomSeed;
    std::mt19937 randomGenerator(randomSeed());
    std::uniform_int_distribution<> randomRange(-30, 300);
    float XValue = randomRange(randomGenerator);
    float YValue = randomRange(randomGenerator);
    mPosition.translate(XValue, YValue, 100);
    mScale.scale(0.5, 0.5, 0.5);
    //qDebug() << "Raindrop location: " << mPosition(0,3) << " " << mPosition(1,3);
    mMatrix = mPosition * mScale;
}

void RainDrop::init(GLint matrixUniform)
{
    mMatrixUniform = matrixUniform;
    initializeOpenGLFunctions();

    //Vertex Array Object - VAO
    glGenVertexArrays( 1, &mVAO );
    glBindVertexArray( mVAO );

    //Vertex Buffer Object to hold vertices - VBO
    glGenBuffers( 1, &mVBO );
    glBindBuffer( GL_ARRAY_BUFFER, mVBO );

    glBufferData( GL_ARRAY_BUFFER, mVertices.size()*sizeof(gsml::Vertex), mVertices.data(), GL_STATIC_DRAW );

    glBindBuffer(GL_ARRAY_BUFFER, mVBO);
    glVertexAttribPointer(0, 3, GL_FLOAT,GL_FALSE,sizeof(gsml::Vertex), (GLvoid*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE,  sizeof(gsml::Vertex),  (GLvoid*)(3 * sizeof(GLfloat)) );
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);
    DoneInit = true;
}

void RainDrop::draw()
{
    glBindVertexArray( mVAO );
    glUniformMatrix4fv( mMatrixUniform, 1, GL_TRUE, mMatrix.constData());
    glDrawArrays(GL_TRIANGLES, 0, mVertices.size());
    LifeTimer -= 1;
    if (mMatrix(2,3) > 0){
        mPosition.translate(0, 0, -1);
        mMatrix = mPosition * mScale;
    }
    else{
        mScale.scale(0.9, 0.9, 0.9);
        mMatrix = mPosition * mScale;
    }
}

void RainDrop::DestroyDrop()
{
    delete this;
}
