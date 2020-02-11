#include "SceneLoader.h"

#include <vector>
#include <random>
#include "SceneLoaderContext.h"

bool init_circuit(SceneLoaderContext* ctx) {
  EntityFactory* factory = ctx->entity_factory_api.factory;

  auto mobile_cube1 = ctx->entity_factory_api.make_dynamic_decoration(factory, "cube", "bloc_color_change", Vec3{ 100.f,100.f,40.f }, Vec3{}, Vec3{ 10.f,10.f,6.f }, 0.01f);
  ctx->scene_api.add_entity_in_zone(ctx->scene_api.scene, mobile_cube1, 0);
  auto mobile_cube2 = ctx->entity_factory_api.make_dynamic_decoration(factory, "cube", "bloc_color_change", Vec3{ 100.f,200.f,40.f }, Vec3{}, Vec3{ 10.f,10.f,3.f }, 0.01f);
  ctx->scene_api.add_entity_in_zone(ctx->scene_api.scene, mobile_cube2, 0);


  /********************************************************************************************************************************************************************
  *                                                                       Limites                                                                            *
  ********************************************************************************************************************************************************************/
  auto limit_wall1 = ctx->entity_factory_api.make_collidable_decoration(factory, "limit", "limit", Vec3{ 0.f, 510.f, 0.f }, Vec3{0.f,0.f,DirectX::XM_PI}, Vec3{ 1.f,50.f,50.f });
  ctx->scene_api.add_entity_in_zone(ctx->scene_api.scene, limit_wall1, 1);
  ctx->scene_api.add_entity_in_zone(ctx->scene_api.scene, limit_wall1, 3);
  auto limit_wall2 = ctx->entity_factory_api.make_collidable_decoration(factory, "limit", "limit", Vec3{ 510.f, 0.f, 0.f }, Vec3{ 0.0f,0.f,-DirectX::XM_PIDIV2 }, Vec3{ 1.f,50.f,50.f });
  ctx->scene_api.add_entity_in_zone(ctx->scene_api.scene, limit_wall2, 1);
  ctx->scene_api.add_entity_in_zone(ctx->scene_api.scene, limit_wall2, 2);
  auto limit_wall3 = ctx->entity_factory_api.make_collidable_decoration(factory, "limit", "limit", Vec3{ 510.f, 1020.f, 0.f }, Vec3{ 0.0f,0.f,DirectX::XM_PIDIV2 }, Vec3{ 1.f,50.f,50.f });
  ctx->scene_api.add_entity_in_zone(ctx->scene_api.scene, limit_wall3, 3);
  ctx->scene_api.add_entity_in_zone(ctx->scene_api.scene, limit_wall3, 4);
  auto limit_wall4 = ctx->entity_factory_api.make_collidable_decoration(factory, "limit", "limit", Vec3{ 1020.f, 510.f, 0.f }, Vec3{ 0.0f,0.f,0.0f }, Vec3{ 1.f,50.f,50.f });
  ctx->scene_api.add_entity_in_zone(ctx->scene_api.scene, limit_wall4, 2);
  ctx->scene_api.add_entity_in_zone(ctx->scene_api.scene, limit_wall4, 4);
  auto limit_ceiling = ctx->entity_factory_api.make_collidable_decoration(factory, "limit", "limit", Vec3{ 510.f, 510.f, 200.f }, Vec3{ 0.0f,-DirectX::XM_PIDIV2,0.0f }, Vec3{ 1.f,50.f,50.f });
  ctx->scene_api.add_entity_in_zone(ctx->scene_api.scene, limit_ceiling, 0);

  
  /********************************************************************************************************************************************************************
  *                                                                       ZONE 1                                                                           *
  ********************************************************************************************************************************************************************/
  float z = 100.f;
  
  auto mur1_z1 = ctx->entity_factory_api.make_collidable_decoration(factory, "cube", "bloc_nograp", Vec3{ 256.f, 532.f - 31.f, 28.f }, Vec3{}, Vec3{ 51.f, 51.f, 10.f });
  ctx->scene_api.add_entity_in_zone(ctx->scene_api.scene, mur1_z1, 1);
  ctx->scene_api.add_entity_in_zone(ctx->scene_api.scene, mur1_z1, 3);

  auto mur2_z1 = ctx->entity_factory_api.make_grapping_box(factory, "cube", "bloc_grap_z1", Vec3{ 510.f, 197.5f, z }, Vec3{}, Vec3{ 20.f, 42.5f, 100.f });
  ctx->scene_api.add_entity_in_zone(ctx->scene_api.scene, mur2_z1, 1);
  ctx->scene_api.add_entity_in_zone(ctx->scene_api.scene, mur2_z1, 2);
  auto mur3_z1 = ctx->entity_factory_api.make_grapping_box(factory, "cube", "bloc_grap_z1", Vec3{ 510.f, 47.5f, z }, Vec3{}, Vec3{ 20.f, 47.5f, 100.f });
  ctx->scene_api.add_entity_in_zone(ctx->scene_api.scene, mur3_z1, 1);
  ctx->scene_api.add_entity_in_zone(ctx->scene_api.scene, mur3_z1, 2);
  auto mur4_z1 = ctx->entity_factory_api.make_grapping_box(factory, "cube", "bloc_grap_z1", Vec3{ 510.f, 125.f, z + 25.f }, Vec3{}, Vec3{ 20.f, 30.f, 75.f });
  ctx->scene_api.add_entity_in_zone(ctx->scene_api.scene, mur4_z1, 1);
  ctx->scene_api.add_entity_in_zone(ctx->scene_api.scene, mur4_z1, 2);

  auto mur5_z1 = ctx->entity_factory_api.make_grapping_box(factory, "cube", "bloc_grap_z1", Vec3{ 500.f, 376.f, 100.f }, Vec3{}, Vec3{ 10.f, 136.f, 100.f });
  ctx->scene_api.add_entity_in_zone(ctx->scene_api.scene, mur5_z1, 1);

  auto e_building1 = ctx->entity_factory_api.make_building(factory, "building_part", "building", "cube", "building_roof", Vec3{ 200.f, 100.f, 20.f }, Vec3{}, Vec3{ 10.f, 10.f, 5.f }, Vec3{ 10.f, 10.f, 0.0001f }, 2,10.0f);
  ctx->scene_api.add_entity_in_zone(ctx->scene_api.scene, e_building1, 1);
  auto e_building2 = ctx->entity_factory_api.make_building(factory, "building_part", "building", "cube", "building_roof", Vec3{ 220.f, 100.f, 20.f }, Vec3{}, Vec3{ 10.f, 10.f, 5.f }, Vec3{ 10.f, 10.f, 0.0001f }, 5, 10.0f);
  ctx->scene_api.add_entity_in_zone(ctx->scene_api.scene, e_building2, 1);
  auto e_building3 = ctx->entity_factory_api.make_building(factory, "building_part", "building", "cube", "building_roof", Vec3{ 240.f, 100.f, 20.f }, Vec3{}, Vec3{ 10.f, 10.f, 5.f }, Vec3{ 10.f, 10.f, 0.0001f }, 3, 10.0f);
  ctx->scene_api.add_entity_in_zone(ctx->scene_api.scene, e_building3, 1);

  auto e_building4 = ctx->entity_factory_api.make_building(factory, "building_part", "building", "cube", "building_roof", Vec3{ 300.f, 300.f, 20.f }, Vec3{}, Vec3{ 10.f, 10.f, 5.f }, Vec3{ 10.f, 10.f, 0.0001f }, 3, 10.0f);
  ctx->scene_api.add_entity_in_zone(ctx->scene_api.scene, e_building4, 1);
  auto e_building5 = ctx->entity_factory_api.make_building(factory, "building_part", "building", "cube", "building_roof", Vec3{ 300.f, 320.f, 20.f }, Vec3{}, Vec3{ 10.f, 10.f, 5.f }, Vec3{ 10.f, 10.f, 0.0001f }, 5, 10.0f);
  ctx->scene_api.add_entity_in_zone(ctx->scene_api.scene, e_building5, 1);
  auto e_building6 = ctx->entity_factory_api.make_building(factory, "building_part", "building", "cube", "building_roof", Vec3{ 300.f, 340.f, 20.f }, Vec3{}, Vec3{ 10.f, 10.f, 5.f }, Vec3{ 10.f, 10.f, 0.0001f }, 3, 10.0f);
  ctx->scene_api.add_entity_in_zone(ctx->scene_api.scene, e_building6, 1);

  auto e_building7 = ctx->entity_factory_api.make_building(factory, "building_part", "building", "cube", "building_roof", Vec3{ 150.f, 200.f, 20.f }, Vec3{}, Vec3{ 10.f, 10.f, 5.f }, Vec3{ 10.f, 10.f, 0.0001f }, 3, 10.0f);
  ctx->scene_api.add_entity_in_zone(ctx->scene_api.scene, e_building7, 1);
  auto e_building8 = ctx->entity_factory_api.make_building(factory, "building_part", "building", "cube", "building_roof", Vec3{ 150.f, 220.f, 20.f }, Vec3{}, Vec3{ 10.f, 10.f, 5.f }, Vec3{ 10.f, 10.f, 0.0001f }, 5, 10.0f);
  ctx->scene_api.add_entity_in_zone(ctx->scene_api.scene, e_building8, 1);
  auto e_building9 = ctx->entity_factory_api.make_building(factory, "building_part", "building", "cube", "building_roof", Vec3{ 170.f, 220.f, 20.f }, Vec3{}, Vec3{ 10.f, 10.f, 5.f }, Vec3{ 10.f, 10.f, 0.0001f }, 4, 10.0f);
  ctx->scene_api.add_entity_in_zone(ctx->scene_api.scene, e_building9, 1);

  auto e_building10 = ctx->entity_factory_api.make_building(factory, "building_part", "building", "cube", "building_roof", Vec3{ 50.f, 400.f, 20.f }, Vec3{}, Vec3{ 10.f, 10.f, 5.f }, Vec3{ 10.f, 10.f, 0.0001f }, 3, 10.0f);
  ctx->scene_api.add_entity_in_zone(ctx->scene_api.scene, e_building10, 1);
  auto e_building11 = ctx->entity_factory_api.make_building(factory, "building_part", "building", "cube", "building_roof", Vec3{ 50.f, 380.f, 20.f }, Vec3{}, Vec3{ 10.f, 10.f, 5.f }, Vec3{ 10.f, 10.f, 0.0001f }, 5, 10.0f);
  ctx->scene_api.add_entity_in_zone(ctx->scene_api.scene, e_building11, 1);
  auto e_building12 = ctx->entity_factory_api.make_building(factory, "building_part", "building", "cube", "building_roof", Vec3{ 50.f, 360.f, 20.f }, Vec3{}, Vec3{ 10.f, 10.f, 5.f }, Vec3{ 10.f, 10.f, 0.0001f }, 4, 10.0f);
  ctx->scene_api.add_entity_in_zone(ctx->scene_api.scene, e_building12, 1);


  auto e_bridge = ctx->entity_factory_api.make_bridge(factory, "wall", "bloc_grap_z1", Vec3{ 510.f, 125.f, 20.f }, Vec3{ 0.f, 0.f, DirectX::XM_PIDIV2 + 0.001f}, Vec3{ 5.f, 10.f, 5.f });
  ctx->scene_api.add_entity_in_zone(ctx->scene_api.scene, e_bridge, 1);
  ctx->scene_api.add_entity_in_zone(ctx->scene_api.scene, e_bridge, 2);

  auto billboard_npc_zone_1_1 = ctx->entity_factory_api.make_billboard_decoration(factory, 3, 4, "npc_material", Vec3{ 400.f, 50.f, 22.f }, BillboardLinearOrientation::OnlyZ, Vec3{ 1.f, 1.f, 1.f });
  auto billboard_npc_zone_1_2 = ctx->entity_factory_api.make_billboard_decoration(factory, 3, 4, "npc_material", Vec3{ 200.f, 150.f, 22.f }, BillboardLinearOrientation::OnlyZ, Vec3{ 1.f, 1.f, 1.f });
  auto billboard_npc_zone_1_3 = ctx->entity_factory_api.make_billboard_decoration(factory, 3, 4, "npc_material", Vec3{ 200.f, 350.f, 22.f }, BillboardLinearOrientation::OnlyZ, Vec3{ 1.f, 1.f, 1.f });
  auto billboard_npc_zone_1_4 = ctx->entity_factory_api.make_billboard_decoration(factory, 3, 4, "npc_material", Vec3{ 400.f, 400.f, 22.f }, BillboardLinearOrientation::OnlyZ, Vec3{ 1.f, 1.f, 1.f });
  auto billboard_npc_zone_1_5 = ctx->entity_factory_api.make_billboard_decoration(factory, 3, 4, "npc_material", Vec3{ 300.f, 100.f, 22.f }, BillboardLinearOrientation::OnlyZ, Vec3{ 1.f, 1.f, 1.f });
  auto billboard_npc_zone_1_6 = ctx->entity_factory_api.make_billboard_decoration(factory, 3, 4, "npc_material", Vec3{ 250.f, 230.f, 22.f }, BillboardLinearOrientation::OnlyZ, Vec3{ 1.f, 1.f, 1.f });
  auto billboard_npc_zone_1_7 = ctx->entity_factory_api.make_billboard_decoration(factory, 3, 4, "npc_material", Vec3{ 350.f, 250.f, 22.f }, BillboardLinearOrientation::OnlyZ, Vec3{ 1.f, 1.f, 1.f });
  auto billboard_npc_zone_1_8 = ctx->entity_factory_api.make_billboard_decoration(factory, 3, 4, "npc_material", Vec3{ 150.f, 300.f, 22.f }, BillboardLinearOrientation::OnlyZ, Vec3{ 1.f, 1.f, 1.f });
  auto billboard_npc_zone_1_9 = ctx->entity_factory_api.make_billboard_decoration(factory, 3, 4, "npc_material", Vec3{ 100.f, 200.f, 22.f }, BillboardLinearOrientation::OnlyZ, Vec3{ 1.f, 1.f, 1.f });
  auto billboard_npc_zone_1_10 = ctx->entity_factory_api.make_billboard_decoration(factory, 3, 4, "npc_material", Vec3{ 300.f, 310.f, 22.f }, BillboardLinearOrientation::OnlyZ, Vec3{ 1.f, 1.f, 1.f });
  auto billboard_npc_zone_1_11 = ctx->entity_factory_api.make_billboard_decoration(factory, 3, 4, "npc_material", Vec3{ 250.f, 250.f, 22.f }, BillboardLinearOrientation::OnlyZ, Vec3{ 1.f, 1.f, 1.f });

  auto chaise_zone_1_1 = ctx->entity_factory_api.make_collidable_decoration(factory, "chair", "bois_fonce", Vec3{ 440.f, 35.f, 20.f }, Vec3{ 0.f , DirectX::XM_PI ,  DirectX::XM_PI  }, Vec3{ 1.f, 1.f, 1.f });
  auto chaise_zone_1_2 = ctx->entity_factory_api.make_collidable_decoration(factory, "chair", "bois_fonce", Vec3{ 440.f, 40.f, 20.f }, Vec3{ 0.f , DirectX::XM_PI ,  DirectX::XM_PI  }, Vec3{ 1.f, 1.f, 1.f });
  auto chaise_zone_1_3 = ctx->entity_factory_api.make_collidable_decoration(factory, "chair", "bois_fonce", Vec3{ 435.f, 35.f, 20.f }, Vec3{ 0.f , DirectX::XM_PI ,  0  }, Vec3{ 1.f, 1.f, 1.f });
  auto chaise_zone_1_4 = ctx->entity_factory_api.make_collidable_decoration(factory, "chair", "bois_fonce", Vec3{ 435.f, 40.f, 20.f }, Vec3{ 0.f , DirectX::XM_PI ,  0  }, Vec3{ 1.f, 1.f, 1.f });

  auto table_zone_1_1 = ctx->entity_factory_api.make_collidable_decoration(factory, "table", "bloc_grap_z2", Vec3{ 437.f, 37.f, 20.f }, Vec3{ 0.f , DirectX::XM_PI ,  DirectX::XM_PI  }, Vec3{ 2.f, 3.f, 2.f });

  auto barriere_zone_1_1 = ctx->entity_factory_api.make_collidable_decoration(factory, "fence", "bois_fonce", Vec3{ 450.f, 60.f, 20.f }, Vec3{ 0.f , DirectX::XM_PI ,  DirectX::XM_PI  }, Vec3{ 2.f, 2.f, 2.f });
  auto barriere_zone_1_2 = ctx->entity_factory_api.make_collidable_decoration(factory, "fence", "bois_fonce", Vec3{ 450.f, 64.f, 20.f }, Vec3{ 0.f , DirectX::XM_PI ,  DirectX::XM_PI  }, Vec3{ 2.f, 2.f, 2.f });
  auto barriere_zone_1_3 = ctx->entity_factory_api.make_collidable_decoration(factory, "fence", "bois_fonce", Vec3{ 450.f, 68.f, 20.f }, Vec3{ 0.f , DirectX::XM_PI ,  DirectX::XM_PI  }, Vec3{ 2.f, 2.f, 2.f });
  auto barriere_zone_1_4 = ctx->entity_factory_api.make_collidable_decoration(factory, "fence", "bois_fonce", Vec3{ 450.f, 76.f, 21.f }, Vec3{ 0.f , DirectX::XM_PI ,  DirectX::XM_PI  }, Vec3{ 2.f, 2.f, 2.f });
  auto barriere_zone_1_5 = ctx->entity_factory_api.make_collidable_decoration(factory, "fence", "bois_fonce", Vec3{ 450.f, 80.f, 21.f }, Vec3{ 0.f , DirectX::XM_PI ,  DirectX::XM_PI  }, Vec3{ 2.f, 2.f, 2.f });
  
  auto barriere_zone_1_6 = ctx->entity_factory_api.make_collidable_decoration(factory, "fence", "bois_fonce", Vec3{ 446.f, 60.f, 20.f }, Vec3{ 0.f , DirectX::XM_PI ,  DirectX::XM_PI/2 }, Vec3{ 2.f, 2.f, 2.f });
  auto barriere_zone_1_7 = ctx->entity_factory_api.make_collidable_decoration(factory, "fence", "bois_fonce", Vec3{ 442.f, 60.f, 20.f }, Vec3{ 0.f , DirectX::XM_PI ,  DirectX::XM_PI / 2 }, Vec3{ 2.f, 2.f, 2.f });
  auto barriere_zone_1_8 = ctx->entity_factory_api.make_collidable_decoration(factory, "fence", "bois_fonce", Vec3{ 438.f, 60.f, 20.f }, Vec3{ 0.f , DirectX::XM_PI ,  DirectX::XM_PI / 2 }, Vec3{ 2.f, 2.f, 2.f });
  
  auto barriere_zone_1_9 = ctx->entity_factory_api.make_collidable_decoration(factory, "fence", "bois_fonce", Vec3{ 446.f, 84.f, 21.f }, Vec3{ 0.f , DirectX::XM_PI ,  DirectX::XM_PI / 2 }, Vec3{ 2.f, 2.f, 2.f });
  auto barriere_zone_1_10 = ctx->entity_factory_api.make_collidable_decoration(factory, "fence", "bois_fonce", Vec3{ 442.f, 84.f, 21.f }, Vec3{ 0.f , DirectX::XM_PI ,  DirectX::XM_PI / 2 }, Vec3{ 2.f, 2.f, 2.f });
  auto barriere_zone_1_11 = ctx->entity_factory_api.make_collidable_decoration(factory, "fence", "bois_fonce", Vec3{ 438.f, 84.f, 21.f }, Vec3{ 0.f , DirectX::XM_PI ,  DirectX::XM_PI / 2 }, Vec3{ 2.f, 2.f, 2.f });

  ctx->scene_api.add_entity_in_zone(ctx->scene_api.scene, billboard_npc_zone_1_1, 1);
  ctx->scene_api.add_entity_in_zone(ctx->scene_api.scene, billboard_npc_zone_1_2, 1);
  ctx->scene_api.add_entity_in_zone(ctx->scene_api.scene, billboard_npc_zone_1_3, 1);
  ctx->scene_api.add_entity_in_zone(ctx->scene_api.scene, billboard_npc_zone_1_4, 1);
  ctx->scene_api.add_entity_in_zone(ctx->scene_api.scene, billboard_npc_zone_1_5, 1);
  ctx->scene_api.add_entity_in_zone(ctx->scene_api.scene, billboard_npc_zone_1_6, 1);
  ctx->scene_api.add_entity_in_zone(ctx->scene_api.scene, billboard_npc_zone_1_7, 1);
  ctx->scene_api.add_entity_in_zone(ctx->scene_api.scene, billboard_npc_zone_1_8, 1);
  ctx->scene_api.add_entity_in_zone(ctx->scene_api.scene, billboard_npc_zone_1_9, 1);
  ctx->scene_api.add_entity_in_zone(ctx->scene_api.scene, billboard_npc_zone_1_10, 1);
  ctx->scene_api.add_entity_in_zone(ctx->scene_api.scene, billboard_npc_zone_1_11, 1);

  ctx->scene_api.add_entity_in_zone(ctx->scene_api.scene, chaise_zone_1_1, 1);
  ctx->scene_api.add_entity_in_zone(ctx->scene_api.scene, chaise_zone_1_2, 1);
  ctx->scene_api.add_entity_in_zone(ctx->scene_api.scene, chaise_zone_1_3, 1);
  ctx->scene_api.add_entity_in_zone(ctx->scene_api.scene, chaise_zone_1_4, 1);

  ctx->scene_api.add_entity_in_zone(ctx->scene_api.scene, table_zone_1_1, 1);

  ctx->scene_api.add_entity_in_zone(ctx->scene_api.scene, barriere_zone_1_1, 1);
  ctx->scene_api.add_entity_in_zone(ctx->scene_api.scene, barriere_zone_1_2, 1);
  ctx->scene_api.add_entity_in_zone(ctx->scene_api.scene, barriere_zone_1_3, 1);
  ctx->scene_api.add_entity_in_zone(ctx->scene_api.scene, barriere_zone_1_4, 1);
  ctx->scene_api.add_entity_in_zone(ctx->scene_api.scene, barriere_zone_1_5, 1);

  ctx->scene_api.add_entity_in_zone(ctx->scene_api.scene, barriere_zone_1_6, 1);
  ctx->scene_api.add_entity_in_zone(ctx->scene_api.scene, barriere_zone_1_7, 1);
  ctx->scene_api.add_entity_in_zone(ctx->scene_api.scene, barriere_zone_1_8, 1);

  ctx->scene_api.add_entity_in_zone(ctx->scene_api.scene, barriere_zone_1_9, 1);
  ctx->scene_api.add_entity_in_zone(ctx->scene_api.scene, barriere_zone_1_10, 1);
  ctx->scene_api.add_entity_in_zone(ctx->scene_api.scene, barriere_zone_1_11, 1);


    /********************************************************************************************************************************************************************
    *                                                                       ZONE 2                                                                                      *
    ********************************************************************************************************************************************************************/

    auto mur_zone_2_1 = ctx->entity_factory_api.make_grapping_box(factory, "cube", "bloc_grap_z2", Vec3{ 530.f, 355.f, 100.f }, Vec3{}, Vec3{ 20.f, 115.f, 100.f });
    auto mur_zone_2_2 = ctx->entity_factory_api.make_grapping_box(factory, "cube", "bloc_grap_z2", Vec3{ 830.f, 360.f, 100.f }, Vec3{}, Vec3{ 195.f, 50.f, 100.f });
    auto mur_zone_2_3 = ctx->entity_factory_api.make_grapping_box(factory, "cube", "bloc_grap_z2", Vec3{ 740.f, 250.f, 100.f }, Vec3{}, Vec3{ 190.f, 10.f, 100.f });
    auto mur4_z2 = ctx->entity_factory_api.make_grapping_box(factory, "cube", "bloc_grap_z2", Vec3{ 702.f, 492.f, z }, Vec3{}, Vec3{ 193.f, 20.f, 100.f }); //OK
    ctx->scene_api.add_entity_in_zone(ctx->scene_api.scene, mur4_z2, 2);

    auto mur5_z2 = ctx->entity_factory_api.make_grapping_box(factory, "cube", "bloc_grap_z2", Vec3{ 750.f, 54.f, z }, Vec3{}, Vec3{ 10.f, 54.f, 100.f }); //OK
    ctx->scene_api.add_entity_in_zone(ctx->scene_api.scene, mur5_z2, 2);
    auto mur6_z2 = ctx->entity_factory_api.make_grapping_box(factory, "cube", "bloc_grap_z2", Vec3{ 752.f, 195.f, z }, Vec3{}, Vec3{ 10.f, 45.f, 100.f }); //OK
    ctx->scene_api.add_entity_in_zone(ctx->scene_api.scene, mur6_z2, 2);


    auto platforme_zone_2_1 = ctx->entity_factory_api.make_grapping_box(factory, "cube", "bloc_grap_z2", Vec3{ 845.f, 285.f, 48.f }, Vec3{}, Vec3{ 20.f, 20.f, 10.f });
    auto platforme_zone_2_2 = ctx->entity_factory_api.make_grapping_box(factory, "cube", "bloc_grap_z2", Vec3{ 770.f, 285.f, 58.f }, Vec3{}, Vec3{ 20.f, 20.f, 10.f });
    auto platforme_zone_2_3 = ctx->entity_factory_api.make_grapping_box(factory, "cube", "bloc_grap_z2", Vec3{ 695.f, 285.f, 68.f }, Vec3{}, Vec3{ 20.f, 20.f, 10.f });
    auto platforme_zone_2_4 = ctx->entity_factory_api.make_grapping_box(factory, "cube", "bloc_grap_z2", Vec3{ 620.f, 285.f, 75.f }, Vec3{}, Vec3{ 20.f, 20.f, 10.f });
    
    auto platforme_zone_2_5 = ctx->entity_factory_api.make_grapping_box(factory, "cube", "bloc_grap_z2", Vec3{ 845.f, 441.f, 60.f }, Vec3{}, Vec3{ 20.f, 20.f, 10.f });
    auto platforme_zone_2_6 = ctx->entity_factory_api.make_grapping_box(factory, "cube", "bloc_grap_z2", Vec3{ 770.f, 441.f, 60.f }, Vec3{}, Vec3{ 20.f, 20.f, 10.f });
    auto platforme_zone_2_7 = ctx->entity_factory_api.make_grapping_box(factory, "cube", "bloc_grap_z2", Vec3{ 695.f, 441.f, 60.f }, Vec3{}, Vec3{ 20.f, 20.f, 10.f });
    auto platforme_zone_2_8 = ctx->entity_factory_api.make_grapping_box(factory, "cube", "bloc_grap_z2", Vec3{ 620.f, 441.f, 60.f }, Vec3{}, Vec3{ 20.f, 20.f, 10.f });
    
    auto platforme_zone_2_9 = ctx->entity_factory_api.make_grapping_box(factory, "cube", "bloc_grap_z2", Vec3{ 750.f, 129.f, 60.f }, Vec3{}, Vec3{ 20.f, 20.f, 10.f });

    auto billboard_arbre_zone_2_1 = ctx->entity_factory_api.make_billboard_decoration(factory, 5, 10,"tree_material", Vec3{ 890.f, 60.f, 27.f }, BillboardLinearOrientation::OnlyZ, Vec3{ 1.f, 1.f, 1.f });
    auto billboard_arbre_zone_2_2 = ctx->entity_factory_api.make_billboard_decoration(factory, 5, 10,"tree_material", Vec3{ 900.f, 50.f, 27.f }, BillboardLinearOrientation::OnlyZ, Vec3{ 1.f, 1.f, 1.f });
    auto billboard_arbre_zone_2_3 = ctx->entity_factory_api.make_billboard_decoration(factory, 5, 10,"tree_material", Vec3{ 900.f, 70.f, 27.f }, BillboardLinearOrientation::OnlyZ, Vec3{ 1.f, 1.f, 1.f });
    auto billboard_arbre_zone_2_4 = ctx->entity_factory_api.make_billboard_decoration(factory, 5, 10,"tree_material", Vec3{ 925.f, 50.f, 27.f }, BillboardLinearOrientation::OnlyZ, Vec3{ 1.f, 1.f, 1.f });
    auto billboard_arbre_zone_2_5 = ctx->entity_factory_api.make_billboard_decoration(factory, 5, 10,"tree_material", Vec3{ 925.f, 70.f, 27.f }, BillboardLinearOrientation::OnlyZ, Vec3{ 1.f, 1.f, 1.f });
    auto billboard_arbre_zone_2_6 = ctx->entity_factory_api.make_billboard_decoration(factory, 5, 10,"tree_material", Vec3{ 915.f, 60.f, 27.f }, BillboardLinearOrientation::OnlyZ, Vec3{ 1.f, 1.f, 1.f });

    auto billboard_npc_zone_2_1 = ctx->entity_factory_api.make_billboard_decoration(factory, 3, 4,"npc_material", Vec3{ 770.f, 100.f, 22.f }, BillboardLinearOrientation::OnlyZ, Vec3{ 1.f, 1.f, 1.f });
    auto billboard_npc_zone_2_2 = ctx->entity_factory_api.make_billboard_decoration(factory, 30, 40,"npc_material", Vec3{ 770.f, 160.f, 52.f }, BillboardLinearOrientation::OnlyZ, Vec3{ 1.f, 1.f, 1.f });
    auto billboard_npc_zone_2_3 = ctx->entity_factory_api.make_billboard_decoration(factory, 30, 40,"npc_material", Vec3{ 850.f, 90.f, 52.f }, BillboardLinearOrientation::OnlyZ, Vec3{ 1.f, 1.f, 1.f });

    Entity murs_collidables_zone_2[400];
    for(int i = 0; i<20;i++)
        for(int j = 0;j<8; j++)
        {
            murs_collidables_zone_2[(i*8)+j] = ctx->entity_factory_api.make_variable_color_dynamic_decoration(factory, "cube", "bloc_color_change", Vec3{ 745.f,105.f + (i+1)*2.05f,21.f+(j*2.2f) }, Vec3{}, Vec3{ 1.2f,1.f,1.f }, 0.1f);
            ctx->scene_api.add_entity_in_zone(ctx->scene_api.scene, murs_collidables_zone_2[(i*8)+j], 0);
        }

    ctx->scene_api.add_entity_in_zone(ctx->scene_api.scene, mur_zone_2_1, 2);
    ctx->scene_api.add_entity_in_zone(ctx->scene_api.scene, mur_zone_2_2, 2);
    ctx->scene_api.add_entity_in_zone(ctx->scene_api.scene, mur_zone_2_3, 2);
    
    ctx->scene_api.add_entity_in_zone(ctx->scene_api.scene, platforme_zone_2_1, 2);
    ctx->scene_api.add_entity_in_zone(ctx->scene_api.scene, platforme_zone_2_2, 2);
    ctx->scene_api.add_entity_in_zone(ctx->scene_api.scene, platforme_zone_2_3, 2);
    ctx->scene_api.add_entity_in_zone(ctx->scene_api.scene, platforme_zone_2_4, 2);
    
    ctx->scene_api.add_entity_in_zone(ctx->scene_api.scene, platforme_zone_2_5, 2);
    ctx->scene_api.add_entity_in_zone(ctx->scene_api.scene, platforme_zone_2_6, 2);
    ctx->scene_api.add_entity_in_zone(ctx->scene_api.scene, platforme_zone_2_7, 2);
    ctx->scene_api.add_entity_in_zone(ctx->scene_api.scene, platforme_zone_2_8, 2);

    ctx->scene_api.add_entity_in_zone(ctx->scene_api.scene, platforme_zone_2_9, 2);

    ctx->scene_api.add_entity_in_zone(ctx->scene_api.scene, billboard_arbre_zone_2_1, 2);
    ctx->scene_api.add_entity_in_zone(ctx->scene_api.scene, billboard_arbre_zone_2_2, 2);
    ctx->scene_api.add_entity_in_zone(ctx->scene_api.scene, billboard_arbre_zone_2_3, 2);
    ctx->scene_api.add_entity_in_zone(ctx->scene_api.scene, billboard_arbre_zone_2_4, 2);
    ctx->scene_api.add_entity_in_zone(ctx->scene_api.scene, billboard_arbre_zone_2_5, 2);
    ctx->scene_api.add_entity_in_zone(ctx->scene_api.scene, billboard_arbre_zone_2_6, 2);

    ctx->scene_api.add_entity_in_zone(ctx->scene_api.scene, billboard_npc_zone_2_1, 2);
    ctx->scene_api.add_entity_in_zone(ctx->scene_api.scene, billboard_npc_zone_2_2, 2);
    ctx->scene_api.add_entity_in_zone(ctx->scene_api.scene, billboard_npc_zone_2_3, 2);

    /********************************************************************************************************************************************************************
    *                                                                       ZONE 3 (Arrivée)                                                                            *
    ********************************************************************************************************************************************************************/
    auto mur1_z3 = ctx->entity_factory_api.make_collidable_decoration(factory, "cube", "bloc_nograp", Vec3{ 102.f, 522.f, z }, Vec3{}, Vec3{ 102.f, 10.f, 100.f });
    ctx->scene_api.add_entity_in_zone(ctx->scene_api.scene, mur1_z3, 3);
    auto mur2_z3 = ctx->entity_factory_api.make_collidable_decoration(factory, "cube", "bloc_nograp", Vec3{ 409.f, 522.f, z }, Vec3{}, Vec3{ 102.f, 10.f, 100.f });
    ctx->scene_api.add_entity_in_zone(ctx->scene_api.scene, mur2_z3, 3);
    auto mur3_z3 = ctx->entity_factory_api.make_grapping_box(factory, "cube", "bloc_grap_z3", Vec3{ 309.f, 827.f, z }, Vec3{}, Vec3{ 202.f, 20.f, 100.f });
    ctx->scene_api.add_entity_in_zone(ctx->scene_api.scene, mur3_z3, 3);
    auto mur4_z3 = ctx->entity_factory_api.make_grapping_box(factory, "cube", "bloc_grap_z3", Vec3{ 182.f, 632.f, z }, Vec3{}, Vec3{ 20.f, 80.f, 100.f });
    ctx->scene_api.add_entity_in_zone(ctx->scene_api.scene, mur4_z3, 3);
    auto mur5_z3 = ctx->entity_factory_api.make_grapping_box(factory, "cube", "bloc_grap_z3", Vec3{ 302.f, 692.f, z }, Vec3{}, Vec3{ 100.f, 20.f, 100.f });
    ctx->scene_api.add_entity_in_zone(ctx->scene_api.scene, mur5_z3, 3);

    auto mur8_z3 = ctx->entity_factory_api.make_grapping_box(factory, "cube", "bloc_grap_z3", Vec3{ 112.f, 692.f, z }, Vec3{}, Vec3{ 50.f, 20.f, 100.f });
    ctx->scene_api.add_entity_in_zone(ctx->scene_api.scene, mur8_z3, 3);

    auto mur6_z3 = ctx->entity_factory_api.make_grapping_box(factory, "cube", "bloc_grap_z3", Vec3{ 102.f, 542.f, z }, Vec3{}, Vec3{ 102.f, 10.f, 100.f });
    ctx->scene_api.add_entity_in_zone(ctx->scene_api.scene, mur6_z3, 3);
    auto mur7_z3 = ctx->entity_factory_api.make_grapping_box(factory, "cube", "bloc_grap_z3", Vec3{ 409.f, 542.f, z }, Vec3{}, Vec3{ 102.f, 10.f, 100.f });
    ctx->scene_api.add_entity_in_zone(ctx->scene_api.scene, mur7_z3, 3);

    auto tube1_z3 = ctx->entity_factory_api.make_grapping_box(factory, "cube", "bloc_grap_z3", Vec3{ 360.f, 760.f, 40.f }, Vec3{}, Vec3{ 4.f, 48.f, 4.f });
    ctx->scene_api.add_entity_in_zone(ctx->scene_api.scene, tube1_z3, 3);
    auto tube2_z3 = ctx->entity_factory_api.make_grapping_box(factory, "cube", "bloc_grap_z3", Vec3{ 200.f, 760.f, 60.f }, Vec3{}, Vec3{ 4.f, 48.f, 4.f });
    ctx->scene_api.add_entity_in_zone(ctx->scene_api.scene, tube2_z3, 3);

    auto plaque1_z3 = ctx->entity_factory_api.make_grapping_box(factory, "cube", "bloc_grap_z3", Vec3{ 250.f, 928.5f, 60.0f }, Vec3{}, Vec3{ 15.f, 15.f, 4.f }); // OK
    ctx->scene_api.add_entity_in_zone(ctx->scene_api.scene, plaque1_z3, 3);
    auto plaque3_z3 = ctx->entity_factory_api.make_grapping_box(factory, "cube", "bloc_grap_z3", Vec3{ 350.f, 928.5f, 80.0f }, Vec3{}, Vec3{ 15.f, 15.f, 4.f }); //OK
    ctx->scene_api.add_entity_in_zone(ctx->scene_api.scene, plaque3_z3, 3);

    auto KIKI = ctx->entity_factory_api.make_collidable_decoration(factory, "kiki", "_IMPORTED_", Vec3{ 102.f, 612.f, 72.f }, Vec3{ 0.f , DirectX::XM_PI / 2,  DirectX::XM_PI / 2 }, Vec3{ 10.f, 10.f, 10.f });
    ctx->scene_api.add_entity_in_zone(ctx->scene_api.scene, KIKI, 3);

    auto e_goal = ctx->entity_factory_api.make_goal(factory, Vec3{5.f, 5.f, 5.f}, Vec3{ 495.f, 947.f, -27.f }, Vec3{0.f, 0.f, 0.f});
    ctx->scene_api.add_entity_in_zone(ctx->scene_api.scene, e_goal, 3);

	auto pilier_de_lumiere = ctx->entity_factory_api.make_billboard_decoration(factory,10.f,10000.f, "pilier_materiel", Vec3{ 495.f, 947.f, -27.f }, BillboardLinearOrientation::OnlyZ, Vec3{ 0.5f, 1.f, 1.f });
	ctx->scene_api.add_entity_in_zone(ctx->scene_api.scene, pilier_de_lumiere, 3);


    /********************************************************************************************************************************************************************
    *                                                                       ZONE 4 (Départ)                                                                                      *
    ********************************************************************************************************************************************************************/


    auto mur1_z4 = ctx->entity_factory_api.make_grapping_box(factory, "cube", "bloc_grap_z4", Vec3{ 702.f, 532.f, z }, Vec3{}, Vec3{ 193.f, 20.f, 100.f }); //OK
    ctx->scene_api.add_entity_in_zone(ctx->scene_api.scene, mur1_z4, 4);
    auto mur2_z4 = ctx->entity_factory_api.make_grapping_box(factory, "cube", "bloc_grap_z4", Vec3{ 875.f, 722.f, z }, Vec3{}, Vec3{ 20.f, 172.f, 100.f }); //OK
    ctx->scene_api.add_entity_in_zone(ctx->scene_api.scene, mur2_z4, 4);
    auto mur3_z4 = ctx->entity_factory_api.make_grapping_box(factory, "cube", "bloc_grap_z4", Vec3{ 700.f, 854.f, z }, Vec3{}, Vec3{ 20.f, 170.f, 100.f }); //OK
    ctx->scene_api.add_entity_in_zone(ctx->scene_api.scene, mur3_z4, 4);
    auto mur4_z4 = ctx->entity_factory_api.make_grapping_box(factory, "cube", "bloc_grap_z4", Vec3{ 1014.f, 704.f, z }, Vec3{}, Vec3{ 20.f, 193.f, 100.f }); //OK
    ctx->scene_api.add_entity_in_zone(ctx->scene_api.scene, mur4_z4, 4);
    auto mur5_z4 = ctx->entity_factory_api.make_grapping_box(factory, "cube", "bloc_grap_z3", Vec3{ 520, 785.f, z }, Vec3{}, Vec3{ 10.f, 236.f, 100.f }); //OK
    ctx->scene_api.add_entity_in_zone(ctx->scene_api.scene, mur5_z4, 4);
    auto mur6_z4 = ctx->entity_factory_api.make_grapping_box(factory, "cube", "bloc_grap_z4", Vec3{ 540, 785.f, z }, Vec3{}, Vec3{ 10.f, 236.f, 100.f }); //OK
    ctx->scene_api.add_entity_in_zone(ctx->scene_api.scene, mur6_z4, 4);
    
    auto mur_saut1_z4 = ctx->entity_factory_api.make_grapping_box(factory, "cube", "bloc_grap_z4", Vec3{ 787.5f, 768.f, 20.0f }, Vec3{}, Vec3{ 70.f, 20.f, 4.f }); //OK
    ctx->scene_api.add_entity_in_zone(ctx->scene_api.scene, mur_saut1_z4, 4); 
    auto mur_saut2_z4 = ctx->entity_factory_api.make_grapping_box(factory, "cube", "bloc_grap_z4", Vec3{ 944.5f, 830.f, 15.0f }, Vec3{}, Vec3{ 30.f, 30.f, 4.f }); // OK
    ctx->scene_api.add_entity_in_zone(ctx->scene_api.scene, mur_saut2_z4, 4);
    auto mur_saut3_z4 = ctx->entity_factory_api.make_grapping_box(factory, "cube", "bloc_grap_z4", Vec3{ 944.5f, 724.f, 20.0f }, Vec3{}, Vec3{ 20.f, 20.f, 4.f }); //OK
    ctx->scene_api.add_entity_in_zone(ctx->scene_api.scene, mur_saut3_z4, 4);
    auto mur_saut4_z4 = ctx->entity_factory_api.make_grapping_box(factory, "cube", "bloc_grap_z4", Vec3{ 944.5f, 638.f, 25.0f }, Vec3{}, Vec3{ 10.f, 10.f, 4.f }); //OK
    ctx->scene_api.add_entity_in_zone(ctx->scene_api.scene, mur_saut4_z4, 4);

    return true;
}

