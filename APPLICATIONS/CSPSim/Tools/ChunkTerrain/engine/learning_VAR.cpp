#pragma warning(disable:4244)   // No warnings on precision truncation
#pragma warning(disable:4305)   // No warnings on precision truncation
#pragma warning(disable:4786)   // stupid symbol size limitation

#ifdef WIN32
#  include <windows.h>
#endif

#include <iostream>
#include <list>
#include <vector>

#include <GL/gl.h>

//NV_vertex_array_range -- wglext.h
typedef void * (APIENTRY * PFNWGLALLOCATEMEMORYNVPROC) (int size, float readfreq, float writefreq, float priority);
typedef void (APIENTRY * PFNWGLFREEMEMORYNVPROC) (void *pointer);

// NV_fence -- glext.h
#ifndef GL_NV_fence
#define GL_NV_fence
#define GL_ALL_COMPLETED_NV               0x84F2
#define GL_FENCE_STATUS_NV                0x84F3
#define GL_FENCE_CONDITION_NV             0x84F4
typedef void (APIENTRY * PFNGLDELETEFENCESNVPROC) (GLsizei n, const GLuint *fences);
typedef void (APIENTRY * PFNGLGENFENCESNVPROC) (GLsizei n, GLuint *fences);
typedef GLboolean (APIENTRY * PFNGLISFENCENVPROC) (GLuint fence);
typedef GLboolean (APIENTRY * PFNGLTESTFENCENVPROC) (GLuint fence);
typedef void (APIENTRY * PFNGLGETFENCEIVNVPROC) (GLuint fence, GLenum pname, GLint *params);
typedef void (APIENTRY * PFNGLFINISHFENCENVPROC) (GLuint fence);
typedef void (APIENTRY * PFNGLSETFENCENVPROC) (GLuint fence, GLenum condition);
#endif

#define GLH_EXT_SINGLE_FILE
//#include "glh_ext.h"
#include "glh_genext.h"
#include "glh_obs.h"
#include "glh_glut.h"

using namespace std;
using namespace glh;

glut_callbacks cb;
glut_simple_mouse_interactor camera, object;
glut_perspective_reshaper reshaper;


#define STRIP_SIZE 48
int TILE_SIZE = 9 * STRIP_SIZE;
int NUM_BUFFERS = 4;
int BUFFER_SIZE = 1000000;
GLfloat * big_array = 0;
GLfloat * big_array_system = 0;
GLfloat * big_array_var = 0;
list< vector<GLuint> > elements;
GLfloat * xy_array = 0;
GLenum primitive = GL_QUAD_STRIP;

float hicoef = .06f;
float locoef = .10f;
float hifreq = 6.1f;
float lofreq = 2.5f;
float phase_rate = .02f;
float phase2_rate = -0.12f;
struct var_buffer
{
    GLfloat * pointer;
    GLuint fence;
};

var_buffer buffer[4];
GLuint not_finished = 0;

LARGE_INTEGER oldCount, newCount;
LARGE_INTEGER frequency;


bool b[256];

// glut-ish callbacks
void display();
void key(unsigned char k, int x, int y);
void idle();

void menu(int entry) { key(entry, 0, 0); }

// my functions
void init_opengl();

void init_ext(const char * ext)
{
    if(!InitExtension(ext))
    { cerr << "Failed to initialize " << ext << "!" << endl; exit(0); }
}

#define SIN_ARRAY_SIZE 1024
GLfloat * sin_array;
GLfloat * cos_array;

// use a fixed-point iterator through a power-of-2-sized array
class periodic_iterator
{
public:

    periodic_iterator(
                        unsigned int array_size,
                        float period, float initial_offset, float delta)
    {
        float array_delta =  array_size * (delta / period); // floating-point steps-per-increment
        increment = (unsigned int)(array_delta * (1<<16)); // fixed-point steps-per-increment

        float offset = array_size * (initial_offset / period); // floating-point initial index
        init_offset = (unsigned int)(offset * (1<<16));       // fixed-point initial index;
        
        array_size_mask = 0;
        int i = 20; // array should be reasonably sized...
        while((array_size & (1<<i)) == 0) 
            i--;
        array_size_mask = (1<<i)-1;        
        index = init_offset;
    }


