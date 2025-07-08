#pragma once

#include "RpgRenderResource.h"



class RpgSceneViewport : public RpgRenderViewport
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

		RpgAsyncTask_RenderPass_Forward* AsyncTaskRenderPassForward;
	};
	FFrameData FrameDatas[RPG_FRAME_BUFFERING];


public:
	RpgPointInt RenderTargetDimension;
	bool bFrustumCulling;


public:
	RpgSceneViewport() noexcept;
	~RpgSceneViewport() noexcept;

public:
	virtual void PreRender(int frameIndex, RpgMaterialResource* materialResource, RpgMeshResource* meshResource, RpgMeshSkinnedResource* meshSkinnedResource, RpgWorldResource* worldResource, const RpgWorld* world) noexcept override;
	virtual void SetupRenderPasses(int frameIndex, RpgAsyncTask_RenderPassArray& out_RenderPasses, const RpgMaterialResource* materialResource, const RpgMeshResource* meshResource, const RpgMeshSkinnedResource* meshSkinnedResource, const RpgWorldResource* worldResource) noexcept override;


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
