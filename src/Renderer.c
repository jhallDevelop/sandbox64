/*================
Implementation of AF_Renderer
n64 Libdragon rendering functions


==================*/
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/gl_integration.h>
#include <rspq_profile.h>

#include "AF_Renderer.h"

#include "Cube.h"
#include "Plane.h"
#include "Camera.h"
#include "ECS/Entities/AF_ECS.h"
#define DEBUG_RDP 0



// Global Camera
static camera_t camera;

// Gloab frame counter
static uint64_t frames = 0;

// Global shader settings
//static GLenum shade_model = GL_SMOOTH;
//static bool fog_enabled = false;

static const GLfloat environment_color[] = { 0.2f, 0.2f, 0.2f, 1.f };
// Define the RGBA values for the ambient light (e.g., soft white light)
static const GLfloat ambientLight[] = {0.75f, 0.75f, 0.75f, 1.0f};  // R, G, B, A


// forward declare
void InfrequenceGLEnable(void);

// Init Rendering
void AF_Renderer_Init(AF_ECS* _ecs){

   	if(_ecs == NULL){
		debugf("Renderer: Renderer_Init has null ecs referenced passed in \n");
		return;
	} 
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
    camera.distance = -12.0f;
    camera.rotation = 0.0f;
    float aspect_ratio = (float)display_get_width() / (float)display_get_height();
    float near_plane = 1.0f;
    float far_plane = 50.0f;

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glFrustum(-near_plane*aspect_ratio, near_plane*aspect_ratio, -near_plane, near_plane, near_plane, far_plane);

    // Retrieve the projection matrix
    // Define the projection matrix manually
    float projectionMatrix[16] = {
        near_plane / (aspect_ratio * near_plane), 0, 0, 0,
        0, near_plane / near_plane, 0, 0,
        0, 0, -(far_plane + near_plane) / (far_plane - near_plane), -1,
        0, 0, -(2 * far_plane * near_plane) / (far_plane - near_plane), 0
    };

    // Now you can use projectionMatrix as needed
    for (int i = 0; i < 16; i++) {
        debugf("Projection Matrix [%d]: %f\n", i, projectionMatrix[i]);
    }

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();


    // Setup materials 
    GLfloat mat_diffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, mat_diffuse);

    // Set other opengl things like fog
    glFogf(GL_FOG_START, 5);
    glFogf(GL_FOG_END, 20);
    glFogfv(GL_FOG_COLOR, environment_color);

    // set ambient light
    // Set the global ambient light
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambientLight);

    glEnable(GL_MULTISAMPLE_ARB);

    rspq_profile_start();
    setup_cube();

    setup_plane();
    make_plane_mesh();

    InfrequenceGLEnable();

    rdpq_set_mode_standard();
    rdpq_mode_filter(FILTER_BILINEAR);
}

// Infrequence opengl calls that don't need to happen unless updating something important.
void InfrequenceGLEnable(void){
    // Set some global render modes that we want to apply to all models
    // Enable opengl things
    glEnable(GL_LIGHTING);
    glEnable(GL_NORMALIZE);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
}
// Update Renderer
// TODO: take in an array of entities 
void AF_Renderer_Update(AF_ECS* _ecs){
	if(_ecs == NULL){
		debugf("Renderer: Renderer_Update has null ecs referenced passed in \n");
		return;
	} 
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

    
    glEnable(GL_LIGHTING);

    //glEnable(GL_TEXTURE_2D);
    //glBindTexture(GL_TEXTURE_2D, textures[texture_index]);
    
    

    // loop through the objects to render
    // TODO: on the CPU, compbine all similar meshes with the same material and render in less draw calls
    for(int i = 0; i < _ecs->entitiesCount; ++i){
        // show debug
        AF_CMesh* mesh = &_ecs->meshes[i];
        if((AF_Component_GetHas(mesh->enabled) == TRUE) && (AF_Component_GetEnabled(mesh->enabled) == TRUE)){
            
            // is debug on
            if(mesh->showDebug == TRUE){
                //render debug
            }
            // Render mesh

            // Render Shapes
            if(_ecs->colliders[i].type == AABB){
                render_cube(&_ecs->transforms[i]);
            }
            if(_ecs->colliders[i].type == Plane){
                
                render_plane(&_ecs->transforms[i]);
            }
        }else{
            
        }

        
    }
    //debugf("RenderCube: x: %f y: %f z: %f\n", _ecs->transforms[2].pos.x, _ecs->transforms[2].pos.y, _ecs->transforms[2].pos.z);
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
    //rdpq_set_mode_standard();
    //rdpq_mode_filter(FILTER_BILINEAR);

    // Disable texturing
    //glDisable(GL_RDPQ_TEXTURING_N64);
    //glDisable(GL_RDPQ_MATERIAL_N64);

    

}

void AF_Renderer_Finish(){
    // Tell opengl to finish up
    gl_context_end();
    // present the frame
    rdpq_detach_show();

    

    // For debugging
    if (DEBUG_RDP){
        rspq_profile_next_frame();

        if (((frames++) % 60) == 0) {
            rspq_profile_dump();
            rspq_profile_reset();
            //debugf("frame %lld\n", frames);
        }
        rspq_wait();
    }
}


// Shutdown Renderer
void AF_Renderer_Shutdown(void){

}