    int get_index() const
    { return (index >> 16) & array_size_mask; }

    void incr() 
    { index += increment; }

    void decr() 
    { index -= increment; }

    void reset()
    { index = init_offset; }

private:
    unsigned int array_size_mask;
    // fraction bits == 16
    unsigned int increment;
    unsigned int init_offset;

    unsigned int index;
};


int main(int argc, char **argv)
{

    periodic_iterator lo_x(SIN_ARRAY_SIZE, 2 * 3.1415927f, 1.24f, (1.f/TILE_SIZE)*lofreq*3.1415927f);
    QueryPerformanceFrequency(&frequency);

 
    
    b['v'] = true; // VAR on
    b[' '] = true; // animation on
    b['i'] = true; // infinite viewer and light

    glutInit(&argc, argv);
    glutInitWindowSize(800, 800);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_DEPTH | GLUT_RGB );
    glutCreateWindow("Very Simple NV_vertex_array_range demo");

    cerr << "Extensions: " << endl << glGetString(GL_EXTENSIONS) << endl;

    init_opengl();

    glut_helpers_initialize();

    cb.keyboard_function = key;
    camera.configure_buttons(1);
    camera.set_camera_mode(true);
    object.configure_buttons(1);
    object.dolly.dolly[2] = -1;

    glut_add_interactor(&cb);
    glut_add_interactor(&object);
    glut_add_interactor(&reshaper);

    int toggles = glutCreateMenu(menu);
    glutAddMenuEntry("infinite/local light and viewer[i]", 'i');
    glutAddMenuEntry("vertex_array_range (still uses fence) [v]", 'v');
    glutAddMenuEntry("points (prefer this over wireframe) [p]", 'p');
    glutAddMenuEntry("wireframe [w]", 'w');
    glutAddMenuEntry("animation [ ]", ' ');
    glutAddMenuEntry("performance stats [r]", 'r');
    glutAddMenuEntry("lighting [d]", 'd');
    glutAddMenuEntry("flush after glDrawElements [f]", 'f');

    int tweak = glutCreateMenu(menu);
    glutAddMenuEntry("t/T to increase/decrease geometry",0);
    glutAddMenuEntry("1/2 to increase/decrease low frequency",0);
    glutAddMenuEntry("3/4 to increase/decrease high frequency",0);
    glutAddMenuEntry("5/6 to increase/decrease low frequency phase shift",0);
    glutAddMenuEntry("7/8 to increase/decrease high frequency phase shift",0);
    glutAddMenuEntry("l/L to increase/decrease low freqency amplitude", 0);
    glutAddMenuEntry("h/H to increase/decrease high freqency amplitude", 0);

    
    int copyrights = glutCreateMenu(menu);
    glutAddMenuEntry("filter_anisotropic (c) 2000 NVIDIA Corporation", 0);
    glutAddMenuEntry("GLH -- Copyright (c) 2000 Cass Everitt", 0);
    glutAddMenuEntry("GLH -- Copyright (c) 2000 NVIDIA Corporation", 0);


    glutCreateMenu(menu);
    glutAddSubMenu("toggles", toggles);
    glutAddSubMenu("tweakables", tweak);
    glutAddSubMenu("copyright info", copyrights);
    glutAddMenuEntry("quit [esc]", 27);
    glutAttachMenu(GLUT_RIGHT_BUTTON);

    glutIdleFunc(idle);
    glutDisplayFunc(display);
    glutMainLoop();
    return 0;
}

void compute_elements()
{
    delete [] xy_array;
    xy_array = new GLfloat[TILE_SIZE];
    {
        for(int i=0; i < TILE_SIZE; i++)
            xy_array[i] = i/(TILE_SIZE-1.f) - .5f; 
    }

    elements.erase(elements.begin(), elements.end());
    for(int i=0; i < TILE_SIZE-1; i++)
    {
        elements.push_back(vector<GLuint>());
        vector<GLuint> & el = elements.back();
        for(int j=0; j < STRIP_SIZE; j++)
        {
            el.push_back( i   *STRIP_SIZE + j);
            el.push_back((i+1)*STRIP_SIZE + j);
        }
    }
}

