#include <GL/gl3w.h>  
#include <GLFW/glfw3.h>

#include "delaunay.h"
#include "core/shader.h"

class TriangulationDrawer {
public:
    Triangulation* t;
    GLuint vao = 0;
    GLuint vbo = 0;
    GLuint ebo = 0;
    Shader sh = Shader("../src/shaders/vertex.vert","../src/shaders/fragment.frag");

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
        glBufferData(GL_ARRAY_BUFFER, t->vcount*sizeof(Vec2), &vecs[0], GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
        std::vector<unsigned int> inds(t->tcount*3);
        for(int i=0;i<t->tcount;i++){
            inds[i+0] = t->triangles[i].v0;
            inds[i+1] = t->triangles[i].v1;
            inds[i+2] = t->triangles[i].v2;
        }
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, t->tcount*3*sizeof(unsigned int), &inds[0], GL_STATIC_DRAW);

        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }

    void draw(){
        sh.use();
        glBindVertexArray(vao);
        //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        glDrawElements(GL_TRIANGLES,t->tcount,GL_UNSIGNED_INT, 0);
    }
};