bool init_lights(SceneLoaderContext* ctx) {
    EntityFactory* factory = ctx->entity_factory_api.factory;

    /********************************************************************************************************************************************************************
    *                                                                       ZONE 1                                                                             *
    ********************************************************************************************************************************************************************/

    auto lightsTown1 = ctx->entity_factory_api.make_meshed_point_light(factory, "sphere_light", "_IMPORTED_", Vec3{ 255.f, 255.f, 60.f }, Vec3{}, Vec3{ 5.f, 5.f, 5.f }, Couleur(1.f, 1.f, 1.f, 1.0f), 100.0f, 1.f);
    ctx->scene_api.add_entity_in_zone(ctx->scene_api.scene, lightsTown1, 1);

    auto lightsTown2 = ctx->entity_factory_api.make_meshed_point_light(factory, "sphere_light", "_IMPORTED_", Vec3{ 100.f, 200.f, 60.f }, Vec3{}, Vec3{ 5.f, 5.f, 5.f }, Couleur(1.f, 1.f, 1.f, 1.0f), 100.0f, 1.f);
    ctx->scene_api.add_entity_in_zone(ctx->scene_api.scene, lightsTown2, 1);
    auto lightsTown3 = ctx->entity_factory_api.make_meshed_point_light(factory, "sphere_light", "_IMPORTED_", Vec3{ 100.f, 300.f, 60.f }, Vec3{}, Vec3{ 5.f, 5.f, 5.f }, Couleur(1.f, 1.f, 1.f, 1.0f), 100.0f, 1.f);
    ctx->scene_api.add_entity_in_zone(ctx->scene_api.scene, lightsTown3, 1);


    auto lightsTown4 = ctx->entity_factory_api.make_meshed_point_light(factory, "sphere_light", "_IMPORTED_", Vec3{ 250.f, 400.f, 60.f }, Vec3{}, Vec3{ 5.f, 5.f, 5.f }, Couleur(1.f, 1.f, 1.f, 1.0f), 100.0f, 1.f);
    ctx->scene_api.add_entity_in_zone(ctx->scene_api.scene, lightsTown4, 1);
    auto lightsTown5 = ctx->entity_factory_api.make_meshed_point_light(factory, "sphere_light", "_IMPORTED_", Vec3{ 400.f, 200.f, 60.f }, Vec3{}, Vec3{ 5.f, 5.f, 5.f }, Couleur(1.f, 1.f, 1.f, 1.0f), 100.0f, 1.f);
    ctx->scene_api.add_entity_in_zone(ctx->scene_api.scene, lightsTown5, 1);
    auto lightsTown6 = ctx->entity_factory_api.make_meshed_point_light(factory, "sphere_light", "_IMPORTED_", Vec3{ 200.f, 300.f, 60.f }, Vec3{}, Vec3{ 5.f, 5.f, 5.f }, Couleur(1.f, 1.f, 1.f, 1.0f), 100.0f, 1.f);
    ctx->scene_api.add_entity_in_zone(ctx->scene_api.scene, lightsTown6, 1);
    auto lightsTown7 = ctx->entity_factory_api.make_meshed_point_light(factory, "sphere_light", "_IMPORTED_", Vec3{ 300.f, 200.f, 60.f }, Vec3{}, Vec3{ 5.f, 5.f, 5.f }, Couleur(1.f, 1.f, 1.f, 1.0f), 100.0f, 1.f);
    ctx->scene_api.add_entity_in_zone(ctx->scene_api.scene, lightsTown7, 1);

    auto lampadaire1_z1 = ctx->entity_factory_api.make_meshed_spot_light(factory, "lampadaire", "_IMPORTED_", Vec3{ 160.f, 231.f, 55.f }, Vec3{ 0.f , DirectX::XM_PI ,   DirectX::XM_PI }, Vec3{ 1.f, 1.f, 1.f }, Couleur(1.f, 1.f, 1.f, 1.0f), Vec3{ 0.f, 0.f, 1.f }, DirectX::XM_PI, 50.0f, 5.0f, Vec3{ 0.f, 0.f, 5.f });
    ctx->scene_api.add_entity_in_zone(ctx->scene_api.scene, lampadaire1_z1, 1);
    auto lampadaire2_z1 = ctx->entity_factory_api.make_meshed_spot_light(factory, "lampadaire", "_IMPORTED_", Vec3{ 312.f, 320.f, 55.f }, Vec3{ 0.f , DirectX::XM_PI ,   -DirectX::XM_PIDIV2 }, Vec3{ 1.f, 1.f, 1.f }, Couleur(1.f, 1.f, 1.f, 1.0f), Vec3{ 0.f, 0.f, 1.f }, DirectX::XM_PI, 50.0f, 5.0f, Vec3{ -2.f, 0.f, 5.f });
    ctx->scene_api.add_entity_in_zone(ctx->scene_api.scene, lampadaire2_z1, 1);

	/********************************************************************************************************************************************************************
	*                                                                       ZONE 3                                                                             *
	********************************************************************************************************************************************************************/

	std::random_device rnd;
	std::mt19937 gen(rnd());
	std::uniform_real_distribution<float> dis(0.f, 1.f);
	std::uniform_real_distribution<float> disA(0.f, 2*DirectX::XM_PI);

	int const  nb_point_lights_kiki = 10;
	Entity lights_kiki[nb_point_lights_kiki];
	for (int i = 0; i < nb_point_lights_kiki; ++i) {
		lights_kiki[i] = ctx->entity_factory_api.make_meshed_point_light(factory, "sphere_light", "_IMPORTED_", Vec3{ 102.f, 612.f, 72.f }, Vec3{ disA(gen), disA(gen), disA(gen) }, Vec3{ 5.f, 5.f, 5.f }, Couleur(0.33f, 0.64f, 0.43f, 1.0f), 50.0f, 1.0f);
		ctx->scene_api.add_entity_in_zone(ctx->scene_api.scene, lights_kiki[i], 3);
		ctx->entity_factory_api.follow_spline(factory, lights_kiki[i], "circle", dis(gen)*0.2f, 0.f);
	}
	
	int const  nb_point_lights = 40;
	Entity lights[nb_point_lights];
	for (int i = 0; i < nb_point_lights; ++i) {
		lights[i] = ctx->entity_factory_api.make_meshed_point_light(factory, "sphere_light", "_IMPORTED_", Vec3{ dis(gen)*442.f+50.f, dis(gen)*50.f +928.f, dis(gen)*10.f + 50.f }, Vec3{ disA(gen), disA(gen), disA(gen) }, Vec3{ 5.f, 5.f, 5.f }, Couleur(0.33f, 0.64f, 0.43f, 1.0f), 50.0f, 1.0f);
		ctx->scene_api.add_entity_in_zone(ctx->scene_api.scene, lights[i], 3);
		ctx->entity_factory_api.follow_spline(factory, lights[i], "circle", dis(gen)*0.2f, 0.f);
	}

    return true;
}