void allocate_big_array()
{
    float priority = .5f;

    float megabytes = (BUFFER_SIZE * sizeof(GL_FLOAT)/1000000.f);
    big_array = (GLfloat *)wglAllocateMemoryNV(BUFFER_SIZE*sizeof(GLfloat), 0, 0, priority);
    if(! big_array)
        big_array = (GLfloat *)wglAllocateMemoryNV(BUFFER_SIZE*sizeof(GLfloat), 0, 0, 1.f);

    if(big_array)  
    {
        cerr << "Allocated " << megabytes << " megabytes of fast memory" << endl;
        big_array_var = big_array;
        big_array_system = new GLfloat[BUFFER_SIZE];
        return;
    }

    cerr << "Unable to allocate " << megabytes
         << " megabytes of fast memory.  Trying less." << endl;
    BUFFER_SIZE /= 2;
    NUM_BUFFERS /= 2;
    megabytes = (BUFFER_SIZE * sizeof(GL_FLOAT)/1000000.f);

    big_array = (GLfloat *)wglAllocateMemoryNV(BUFFER_SIZE*sizeof(GLfloat), 0, 0, priority);
    if(! big_array)
        big_array = (GLfloat *)wglAllocateMemoryNV(BUFFER_SIZE*sizeof(GLfloat), 0, 0, 1.f);
    if(big_array)  
    {
        cerr << "Allocated " << megabytes << " megabytes of fast memory" << endl;
        big_array_var = big_array;
        big_array_system = new GLfloat[BUFFER_SIZE];
        return;
    }

    cerr << "Unable to allocate " << megabytes
         << " megabytes of fast memory.  Trying less." << endl;
    BUFFER_SIZE /= 2;
    megabytes = (BUFFER_SIZE * sizeof(GL_FLOAT)/1000000.f);

    big_array = (GLfloat *)wglAllocateMemoryNV(BUFFER_SIZE*sizeof(GLfloat), 0, 0, priority);
    if(! big_array)
        big_array = (GLfloat *)wglAllocateMemoryNV(BUFFER_SIZE*sizeof(GLfloat), 0, 0, 1.f);
    if(big_array)  
    {
        cerr << "Allocated " << megabytes << " megabytes of fast memory" << endl;
        big_array_var = big_array;
        big_array_system = new GLfloat[BUFFER_SIZE];
        return;
    }
    cerr << "Unable to allocate " << megabytes
         << " megabytes of fast memory.  Giving up." << endl;
    exit(0);
}

void init_opengl()
{
    glEnable(GL_DEPTH_TEST);
    init_ext("GL_NV_vertex_array_range");
    init_ext("GL_NV_fence");

    glEnable(GL_LIGHT0);
    glEnable(GL_LIGHTING);
    glEnable(GL_NORMALIZE);
    glMaterial(GL_FRONT_AND_BACK, GL_AMBIENT, vec4f(.1f,.1f,0,1));
    glMaterial(GL_FRONT_AND_BACK, GL_DIFFUSE, vec4f(.6f,.6f,.1f,1));
    glMaterial(GL_FRONT_AND_BACK, GL_SPECULAR, vec4f(1,1,.75f,1));
    glMaterial(GL_FRONT_AND_BACK, GL_SHININESS, 128.f);

    glLight(GL_LIGHT0, GL_POSITION, vec4f(.5f, 0, .5f, 0));
    glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_FALSE);

    allocate_big_array();    

    for(int i=0; i < NUM_BUFFERS; i++)
        buffer[i].pointer = big_array + (i * BUFFER_SIZE / NUM_BUFFERS);


    sin_array = new GLfloat[SIN_ARRAY_SIZE];
    cos_array = new GLfloat[SIN_ARRAY_SIZE];
    {
        for(int i=0; i < SIN_ARRAY_SIZE; i++)
        {
            sin_array[i] = sin((i*6.283185)/SIN_ARRAY_SIZE);
            cos_array[i] = cos((i*6.283185)/SIN_ARRAY_SIZE);
        }
    }

    glVertexArrayRangeNV(1000000*sizeof(GLfloat), big_array);
    glEnableClientState(GL_VERTEX_ARRAY_RANGE_NV);
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_NORMAL_ARRAY);

    compute_elements();

    glGenFencesNV(1, & buffer[0].fence);
    glGenFencesNV(1, & buffer[1].fence);
    glGenFencesNV(1, & buffer[2].fence);
    glGenFencesNV(1, & buffer[3].fence);
}

