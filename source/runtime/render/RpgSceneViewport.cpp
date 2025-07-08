#include "RpgSceneViewport.h"
#include "core/world/RpgWorld.h"
#include "world/RpgRenderComponent.h"
#include "async_task/RpgAsyncTask_RenderPass.h"
#include "animation/world/RpgAnimationComponent.h"



RpgSceneViewport::RpgSceneViewport() noexcept
	: FrameDatas()
{
	FovDegree = 90.0f;
	NearClipZ = 10.0f;
	FarClipZ = 10000.0f;
	bOrthographicProjection = false;
	bDirtyProjection = true;
	RenderTargetDimension = RpgPointInt(1600, 900);
	bFrustumCulling = false;

	for (int f = 0; f < RPG_FRAME_BUFFERING; ++f)
	{
		FFrameData& frame = FrameDatas[f];
		frame.AsyncTaskRenderPassForward = RpgThreadPool::CreateTask<RpgAsyncTask_RenderPass_Forward>();
	}


#ifndef RPG_BUILD_SHIPPING
	for (int f = 0; f < RPG_FRAME_BUFFERING; ++f)
	{
		FFrameDebug& debug = FrameDebugs[f];
		debug.LineMaterialId = RPG_INDEX_INVALID;
		debug.CameraId = RPG_INDEX_INVALID;
	}
#endif // !RPG_BUILD_SHIPPING

}


RpgSceneViewport::~RpgSceneViewport() noexcept
{
	for (int f = 0; f < RPG_FRAME_BUFFERING; ++f)
	{
		FFrameData& frame = FrameDatas[f];
		RpgThreadPool::DestroyTask(frame.AsyncTaskRenderPassForward);
	}
}


void RpgSceneViewport::PreRender(int frameIndex, RpgMaterialResource* materialResource, RpgMeshResource* meshResource, RpgMeshSkinnedResource* meshSkinnedResource, RpgWorldResource* worldResource, const RpgWorld* world) noexcept
{
	FFrameData& frame = FrameDatas[frameIndex];
	frame.DrawOpaqueMeshes.Clear();
	frame.DrawOpaqueSkinnedMeshes.Clear();
	frame.DrawTransparencies.Clear();


	// Resize render target
	{
		if (!frame.RenderTargetTexture)
		{
			frame.RenderTargetTexture = RpgTexture2D::s_CreateSharedRenderTarget("TEXRT_SceneViewport", RpgTextureFormat::TEX_RT_RGBA, RenderTargetDimension.X, RenderTargetDimension.Y);
		}

		frame.RenderTargetTexture->Resize(RenderTargetDimension.X, RenderTargetDimension.Y);
		frame.RenderTargetTexture->GPU_UpdateResource();


		// Resize depth-stencil
		if (!frame.DepthStencilTexture)
		{
			frame.DepthStencilTexture = RpgTexture2D::s_CreateSharedDepthStencil("TEXDS_SceneViewport", RpgTextureFormat::TEX_DS_32, RenderTargetDimension.X, RenderTargetDimension.Y);
		}

		frame.DepthStencilTexture->Resize(RenderTargetDimension.X, RenderTargetDimension.Y);
		frame.DepthStencilTexture->GPU_UpdateResource();
	}


	// Update camera view projection
	{
		const RpgMatrixTransform worldMatrixTransform(ViewPosition, ViewRotation);

		ViewMatrix = worldMatrixTransform.GetInverse();

		ProjectionMatrix = bOrthographicProjection ?
			RpgMatrixProjection::CreateOrthographic(0.0f, static_cast<float>(RenderTargetDimension.X), 0.0f, static_cast<float>(RenderTargetDimension.Y), NearClipZ, FarClipZ) :
			RpgMatrixProjection::CreatePerspective(static_cast<float>(RenderTargetDimension.X) / static_cast<float>(RenderTargetDimension.Y), FovDegree, NearClipZ, FarClipZ);

		ViewFrustum.CreateFromMatrix(worldMatrixTransform, ProjectionMatrix);
	}


	// Build draw calls
	const RpgWorldResource::FCameraID cameraId = worldResource->AddCamera(ViewMatrix, ProjectionMatrix, ViewPosition, NearClipZ, FarClipZ);

	// capture meshes
	RpgArray<RpgMatrixTransform> tempBoneSkinningTransforms;

	for (auto it = world->Component_CreateConstIterator<RpgRenderComponent_Mesh>(); it; ++it)
	{
		const RpgRenderComponent_Mesh& comp = it.GetValue();
		if (!comp.bIsVisible || (bFrustumCulling && !ViewFrustum.TestIntersectBoundingAABB(comp.Bound)))
		{
			continue;
		}

		const RpgSharedModel& model = comp.Model;

		// TODO: Determine LOD level based on distance from the camera

		for (int m = 0; m < model->GetMeshCount(); ++m)
		{
			const RpgSharedMesh& mesh = model->GetMeshLod(m, 0);
			const RpgSharedMaterial& material = model->GetMaterial(m);
			const bool bHasSkin = mesh->HasSkin();
			bool bIsStaticMesh = true;

			RpgDrawIndexed draw;
			draw.Material = material ? materialResource->AddMaterial(material) : materialResource->AddMaterial(RpgMaterial::s_GetDefault(RpgMaterialDefault::MESH_PHONG));
			draw.ObjectParam.CameraIndex = cameraId;
			draw.ObjectParam.TransformIndex = worldResource->AddTransform(world->GameObject_GetWorldTransformMatrix(comp.GameObject));
			
			if (bHasSkin)
			{
				const RpgAnimationComponent* animComp = world->GameObject_GetComponent<RpgAnimationComponent>(comp.GameObject);

				// draw as static mesh if no animation component
				bIsStaticMesh = (animComp == nullptr);

				if (animComp)
				{
					const RpgAnimationSkeleton* skeleton = animComp->GetSkeleton().Get();
					RPG_Check(skeleton);

					const int boneCount = skeleton->GetBoneCount();
					tempBoneSkinningTransforms.Resize(boneCount);

					for (int b = 0; b < boneCount; ++b)
					{
						tempBoneSkinningTransforms[b] = skeleton->GetBoneInverseBindPoseTransform(b) * animComp->GetFinalPose().GetBonePoseTransform(b);
					}

					const RpgMeshSkinnedResource::FMeshID meshId = meshSkinnedResource->AddMesh(mesh, draw.IndexCount, draw.IndexStart, draw.IndexVertexOffset);
					meshSkinnedResource->AddObjectBoneSkinningTransforms(meshId, tempBoneSkinningTransforms);
				}
			}

			if (bIsStaticMesh)
			{
				meshResource->AddMesh(mesh, draw.IndexCount, draw.IndexStart, draw.IndexVertexOffset);
			}

			if (material->IsTransparency())
			{
				frame.DrawTransparencies.AddValue(draw);
			}
			else
			{
				if (bIsStaticMesh)
				{
					frame.DrawOpaqueMeshes.AddValue(draw);
				}
				else
				{
					frame.DrawOpaqueSkinnedMeshes.AddValue(draw);
				}
			}
		}
	}


	// capture lights
	for (auto it = world->Component_CreateConstIterator<RpgRenderComponent_Light>(); it; ++it)
	{
		const RpgRenderComponent_Light& comp = it.GetValue();
		if (comp.Type == RpgRenderLight::TYPE_NONE || !comp.bIsVisible)
		{
			continue;
		}

		const RpgTransform transform = world->GameObject_GetWorldTransform(comp.GameObject);
		worldResource->AddLight_Point(comp.GameObject.GetIndex(), transform.Position, comp.ColorIntensity, comp.AttenuationRadius, comp.AttenuationFallOffExp, comp.bCastShadow);
	}


#ifndef RPG_BUILD_SHIPPING
	FFrameDebug& debug = FrameDebugs[frameIndex];
	debug.LineMaterialId = materialResource->AddMaterial(RpgMaterial::s_GetDefault(RpgMaterialDefault::DEBUG_PRIMITIVE_LINE));
	debug.LineNoDepthMaterialId = materialResource->AddMaterial(RpgMaterial::s_GetDefault(RpgMaterialDefault::DEBUG_PRIMITIVE_LINE_NO_DEPTH));
	debug.CameraId = cameraId;
#endif // !RPG_BUILD_SHIPPING

}


