#include <QDebug>
#include <QImage>
#include <cstdio>
#include <iostream>
#include "trianglesurface.h"
#include "vertex.h"

TriangleSurface::TriangleSurface() : VisualObject()
{
    gsml::Vertex v{};
    v.set_xyz(0,0,0); v.set_rgb(1,0,0); mVertices.push_back(v);
    v.set_xyz(0.5,0,0); v.set_rgb(0,1,0); mVertices.push_back(v);
    v.set_xyz(0.5,0.5,0); v.set_rgb(0,0,1); mVertices.push_back(v);
    v.set_xyz(0,0,0); v.set_rgb(0,1,0); mVertices.push_back(v);
    v.set_xyz(0.5,0.5,0); v.set_rgb(1,0,0); mVertices.push_back(v);
    v.set_xyz(0,0.5,0); v.set_rgb(0,0,1); mVertices.push_back(v);
}

TriangleSurface::TriangleSurface(std::string filnavn) : VisualObject()
{
    readFile(filnavn);
    //mMatrix.setToIdentity();
    //mMatrix.translate(0,0,5);
}

TriangleSurface::~TriangleSurface()
{
    //qDebug() << "TriangleSurface::~TriangleSurface()";
    //delete [] m_vertices;
    //qDebug() << "TriangleSurface::~TriangleSurface() - vertices deleted";
}

void TriangleSurface::readFile(std::string filnavn)
{
    std::vector<gsml::Vertex> tempVertices;
    std::ifstream inn;
    gsml::Vertex vertex;
    inn.open(filnavn.c_str());
    // Read in file
    if (inn.is_open())
    {
        int n;
        inn >> n;
        tempVertices.reserve(n);
        float VertexX = 0.0f;
        float VertexY = 0.0f;
        float VertexZ = 0.0f;
        // To make up for the huge numbers in the file
        float XDifference = 375500.0f;
        float YDifference = 6919100.0f;
        float ZDifference = 320.0f;
        for (int i=0; i<n; i++)
        {
            // Reads in one number at a time
            inn >> VertexX;
            inn >> VertexY;
            inn >> VertexZ;

            VertexX -= XDifference;
            VertexY -= YDifference;
            VertexZ -= ZDifference;
            // Finds the highest and lowest X and Y values.
            if (MinMaxFound){
                if (VertexX < MinX){
                    MinX = VertexX;
                }
                else if (VertexX > MaxX){
                    MaxX = VertexX;
                }
                if (VertexY < MinY){
                    MinY = VertexY;
                }
                else if (VertexY > MaxY){
                    MaxY = VertexY;
                }
            }
            else{
                MinX = VertexX;
                MaxX = VertexX;
                MinY = VertexY;
                MaxY = VertexY;
                MinMaxFound = true;
            }
            // Pushes the vertex
            vertex.set_xyz(VertexX, VertexY, VertexZ);
            tempVertices.push_back(vertex);
            VertexX = 0.0f;
            VertexY = 0.0f;
            VertexZ = 0.0f;

        }
        inn.close();
    }
    qDebug() << "Max X: " << MaxX;
    qDebug() << "Min X: " << MinX;
    qDebug() << "Max Y: " << MaxY;
    qDebug() << "Min Y: " << MinY;
    // Finds the average height of the Z-coordinate in every 5x5 square
    int PointCount = 0;
    float ZVertexTotal = 0.f;
    // First goes along the X-axis
    for (int t = 0; t < (MaxX-MinX)/5; t++){
        // Then checks for every point along the Y-axis
        for (int v = 0; v < (MaxY-MinY)/5; v++){
            // Checks every vertex...
            for (int u = 0; u < tempVertices.size(); u++){
                // ...if it fits in the 5x5 square, first the x-coordinate then the y-coordinate
                if (tempVertices[u].m_xyz[0] > (MinX+5*t) && tempVertices[u].m_xyz[0] < (5+MinX+5*t)){
                    if (tempVertices[u].m_xyz[1] > (MinY+5*v) && tempVertices[u].m_xyz[1] < (5+MinY+5*v)){
                        // Increases the total points in that square as well as the total amount of Z-coordinate
                        PointCount++;
                        ZVertexTotal += tempVertices[u].m_xyz[2];
                        //qDebug() << tempVertices[u].m_xyz[0] << " " << tempVertices[u].m_xyz[1] << " " << tempVertices[u].m_xyz[2];
                    }
                }
            }
            if (PointCount > 0){
                //qDebug() << PointCount;
                //qDebug() << "Average height: " << ZVertexTotal/PointCount;
                vertex.set_xyz((MinX+5*t)+2.5, (MinY+5*v)+2.5, ZVertexTotal/PointCount);
                mVertices.push_back(vertex);
                //qDebug() << mVertices.back().m_xyz[0] << " " << mVertices.back().m_xyz[1] << " " << mVertices.back().m_xyz[2];
            }
            else{
                vertex.set_xyz((MinX+5*t)+2.5, (MinY+5*v)+2.5, 0);
                mVertices.push_back(vertex);
            }
            PointCount = 0;
            ZVertexTotal = 0.f;
        }
    }

    // Setting up index based on the number of vertices
    int XSquares = (MaxX-MinX)/5;
    int YSquares = (MaxY-MinY)/5;
    int Index = 0;
    for (int i = 0; i < YSquares; i++){
        for (int j = 0; j < XSquares; j++){
            indices.push_back(Index);
            indices.push_back(Index+1);
            indices.push_back(Index+MaxY+1);
            indices.push_back(Index+MaxY);
            indices.push_back(Index);
            indices.push_back(Index+MaxY+1);
            Index++;
            //qDebug() << "Box";
        }
        Index++;
        //qDebug() << "Next row";
    }
    // X: -25 | 405
    // Y: 25 | 105
    // 5 x 5 ruter
}

