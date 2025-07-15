#pragma once


// Number of frame buffering
#define RPG_FRAME_BUFFERING		3


// Maximum model meshes/materials 
#define RPG_MODEL_MAX_MESH	8

// Maximum model LOD count
#define RPG_MODEL_MAX_LOD	4


// Minimum texture dimension
#define RPG_TEXTURE_MIN_DIM		64

// Maxiimum texture dimension
#define RPG_TEXTURE_MAX_DIM		4096

// Maximum texture mip count
#define RPG_TEXTURE_MAX_MIP		13


// Maximum material param vector
#define RPG_MATERIAL_PARAM_VECTOR_COUNT      12

// Maximum material param scalar
#define RPG_MATERIAL_PARAM_SCALAR_COUNT      16


// Font size small
#define RPG_FONT_SIZE_SMALL		10

// Font size medium
#define RPG_FONT_SIZE_MEDIUM	15

// Font size large
#define RPG_FONT_SIZE_LARGE		20

// Font tab indent pixel size
#define RPG_FONT_TAB_INDENT_PX	16


// Maximum bone count in skeleton
#define RPG_SKELETON_MAX_BONE				254

// Invalid skeleton bone index
#define RPG_SKELETON_BONE_INDEX_INVALID		255



// Spawn dedicated render thread
#define RPG_RENDER_MULTITHREADED					0

// All copy, compute, render execute in async task threadpool
#define RPG_RENDER_ASYNC_TASK						1

// Maximum view per world in single frame rendering
#define RPG_RENDER_CAMERA_MAX_COUNT                 4

// First index of point light in shader constant
#define RPG_RENDER_LIGHT_POINT_INDEX				0

// Maximum point light in single frame rendering
#define RPG_RENDER_LIGHT_POINT_MAX_COUNT			200

// First index of spot light in shader constant
#define RPG_RENDER_LIGHT_SPOT_INDEX				    (RPG_RENDER_LIGHT_POINT_INDEX + RPG_RENDER_LIGHT_POINT_MAX_COUNT)

// Maximum spot light in single frame rendering
#define RPG_RENDER_LIGHT_SPOT_MAX_COUNT			    64

// First index of directional light in shader constant
#define RPG_RENDER_LIGHT_DIRECTIONAL_INDEX			(RPG_RENDER_LIGHT_SPOT_INDEX + RPG_RENDER_LIGHT_SPOT_MAX_COUNT)

// Maximum directional light in single frame rendering
#define RPG_RENDER_LIGHT_DIRECTIONAL_MAX_COUNT		4

// Maximum light all types in single frame rendering
#define RPG_RENDER_MAX_LIGHT                        (RPG_RENDER_LIGHT_DIRECTIONAL_MAX_COUNT + RPG_RENDER_LIGHT_POINT_MAX_COUNT + RPG_RENDER_LIGHT_SPOT_MAX_COUNT)

// Maximum virtual camera in single frame rendering
#define RPG_RENDER_MAX_CAMERA                       (RPG_RENDER_CAMERA_MAX_COUNT + RPG_RENDER_MAX_LIGHT)

