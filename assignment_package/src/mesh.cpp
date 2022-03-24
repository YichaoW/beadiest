#include "mesh.h"
#include<la.h>
#include <iostream>
#include <string>

int Vertex::comp_id = 0;
int Face::comp_id = 0;
int HalfEdge::comp_id = 0;


Vertex::Vertex(glm::vec3 pos, int id)
    : pos(pos), id(id)
{
    setText(QString::number(id));
}

Face::Face(glm::vec3 color, int id)
    : color(color), id(id)
{
    setText(QString::number(id));
}

HalfEdge::HalfEdge(Face* f, Vertex* v, int id)
    : f(f), v(v), id(id)
{
    setText(QString::number(id));
}

Mesh::Mesh(OpenGLContext* mp_context)
    : Drawable(mp_context)
{}

void Mesh::create() {
    this->destroy();
    std::vector<glm::vec4> pos = std::vector<glm::vec4>();
    std::vector<glm::vec4> nor = std::vector<glm::vec4>();
    std::vector<glm::vec4> col = std::vector<glm::vec4>();
    std::vector<GLuint> idx = std::vector<GLuint>();

    int index = -1;
    for (const uPtr<Face> &face : faces) {
        // index for start of face
        index++;
        glm::vec4 normal;
        HalfEdge* curr = face->he;
        int i = 0;
        do {

            // update indices
            if (i >= 2) {
                idx.push_back(index);
                idx.push_back(index + i - 1);
                idx.push_back(index + i);
            }

            // pushing position, normal, color
            pos.push_back(glm::vec4(curr->v->pos, 1));
            col.push_back(glm::vec4(curr->f->color, 1));
            glm::vec3 pos1 = glm::vec3(curr->v->pos);
            glm::vec3 pos2 = glm::vec3(curr->next->v->pos);
            nor.push_back(glm::vec4(glm::normalize(glm::cross(pos1, pos2)), 0));

            // incrementing vars
            i++;
            curr = curr->next;
        } while (curr != face->he);
        index += i - 1;
    }

    count = idx.size();

    generateIdx();
    mp_context->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bufIdx);
    mp_context->glBufferData(GL_ELEMENT_ARRAY_BUFFER, idx.size() * sizeof(GLuint), idx.data(), GL_STATIC_DRAW);

    generatePos();
    mp_context->glBindBuffer(GL_ARRAY_BUFFER, bufPos);
    mp_context->glBufferData(GL_ARRAY_BUFFER, pos.size() * sizeof(glm::vec4), pos.data(), GL_STATIC_DRAW);

    generateNor();
    mp_context->glBindBuffer(GL_ARRAY_BUFFER, bufNor);
    mp_context->glBufferData(GL_ARRAY_BUFFER, nor.size() * sizeof(glm::vec4), nor.data(), GL_STATIC_DRAW);

    generateCol();
    mp_context->glBindBuffer(GL_ARRAY_BUFFER, bufCol);
    mp_context->glBufferData(GL_ARRAY_BUFFER, col.size() * sizeof(glm::vec4), col.data(), GL_STATIC_DRAW);
}

