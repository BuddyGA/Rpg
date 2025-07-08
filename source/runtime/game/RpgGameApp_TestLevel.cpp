#include "RpgGameApp.h"
#include "asset/RpgAssetImporter.h"
#include "core/world/RpgWorld.h"
#include "render/world/RpgRenderComponent.h"
#include "animation/world/RpgAnimationComponent.h"



static void TestLevel_PrimitiveShapes(RpgWorld* world) noexcept
{
	RpgSharedModel boxModel = RpgModel::s_CreateShared("MDL_DEF_box");
	{
		boxModel->AddLod();
		boxModel->AddMeshEmpty();

		RpgVertexMeshPositionArray vertexPositions;
		RpgVertexMeshNormalTangentArray vertexNormalTangents;
		RpgVertexMeshTexCoordArray vertexTexCoords;
		RpgVertexIndexArray indices;
		RpgVertexGeometryFactory::CreateMeshBox(vertexPositions, vertexNormalTangents, vertexTexCoords, indices, RpgVector3(-64.0f), RpgVector3(64.0f));

		RpgSharedMesh& mesh = boxModel->GetMeshLod(0, 0);
		mesh->UpdateVertexData(vertexPositions.GetCount(), vertexPositions.GetData(), vertexNormalTangents.GetData(), vertexTexCoords.GetData(), nullptr, indices.GetCount(), indices.GetData());

		boxModel->SetMaterial(0, RpgMaterial::s_GetDefault(RpgMaterialDefault::MESH_PHONG));
	}

	const RpgGameObjectID box0 = world->GameObject_Create("box_0", RpgTransform());
	{
		// Add render component
		RpgRenderComponent_Mesh* meshComp = world->GameObject_AddComponent<RpgRenderComponent_Mesh>(box0);
		meshComp->Model = boxModel;
		meshComp->bIsVisible = true;
	}
}


static void TestLevel_OBJ(RpgWorld* world, const RpgFilePath& sourceFilePath, float scale) noexcept
{
	RpgAssetImportSetting_Model setting;
	setting.SourceFilePath = sourceFilePath;
	setting.Scale = scale;
	setting.bImportMaterialTexture = true;
	setting.bImportSkeleton = true;
	setting.bImportAnimation = true;
	setting.bGenerateTextureMipMaps = false;

	RpgArray<RpgSharedModel> importedModels;
	RpgSharedAnimationSkeleton importedSkeleton;
	RpgArray<RpgSharedAnimationClip> importedAnimations;
	g_AssetImporter->ImportModel(importedModels, importedSkeleton, importedAnimations, setting);

	for (int i = 0; i < importedModels.GetCount(); ++i)
	{
		const RpgSharedModel& model = importedModels[i];

		RpgTransform transform;
		transform.Position = RpgVector3(0.0f, 0.0f, 0.0f);
		transform.Rotation = RpgQuaternion::FromPitchYawRollDegree(0.0f, 0.0f, 0.0f);

		RpgGameObjectID gameObject = world->GameObject_Create(model->GetName(), transform);
		RpgRenderComponent_Mesh* meshComp = world->GameObject_AddComponent<RpgRenderComponent_Mesh>(gameObject);
		meshComp->Model = model;
		meshComp->bIsVisible = true;

		if (model->HasSkin())
		{
			RPG_PLATFORM_Check(importedSkeleton);
			RpgAnimationComponent* animComp = world->GameObject_AddComponent<RpgAnimationComponent>(gameObject);
			animComp->SetSkeleton(importedSkeleton);
			animComp->Clip = importedAnimations[0];
			animComp->PlayRate = 1.0f;
			animComp->bLoopAnim = true;
		}
	}

	g_AssetImporter->Reset();
}



