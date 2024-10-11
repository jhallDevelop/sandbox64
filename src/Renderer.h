/*================
Libdragon rendering functions


==================*/
#ifndef RENDERER_H
#define RENDERER_H
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/gl_integration.h>
#include <rspq_profile.h>
#include "Cube.h"
#include "Camera.h"
#define DEBUG_RDP 0


// Global Camera
static camera_t camera;

// Gloab frame counter
static uint64_t frames = 0;

// Global shader settings
static GLenum shade_model = GL_SMOOTH;
static bool fog_enabled = false;

static const GLfloat environment_color[] = { 0.85f, 0.85f, 0.85f, 1.f };



// Init Rendering
static inline void Renderer_Init(void){

    
	debugf("InitRendering\n");
	// Set ??
    dfs_init(DFS_DEFAULT_LOCATION);

    // Display resolution
    display_init(RESOLUTION_320x240, DEPTH_16_BPP, 3, GAMMA_NONE, FILTERS_RESAMPLE_ANTIALIAS_DEDITHER);

    // setup the rdpq
    rdpq_init();
    // setup
    gl_init();

#if DEBUG_RDP
    rdpq_debug_start();
    rdpq_debug_log(true);
#endif


    // Setup camera things
    camera.distance = -10.0f;
    camera.rotation = 0.0f;
    float aspect_ratio = (float)display_get_width() / (float)display_get_height();
    float near_plane = 1.0f;
    float far_plane = 50.0f;

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glFrustum(-near_plane*aspect_ratio, near_plane*aspect_ratio, -near_plane, near_plane, near_plane, far_plane);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();


    // Setup materials 
    GLfloat mat_diffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, mat_diffuse);

    // Set other opengl things like fog
    glFogf(GL_FOG_START, 5);
    glFogf(GL_FOG_END, 20);
    glFogfv(GL_FOG_COLOR, environment_color);

    glEnable(GL_MULTISAMPLE_ARB);

    rspq_profile_start();


}
// Update Renderer

static inline void Renderer_Update(void){
    // Get the display and z buffer 
    surface_t *disp = display_get();
    surface_t *zbuf = display_get_zbuf();
    // attatch the buffer to the gpu
    rdpq_attach(disp, zbuf);

    // Begin opengl things
    gl_context_begin();

    // clear the background
    glClearColor(environment_color[0], environment_color[1], environment_color[2], environment_color[3]);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // set the matrix mode
    glMatrixMode(GL_MODELVIEW);
   
    // attatch the camera transform
    camera_transform(&camera);

    
    // Set some global render modes that we want to apply to all models
    // Enable opengl things
    glEnable(GL_LIGHTING);
    glEnable(GL_NORMALIZE);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    //glEnable(GL_TEXTURE_2D);
    //glBindTexture(GL_TEXTURE_2D, textures[texture_index]);
    
    // Render Shapes
    //render_plane();
    render_cube();
    // bind the textures
    //glBindTexture(GL_TEXTURE_2D, textures[(texture_index + 1)%4]);
    //glDisable(GL_TEXTURE_2D);
    //disable the lighting
    glDisable(GL_LIGHTING);

    // Draw a primitive with GL_RDPQ_TEXTURING_N64
    //glEnable(GL_RDPQ_TEXTURING_N64);
    //glEnable(GL_RDPQ_MATERIAL_N64);

    // When rendering with GL_RDPQ_TEXTURING_N64 we need to manualy specify the
    // tile size and if a 0.5 offset should be used since the ucode itself cannot
    // determine these. Here we set the tile size to be 32x32 and we apply an offset
    // since we are using bilinear texture filtering
    //glTexSizeN64(32, 32);
    //rdpq_sprite_upload(TILE0, sprites[0], &(rdpq_texparms_t){.s.repeats = REPEAT_INFINITE, .t.repeats = REPEAT_INFINITE});
    rdpq_set_mode_standard();
    rdpq_mode_filter(FILTER_BILINEAR);

    // Disable texturing
    //glDisable(GL_RDPQ_TEXTURING_N64);
    //glDisable(GL_RDPQ_MATERIAL_N64);

    gl_context_end();

    rdpq_detach_show();

    rspq_profile_next_frame();

    if (((frames++) % 60) == 0) {
        rspq_profile_dump();
        rspq_profile_reset();
        debugf("frame %lld\n", frames);
    }

    // TODO: what does this do?
    if (DEBUG_RDP){
        rspq_wait();
    }

}
// Shutdown Renderer
static inline void Renderer_Shutdown(void){

}
#endif