void Mesh::drawCube() {

    glm::vec3 cyan = glm::vec3(0, 255, 255);
    glm::vec3 red = glm::vec3(255, 0, 0);
    glm::vec3 green = glm::vec3(0, 255, 0);
    glm::vec3 blue = glm::vec3(0, 0, 255);
    glm::vec3 black = glm::vec3(0, 0, 0);
    glm::vec3 yellow = glm::vec3(255, 255, 0);

    // front face
    uPtr<Face> front = mkU<Face>(green, 1);
    uPtr<Vertex> v1 = mkU<Vertex>(glm::vec3(-0.5, -0.5, 0.5), 1);
    uPtr<Vertex> v2 = mkU<Vertex>(glm::vec3(-0.5, 0.5, 0.5), 2);
    uPtr<Vertex> v3 = mkU<Vertex>(glm::vec3(0.5, 0.5, 0.5), 3);
    uPtr<Vertex> v4 = mkU<Vertex>(glm::vec3(0.5, -0.5, 0.5), 4);

    uPtr<HalfEdge> he2 = mkU<HalfEdge>(front.get(), v4.get(), 2);
    uPtr<HalfEdge> he11 = mkU<HalfEdge>(front.get(), v3.get(), 11);
    uPtr<HalfEdge> he17 = mkU<HalfEdge>(front.get(), v2.get(), 17);
    uPtr<HalfEdge> he10 = mkU<HalfEdge>(front.get(), v1.get(), 10);

    he2->next = he11.get();
    he11->next = he17.get();
    he17->next = he10.get();
    he10->next = he2.get();

    front->he = he2.get();
    v1->he = he10.get();
    v2->he = he17.get();
    v3->he = he11.get();
    v4->he = he2.get();

    // back face
    uPtr<Face> back = mkU<Face>(red, 2);
    uPtr<Vertex> v5 = mkU<Vertex>(glm::vec3(-0.5, -0.5, -0.5), 5);
    uPtr<Vertex> v6 = mkU<Vertex>(glm::vec3(-0.5, 0.5, -0.5), 6);
    uPtr<Vertex> v7 = mkU<Vertex>(glm::vec3(0.5, 0.5, -0.5), 7);
    uPtr<Vertex> v8 = mkU<Vertex>(glm::vec3(0.5, -0.5, -0.5), 8);

    uPtr<HalfEdge> he6 = mkU<HalfEdge>(back.get(), v5.get(), 6);
    uPtr<HalfEdge> he13 = mkU<HalfEdge>(back.get(), v8.get(), 13);
    uPtr<HalfEdge> he22 = mkU<HalfEdge>(back.get(), v7.get(), 22);
    uPtr<HalfEdge> he16 = mkU<HalfEdge>(back.get(), v6.get(), 16);

    he6->next = he16.get();
    he13->next = he6.get();
    he22->next = he13.get();
    he16->next = he22.get();

    back->he = he6.get();
    v5->he = he6.get();
    v6->he = he16.get();
    v7->he = he22.get();
    v8->he = he13.get();

    // left face
    uPtr<Face> left = mkU<Face>(cyan, 3);

    uPtr<HalfEdge> he9 = mkU<HalfEdge>(left.get(), v2.get(), 9);
    uPtr<HalfEdge>he19 = mkU<HalfEdge>(left.get(), v6.get(), 19);
    uPtr<HalfEdge> he15 = mkU<HalfEdge>(left.get(), v5.get(), 15);
    uPtr<HalfEdge> he8 = mkU<HalfEdge>(left.get(), v1.get(), 8);

    he9->next = he19.get();
    he19->next = he15.get();
    he15->next = he8.get();
    he8->next = he9.get();

    left->he = he9.get();

    // right face
    uPtr<Face> right = mkU<Face>(blue, 4);

    uPtr<HalfEdge> he14 = mkU<HalfEdge>(right.get(), v7.get(), 14);
    uPtr<HalfEdge> he3 = mkU<HalfEdge>(right.get(), v8.get(), 3);
    uPtr<HalfEdge> he12 = mkU<HalfEdge>(right.get(), v4.get(), 12);
    uPtr<HalfEdge>he24 = mkU<HalfEdge>(right.get(), v3.get(), 24);

    he14->next = he24.get();
    he3->next = he14.get();
    he12->next = he3.get();
    he24->next = he12.get();

    right->he = he14.get();

    // top face
    uPtr<Face> top = mkU<Face>(black, 5);

    uPtr<HalfEdge> he18 = mkU<HalfEdge>(top.get(), v3.get(), 18);
    uPtr<HalfEdge> he23 = mkU<HalfEdge>(top.get(), v7.get(), 23);
    uPtr<HalfEdge> he21 = mkU<HalfEdge>(top.get(), v6.get(), 21);
    uPtr<HalfEdge> he20 = mkU<HalfEdge>(top.get(), v2.get(), 20);

    he18->next = he23.get();
    he23->next = he21.get();
    he21->next = he20.get();
    he20->next = he18.get();

    top->he = he18.get();

    // bottom face
    uPtr<Face> bottom = mkU<Face>(yellow, 6);

    uPtr<HalfEdge> he1 = mkU<HalfEdge>(bottom.get(), v1.get(), 1);
    uPtr<HalfEdge> he4 = mkU<HalfEdge>(bottom.get(), v4.get(), 4);
    uPtr<HalfEdge> he5 = mkU<HalfEdge>(bottom.get(), v8.get(), 5);
    uPtr<HalfEdge> he7 = mkU<HalfEdge>(bottom.get(), v5.get(), 7);

    he1->next = he7.get();
    he4->next = he1.get();
    he5->next = he4.get();
    he7->next = he5.get();

    bottom->he = he1.get();

    //symmetries
    he1->sym = he2.get();
    he2->sym = he1.get();
    he3->sym = he4.get();
    he4->sym = he3.get();
    he5->sym = he6.get();
    he6->sym = he5.get();
    he7->sym = he8.get();
    he8->sym = he7.get();
    he9->sym = he10.get();
    he10->sym = he9.get();
    he11->sym = he12.get();
    he12->sym = he11.get();
    he13->sym = he14.get();
    he14->sym = he13.get();
    he15->sym = he16.get();
    he16->sym = he15.get();
    he17->sym = he18.get();
    he18->sym = he17.get();
    he19->sym = he20.get();
    he20->sym = he19.get();
    he21->sym = he22.get();
    he22->sym = he21.get();
    he23->sym = he24.get();
    he24->sym = he23.get();

    this->hes.push_back(std::move(he1));
    this->hes.push_back(std::move(he2));
    this->hes.push_back(std::move(he3));
    this->hes.push_back(std::move(he4));
    this->hes.push_back(std::move(he5));
    this->hes.push_back(std::move(he6));
    this->hes.push_back(std::move(he7));
    this->hes.push_back(std::move(he8));
    this->hes.push_back(std::move(he9));
    this->hes.push_back(std::move(he10));
    this->hes.push_back(std::move(he11));
    this->hes.push_back(std::move(he12));
    this->hes.push_back(std::move(he13));
    this->hes.push_back(std::move(he14));
    this->hes.push_back(std::move(he15));
    this->hes.push_back(std::move(he16));
    this->hes.push_back(std::move(he17));
    this->hes.push_back(std::move(he18));
    this->hes.push_back(std::move(he19));
    this->hes.push_back(std::move(he20));
    this->hes.push_back(std::move(he21));
    this->hes.push_back(std::move(he22));
    this->hes.push_back(std::move(he23));
    this->hes.push_back(std::move(he24));

    this->vertices.push_back(std::move(v1));
    this->vertices.push_back(std::move(v2));
    this->vertices.push_back(std::move(v3));
    this->vertices.push_back(std::move(v4));
    this->vertices.push_back(std::move(v5));
    this->vertices.push_back(std::move(v6));
    this->vertices.push_back(std::move(v7));
    this->vertices.push_back(std::move(v8));

    this->faces.push_back(std::move(front));
    this->faces.push_back(std::move(back));
    this->faces.push_back(std::move(right));
    this->faces.push_back(std::move(left));
    this->faces.push_back(std::move(top));
    this->faces.push_back(std::move(bottom));

    Vertex::comp_id += 8;
    Face::comp_id += 6;
    HalfEdge::comp_id += 24;

    this->create();
}

