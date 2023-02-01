
#ifndef __FG_RENDERER_HXX
#define __FG_RENDERER_HXX 1

#include <simgear/screen/extensions.hxx>
#include <simgear/scene/sky/sky.hxx>

#define FG_ENABLE_MULTIPASS_CLOUDS 1

class SGSky;
extern SGSky *thesky;

extern glPointParameterfProc glPointParameterfPtr;
extern glPointParameterfvProc glPointParameterfvPtr;
extern bool glPointParameterIsSupported;
extern bool glPointSpriteIsSupported;


class FGRenderer {

public:

    FGRenderer();
    ~FGRenderer();

    void init();

    void build_states();
    static void resize(int width, int height );

    // calling update( refresh_camera_settings = false ) will not
    // touch window or camera settings.  This is useful for the tiled
    // renderer which needs to set the view frustum itself.
    static void update( bool refresh_camera_settings );
    inline static void update() { update( true ); }


    /** FlightGear code should use this routine to set the FOV rather
     *  than calling the ssg routine directly
     */
    static void setFOV( float w, float h );


    /** FlightGear code should use this routine to set the Near/Far
     *  clip planes rather than calling the ssg routine directly
     */
    static void setNearFar( float n, float f );

    /** Get the pick start point and direction in global coordinates.
     *  The inputs are expected to be the x and y coordinates of the
     *  screen point relative to the window.
     */
    static bool getPickInfo( SGVec3d& p, SGVec3d& d, unsigned x, unsigned y );
};

#endif
