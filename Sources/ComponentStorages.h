#ifndef DEF_ECS_COMPONENT_STORAGES_H
#define DEF_ECS_COMPONENT_STORAGES_H

#include "VectorStorage.h"
#include "MapStorage.h"
#include "TransformComponent.h"
#include "RigidBodyComponent.h"
#include "StaticMeshComponent.h"
#include "BillboardComponent.h"
#include "CameraComponent.h"
#include "LightComponent.h"
#include "InputComponent.h"
#include "UpdateComponent.h"
#include "TriggerComponent.h"
#include "GrapplingComponent.h"
#include "VariableColorComponent.h"
#include "SplineComponent.h"

using TransformStorage = VectorStorage<TransformComponent>;
using RigidBodyStorage = VectorStorage<RigidBodyComponent>;
using StaticMeshStorage = VectorStorage<StaticMeshComponent>;
using BillboardStorage = VectorStorage<BillboardComponent>;
using GrapplingStorage = VectorStorage<GrapplingComponent>;
using CameraStorage = MapStorage<CameraComponent>;
using LightStorage = MapStorage<LightComponent>;
using InputStorage = MapStorage<InputComponent>;
using UpdateStorage = MapStorage<UpdateComponent>;
using TriggerStorage = MapStorage<TriggerComponent>;
using VariableColorStorage = MapStorage<VariableColorComponent>;
using SplineStorage = MapStorage<SplineComponent>;

#endif