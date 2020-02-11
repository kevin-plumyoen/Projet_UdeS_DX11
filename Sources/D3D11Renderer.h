#ifndef DEF_D3D11_RENDERER_H
#define DEF_D3D11_RENDERER_H

#include "Renderer.h"
#include "RenderTexture.h"
#include "ScreenBillboard.h"
#include "LightingSystem.h"
#include "AfficheurTexte.h"
#include "PostFX.h"
#include "GizmoRenderer.h"
#include "Bloom.h"
#include "RadialBlur.h"

#include <vector>

namespace PM3D {
    class CDispositifD3D11;
}

struct ID3D11BlendState;
struct ID3D11DeviceContext;

class D3D11Renderer : public Renderer {
public:
    using Dispositif = PM3D::CDispositifD3D11;
private:
    Dispositif& dispositif;
    GizmoRenderer gizmo_renderer;
    CScreenBillboard screen_billboard;
    D3D11RenderTexture texture_rendertarget;
    D3D11RenderTexture multipasses_effect_rendertarget[2];
    std::size_t multipasses_effect_rendertarget_index;
    std::shared_ptr<PostFX> null_effect;
    PostFX hud_effect;
    std::vector<std::shared_ptr<PostFX>> post_fxs;

    std::vector<RenderObject3DCommand> render_object_commands;
    std::vector<RenderObject3DCommand> render_transparent_objects_commands;
    std::vector<RenderHudObjectCommand> render_hud_objects_commands;
    std::vector<RenderDebugGizmoCommand> render_gizmo_commands;
    std::vector<RenderTerrainCommand> render_terrain_commands;

    void setup_material(Materiel& material,Vec3 object_position);
    void setup_material(MaterielTerrain& material, Vec3 object_position);
    bool render_frame_on_texture();
    void apply_post_fx_passes();
    void render_texture_to_screen();
    void clear_render_commands();

    void sort_render_commands(const Camera& active_camera);
    void sort_opaque_commands(const Camera& active_camera);
    void sort_hud_commands();
    void sort_transparent_commands(const Camera& active_camera);
    void render_opaque_objects(const Camera& active_camera, ID3D11DeviceContext* pImmediateContext);
    void render_transparent_objects(const Camera& active_camera, ID3D11DeviceContext* pImmediateContext);
    void render_hud_objects(ID3D11DeviceContext* pImmediateContext);
    void render_gizmos(ID3D11DeviceContext*);
    void render_terrain(const Camera& active_camera, ID3D11DeviceContext* pImmediateContext);

public:
    D3D11Renderer(Dispositif& dispositif);
    virtual ~D3D11Renderer() = default;

    void push_opaque_command(const RenderObject3DCommand& command) override;
    void push_opaque_command(RenderObject3DCommand&& command) override;
    void push_transparent_command(const RenderObject3DCommand& command) override;
    void push_transparent_command(RenderObject3DCommand&& command) override;
    void push_hud_command(const RenderHudObjectCommand& command) override;
    void push_hud_command(RenderHudObjectCommand&& command) override;
    void push_debug_gizmo_command(const RenderDebugGizmoCommand& command) override;
    void push_debug_gizmo_command(RenderDebugGizmoCommand&& command) override;
    void push_terrain_command(const RenderTerrainCommand& command) override;
    void push_terrain_command(RenderTerrainCommand&& command) override;

    void render_frame() override;
    void apply_post_fx(std::shared_ptr<PostFX> pfx) override;
    void apply_post_fxs(std::vector<std::shared_ptr<PostFX>> fxs) override;
    void reset_post_fx() override;
    void on_screen_sized_changed(unsigned int new_width, unsigned int new_height) override;
};

#endif