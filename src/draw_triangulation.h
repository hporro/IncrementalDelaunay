#ifndef DRAW_TRIANGULATION
#define DRAW_TRIANGULATION

#include <GL/gl3w.h>  
#include <GLFW/glfw3.h>

#include "delaunay.h"
#include "core/shader.h"

template<class T>
void print(std::vector<T> v){
    for(T e: v){
        std::cout << e << " ";
    }std::cout << std::endl;
}

class TriangulationDrawer {
public:
    Triangulation* t;
    GLuint vao = 0;
    GLuint vbo = 0;
    GLuint ebo = 0;
    float color[3];
    Shader sh = Shader("vertex.vert","fragment.frag");

    TriangulationDrawer(Triangulation *t) : t(t){
        genBuffers();
    }

    void genBuffers(){
        glDeleteVertexArrays(1, &vao);
        glDeleteBuffers(1, &vbo);
        glDeleteBuffers(1, &ebo);

        glGenVertexArrays(1, &vao);
        glGenBuffers(1, &vbo);
        glGenBuffers(1, &ebo);

        glBindVertexArray(vao);

        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        std::vector<Vec2> vecs(t->vcount);
        for(int i=0;i<t->vcount;i++)vecs[i]=t->vertices[i].pos;
        glBufferData(GL_ARRAY_BUFFER, t->vcount*sizeof(Vec2), &vecs[0].x, GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
        std::vector<unsigned int> inds(t->tcount*3);
        for(int i=0;i<t->tcount;i++){
            inds[i*3+0] = t->triangles[i].v[0];
            inds[i*3+1] = t->triangles[i].v[1];
            inds[i*3+2] = t->triangles[i].v[2];
        }
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, t->tcount*3*sizeof(unsigned int), &inds[0], GL_STATIC_DRAW);

        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2*sizeof(float), (void*)0);

        glEnableVertexAttribArray(0);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }

    void draw(){
        sh.use();
        sh.setVec3("aColor",glm::vec3(color[0],color[1],color[2]));
        glBindVertexArray(vao);
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        glDrawElements(GL_TRIANGLES,t->tcount*3,GL_UNSIGNED_INT, 0);
    }

    void draw_points(){
        sh.use();
        sh.setFloat("a",t->a);
        sh.setFloat("pox",t->pox);
        sh.setFloat("poy",t->poy);
        sh.setVec3("aColor",glm::vec3(1-color[0],1-color[1],1-color[2]));
        glBindVertexArray(vao);
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        glDrawElements(GL_POINTS,t->tcount*3,GL_UNSIGNED_INT, 0);
    }

    void changeTriangulation(Triangulation* t){
        this->t = t;
        genBuffers();
    }
    void setColor(float color[3]){
        this->color[0] = color[0];
        this->color[1] = color[1];
        this->color[2] = color[2];
    }
};

#endif