#include "RpgMaterial.h"


RPG_LOG_DECLARE_CATEGORY_STATIC(RpgLogMaterial, VERBOSITY_DEBUG)



RpgMaterial::RpgMaterial(const RpgName& in_Name, const RpgRenderPipelineState& in_RenderState, const RpgMaterialParameterLayout& in_ParameterLayout) noexcept
{
	Name = in_Name;
	RenderState = in_RenderState;
	ParameterLayout = in_ParameterLayout;
	ParameterTextureLock = SDL_CreateRWLock();
	ParameterVectorLock = SDL_CreateRWLock();
	ParameterScalarLock = SDL_CreateRWLock();
	Flags = FLAG_None;
}


RpgMaterial::RpgMaterial(const RpgName& in_Name, const RpgSharedMaterial& in_ParentMaterial) noexcept
{
	Name = in_Name;
	ParentMaterial = in_ParentMaterial;
	RenderState = ParentMaterial->RenderState;
	ParameterLayout = ParentMaterial->ParameterLayout;
	ParameterTextureLock = SDL_CreateRWLock();
	ParameterVectorLock = SDL_CreateRWLock();
	ParameterScalarLock = SDL_CreateRWLock();
	Flags = FLAG_None;
}


RpgMaterial::~RpgMaterial() noexcept
{
	RPG_LogDebug(RpgLogTemp, "Destroy material [%s]", *Name);

	SDL_DestroyRWLock(ParameterTextureLock);
	SDL_DestroyRWLock(ParameterVectorLock);
	SDL_DestroyRWLock(ParameterScalarLock);
}




RpgSharedMaterial RpgMaterial::s_CreateShared(const RpgName& name, const RpgRenderPipelineState& renderState, const RpgMaterialParameterLayout& parameterLayout) noexcept
{
	return RpgSharedMaterial(new RpgMaterial(name, renderState, parameterLayout));
}


RpgSharedMaterial RpgMaterial::s_CreateSharedInstance(const RpgName& name, const RpgSharedMaterial& parentMaterial) noexcept
{
	return RpgSharedMaterial(new RpgMaterial(name, parentMaterial));
}



static RpgArray<RpgSharedMaterial> DefaultMaterials;