void TriangleSurface::writeFile(std::string filnavn)
{
    std::ofstream ut;
    ut.open(filnavn.c_str());

    if (ut.is_open())
    {

        auto n = mVertices.size();
        gsml::Vertex vertex;
        ut << n << std::endl;
        for (auto it=mVertices.begin(); it != mVertices.end(); it++)
        {
            //vertex = *it;
            ut << *it << std::endl;
        }
        ut.close();
    }
}

void TriangleSurface::init(GLint matrixUniform)
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

    // 1rst attribute buffer : vertices
    glBindBuffer(GL_ARRAY_BUFFER, mVBO);
    glVertexAttribPointer(0, 3, GL_FLOAT,GL_FALSE,sizeof(gsml::Vertex), (GLvoid*)0);
    glEnableVertexAttribArray(0);

    unsigned int indexBuffer;
    glGenBuffers( 1, &indexBuffer );
    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, indexBuffer );
    glBufferData( GL_ELEMENT_ARRAY_BUFFER, indices.size()*sizeof( unsigned int ), indices.data(), GL_STATIC_DRAW );

    // 2nd attribute buffer : colors
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE,  sizeof(gsml::Vertex),  (GLvoid*)(3 * sizeof(GLfloat)) );
    glEnableVertexAttribArray(1);

    //enable the matrixUniform
    // mMatrixUniform = glGetUniformLocation( matrixUniform, "matrix" );

    glBindVertexArray(0);
}

void TriangleSurface::draw()
{
    glBindVertexArray( mVAO );
    glUniformMatrix4fv( mMatrixUniform, 1, GL_TRUE, mMatrix.constData());
    //glDrawArrays(GL_TRIANGLE_FAN, 0, mVertices.size());//mVertices.size());
    if (meshType == "point"){
        glDrawElements(GL_POINTS, static_cast<GLsizei>(indices.size()), GL_UNSIGNED_INT, nullptr);
    }
    if (meshType == "line"){
        glDrawElements(GL_LINES, static_cast<GLsizei>(indices.size()), GL_UNSIGNED_INT, nullptr);
    }
    if (meshType == "fill"){
        glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(indices.size()), GL_UNSIGNED_INT, nullptr);
    }
}