void RpgSceneViewport::SetupRenderPasses(int frameIndex, RpgAsyncTask_RenderPassArray& out_RenderPasses, const RpgMaterialResource* materialResource, const RpgMeshResource* meshResource, const RpgMeshSkinnedResource* meshSkinnedResource, const RpgWorldResource* worldResource) noexcept
{
	FFrameData& frame = FrameDatas[frameIndex];

	RpgAsyncTask_RenderPass_Forward* forwardPass = frame.AsyncTaskRenderPassForward;
	forwardPass->Reset();
	forwardPass->MaterialResource = materialResource;
	forwardPass->MeshResource = meshResource;
	forwardPass->MeshSkinnedResource = meshSkinnedResource;
	forwardPass->WorldResource = worldResource;
	forwardPass->RenderTargetTexture = frame.RenderTargetTexture.Get();
	forwardPass->DepthStencilTexture = frame.DepthStencilTexture.Get();
	forwardPass->DrawMeshData = frame.DrawOpaqueMeshes.GetData();
	forwardPass->DrawMeshCount = frame.DrawOpaqueMeshes.GetCount();
	forwardPass->DrawSkinnedMeshData = frame.DrawOpaqueSkinnedMeshes.GetData();
	forwardPass->DrawSkinnedMeshCount = frame.DrawOpaqueSkinnedMeshes.GetCount();

#ifndef RPG_BUILD_SHIPPING
	FFrameDebug& debug = FrameDebugs[frameIndex];
	forwardPass->DebugDrawLineMaterialId = debug.LineMaterialId;
	forwardPass->DebugDrawLineNoDepthMaterialId = debug.LineNoDepthMaterialId;
	forwardPass->DebugDrawCameraId = debug.CameraId;
#endif // !RPG_BUILD_SHIPPING

	out_RenderPasses.AddValue(forwardPass);
}
