#include "D3D11Renderer.h"
#include "Shader.h"
#include "Materiel.h"
#include "MaterielTerrain.h"
#include "objet3d.h"
#include "Camera.h"
#include "Logger.h"
#include <future>

D3D11Renderer::D3D11Renderer(Dispositif& dispositif)
: dispositif{dispositif}
, gizmo_renderer{&dispositif}
, screen_billboard(Vec2{0.f, 0.f}, Vec2{1.f, 1.f}, &dispositif)
, texture_rendertarget{dispositif}
, multipasses_effect_rendertarget{{dispositif}, {dispositif}}
, null_effect{std::make_shared<PostFX>(L"Screen.fx", &dispositif)}
, hud_effect{L"Hud.fx", &dispositif}
, post_fxs({null_effect}) {

}

void D3D11Renderer::push_opaque_command(const RenderObject3DCommand& command) {
    render_object_commands.push_back(command);
}

void D3D11Renderer::push_opaque_command(RenderObject3DCommand&& command) {
    render_object_commands.push_back(std::move(command));
}

void D3D11Renderer::push_transparent_command(const RenderObject3DCommand& command) {
    render_transparent_objects_commands.push_back(command);
}

void D3D11Renderer::push_transparent_command(RenderObject3DCommand&& command) {
    render_transparent_objects_commands.push_back(std::move(command));
}

void D3D11Renderer::push_hud_command(const RenderHudObjectCommand& command) {
    render_hud_objects_commands.push_back(command);
}

void D3D11Renderer::push_hud_command(RenderHudObjectCommand&& command) {
    render_hud_objects_commands.push_back(std::move(command));
}

void D3D11Renderer::push_debug_gizmo_command(const RenderDebugGizmoCommand& command) {
    render_gizmo_commands.push_back(command);
}

void D3D11Renderer::push_debug_gizmo_command(RenderDebugGizmoCommand&& command) {
    render_gizmo_commands.push_back(std::move(command));
}

void D3D11Renderer::push_terrain_command(const RenderTerrainCommand& command) {
    render_terrain_commands.push_back(command);
}

void D3D11Renderer::push_terrain_command(RenderTerrainCommand&& command) {
    render_terrain_commands.push_back(std::move(command));
}

void D3D11Renderer::setup_material(Materiel& material, Vec3 object_position) {
    using namespace DirectX;
    auto light_settings = lights_.getLightsParams(object_position);
    material.load_params(light_settings.first);
    material.load_params(light_settings.second);
}

void D3D11Renderer::setup_material(MaterielTerrain& material, Vec3 object_position) {
  using namespace DirectX;
  auto light_settings = lights_.getLightsParams(object_position);
  material.load_params(light_settings.first);
  material.load_params(light_settings.second);
}

void D3D11Renderer::sort_render_commands(const Camera& active_camera) {
    // Sort commands in parallel
    auto sort_opaques = std::async(std::launch::async, [this](const Camera& active_camera) {
        sort_opaque_commands(active_camera);
    }, std::cref(active_camera));
    
    auto sort_transparents = std::async(std::launch::async, [this](const Camera& active_camera) {
        sort_transparent_commands(active_camera);
    }, std::cref(active_camera));
   
    auto sort_huds = std::async(std::launch::async, [this]() {
        sort_hud_commands();
    });

    sort_opaques.wait();
    sort_transparents.wait();
    sort_huds.wait();
}

void D3D11Renderer::sort_opaque_commands(const Camera& active_camera) {
    using namespace DirectX;
    std::sort(render_object_commands.begin(), render_object_commands.end(), [&active_camera](const RenderObject3DCommand& a, const RenderObject3DCommand& b) {
        if(a.sort_index < b.sort_index) return true;
        if(b.sort_index < a.sort_index) return false;

        const XMVECTOR translation_row_a = {a.model_transform.m[3][0], a.model_transform.m[3][1], a.model_transform.m[3][2], a.model_transform.m[3][3] };
        const XMVECTOR translation_row_b = {b.model_transform.m[3][0], b.model_transform.m[3][1], b.model_transform.m[3][2], b.model_transform.m[3][3] };

        const XMFLOAT3 camera_position = active_camera.getPosition();
        const XMVECTOR camera_position_ = XMLoadFloat3(&camera_position);

        const XMVECTOR diff_a_ = translation_row_a - camera_position_;
        const XMVECTOR diff_b_ = translation_row_b - camera_position_;

        XMFLOAT3 diff_a, diff_b;
        XMStoreFloat3(&diff_a, diff_a_);
        XMStoreFloat3(&diff_b, diff_b_);

        const float squared_distance_a = diff_a.x * diff_a.x + diff_a.y * diff_a.y + diff_a.z * diff_a.z;
        const float squared_distance_b = diff_b.x * diff_b.x + diff_b.y * diff_b.y + diff_b.z * diff_b.z;

        // Le plus près de la caméra
        return squared_distance_a < squared_distance_b;
    });
}

