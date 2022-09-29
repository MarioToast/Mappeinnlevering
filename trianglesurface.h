#ifndef TRIANGLESURFACE_H
#define TRIANGLESURFACE_H

#include "visualobject.h"
#include "vector3d.h"

class TriangleSurface : public VisualObject
{
public:
    TriangleSurface();
    TriangleSurface(std::string filnavn);
    ~TriangleSurface() override;
    void readFile(std::string filnavn);
    void writeFile(std::string filnavn);
    void init(GLint matrixUniform) override;
    void draw() override;
    void construct();
    void construct_cylinder();
    void construct_plane();
    std::vector<gsml::Vertex>& get_vertices() { return mVertices; } // 191120
    float MinX = 0;
    float MaxX = 0;
    float MinY = 0;
    float MaxY = 0;
    bool MinMaxFound = false;
};

#endif // TRIANGLESURFACE_H
