#ifndef DEF_RENDERER_H
#define DEF_RENDERER_H

#include "RenderCommand.h"
#include "CameraManager.h"
#include "LightManager.h"

class Camera;
class PostFX;

struct NoActiveCamera{};

struct Renderer {
    CameraManager cameras_;
    LightManager lights_;
public:
    virtual ~Renderer() = default;

    virtual void push_opaque_command(const RenderObject3DCommand& command) = 0;
    virtual void push_opaque_command(RenderObject3DCommand&& command) = 0;
    virtual void push_transparent_command(const RenderObject3DCommand& command) = 0;
    virtual void push_transparent_command(RenderObject3DCommand&& command) = 0;
    virtual void push_hud_command(const RenderHudObjectCommand& command) = 0;
    virtual void push_hud_command(RenderHudObjectCommand&& command) = 0;
    virtual void push_debug_gizmo_command(const RenderDebugGizmoCommand&) {}
    virtual void push_debug_gizmo_command(RenderDebugGizmoCommand&&) {}
    virtual void push_terrain_command(const RenderTerrainCommand&) = 0;
    virtual void push_terrain_command(RenderTerrainCommand&&) = 0;
    virtual void render_frame() = 0;
    virtual void apply_post_fx(std::shared_ptr<PostFX> pfx) = 0;
    virtual void apply_post_fxs(std::vector<std::shared_ptr<PostFX>> pfxs) = 0;
    virtual void reset_post_fx() = 0;

    CameraManager& cameras() noexcept { return cameras_; }
    LightManager& lights() noexcept { return lights_; }
    const CameraManager& cameras() const noexcept { return cameras_; }
    const LightManager& lights() const noexcept { return lights_; }

    virtual void on_screen_sized_changed(unsigned int new_width, unsigned int new_height) = 0;
};

#endif