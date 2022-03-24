#include "mygl.h"
#include <la.h>

#include <iostream>
#include <QApplication>
#include <QKeyEvent>


MyGL::MyGL(QWidget *parent)
    : OpenGLContext(parent),
      m_geomSquare(this),
      m_progLambert(this),
      m_progFlat(this), m_glCamera(),
      m_cube(this),
      m_vertDisplay(this),
      m_heDisplay(this),
      m_faceDisplay(this)
{
    setFocusPolicy(Qt::StrongFocus);
}

MyGL::~MyGL()
{
    makeCurrent();
    glDeleteVertexArrays(1, &vao);
    m_geomSquare.destroy();
}

void MyGL::initializeGL()
{
    // Create an OpenGL context using Qt's QOpenGLFunctions_3_2_Core class
    // If you were programming in a non-Qt context you might use GLEW (GL Extension Wrangler)instead
    initializeOpenGLFunctions();
    // Print out some information about the current OpenGL context
    debugContextVersion();

    // Set a few settings/modes in OpenGL rendering
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LINE_SMOOTH);
    glEnable(GL_POLYGON_SMOOTH);
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
    glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);
    // Set the size with which points should be rendered
    glPointSize(5);
    // Set the color with which the screen is filled at the start of each render call.
    glClearColor(0.5, 0.5, 0.5, 1);

    printGLErrorLog();

    // Create a Vertex Attribute Object
    glGenVertexArrays(1, &vao);

    //Create the instances of Cylinder and Sphere.
    m_geomSquare.create();

    // Create Cube
    m_cube.drawCube();

    // Create and set up the diffuse shader
    m_progLambert.create(":/glsl/lambert.vert.glsl", ":/glsl/lambert.frag.glsl");
    // Create and set up the flat lighting shader
    m_progFlat.create(":/glsl/flat.vert.glsl", ":/glsl/flat.frag.glsl");


    // We have to have a VAO bound in OpenGL 3.2 Core. But if we're not
    // using multiple VAOs, we can just bind one once.
    glBindVertexArray(vao);

    emit(sig_created());
}

void MyGL::resizeGL(int w, int h)
{
    //This code sets the concatenated view and perspective projection matrices used for
    //our scene's camera view.
    m_glCamera = Camera(w, h);
    glm::mat4 viewproj = m_glCamera.getViewProj();

    // Upload the view-projection matrix to our shaders (i.e. onto the graphics card)

    m_progLambert.setViewProjMatrix(viewproj);
    m_progFlat.setViewProjMatrix(viewproj);

    printGLErrorLog();
}

//This function is called by Qt any time your GL window is supposed to update
//For example, when the function update() is called, paintGL is called implicitly.
void MyGL::paintGL()
{
    // Clear the screen so that we only see newly drawn images
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);



    m_progFlat.setViewProjMatrix(m_glCamera.getViewProj());
    m_progLambert.setViewProjMatrix(m_glCamera.getViewProj());
    m_progLambert.setCamPos(m_glCamera.eye);
    m_progFlat.setModelMatrix(glm::mat4(1.f));

    //Create a model matrix. This one rotates the square by PI/4 radians then translates it by <-2,0,0>.
    //Note that we have to transpose the model matrix before passing it to the shader
    //This is because OpenGL expects column-major matrices, but you've
    //implemented row-major matrices.
    glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(-2,0,0)) * glm::rotate(glm::mat4(), 0.25f * 3.14159f, glm::vec3(0,1,0));
    //Send the geometry's transformation matrix to the shader
    m_progLambert.setModelMatrix(model);
    //Draw the example sphere using our lambert shader
    //m_progLambert.draw(m_geomSquare);

    m_progFlat.draw(m_cube);

    //Now do the same to render the cylinder
    //We've rotated it -45 degrees on the Z axis, then translated it to the point <2,2,0>
    model = glm::translate(glm::mat4(1.0f), glm::vec3(2,2,0)) * glm::rotate(glm::mat4(1.0f), glm::radians(-45.0f), glm::vec3(0,0,1));
    m_progLambert.setModelMatrix(model);
    //m_progLambert.draw(m_geomSquare);

    glDisable(GL_DEPTH_TEST);
    if (m_vertDisplay.representedVertex) {
        m_progFlat.draw(m_vertDisplay);
    }
    if (m_heDisplay.representedHalfEdge) {
        m_progFlat.draw(m_heDisplay);
    }
    if (m_faceDisplay.representedFace) {
        m_progFlat.draw(m_faceDisplay);
    }
    glEnable(GL_DEPTH_TEST);
}