bool init_player(SceneLoaderContext* ctx) {
    EntityFactory* factory = ctx->entity_factory_api.factory;

    Entity player_entity = ctx->entity_factory_api.make_player(factory, Vec3{600.f, 900.f, 20.f }, Vec3{0.f, 0.f, 0.f});
    Entity camera_entity = ctx->entity_factory_api.make_camera(factory, player_entity, "main_camera", Vec3{ 10.f, 0.f, 0.f}, Vec3{0.f, 0.f, 0.f});
    Entity fps_camera_entity = ctx->entity_factory_api.make_first_person_camera(factory, player_entity, "main_camera", Vec3{0.f, 0.f, 0.f});

    Entity camera_manager = ctx->entity_factory_api.make_cameras_manager(factory, fps_camera_entity, camera_entity, player_entity);

    ctx->scene_api.add_entity_in_zone(ctx->scene_api.scene, player_entity, 0);

    Entity e_grappling_manager = ctx->entity_factory_api.make_grappling_manager(factory, player_entity, camera_manager);

    ctx->camera_entity = camera_manager;

    ctx->entity_factory_api.make_game_controller(factory);

    ctx->entity_factory_api.make_chrono(factory);

    return true;
}


extern "C" {

SCENE_API int _stdcall load_scene(void* required) {
    SceneLoaderContext* loader_ctx = static_cast<SceneLoaderContext*>(required);
    if(loader_ctx == nullptr) return 1;

    if (!init_circuit(loader_ctx)) {
        return 2;
    }

    if (!init_lights(loader_ctx)) {
        return 3;
    }

    if (!init_player(loader_ctx)) {
        return 4;
    }

    return 0;
}

}