void RpgMaterial::s_CreateDefaults() noexcept
{
	RPG_LogDebug(RpgLogMaterial, "Create default materials...");

	DefaultMaterials.Resize(RpgMaterialDefault::MAX_COUNT);

	// mesh2d
	{
		RpgRenderPipelineState renderState{};
		renderState.PixelShaderName = RPG_SHADER_DEFAULT_PS_TEXTURE_COLOR_NAME;
		renderState.VertexMode = RpgRenderVertexMode::MESH_2D;
		renderState.BlendMode = RpgRenderColorBlendMode::TRANSPARENCY;
		renderState.RasterMode = RpgRenderRasterMode::SOLID;

		DefaultMaterials[RpgMaterialDefault::MESH_2D] = s_CreateShared("MAT_DEF_Mesh2d", renderState);
	}

	// font2d
	{
		RpgRenderPipelineState renderState{};
		renderState.PixelShaderName = RPG_SHADER_DEFAULT_PS_TEXTURE_COLOR_NAME;
		renderState.VertexMode = RpgRenderVertexMode::MESH_2D;
		renderState.BlendMode = RpgRenderColorBlendMode::TRANSPARENCY;
		renderState.RasterMode = RpgRenderRasterMode::SOLID;
		renderState.PixelShaderName = RPG_SHADER_DEFAULT_PS_TEXTURE_FONT_NAME;

		DefaultMaterials[RpgMaterialDefault::FONT_2D] = s_CreateShared("MAT_DEF_Font2d", renderState);
	}

	// mesh phong
	{
		RpgRenderPipelineState renderState{};
		renderState.PixelShaderName = RPG_SHADER_DEFAULT_PS_PHONG_NAME;
		renderState.VertexMode = RpgRenderVertexMode::MESH;
		renderState.RasterMode = RpgRenderRasterMode::SOLID;
		renderState.BlendMode = RpgRenderColorBlendMode::NONE;
		renderState.bDepthTest = true;
		renderState.bDepthWrite = true;

		RpgMaterialParameterLayout paramLayout{};
		paramLayout.AddVector("base_color", RpgVector4(1.0f));
		paramLayout.AddVector("specular_color", RpgVector4(1.0f));
		paramLayout.AddScalar("specular_intensity", 0.0f);
		paramLayout.AddScalar("shininess", 0.0f);
		paramLayout.AddScalar("opacity", 1.0f);

		DefaultMaterials[RpgMaterialDefault::MESH_PHONG] = s_CreateShared("MAT_DEF_MeshPhong", renderState, paramLayout);
	}


	// fullscreen
	{
		RpgRenderPipelineState renderState{};
		renderState.PixelShaderName = RPG_SHADER_DEFAULT_PS_FULLSCREEN_GAMMA_NAME;
		renderState.VertexMode = RpgRenderVertexMode::NONE;
		renderState.BlendMode = RpgRenderColorBlendMode::NONE;
		renderState.RasterMode = RpgRenderRasterMode::SOLID;

		RpgMaterialParameterLayout paramLayout{};
		paramLayout.AddScalar("gamma", 2.2f);

		DefaultMaterials[RpgMaterialDefault::FULLSCREEN] = s_CreateShared("MAT_DEF_Fullscreen", renderState, paramLayout);
	}


	// Debug primitive2d line
	{
		RpgRenderPipelineState renderState{};
		renderState.PixelShaderName = RPG_SHADER_DEFAULT_PS_COLOR_NAME;
		renderState.VertexMode = RpgRenderVertexMode::PRIMITIVE_2D;
		renderState.BlendMode = RpgRenderColorBlendMode::NONE;
		renderState.RasterMode = RpgRenderRasterMode::LINE;

		DefaultMaterials[RpgMaterialDefault::DEBUG_PRIMITIVE_2D_LINE] = s_CreateShared("MAT_DEF_DebugPrimitive2dLine", renderState);
	}

	// Debug primitive2d mesh
	{
		RpgRenderPipelineState renderState{};
		renderState.PixelShaderName = RPG_SHADER_DEFAULT_PS_COLOR_NAME;
		renderState.VertexMode = RpgRenderVertexMode::PRIMITIVE_2D;
		renderState.BlendMode = RpgRenderColorBlendMode::NONE;
		renderState.RasterMode = RpgRenderRasterMode::SOLID;

		DefaultMaterials[RpgMaterialDefault::DEBUG_PRIMITIVE_2D_MESH] = s_CreateShared("MAT_DEF_DebugPrimitive2dMesh", renderState);
	}

	// Debug primitive line
	{
		RpgRenderPipelineState renderState{};
		renderState.PixelShaderName = RPG_SHADER_DEFAULT_PS_COLOR_NAME;
		renderState.VertexMode = RpgRenderVertexMode::PRIMITIVE;
		renderState.BlendMode = RpgRenderColorBlendMode::NONE;
		renderState.RasterMode = RpgRenderRasterMode::LINE;
		renderState.bDepthTest = true;
		renderState.bDepthWrite = false;

		DefaultMaterials[RpgMaterialDefault::DEBUG_PRIMITIVE_LINE] = s_CreateShared("MAT_DEF_DebugPrimitiveLine", renderState);

		// no-depth
		renderState.BlendMode = RpgRenderColorBlendMode::FADE;
		renderState.bDepthTest = false;

		DefaultMaterials[RpgMaterialDefault::DEBUG_PRIMITIVE_LINE_NO_DEPTH] = s_CreateShared("MAT_DEF_DebugPrimitiveLineNoDepth", renderState);
	}

	// Debug primitive mesh
	{
		RpgRenderPipelineState renderState{};
		renderState.PixelShaderName = RPG_SHADER_DEFAULT_PS_COLOR_NAME;
		renderState.VertexMode = RpgRenderVertexMode::PRIMITIVE;
		renderState.BlendMode = RpgRenderColorBlendMode::NONE;
		renderState.RasterMode = RpgRenderRasterMode::SOLID;
		renderState.bDepthTest = true;
		renderState.bDepthWrite = false;

		DefaultMaterials[RpgMaterialDefault::DEBUG_PRIMITIVE_MESH] = s_CreateShared("MAT_DEF_DebugPrimitiveMesh", renderState);

		// no-depth
		renderState.BlendMode = RpgRenderColorBlendMode::FADE;
		renderState.bDepthTest = false;

		DefaultMaterials[RpgMaterialDefault::DEBUG_PRIMITIVE_MESH_NO_DEPTH] = s_CreateShared("MAT_DEF_DebugPrimitiveMesh_NoDepth", renderState);
	}
}


void RpgMaterial::s_DestroyDefaults() noexcept
{
	DefaultMaterials.Clear(true);
}

const RpgSharedMaterial& RpgMaterial::s_GetDefault(RpgMaterialDefault::EType type) noexcept
{
	return DefaultMaterials[type];
}
