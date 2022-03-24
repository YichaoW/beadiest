#ifndef MYGL_H
#define MYGL_H

#include <openglcontext.h>
#include <utils.h>
#include <shaderprogram.h>
#include <scene/squareplane.h>
#include "camera.h"
#include "mesh.h"
#include <QListWidgetItem>

#include <QOpenGLVertexArrayObject>
#include <QOpenGLShaderProgram>


class MyGL
    : public OpenGLContext
{
    Q_OBJECT
private:
    SquarePlane m_geomSquare;// The instance of a unit cylinder we can use to render any cylinder
    ShaderProgram m_progLambert;// A shader program that uses lambertian reflection
    ShaderProgram m_progFlat;// A shader program that uses "flat" reflection (no shadowing at all)

    GLuint vao; // A handle for our vertex array object. This will store the VBOs created in our geometry classes.
                // Don't worry too much about this. Just know it is necessary in order to render geometry.



    Camera m_glCamera;


public:
    Mesh m_cube;
    VertexDisplay m_vertDisplay;
    HalfEdgeDisplay m_heDisplay;
    FaceDisplay m_faceDisplay;

    explicit MyGL(QWidget *parent = nullptr);
    ~MyGL();

    void initializeGL();
    void resizeGL(int w, int h);
    void paintGL();

    void addHalfEdge();

    void triangulate();

public slots:
    void slot_setSelectedVert(QListWidgetItem *i);
    void slot_setSelectedHE(QListWidgetItem *i);
    void slot_setSelectedFace(QListWidgetItem *i);

    void slot_changeBlue(double i);
    void slot_changeRed(double i);
    void slot_changeGreen(double i);

    void slot_changeX(double i);
    void slot_changeY(double i);
    void slot_changeZ(double i);




signals:
    void sig_created();

protected:
    void keyPressEvent(QKeyEvent *e);
};


#endif // MYGL_H
