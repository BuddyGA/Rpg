#pragma once

#include "RpgRenderResource.h"
#include "async_task/RpgAsyncTask_RenderPass.h"



struct RpgSceneMesh
{
	RpgGameObjectID GameObject;
	RpgMatrixTransform WorldTransformMatrix;
	RpgSharedMaterial Material;
	RpgSharedMesh Mesh;
	int Lod{ 0 };
};


struct RpgSceneLight
{
	RpgGameObjectID GameObject;
	RpgTransform WorldTransform;
	RpgRenderLight::EType Type{ RpgRenderLight::TYPE_NONE };
	RpgColorLinear ColorIntensity;
	float AttenuationRadius{ 0.0f };
	float AttenuationFallOffExp{ 0.0f };
	float SpotInnerConeDegree{ 0.0f };
	float SpotOuterConeDegree{ 0.0f };
	RpgShadowViewport* ShadowViewport{ nullptr };
};



class RpgSceneViewport
{
private:
	RpgMatrixTransform ViewMatrix;
	RpgMatrixProjection ProjectionMatrix;
	RpgQuaternion ViewRotation;
	RpgVector3 ViewPosition;
	RpgBoundingFrustum ViewFrustum;
	float FovDegree;
	float NearClipZ;
	float FarClipZ;
	bool bOrthographicProjection;
	bool bDirtyProjection;


	struct FFrameData
	{
		RpgSharedTexture2D RenderTargetTexture;
		RpgSharedTexture2D DepthStencilTexture;
		
		RpgArray<RpgDrawIndexed> DrawOpaqueMeshes;
		RpgArray<RpgDrawIndexed> DrawOpaqueSkinnedMeshes;

		RpgArray<RpgDrawIndexed> DrawTransparencies;

		RpgUniquePtr<RpgAsyncTask_RenderPass_Forward> AsyncTaskRenderPassForward;
	};
	FFrameData FrameDatas[RPG_FRAME_BUFFERING];


public:
	RpgPointInt RenderTargetDimension;
	RpgArray<RpgSceneMesh> Meshes;
	RpgArray<RpgSceneLight> Lights;


public:
	RpgSceneViewport() noexcept;

	void PreRender(RpgRenderFrameContext& frameContext, RpgWorldResource* worldResource, const RpgWorld* world) noexcept;
	void SetupRenderPasses(const RpgRenderFrameContext& frameContext, const RpgWorldResource* worldResource, const RpgWorld* world, RpgAsyncTask_RenderPassShadowArray& out_ShadowPasses, RpgAsyncTask_RenderPassForwardArray& out_ForwardPasses) noexcept;


	inline void SetViewRotationAndPosition(const RpgQuaternion& in_Rotation, const RpgVector3& in_Position) noexcept
	{
		ViewRotation = in_Rotation;
		ViewPosition = in_Position;
	}

	inline void GetViewRotationAndPosition(RpgQuaternion& out_Rotation, RpgVector3& out_Position) const noexcept
	{
		out_Rotation = ViewRotation;
		out_Position = ViewPosition;
	}


	inline void SetProjectionPerspective(float in_FovDegree, float in_NearClipZ, float in_FarClipZ) noexcept
	{
		FovDegree = in_FovDegree;
		NearClipZ = in_NearClipZ;
		FarClipZ = in_FarClipZ;
		bOrthographicProjection = false;
		bDirtyProjection = true;
	}


	inline void SetProjectionOrthographic(float in_NearClipZ, float in_FarClipZ) noexcept
	{
		NearClipZ = in_NearClipZ;
		FarClipZ = in_FarClipZ;
		bOrthographicProjection = true;
		bDirtyProjection = true;
	}


	inline void UpdateViewProjection() noexcept
	{
		const RpgMatrixTransform worldMatrixTransform(ViewPosition, ViewRotation);

		ViewMatrix = worldMatrixTransform.GetInverse();

		ProjectionMatrix = bOrthographicProjection ?
			RpgMatrixProjection::CreateOrthographic(0.0f, static_cast<float>(RenderTargetDimension.X), 0.0f, static_cast<float>(RenderTargetDimension.Y), NearClipZ, FarClipZ) :
			RpgMatrixProjection::CreatePerspective(static_cast<float>(RenderTargetDimension.X) / static_cast<float>(RenderTargetDimension.Y), FovDegree, NearClipZ, FarClipZ);

		ViewFrustum.CreateFromMatrix(worldMatrixTransform, ProjectionMatrix);
	}


	inline const RpgBoundingFrustum& GetViewFrustum() const noexcept
	{
		return ViewFrustum;
	}

	inline const RpgSharedTexture2D& GetRenderTargetTexture(int frameIndex) const noexcept
	{
		return FrameDatas[frameIndex].RenderTargetTexture;
	}

	inline const RpgSharedTexture2D& GetDepthStencilTexture(int frameIndex) const noexcept
	{
		return FrameDatas[frameIndex].DepthStencilTexture;
	}


#ifndef RPG_BUILD_SHIPPING
private:
	struct FFrameDebug
	{
		RpgMaterialResource::FMaterialID LineMaterialId;
		RpgMaterialResource::FMaterialID LineNoDepthMaterialId;
		RpgWorldResource::FCameraID CameraId;
	};
	FFrameDebug FrameDebugs[RPG_FRAME_BUFFERING];
#endif // !RPG_BUILD_SHIPPING

};