void D3D11Renderer::sort_hud_commands() {
    std::sort(render_hud_objects_commands.begin(), render_hud_objects_commands.end(), [](const RenderHudObjectCommand& a, const RenderHudObjectCommand& b) {
        return a.sort_index < b.sort_index;
    });
}

void D3D11Renderer::sort_transparent_commands(const Camera& active_camera) {
    using namespace DirectX;
    std::sort(render_transparent_objects_commands.begin(), render_transparent_objects_commands.end(), [&active_camera](const RenderObject3DCommand& a, const RenderObject3DCommand& b) {
        if(a.sort_index < b.sort_index) return true;
        if(b.sort_index < a.sort_index) return false;

        const XMVECTOR translation_row_a = {a.model_transform.m[3][0], a.model_transform.m[3][1], a.model_transform.m[3][2], a.model_transform.m[3][3] };
        const XMVECTOR translation_row_b = {b.model_transform.m[3][0], b.model_transform.m[3][1], b.model_transform.m[3][2], b.model_transform.m[3][3] };

        const XMFLOAT3 camera_position = active_camera.getPosition();
        const XMVECTOR camera_position_ = XMLoadFloat3(&camera_position);

        const XMVECTOR diff_a_ = translation_row_a - camera_position_;
        const XMVECTOR diff_b_ = translation_row_b - camera_position_;

        XMFLOAT3 diff_a, diff_b;
        XMStoreFloat3(&diff_a, diff_a_);
        XMStoreFloat3(&diff_b, diff_b_);

        const float squared_distance_a = diff_a.x * diff_a.x + diff_a.y * diff_a.y + diff_a.z * diff_a.z;
        const float squared_distance_b = diff_b.x * diff_b.x + diff_b.y * diff_b.y + diff_b.z * diff_b.z;

        // Le plus loin de la caméra
        return squared_distance_a > squared_distance_b;
    });
}

void D3D11Renderer::render_terrain(const Camera& active_camera, ID3D11DeviceContext* pImmediateContext) {
  using namespace DirectX;

  for (RenderTerrainCommand& render_object : render_terrain_commands) {
    setup_material(*render_object.materiel_terrain, Vec3{ 0.0f,0.0f,0.0f });

    // Setup transforms
    Shader::ShadersWorldParams wp;
    wp.matWorld = XMMatrixTranspose(render_object.model_transform);

    wp.matWorldViewProj = XMMatrixTranspose(render_object.model_transform * active_camera.view() * active_camera.projection());
    wp.vAWorld = Couleur(0.2f, 0.2f, 0.2f, 1.0f);
    wp.vCamera = XMVectorSet(active_camera.getPosition().x,
      active_camera.getPosition().y,
      active_camera.getPosition().z, 1.0f);
    render_object.materiel_terrain->load_params(wp);

    pImmediateContext->IASetInputLayout(render_object.materiel_terrain->shader->pVertexLayout);

    render_object.materiel_terrain->apply();
    render_object.object->Draw();
  }
}

void D3D11Renderer::render_opaque_objects(const Camera& active_camera, ID3D11DeviceContext* pImmediateContext) {
    using namespace DirectX;

    for (RenderObject3DCommand& render_object : render_object_commands) {
        if(!render_object.material->is_visible) continue;
        setup_material(*render_object.material,Vec3{0.0f,0.0f,0.0f});

        // Setup transforms
        Shader::ShadersWorldParams wp;
        wp.matWorld = XMMatrixTranspose(render_object.model_transform);

        wp.matWorldViewProj = XMMatrixTranspose(render_object.model_transform * active_camera.view() * active_camera.projection());
        wp.vAWorld = Couleur(0.2f,0.2f,0.2f,1.0f);
        wp.vCamera = XMVectorSet(active_camera.getPosition().x, 
                                    active_camera.getPosition().y, 
                                    active_camera.getPosition().z, 1.0f);
        render_object.material->load_params(wp);

        pImmediateContext->IASetInputLayout(render_object.material->shader->pVertexLayout);

        render_object.material->apply();
        render_object.object->Draw();
    }
}

void D3D11Renderer::render_transparent_objects(const Camera& active_camera, ID3D11DeviceContext* pImmediateContext) {
    using namespace DirectX;

    for (RenderObject3DCommand& render_object : render_transparent_objects_commands) {
        if(!render_object.material->is_visible) continue;
        setup_material(*render_object.material, Vec3{0.0f,0.0f,0.0f});

        // Setup transforms
        Shader::ShadersWorldParams wp;
        wp.matWorld = XMMatrixTranspose(render_object.model_transform);

        wp.matWorldViewProj = XMMatrixTranspose(render_object.model_transform * active_camera.view() * active_camera.projection());
        wp.vAWorld = Couleur(0.2f,0.2f,0.2f,1.0f);
        wp.vCamera = XMVectorSet(active_camera.getPosition().x, 
                                    active_camera.getPosition().y, 
                                    active_camera.getPosition().z, 1.0f);
        render_object.material->load_params(wp);

        pImmediateContext->IASetInputLayout(render_object.material->shader->pVertexLayout);

        render_object.material->apply();
        render_object.object->Draw();
    }
}

