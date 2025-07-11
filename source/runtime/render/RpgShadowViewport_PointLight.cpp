#include "RpgShadowViewport.h"
#include "core/world/RpgWorld.h"
#include "world/RpgRenderComponent.h"


RpgShadowViewport_PointLight::RpgShadowViewport_PointLight() noexcept
{
	AttenuationRadius = 800.0f;
}


RpgShadowViewport_PointLight::~RpgShadowViewport_PointLight() noexcept
{
	RPG_LogDebug(RpgLogTemp, "Destruct (RpgShadowViewport_PointLight)");
}


void RpgShadowViewport_PointLight::PreRender(RpgRenderFrameContext& frameContext, RpgWorldResource* worldResource, const RpgWorld* world, const RpgWorldResource::FLightID* lightIds, int lightCount) noexcept
{
	RPG_Assert(lightIds);
	RPG_Assert(lightCount == 6);

	FFrameData& frame = FrameDatas[frameContext.Index];
	frame.DrawMeshes.Clear();
	frame.DrawSkinnedMeshes.Clear();

	constexpr const char* postfixName[6] = { "+X", "-X", "+Y", "-Y", "+Z", "-Z", };

	const uint16_t shadowTextureDimension = RpgRenderLight::SHADOW_TEXTURE_DIMENSION_POINT_LIGHT[frameContext.ShadowQuality];

	for (int i = 0; i < 6; ++i)
	{
		RpgSharedTexture2D& depthTexture = frame.DepthTextures[i];

		if (!depthTexture)
		{
			const RpgName name = RpgName::Format("TEXD_ShdwVprt_PL_%s", postfixName[i]);
			depthTexture = RpgTexture2D::s_CreateSharedDepthStencil(name, RpgTextureFormat::TEX_DS_16, shadowTextureDimension, shadowTextureDimension);
		}

		depthTexture->Resize(shadowTextureDimension, shadowTextureDimension);
		depthTexture->GPU_UpdateResource();
	}

	RpgTransform transform = world->GameObject_GetWorldTransform(GameObject);
	const float fovDegree = 91.0f;
	const float nearClipZ = 1.0f;
	const float farClipZ = AttenuationRadius * 1.05f;
	const RpgMatrixProjection projMatrix = RpgMatrixProjection::CreatePerspective(1.0f, fovDegree, nearClipZ, farClipZ);


	// setup shadow camera for each face
	// +X
	{
		transform.Rotation = RpgQuaternion::FromPitchYawRollDegree(0.0f, 90.0f, 0.0f);

		FViewInfo& view = FaceViews[0];
		view.ViewMatrix = transform.ToMatrixTransform().GetInverse();
		view.ProjectionMatrix = projMatrix;
		view.FarClipZ = farClipZ;
		view.CameraId = worldResource->AddCamera(view.ViewMatrix, view.ProjectionMatrix, transform.Position, nearClipZ, farClipZ);
	}
	// -X
	{
		transform.Rotation = RpgQuaternion::FromPitchYawRollDegree(0.0f, -90.0f, 0.0f);

		FViewInfo& view = FaceViews[1];
		view.ViewMatrix = transform.ToMatrixTransform().GetInverse();
		view.ProjectionMatrix = RpgMatrixProjection::CreatePerspective(1.0f, fovDegree, nearClipZ, farClipZ);
		view.FarClipZ = farClipZ;
		view.CameraId = worldResource->AddCamera(view.ViewMatrix, view.ProjectionMatrix, transform.Position, nearClipZ, farClipZ);
	}
	// +Y
	{
		transform.Rotation = RpgQuaternion::FromPitchYawRollDegree(-90.0f, 0.0f, 0.0f);

		FViewInfo& view = FaceViews[2];
		view.ViewMatrix = transform.ToMatrixTransform().GetInverse();
		view.ProjectionMatrix = RpgMatrixProjection::CreatePerspective(1.0f, fovDegree, nearClipZ, farClipZ);
		view.FarClipZ = farClipZ;
		view.CameraId = worldResource->AddCamera(view.ViewMatrix, view.ProjectionMatrix, transform.Position, nearClipZ, farClipZ);
	}
	// -Y
	{
		transform.Rotation = RpgQuaternion::FromPitchYawRollDegree(90.0f, 0.0f, 0.0f);

		FViewInfo& view = FaceViews[3];
		view.ViewMatrix = transform.ToMatrixTransform().GetInverse();
		view.ProjectionMatrix = RpgMatrixProjection::CreatePerspective(1.0f, fovDegree, nearClipZ, farClipZ);
		view.FarClipZ = farClipZ;
		view.CameraId = worldResource->AddCamera(view.ViewMatrix, view.ProjectionMatrix, transform.Position, nearClipZ, farClipZ);
	}
	// +Z
	{
		transform.Rotation = RpgQuaternion::FromPitchYawRollDegree(0.0f, 0.0f, 0.0f);

		FViewInfo& view = FaceViews[4];
		view.ViewMatrix = transform.ToMatrixTransform().GetInverse();
		view.ProjectionMatrix = RpgMatrixProjection::CreatePerspective(1.0f, fovDegree, nearClipZ, farClipZ);
		view.FarClipZ = farClipZ;
		view.CameraId = worldResource->AddCamera(view.ViewMatrix, view.ProjectionMatrix, transform.Position, nearClipZ, farClipZ);
	}
	// -Z
	{
		transform.Rotation = RpgQuaternion::FromPitchYawRollDegree(0.0f, 180.0f, 0.0f);

		FViewInfo& view = FaceViews[5];
		view.ViewMatrix = transform.ToMatrixTransform().GetInverse();
		view.ProjectionMatrix = RpgMatrixProjection::CreatePerspective(1.0f, fovDegree, nearClipZ, farClipZ);
		view.FarClipZ = farClipZ;
		view.CameraId = worldResource->AddCamera(view.ViewMatrix, view.ProjectionMatrix, transform.Position, nearClipZ, farClipZ);
	}


	for (int i = 0; i < 6; ++i)
	{
		const RpgD3D12::FResourceDescriptor shadowDepthDescriptor = RpgD3D12::AllocateDescriptor_TDI(frame.DepthTextures[i]->GPU_GetResource(), DXGI_FORMAT_R16_UNORM);
		FaceViews[i].ShadowTextureDescriptorIndex = shadowDepthDescriptor.Index;
		worldResource->SetLightShadow(lightIds[i], FaceViews[i].CameraId, shadowDepthDescriptor.Index);
	}


	// build draw calls
	for (auto it = world->Component_CreateConstIterator<RpgRenderComponent_Mesh>(); it; ++it)
	{
		const RpgRenderComponent_Mesh& comp = it.GetValue();
		if (!(comp.Model && comp.bIsVisible))
		{
			continue;
		}

		const RpgSharedModel& model = comp.Model;
		const RpgMatrixTransform worldTransformMatrix = world->GameObject_GetWorldTransformMatrix(comp.GameObject);

		for (int m = 0; m < model->GetMeshCount(); ++m)
		{
			const RpgSharedMesh& mesh = model->GetMeshLod(m, 0);
			RpgDrawIndexedDepth* draw = nullptr;

			if (mesh->HasSkin())
			{
				draw = &frame.DrawSkinnedMeshes.Add();
				frameContext.MeshSkinnedResource->AddMesh(mesh, draw->IndexCount, draw->IndexStart, draw->IndexVertexOffset);
			}
			else
			{
				draw = &frame.DrawMeshes.Add();
				frameContext.MeshResource->AddMesh(mesh, draw->IndexCount, draw->IndexStart, draw->IndexVertexOffset);
			}

			draw->ObjectParam.TransformIndex = worldResource->AddTransform(comp.GameObject.GetIndex(), worldTransformMatrix);
		}
	}
}


