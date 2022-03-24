#ifndef MESH_H
#define MESH_H
#include "drawable.h"
#include "smartpointerhelp.h"

#include <QListWidgetItem>
#include <QOpenGLContext>
#include <QOpenGLBuffer>
#include <QOpenGLShaderProgram>

class HalfEdge;

class Vertex : public QListWidgetItem {
public:
    static int comp_id;
    glm::vec3 pos;
    HalfEdge* he;
    int id;

    Vertex(glm::vec3 pos, int id);
};

class Face : public QListWidgetItem {
public:
    static int comp_id;
    HalfEdge* he;
    glm::vec3 color;
    int id;

    Face(glm::vec3 color, int id);
};

class HalfEdge : public QListWidgetItem {
public:
    static int comp_id;
    HalfEdge* next;
    HalfEdge* sym;
    Face* f;
    Vertex* v;
    int id;

    HalfEdge(Face* f, Vertex* v, int id);

    void setVertex(Vertex* vert);
};

class Mesh : public Drawable {
public:
    std::vector<uPtr<HalfEdge>> hes;
    std::vector<uPtr<Vertex>> vertices;
    std::vector<uPtr<Face>> faces;

    Mesh(OpenGLContext* mp_context);

    void create() override;

    void drawCube();

    void splitEdge(HalfEdge* he);

    void triangulateFace(Face* f);
};

class VertexDisplay : public Drawable {

public:
    Vertex *representedVertex;
    VertexDisplay(OpenGLContext* mp_context);
    // Creates VBO data to make a visual
    // representation of the currently selected Vertex
    void create() override;
    // Change which Vertex representedVertex points to
    void updateVertex(Vertex* v);

    virtual GLenum drawMode() override;
};

class HalfEdgeDisplay : public Drawable {

public:
    HalfEdge *representedHalfEdge;
    HalfEdgeDisplay(OpenGLContext* mp_context);

    void create() override;
    void updateHalfEdge(HalfEdge* he);

    virtual GLenum drawMode() override;
};

class FaceDisplay : public Drawable {

public:
    Face *representedFace;
    FaceDisplay(OpenGLContext* mp_context);

    void create() override;
    void updateFace(Face* f);

    virtual GLenum drawMode() override;
};

#endif // MESH_H