void key(unsigned char k, int x, int y)
{
    b[k] = ! b[k];
    if(k==27 || k=='q') exit(0);
    if(k=='w')
    {
        if(b[k])
            primitive=GL_LINE_STRIP;
        else
            primitive=GL_QUAD_STRIP;
    }
    if(k=='p')
    {
        if(b[k])
            primitive=GL_POINTS;
        else
            primitive=GL_QUAD_STRIP;
    }
    if('v'==k)
    {
        if(b[k])
        {
            glEnableClientState(GL_VERTEX_ARRAY_RANGE_NV);
            big_array = big_array_var;
        }
        else
        {
            glDisableClientState(GL_VERTEX_ARRAY_RANGE_NV);
            big_array = big_array_system;
        }

        for(int i=0; i < NUM_BUFFERS; i++)
            buffer[i].pointer = big_array + (i * BUFFER_SIZE / NUM_BUFFERS);
        
    }
    if('d'==k)
    {
        if(b[k])
            glDisable(GL_LIGHTING);
        else
            glEnable(GL_LIGHTING);
    }
    if('i'==k)
    {
        if(b[k]) // infinite light
        {
            glLight(GL_LIGHT0, GL_POSITION, vec4f(.5f, 0, .5f, 0));
            glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_FALSE);
        }
        else
        {
            glLight(GL_LIGHT0, GL_POSITION, vec4f(.5f, 0, -.5f,1));
            glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE);
        }
    }
    if(' '==k)
    {
        if(b[k])
            glutIdleFunc(idle);
        else
            glutIdleFunc(0);
    }
    if('h'==k)
        hicoef += .005;
    if('H'==k)
        hicoef -= .005;
    if('l'==k)
        locoef += .005;
    if('L'==k)
        locoef -= .005;
    if('1'==k)
        lofreq += .1f;
    if('2'==k)
        lofreq -= .1f;
    if('3'==k)
        hifreq += .1f;
    if('4'==k)
        hifreq -= .1f;
    if('5'==k)
        phase_rate += .01f;
    if('6'==k)
        phase_rate -= .01f;
    if('7'==k)
        phase2_rate += .01f;
    if('8'==k)
        phase2_rate -= .01f;

    if('t'==k)
        if(TILE_SIZE < 864) 
        {
            TILE_SIZE += STRIP_SIZE;
            compute_elements();
        }

    if('T'==k)
        if(TILE_SIZE > STRIP_SIZE)
        { 
            TILE_SIZE -= STRIP_SIZE;
            compute_elements();
        }

    glutPostRedisplay();
}


float phase = 0.f;
float phase2 = 0.f;
void idle()
{
    phase += phase_rate;
    phase2 += phase2_rate;

    if(phase > 2*3.1415927)
        phase = 0;
    if(phase2 > 2*3.1415927)
        phase2 = 0;

    glutPostRedisplay();
}

struct draw_strips
{
    void operator()(const vector<GLuint> & v)
    {
        glDrawElements(primitive, v.size(), GL_UNSIGNED_INT, &v[0]);
        if(b['f'])
            glFlush();
    }
};