void RpgShadowViewport_PointLight::SetupRenderPasses(const RpgRenderFrameContext& frameContext, const RpgWorldResource* worldResource, const RpgWorld* world, RpgAsyncTask_RenderPassShadowArray& out_ShadowPasses) noexcept
{
	FFrameData& frame = FrameDatas[frameContext.Index];

	RpgAsyncTask_RenderPass_Shadow* shadowPass = &frame.AsyncTaskRenderPassShadow;
	shadowPass->Reset();
	shadowPass->FrameContext = frameContext;
	shadowPass->WorldResource = worldResource;

	for (int i = 0; i < 6; ++i)
	{
		shadowPass->DepthTextures.AddValue(frame.DepthTextures[i].Get());
		shadowPass->CameraIds.AddValue(FaceViews[i].CameraId);
	}

	shadowPass->DrawMeshData = frame.DrawMeshes.GetData();
	shadowPass->DrawMeshCount = frame.DrawMeshes.GetCount();
	shadowPass->DrawSkinnedMeshData = frame.DrawSkinnedMeshes.GetData();
	shadowPass->DrawSkinnedMeshCount = frame.DrawSkinnedMeshes.GetCount();
	shadowPass->bIsOmniDirectional = true;

	out_ShadowPasses.AddValue(shadowPass);
}