VertexDisplay::VertexDisplay(OpenGLContext* mp_context)
    : Drawable(mp_context)
{}

void VertexDisplay::updateVertex(Vertex* v) {
    this->representedVertex = v;
    this->create();
}

void VertexDisplay::create() {
    this->destroy();
    std::vector<glm::vec4> pos {glm::vec4(this->representedVertex->pos, 1)};
    std::vector<glm::vec4> col = {glm::vec4(255, 255, 255, 1)};
    std::vector<GLuint> idx {0};

    count = idx.size();

    generateIdx();
    mp_context->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bufIdx);
    mp_context->glBufferData(GL_ELEMENT_ARRAY_BUFFER, idx.size() * sizeof(GLuint), idx.data(), GL_STATIC_DRAW);

    generatePos();
    mp_context->glBindBuffer(GL_ARRAY_BUFFER, bufPos);
    mp_context->glBufferData(GL_ARRAY_BUFFER, pos.size() * sizeof(glm::vec4), pos.data(), GL_STATIC_DRAW);

    generateCol();
    mp_context->glBindBuffer(GL_ARRAY_BUFFER, bufCol);
    mp_context->glBufferData(GL_ARRAY_BUFFER, col.size() * sizeof(glm::vec4), col.data(), GL_STATIC_DRAW);
}

GLenum VertexDisplay::drawMode() {
    return GL_POINTS;
}

HalfEdgeDisplay::HalfEdgeDisplay(OpenGLContext* mp_context)
    : Drawable(mp_context)
{}

void HalfEdgeDisplay::updateHalfEdge(HalfEdge* he) {
    this->representedHalfEdge = he;
    this->create();
}

void HalfEdgeDisplay::create() {

    this->destroy();
    std::vector<glm::vec4> pos {
                               glm::vec4(this->representedHalfEdge->v->pos, 1),
                               glm::vec4(this->representedHalfEdge->sym->v->pos, 1)
                               };
    std::vector<glm::vec4> col = {glm::vec4(1, 1, 0, 1), glm::vec4(1, 0, 0, 1)};
    std::vector<GLuint> idx {0, 1};

    count = idx.size();

    generateIdx();
    mp_context->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bufIdx);
    mp_context->glBufferData(GL_ELEMENT_ARRAY_BUFFER, idx.size() * sizeof(GLuint), idx.data(), GL_STATIC_DRAW);

    generatePos();
    mp_context->glBindBuffer(GL_ARRAY_BUFFER, bufPos);
    mp_context->glBufferData(GL_ARRAY_BUFFER, pos.size() * sizeof(glm::vec4), pos.data(), GL_STATIC_DRAW);

    generateCol();
    mp_context->glBindBuffer(GL_ARRAY_BUFFER, bufCol);
    mp_context->glBufferData(GL_ARRAY_BUFFER, col.size() * sizeof(glm::vec4), col.data(), GL_STATIC_DRAW);
}

