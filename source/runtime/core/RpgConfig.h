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



// Use dedicated render thread
#define RPG_RENDER_MULTITHREADED					0

// All copy, compute, render execute in async task threadpool
#define RPG_RENDER_ASYNC_TASK						1
