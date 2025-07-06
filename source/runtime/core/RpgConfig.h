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

// First index of directional light in shader constant
#define RPG_RENDER_LIGHT_DIRECTIONAL_INDEX			0

// Maximum directional light in single frame rendering
#define RPG_RENDER_LIGHT_DIRECTIONAL_MAX_COUNT		4

// First index of point light in shader constant
#define RPG_RENDER_LIGHT_POINT_INDEX				(RPG_RENDER_LIGHT_DIRECTIONAL_INDEX + RPG_RENDER_LIGHT_DIRECTIONAL_MAX_COUNT)

// Maximum point light in single frame rendering
#define RPG_RENDER_LIGHT_POINT_MAX_COUNT			200

// First index of spot light in shader constant
#define RPG_RENDER_LIGHT_SPOT_INDEX				    (RPG_RENDER_LIGHT_POINT_INDEX + RPG_RENDER_LIGHT_POINT_MAX_COUNT)

// Maximum spot light in single frame rendering
#define RPG_RENDER_LIGHT_SPOT_MAX_COUNT			    64

// Maximum light all types in single frame rendering
#define RPG_RENDER_MAX_LIGHT                        (RPG_RENDER_LIGHT_DIRECTIONAL_MAX_COUNT + RPG_RENDER_LIGHT_POINT_MAX_COUNT + RPG_RENDER_LIGHT_SPOT_MAX_COUNT)

// Maximum virtual camera in single frame rendering
#define RPG_RENDER_MAX_CAMERA                       (RPG_RENDER_CAMERA_MAX_COUNT + RPG_RENDER_MAX_LIGHT)



#define RPG_SHADER_DEFAULT_VS_DEPTH_NAME				"VertexDepth"
#define RPG_SHADER_DEFAULT_VS_DEPTH_CUBE_NAME			"VertexDepthCube"
#define RPG_SHADER_DEFAULT_VS_MESH_NAME					"VertexMesh"
#define RPG_SHADER_DEFAULT_VS_MESH_SKINNED_NAME			"VertexMeshSkinned"
#define RPG_SHADER_DEFAULT_VS_MESH_2D_NAME				"VertexMesh2D"
#define RPG_SHADER_DEFAULT_VS_PRIMITIVE_NAME			"VertexPrimitive"
#define RPG_SHADER_DEFAULT_VS_PRIMITIVE_2D_NAME			"VertexPrimitive2D"
#define RPG_SHADER_DEFAULT_VS_FULLSCREEN_NAME			"VertexFullscreen"
#define RPG_SHADER_DEFAULT_GS_CUBE_SHADOW_NAME			"GeometryCubeShadow"
#define RPG_SHADER_DEFAULT_PS_COLOR_NAME				"PixelColor"
#define RPG_SHADER_DEFAULT_PS_TEXTURE_COLOR_NAME		"PixelTextureColor"
#define RPG_SHADER_DEFAULT_PS_TEXTURE_FONT_NAME			"PixelTextureFont"
#define RPG_SHADER_DEFAULT_PS_TEXTURE_DEPTH_NAME		"PixelTextureDepth"
#define RPG_SHADER_DEFAULT_PS_PHONG_NAME				"PixelPhong"
#define RPG_SHADER_DEFAULT_PS_PHONG_MASK_NAME			"PixelPhongMask"
#define RPG_SHADER_DEFAULT_PS_FULLSCREEN_GAMMA_NAME     "PixelFullscreenGamma"
#define RPG_SHADER_DEFAULT_CS_FRUSTUM_NAME				"ComputeFrustum"
#define RPG_SHADER_DEFAULT_CS_SKINNING_NAME				"ComputeSkinning"
