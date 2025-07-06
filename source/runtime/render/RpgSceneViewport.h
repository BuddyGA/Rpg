#pragma once

#include "RpgRenderResource.h"



class RpgSceneViewport : public RpgRenderViewport
{
private:
	RpgMatrixTransform CameraViewMatrix;
	RpgMatrixProjection CameraProjectionMatrix;
	RpgQuaternion CameraRotation;
	RpgVector3 CameraPosition;
	RpgBoundingFrustum CameraFrustum;
	float CameraFovDegree;
	float CameraNearClipZ;
	float CameraFarClipZ;
	bool bOrthographicProjection;
	bool bDirtyProjection;
	RpgArray<RpgGameObjectID> CapturedGameObjects;


	struct FFrameData
	{
		RpgSharedTexture2D RenderTargetTexture;
		RpgSharedTexture2D DepthStencilTexture;
		
		RpgArray<RpgDrawIndexed> DrawOpaqueMeshes;
		RpgArray<RpgDrawIndexed> DrawOpaqueSkinnedMeshes;

		RpgArray<RpgDrawIndexed> DrawTransparencies;

		RpgAsyncTask_RenderPass_Forward* AsyncTaskRenderPassForward;
	};
	FFrameData FrameDatas[RPG_RENDER_FRAME_BUFFERING];


public:
	RpgPointInt RenderTargetDimension;
	bool bFrustumCulling;


public:
	RpgSceneViewport() noexcept;
	~RpgSceneViewport() noexcept;

public:
	virtual void PreRender(int frameIndex, RpgMaterialResource* materialResource, RpgMeshResource* meshResource, RpgMeshSkinnedResource* meshSkinnedResource, RpgWorldResource* worldResource, const RpgWorld* world) noexcept override;
	virtual void SetupRenderPasses(int frameIndex, RpgAsyncTask_RenderPassArray& out_RenderPasses, const RpgMaterialResource* materialResource, const RpgMeshResource* meshResource, const RpgMeshSkinnedResource* meshSkinnedResource, const RpgWorldResource* worldResource) noexcept override;


	inline void SetCameraRotationAndPosition(const RpgQuaternion& in_Rotation, const RpgVector3& in_Position) noexcept
	{
		CameraRotation = in_Rotation;
		CameraPosition = in_Position;
	}

	inline void GetCameraRotationAndPosition(RpgQuaternion& out_Rotation, RpgVector3& out_Position) const noexcept
	{
		out_Rotation = CameraRotation;
		out_Position = CameraPosition;
	}

	[[nodiscard]] inline const RpgSharedTexture2D& GetRenderTargetTexture(int frameIndex) const noexcept
	{
		return FrameDatas[frameIndex].RenderTargetTexture;
	}

	[[nodiscard]] inline const RpgSharedTexture2D& GetDepthStencilTexture(int frameIndex) const noexcept
	{
		return FrameDatas[frameIndex].DepthStencilTexture;
	}

	[[nodiscard]] inline const RpgArray<RpgGameObjectID>& GetCapturedGameObjects() const noexcept
	{
		return CapturedGameObjects;
	}


#ifndef RPG_BUILD_SHIPPING
private:
	struct FFrameDebug
	{
		RpgMaterialResource::FMaterialID LineMaterialId;
		RpgMaterialResource::FMaterialID LineNoDepthMaterialId;
		RpgWorldResource::FCameraID CameraId;
	};
	FFrameDebug FrameDebugs[RPG_RENDER_FRAME_BUFFERING];
#endif // !RPG_BUILD_SHIPPING

};