void MyGL::keyPressEvent(QKeyEvent *e)
{
    float amount = 2.0f;
    if(e->modifiers() & Qt::ShiftModifier){
        amount = 10.0f;
    }
    // http://doc.qt.io/qt-5/qt.html#Key-enum
    // This could all be much more efficient if a switch
    // statement were used
    if (e->key() == Qt::Key_Escape) {
        QApplication::quit();
    } else if (e->key() == Qt::Key_Right) {
        m_glCamera.RotateAboutUp(-amount);
    } else if (e->key() == Qt::Key_Left) {
        m_glCamera.RotateAboutUp(amount);
    } else if (e->key() == Qt::Key_Up) {
        m_glCamera.RotateAboutRight(-amount);
    } else if (e->key() == Qt::Key_Down) {
        m_glCamera.RotateAboutRight(amount);
    } else if (e->key() == Qt::Key_1) {
        m_glCamera.fovy += amount;
    } else if (e->key() == Qt::Key_2) {
        m_glCamera.fovy -= amount;
    } else if (e->key() == Qt::Key_W) {
        m_glCamera.TranslateAlongLook(amount);
    } else if (e->key() == Qt::Key_S) {
        m_glCamera.TranslateAlongLook(-amount);
    } else if (e->key() == Qt::Key_D) {
        m_glCamera.TranslateAlongRight(amount);
    } else if (e->key() == Qt::Key_A) {
        m_glCamera.TranslateAlongRight(-amount);
    } else if (e->key() == Qt::Key_Q) {
        m_glCamera.TranslateAlongUp(-amount);
    } else if (e->key() == Qt::Key_E) {
        m_glCamera.TranslateAlongUp(amount);
    } else if (e->key() == Qt::Key_R) {
        m_glCamera = Camera(this->width(), this->height());
    }

    else if (e->key() == Qt::Key_N) {
        if (m_heDisplay.representedHalfEdge) {
            m_heDisplay.updateHalfEdge(m_heDisplay.representedHalfEdge->next);
        }
    }

    else if (e->key() == Qt::Key_M) {
        if (m_heDisplay.representedHalfEdge) {
            m_heDisplay.updateHalfEdge(m_heDisplay.representedHalfEdge->sym);
        }
    }

    else if (e->key() == Qt::Key_F) {
        if (m_heDisplay.representedHalfEdge) {
            m_faceDisplay.updateFace(m_heDisplay.representedHalfEdge->f);
        }
    }

    else if (e->key() == Qt::Key_V) {
        if (m_heDisplay.representedHalfEdge) {
            m_vertDisplay.updateVertex(m_heDisplay.representedHalfEdge->v);
        }
    }

    else if (e->modifiers() == Qt::ShiftModifier && e->text() == QString('H')) {
        if (m_faceDisplay.representedFace) {
            m_heDisplay.updateHalfEdge(m_faceDisplay.representedFace->he);
        }
    }

    else if (e->key() == Qt::Key_H) {
        if (m_vertDisplay.representedVertex) {
            m_heDisplay.updateHalfEdge(m_vertDisplay.representedVertex->he);
        }
    }

    m_glCamera.RecomputeAttributes();
    update();  // Calls paintGL, among other things
}

void MyGL::slot_setSelectedVert(QListWidgetItem *i) {
    m_vertDisplay.updateVertex(dynamic_cast<Vertex*>(i));
    update();
}

void MyGL::slot_setSelectedHE(QListWidgetItem *i) {
    m_heDisplay.updateHalfEdge(dynamic_cast<HalfEdge*>(i));
    update();
}

void MyGL::slot_setSelectedFace(QListWidgetItem *i) {
    m_faceDisplay.updateFace(dynamic_cast<Face*>(i));
    update();
}

void MyGL::slot_changeBlue(double i) {
    if (m_faceDisplay.representedFace) {
        glm::vec3 c = m_faceDisplay.representedFace->color;
        m_faceDisplay.representedFace->color = glm::vec3(c.r, c.g, i);
        m_cube.create();
        update();
    }
}

void MyGL::slot_changeRed(double i) {
    if (m_faceDisplay.representedFace) {
        glm::vec3 c = m_faceDisplay.representedFace->color;
        m_faceDisplay.representedFace->color = glm::vec3(i, c.g, c.b);
        m_cube.create();
        update();
    }
}

void MyGL::slot_changeGreen(double i) {
    if (m_faceDisplay.representedFace) {
        glm::vec3 c = m_faceDisplay.representedFace->color;
        m_faceDisplay.representedFace->color = glm::vec3(c.r, i, c.b);
        m_cube.create();
        update();
    }
}

void MyGL::slot_changeX(double i) {
    if (m_vertDisplay.representedVertex) {
        glm::vec3 p = m_vertDisplay.representedVertex->pos;
        m_vertDisplay.representedVertex->pos = glm::vec3((float) i, p.y, p.z);
        m_cube.create();
        update();
    }
}

void MyGL::slot_changeY(double i) {
    if (m_vertDisplay.representedVertex) {
        glm::vec3 p = m_vertDisplay.representedVertex->pos;
        m_vertDisplay.representedVertex->pos = glm::vec3(p.x, (float) i, p.z);
        m_cube.create();
        update();
    }

}

void MyGL::slot_changeZ(double i) {
    if (m_vertDisplay.representedVertex) {
        glm::vec3 p = m_vertDisplay.representedVertex->pos;
        m_vertDisplay.representedVertex->pos = glm::vec3(p.x, p.y, (float) i);
        m_cube.create();
        update();
    }
}

void MyGL::addHalfEdge() {
    if (m_heDisplay.representedHalfEdge) {
        m_cube.splitEdge(m_heDisplay.representedHalfEdge);
        m_cube.create();
        update();
    }
}

void MyGL::triangulate() {
    if (m_faceDisplay.representedFace) {
        m_cube.triangulateFace(m_faceDisplay.representedFace);
        m_cube.create();
        update();
    }
}