void TriangleSurface::construct()
{
    float xmin=0.0f, xmax=1.0f, ymin=0.0f, ymax=1.0f, h=0.25f;
    for (auto x=xmin; x<xmax; x+=h)
        for (auto y=ymin; y<ymax; y+=h)
        {
            float z = sin(M_PI*x)*sin(M_PI*y);
            mVertices.push_back(gsml::Vertex{x,y,z,x,y,z});
            z = sin(M_PI*(x+h))*sin(M_PI*y);
            mVertices.push_back(gsml::Vertex{x+h,y,z,x,y,z});
            z = sin(M_PI*x)*sin(M_PI*(y+h));
            mVertices.push_back(gsml::Vertex{x,y+h,z,x,y,z});
            mVertices.push_back(gsml::Vertex{x,y+h,z,x,y,z});
            z = sin(M_PI*(x+h))*sin(M_PI*y);
            mVertices.push_back(gsml::Vertex{x+h,y,z,x,y,z});
            z = sin(M_PI*(x+h))*sin(M_PI*(y+h));
            mVertices.push_back(gsml::Vertex{x+h,y+h,z,x,y,z});
        }
}

void TriangleSurface::construct_cylinder()
{
    float h=0.5;
    const int SEKTORER=12;
    float t=2*M_PI/SEKTORER;
    mVertices.clear();
    for (int k=0; k<2; k++)
    {
        for (int i=0; i<SEKTORER; i++)
        {
            float x0=cos(i*t);
            float y0=sin(i*t);
            float z0=h*k;
            float x1=cos((i+1)*t);
            float y1=sin((i+1)*t);
            float z2=h*(k+1);
            mVertices.push_back(gsml::Vertex{x0,y0,z0,0,0,1,0,0});
            mVertices.push_back(gsml::Vertex{x0,y0,z2,0,0,1,1,0});
            mVertices.push_back(gsml::Vertex{x1,y1,z0,0,0,1,0,1});
            mVertices.push_back(gsml::Vertex{x0,y0,z2,1,1,0,0,1});
            mVertices.push_back(gsml::Vertex{x1,y1,z2,1,1,0,1,0});
            mVertices.push_back(gsml::Vertex{x1,y1,z0,1,1,0,1,1});
            /*            float x0=cos(i*t);
            float y0=sin(i*t);
            float z0=h*k;
            float x1=cos((i+1)*t);
            float y1=sin((i+1)*t);
            float z2=h*(k+1);
            mVertices.push_back(Vertex{x0,y0,z0,0,0,1});
            mVertices.push_back(Vertex{x1,y1,z0,0,0,1});
            mVertices.push_back(Vertex{x0,y0,z2,0,0,1});
            mVertices.push_back(Vertex{x0,y0,z2,1,1,0});
            mVertices.push_back(Vertex{x1,y1,z0,1,1,0});
            mVertices.push_back(Vertex{x1,y1,z2,1,1,0});
*/        }
    }
}

void TriangleSurface::construct_plane()
{
    float dx=2.0;
    float dy=2.0;
    mVertices.clear();
    for (float y=-2.0; y<2.0; y+=dy)
    {
        for (float x=-3.0; x<3.0; x+=dx)
        {
            float x0=x;
            float y0=y;
            float x1=x0+dx;
            float y1=y0+dy;
            mVertices.push_back(gsml::Vertex{x0,y0,0,0,0.5,0});
            mVertices.push_back(gsml::Vertex{x1,y0,0,0,0.5,0});
            mVertices.push_back(gsml::Vertex{x0,y1,0,0,0.5,0});
            mVertices.push_back(gsml::Vertex{x0,y1,0,0,0.5,0});
            mVertices.push_back(gsml::Vertex{x1,y0,0,0,0.5,0});
            mVertices.push_back(gsml::Vertex{x1,y1,0,0,0.5,0});
        }
    }
}