int framecount = 0;
double cum_fps = 0;
void display()
{

    float delta = (1.f/TILE_SIZE);
    periodic_iterator lo_x(SIN_ARRAY_SIZE, 2 * 3.1415927f, phase, (1.f/TILE_SIZE)*lofreq*3.1415927f);
    periodic_iterator lo_y(lo_x);
    periodic_iterator hi_x(SIN_ARRAY_SIZE, 2 * 3.1415927f, phase2, (1.f/TILE_SIZE)*hifreq*3.1415927f);
    periodic_iterator hi_y(hi_x);

    QueryPerformanceCounter(&oldCount);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glPushMatrix();
    camera.apply_inverse_transform();

    object.apply_transform();

    not_finished = 0;
    int curr;
    int num_slabs = TILE_SIZE/STRIP_SIZE;
    for(int slab=0; slab < num_slabs; slab++)
    {
        curr = (slab)%NUM_BUFFERS;
        if(slab >= NUM_BUFFERS)
        {
            if(!glTestFenceNV(buffer[curr].fence))
            {
                not_finished++;
                glFinishFenceNV(buffer[curr].fence);
            }
        }
        GLfloat * v = buffer[curr].pointer;

        glVertexPointer(3, GL_FLOAT, 6 * sizeof(GLfloat), v);
        glNormalPointer(GL_FLOAT, 6 * sizeof(GLfloat), (v + 3) );

        GLfloat * psin = sin_array;
        GLfloat * pcos = cos_array;

        {
            GLfloat ysinlo[STRIP_SIZE];
            GLfloat ycoslo[STRIP_SIZE];
            GLfloat ysinhi[STRIP_SIZE];
            GLfloat ycoshi[STRIP_SIZE];

            for(int jj=0; jj < STRIP_SIZE; jj++)
            {
                ysinlo[jj] = psin[lo_y.get_index()];
                ycoslo[jj] = pcos[lo_y.get_index()]; lo_y.incr();
                ysinhi[jj] = psin[hi_y.get_index()];
                ycoshi[jj] = pcos[hi_y.get_index()]; hi_y.incr();
            }
            lo_y.decr();
            hi_y.decr();

            for(int i=0; i < TILE_SIZE; i++)
            {
                GLfloat x = xy_array[i];
                unsigned int lo_x_index = lo_x.get_index();
                unsigned int hi_x_index = hi_x.get_index();

                int j_offset = (STRIP_SIZE-1)*slab;
                GLfloat nx = locoef * -pcos[lo_x_index] + hicoef * -pcos[hi_x_index];
                for(int j=0; j < STRIP_SIZE; j++)
                {
                    GLfloat y = xy_array[j+j_offset];
                    GLfloat ny;

                    *v++ = x;
                    *v++ = y;
                    *v++ = locoef * (psin[lo_x_index] + ysinlo[j]) + 
                           hicoef * (psin[hi_x_index] + ysinhi[j]);
                    *v++ = nx;
                    *v++ = ny = locoef * -ycoslo[j] + hicoef * -ycoshi[j];
                    *v++ = .15f;//.15f * (1.f - sqrt(nx * nx + ny * ny));
                }
                lo_x.incr();
                hi_x.incr();
            }
            lo_x.reset();
            hi_x.reset();
        }
        
        for_each(elements.begin(), elements.end(), draw_strips());
        glSetFenceNV(buffer[curr].fence, GL_ALL_COMPLETED_NV);

    }    
    glPopMatrix();


    glFinishFenceNV(buffer[curr].fence);

    QueryPerformanceCounter(&newCount);
    double fps = (double)frequency.QuadPart/(double)(newCount.QuadPart - oldCount.QuadPart);
    
    if(b['r'])
    {
        cum_fps += fps;
        framecount++;
        if(framecount > 30)
        {
            cum_fps /= 30.f;
            double ppf =  TILE_SIZE * TILE_SIZE * 2;
            double mpps = ppf * cum_fps / 1000000.0;
            cerr << "fps: " << cum_fps << "  polys/frame: " << ppf << "  million polys/sec: " << mpps << endl;
            framecount = 0;
            cum_fps = 0;    
            //if(not_finished > 0)
            //    cerr << "not finished on " << not_finished << " of " << num_slabs << endl;
        }
    }

    glutSwapBuffers();
}