void D3D11Renderer::render_hud_objects(ID3D11DeviceContext* /*pImmediateContext*/) {
    dispositif.disable_depth_testing();

    for (RenderHudObjectCommand& command : render_hud_objects_commands) {
        hud_effect.apply_pass(0, command.texture_resource_view);
        command.billboard->Draw();
    }

    dispositif.enable_depth_testing();
}

void D3D11Renderer::render_gizmos(ID3D11DeviceContext* ctx) {
    dispositif.disable_depth_testing();

    // Draw gizmo instance
    std::vector<Mat4x4> transforms;
    transforms.reserve(render_gizmo_commands.size());
    std::transform(render_gizmo_commands.begin(), render_gizmo_commands.end(), std::back_inserter(transforms), [](const RenderDebugGizmoCommand& command) {
        return command.model_transform;
    });

    gizmo_renderer.render(ctx, std::move(transforms));

    dispositif.enable_depth_testing();
}

bool D3D11Renderer::render_frame_on_texture() {
    using namespace DirectX;

    if (!cameras().has_active()) {
        log_warning("d3d11renderer", "skip rendering frame because there is no active camera");
        return false;
    }

    const Camera& active_camera = cameras().rendering_camera();

    sort_render_commands(active_camera);

    // Active le rendu dans une texture
    texture_rendertarget.use_as_target();
    texture_rendertarget.clear();

    ID3D11DeviceContext* pImmediateContext = dispositif.GetImmediateContext();

    dispositif.disable_alpha_blending();
    render_opaque_objects(active_camera, pImmediateContext);
    render_terrain(active_camera, pImmediateContext);

    dispositif.enable_alpha_blending();
    render_transparent_objects(active_camera, pImmediateContext);

    ID3D11ShaderResourceView* null_resource_view[] = {nullptr, nullptr};
    dispositif.GetImmediateContext()->PSSetShaderResources(0, 2, null_resource_view);

    return true;
}

void D3D11Renderer::apply_post_fx_passes() {
    ID3D11ShaderResourceView* rendered_frame_texture = texture_rendertarget.shader_resource_view();

    for(std::size_t post_fx_index = 0; post_fx_index < post_fxs.size(); ++post_fx_index) {
        PostFX* post_fx_ptr = post_fxs[post_fx_index].get();

        post_fx_ptr->pre_pass(PrePassContext{screen_billboard, 
                                             rendered_frame_texture, 
                                             multipasses_effect_rendertarget, 
                                             dispositif.GetImmediateContext()});
        post_fx_ptr->render_on_billboard(screen_billboard, 
                                         rendered_frame_texture, 
                                         dispositif.GetImmediateContext(), 
                                         multipasses_effect_rendertarget);

        // Swap both render targets
        if (post_fx_ptr->pass_count() % 2 == 1) {
            multipasses_effect_rendertarget[0].swap(multipasses_effect_rendertarget[1]);
        }
        
        rendered_frame_texture = multipasses_effect_rendertarget[1].shader_resource_view();
    }
}

void D3D11Renderer::render_texture_to_screen() {
    apply_post_fx_passes();

    dispositif.ResetTargetView();

    ID3D11DeviceContext* ctx = dispositif.GetImmediateContext();
    ctx->ClearDepthStencilView(dispositif.GetDepthStencilView(), D3D11_CLEAR_DEPTH, 1.0f, 0);

    ID3D11ShaderResourceView* resource_view_with_postfx = multipasses_effect_rendertarget[1].shader_resource_view();

    null_effect->apply_pass(0, resource_view_with_postfx);
    screen_billboard.Draw();

    render_hud_objects(ctx);

    ID3D11ShaderResourceView* null_resource_view[] = { nullptr, nullptr };
    dispositif.GetImmediateContext()->PSSetShaderResources(0, 2, null_resource_view);
}

void D3D11Renderer::clear_render_commands() {
    render_object_commands.clear();
    render_transparent_objects_commands.clear();
    render_hud_objects_commands.clear();
    render_gizmo_commands.clear();
    render_terrain_commands.clear();
}

void D3D11Renderer::render_frame() {
    if(render_frame_on_texture()) {
        render_texture_to_screen();
    }
   
    clear_render_commands();
}

void D3D11Renderer::apply_post_fx(std::shared_ptr<PostFX> pfx) {
    apply_post_fxs(std::vector<std::shared_ptr<PostFX>>{pfx});
}

void D3D11Renderer::apply_post_fxs(std::vector<std::shared_ptr<PostFX>> pfxs) {
    post_fxs = pfxs;
}

void D3D11Renderer::reset_post_fx() {
    apply_post_fxs(std::vector<std::shared_ptr<PostFX>>{null_effect});
}

void D3D11Renderer::on_screen_sized_changed(unsigned int new_width, unsigned int new_height) {
    texture_rendertarget.resize(new_width, new_height);
    for (int i = 0; i < 2; ++i) {
        multipasses_effect_rendertarget[i].resize(new_width, new_height);
    }
}