static void TestLevel_Animations(RpgWorld* world) noexcept
{
	RpgSharedModel models[2];
	RpgSharedAnimationSkeleton skeletons[2];
	RpgSharedAnimationClip animationClips[2];

	RpgAssetImportSetting_Model setting;
	setting.SourceFilePath = RpgFileSystem::GetAssetRawDirPath() + "model/CesiumMan.glb";
	setting.Scale = 100.0f;
	setting.bImportMaterialTexture = true;
	setting.bImportSkeleton = true;
	setting.bImportAnimation = true;
	setting.bGenerateTextureMipMaps = true;

	RpgArray<RpgSharedModel> importedModels;
	RpgSharedAnimationSkeleton importedSkeleton;
	RpgArray<RpgSharedAnimationClip> importedAnimations;
	g_AssetImporter->ImportModel(importedModels, importedSkeleton, importedAnimations, setting);
	models[0] = importedModels[0];
	skeletons[0] = importedSkeleton;
	animationClips[0] = importedAnimations[0];
	
	setting.SourceFilePath = RpgFileSystem::GetAssetRawDirPath() + "model/RiggedFigure.glb";
	g_AssetImporter->ImportModel(importedModels, importedSkeleton, importedAnimations, setting);
	models[1] = importedModels[0];
	skeletons[1] = importedSkeleton;
	animationClips[1] = importedAnimations[0];

	const int DIM_X = 16;
	const int DIM_Z = 16;
	const float OFFSET = 128.0f;
	const RpgVector3 startPos(-(DIM_X * OFFSET * 0.5f), 0.0f, -(DIM_Z * OFFSET * 0.5f));
	RpgVector3 spawnPos = startPos;
	int modelIndex = 0;

	for (int x = 0; x < DIM_X; ++x)
	{
		spawnPos.Z = startPos.Z;

		for (int z = 0; z < DIM_Z; ++z)
		{
			RpgTransform transform;
			transform.Position = spawnPos;
			transform.Rotation = RpgQuaternion::FromPitchYawRollDegree(90.0f, 0.0f, 0.0f);

			RpgGameObjectID gameObject = world->GameObject_Create(RpgName::Format("test_%i_%i", x, z), transform);

			RpgRenderComponent_Mesh* meshComp = world->GameObject_AddComponent<RpgRenderComponent_Mesh>(gameObject);
			meshComp->Model = models[modelIndex];
			meshComp->bIsVisible = true;

			RpgAnimationComponent* animComp = world->GameObject_AddComponent<RpgAnimationComponent>(gameObject);
			animComp->SetSkeleton(skeletons[modelIndex]);
			animComp->Clip = animationClips[modelIndex];
			animComp->PlayRate = 1.5f;
			animComp->bLoopAnim = true;

			spawnPos.Z += OFFSET;
			modelIndex = (modelIndex + 1) % 2;
		}

		spawnPos.X += OFFSET;
	}
}


void RpgGameApp::CreateTestLevel() noexcept
{
	//TestLevel_PrimitiveShapes(MainWorld);
	//TestLevel_OBJ(MainWorld, RpgFileSystem::GetAssetRawDirPath() + "model/sponza_phong/sponza.obj", 1.0f);
	//TestLevel_OBJ(MainWorld, RpgFileSystem::GetAssetRawDirPath() + "model/san_miguel/san-miguel-low-poly.obj", 100.0f);
	//TestLevel_OBJ(MainWorld, RpgFileSystem::GetAssetRawDirPath() + "model/lost_empire/lost_empire.obj", 100.0f);
	//TestLevel_OBJ(MainWorld, RpgFileSystem::GetAssetRawDirPath() + "model/sibenik/sibenik.obj", 100.0f);
	//TestLevel_OBJ(MainWorld, RpgFileSystem::GetAssetRawDirPath() + "model/bunny/bunny.obj", 100.0f);
	//TestLevel_OBJ(MainWorld, RpgFileSystem::GetAssetRawDirPath() + "model/RiggedFigure.glb", 100.0f);
	//TestLevel_OBJ(MainWorld, RpgFileSystem::GetAssetRawDirPath() + "model/CesiumMilkTruck.glb", 100.0f);

	TestLevel_Animations(MainWorld);
}
