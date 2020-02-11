#ifndef DEF_SCENE_LOADER_H
#define DEF_SCENE_LOADER_H

#ifdef SCENE_EXPORTS
#define SCENE_API __declspec(dllexport)
#else
#define SCENE_API __declspec(dllimport)
#endif

#ifdef __cplusplus
extern "C" {
#endif

SCENE_API int _stdcall load_scene(void* required);

#ifdef __cplusplus
}
#endif

#endif