GLenum HalfEdgeDisplay::drawMode() {
    return GL_LINES;
}

FaceDisplay::FaceDisplay(OpenGLContext* mp_context)
    : Drawable(mp_context)
{}

void FaceDisplay::updateFace(Face* f) {
    this->representedFace = f;
    this->create();
}

void FaceDisplay::create() {
    this->destroy();
    std::vector<glm::vec4> pos = std::vector<glm::vec4>();
    std::vector<glm::vec4> col = std::vector<glm::vec4>();
    std::vector<GLuint> idx = std::vector<GLuint>();

    glm::vec3 faceColor = this->representedFace->color;
    glm::vec4 color {255-faceColor.r, 255-faceColor.g, 255-faceColor.b, 1};
    int i = 0;
    HalfEdge* curr = this->representedFace->he;
    do {
        if (i >= 1) {
            idx.push_back(i-1);
            idx.push_back(i);
        }

        pos.push_back(glm::vec4(curr->v->pos, 1));
        col.push_back(color);
        i++;
        curr = curr->next;
    } while (curr != this->representedFace->he);
    idx.push_back(i - 1);
    idx.push_back(0);

    count = idx.size();

    generateIdx();
    mp_context->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bufIdx);
    mp_context->glBufferData(GL_ELEMENT_ARRAY_BUFFER, idx.size() * sizeof(GLuint), idx.data(), GL_STATIC_DRAW);

    generatePos();
    mp_context->glBindBuffer(GL_ARRAY_BUFFER, bufPos);
    mp_context->glBufferData(GL_ARRAY_BUFFER, pos.size() * sizeof(glm::vec4), pos.data(), GL_STATIC_DRAW);

    generateCol();
    mp_context->glBindBuffer(GL_ARRAY_BUFFER, bufCol);
    mp_context->glBufferData(GL_ARRAY_BUFFER, col.size() * sizeof(glm::vec4), col.data(), GL_STATIC_DRAW);
}

GLenum FaceDisplay::drawMode() {
    return GL_LINES;
}

void Mesh::splitEdge(HalfEdge* he) {
    Vertex* v1 = he->v;
    Vertex* v2 = he->sym->v;
    uPtr<Vertex> v3 = mkU<Vertex>((v1->pos + v2->pos) / 2.f, (float)Vertex::comp_id + 1);
    Vertex::comp_id += 1;

    HalfEdge* he1 = he;
    HalfEdge* he2 = he->sym;
    uPtr<HalfEdge> he1b = mkU<HalfEdge>(he1->f, he1->v, (float)HalfEdge::comp_id + 1);
    HalfEdge::comp_id += 1;
    uPtr<HalfEdge> he2b = mkU<HalfEdge>(he2->f, he2->v, (float)HalfEdge::comp_id + 1);
    HalfEdge::comp_id += 1;

    he1b->setVertex(v1);
    he2b->setVertex(v2);
    he1b->next = he1->next;
    he2b->next = he2->next;

    he1->next = he1b.get();
    he2->next = he2b.get();

    he1->setVertex(v3.get());
    he2->setVertex(v3.get());

    he2->sym = he1b.get();
    he1b->sym = he2;

    he1->sym = he2b.get();
    he2b->sym = he1;

    this->hes.push_back(std::move(he1b));
    this->hes.push_back(std::move(he2b));
    this->vertices.push_back(std::move(v3));
}

void HalfEdge::setVertex(Vertex* vert) {
    this->v = vert;
    vert->he = this;
}

void Mesh::triangulateFace(Face *f) {
    HalfEdge* he = f->he;

    uPtr<HalfEdge> he_a = mkU<HalfEdge>(f, he->v, (float)HalfEdge::comp_id + 1);
    HalfEdge::comp_id += 1;
    uPtr<HalfEdge> he_b = mkU<HalfEdge>(f, he->next->next->v, (float)HalfEdge::comp_id + 1);
    HalfEdge::comp_id += 1;
    he_a->sym = he_b.get();
    he_b->sym = he_a.get();

    uPtr<Face> new_f = mkU<Face>(f->color, (float)Face::comp_id + 1);
    Face::comp_id += 1;

    he_a->f = new_f.get();
    he->next->f = new_f.get();
    he->next->next->f = new_f.get();
    new_f->he = he_a.get();

    he_b->next = he->next->next->next;
    he->next->next->next = he_a.get();

    he_a->next = he->next;

    he->next = he_b.get();

    this->hes.push_back(std::move(he_a));
    this->hes.push_back(std::move(he_b));
    this->faces.push_back(std::move(new_f));
}
