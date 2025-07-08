#include "RpgShaderManager.h"
#include "core/RpgThreadPool.h"
#include "core/RpgFilePath.h"
#include "core/RpgConfig.h"


#define RPG_SHADER_Check(hr)	RPG_Check((hr) == S_OK)

RPG_LOG_DECLARE_CATEGORY_STATIC(RpgLogShader, VERBOSITY_DEBUG)



static const wchar_t* RPG_DXC_ENTRY_POINTS[RpgShader::TYPE_MAX_COUNT] =
{
	L"__none__",
	L"VS_Main",
	L"AS_Main",
	L"MS_Main",
	L"GS_Main",
	L"PS_Main",
	L"CS_Main"
};


static const wchar_t* RPG_DXC_TARGET_PROFILES[RpgShader::TYPE_MAX_COUNT] =
{
	L"__none__",
	L"vs_6_0",
	L"as_6_0",
	L"ms_6_0",
	L"gs_6_0",
	L"ps_6_0",
	L"cs_6_0",
};



class RpgAsyncTask_CompileShader : public RpgThreadTask
{
private:
	struct FDefine
	{
		wchar_t Name[32];
		wchar_t Value[32];
	};

	ComPtr<IDxcBlob> CodeBlob;

public:
	RpgName Name;
	RpgString FilePath;
	RpgShader::EType Type;
	RpgShader::FCompileMacros CompileMacros;


public:
	RpgAsyncTask_CompileShader() noexcept
	{
		Type = RpgShader::TYPE_NONE;
	}


	virtual void Reset() noexcept override
	{
		RpgThreadTask::Reset();

		CodeBlob.Reset();
		Name = "";
		FilePath = "";
		Type = RpgShader::TYPE_NONE;
		CompileMacros.Clear();
	}


	virtual void Execute() noexcept override
	{
		RPG_LogDebug(RpgLogShader, "[ThreadId-%u] Execute compile shader task\n\tName: %s\n\tFile: %s\n", SDL_GetCurrentThreadID(), *Name, *FilePath);
		RPG_Assert(Type >= RpgShader::TYPE_VERTEX && Type < RpgShader::TYPE_MAX_COUNT);
		
		RpgArrayInline<FDefine, 8> defines;
		defines.Resize(CompileMacros.GetCount());

		for (int i = 0; i < CompileMacros.GetCount(); ++i)
		{
			FDefine& def = defines[i];
			RpgPlatformMemory::CStringToWide(def.Name, *CompileMacros[i], 32);
			RpgPlatformMemory::CStringToWide(def.Value, "1", 32);
		}

		RpgArrayInline<DxcDefine, 8> dxcDefines;
		dxcDefines.Resize(defines.GetCount());

		for (int i = 0; i < defines.GetCount(); ++i)
		{
			DxcDefine& dxcDef = dxcDefines[i];
			dxcDef.Name = defines[i].Name;
			dxcDef.Value = defines[i].Value;
		}

		const wchar_t* entryPoint = RPG_DXC_ENTRY_POINTS[Type];
		const wchar_t* targetProfile = RPG_DXC_TARGET_PROFILES[Type];

		const wchar_t* args[] =
		{
			L"-HV", L"2021",		// Compiler version
			L"-Ges",				// Enable strict mode
			L"-Zpr",				// Matrix in row_major

		#ifdef RPG_BUILD_DEBUG
			L"-Qembed_debug",       // Embed PDB
			L"-Od",					// Disable optimizations
			L"-WX",					// Treat warnings as errors
			L"-Zi",					// Enable debug information

		#elif RPG_BUILD_DEVELOPMENT
			L"-Zs",					// Enable debug information slim format
			L"-O2",					// Optimization level 2

		#else // RPG_BUILD_SHIPPING
			L"-Qstrip_reflect",		// Strip reflection into separate blob
			L"-O3",					// Optimization level 3
			L"-Vd",					// Disable validation

		#endif // RPG_BUILD_DEBUG
		};


	#ifdef RPG_BUILD_DEBUG
		const DWORD fileVersionSize = GetFileVersionInfoSizeA("dxcompiler.dll", NULL);
		void* fileVersionData = RpgPlatformMemory::MemMalloc(fileVersionSize);
		if (GetFileVersionInfoA("dxcompiler.dll", 0, fileVersionSize, fileVersionData))
		{
			VS_FIXEDFILEINFO* ffi = nullptr;
			UINT len = 0;

			if (VerQueryValueA(fileVersionData, "\\", (LPVOID*)&ffi, &len) && len >= sizeof(VS_FIXEDFILEINFO))
			{
				DWORD major = HIWORD(ffi->dwFileVersionMS);
				DWORD minor = LOWORD(ffi->dwFileVersionMS);
				DWORD build = HIWORD(ffi->dwFileVersionLS);
				DWORD revision = LOWORD(ffi->dwFileVersionLS);
				RPG_Check(major >= 1 && minor >= 8);
			}
		}

		if (fileVersionData)
		{
			RpgPlatformMemory::MemFree(fileVersionData);
			fileVersionData = nullptr;
		}
	#endif // RPG_BUILD_DEBUG


		ComPtr<IDxcUtils> dxcUtils;
		RPG_SHADER_Check(DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(&dxcUtils)));

		wchar_t hlslFilePath[MAX_PATH];
		RpgPlatformMemory::CStringToWide(hlslFilePath, *FilePath, MAX_PATH);

		ComPtr<IDxcCompilerArgs> dxcCompilerArgs;
		RPG_SHADER_Check(dxcUtils->BuildArguments(hlslFilePath, entryPoint, targetProfile, args, _countof(args), dxcDefines.GetData(), static_cast<UINT32>(dxcDefines.GetCount()), &dxcCompilerArgs));

		ComPtr<IDxcIncludeHandler> dxcIncludeHandler;
		RPG_SHADER_Check(dxcUtils->CreateDefaultIncludeHandler(&dxcIncludeHandler));
		
		ComPtr<IDxcCompiler3> dxcCompiler;
		RPG_SHADER_Check(DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(&dxcCompiler)));

		UINT32 codePage = DXC_CP_UTF8;
		ComPtr<IDxcBlobEncoding> dxcSourceFileBlobEncoding;
		RPG_SHADER_Check(dxcUtils->LoadFile(hlslFilePath, &codePage, &dxcSourceFileBlobEncoding));

		DxcBuffer dxcSourceData{};
		dxcSourceData.Encoding = DXC_CP_UTF8;
		dxcSourceData.Ptr = dxcSourceFileBlobEncoding->GetBufferPointer();
		dxcSourceData.Size = dxcSourceFileBlobEncoding->GetBufferSize();

		ComPtr<IDxcResult> dxcResult;
		RPG_SHADER_Check(dxcCompiler->Compile(&dxcSourceData, dxcCompilerArgs->GetArguments(), dxcCompilerArgs->GetCount(), dxcIncludeHandler.Get(), IID_PPV_ARGS(&dxcResult)));

		ComPtr<IDxcBlobEncoding> dxcErrorBlob;
		RPG_SHADER_Check(dxcResult->GetErrorBuffer(&dxcErrorBlob));

		if (dxcErrorBlob && dxcErrorBlob->GetBufferSize())
		{
			RPG_LogError(RpgLogShader, "[ThreadId-%u] Compile shader FAILED!\n\tName: %s\n\tFile: %s\n\tMessage: %s\n", SDL_GetCurrentThreadID(), *Name, *FilePath, (const char*)dxcErrorBlob->GetBufferPointer());
			CodeBlob.Reset();
		}
		else
		{
			RPG_LogDebug(RpgLogShader, "[ThreadId-%u] Compile shader SUCCESS!\n\tName: %s\n\tFile: %s\n", SDL_GetCurrentThreadID(), *Name, *FilePath);
			RPG_SHADER_Check(dxcResult->GetOutput(DXC_OUT_OBJECT, IID_PPV_ARGS(&CodeBlob), nullptr));
		}
	}


	virtual const char* GetTaskName() const noexcept override
	{
		return "RpgAsyncTask_CompileShader";
	}


	[[nodiscard]] inline ComPtr<IDxcBlob> GetResult() const noexcept
	{
		return CodeBlob;
	}

};



namespace RpgShaderManager
{
	static bool bInitialized;


	struct FShaderData
	{
		enum ECompileState : uint8_t
		{
			COMPILE_STATE_NONE = 0,
			COMPILE_STATE_PENDING,
			COMPILE_STATE_COMPILING,
			COMPILE_STATE_COMPILED
		};

		ComPtr<IDxcBlob> CodeBlob;
		ECompileState State = COMPILE_STATE_NONE;
	};


	static RpgArray<RpgName> ShaderNames;
	static RpgArray<FShaderData> ShaderDatas;
	static RpgArray<RpgAsyncTask_CompileShader*> AsyncTaskCompileShaders;

};



void RpgShaderManager::Initialize() noexcept
{
	if (bInitialized)
	{
		return;
	}

	RPG_Log(RpgLogShader, "Initialize shader manager");

#ifndef RPG_BUILD_SHIPPING
	const RpgString hlslDirPath = RpgFileSystem::GetSourceDirPath() + "runtime/shader/hlsl/";

	AddShader(RPG_SHADER_DEFAULT_VS_PRIMITIVE_NAME, hlslDirPath + "VS_Primitive.hlsl", RpgShader::TYPE_VERTEX);
	AddShader(RPG_SHADER_DEFAULT_VS_MESH_NAME, hlslDirPath + "VS_Mesh.hlsl", RpgShader::TYPE_VERTEX);
	//AddShader(RPG_SHADER_DEFAULT_VS_MESH_SKINNED_NAME, hlslDirPath + "VS_MeshSkinned.hlsl", RpgShader::TYPE_VERTEX);
	AddShader(RPG_SHADER_DEFAULT_VS_PRIMITIVE_2D_NAME, hlslDirPath + "VS_Primitive2D.hlsl", RpgShader::TYPE_VERTEX);
	AddShader(RPG_SHADER_DEFAULT_VS_MESH_2D_NAME, hlslDirPath + "VS_Mesh2D.hlsl", RpgShader::TYPE_VERTEX);
	AddShader(RPG_SHADER_DEFAULT_VS_FULLSCREEN_NAME, hlslDirPath + "VS_Fullscreen.hlsl", RpgShader::TYPE_VERTEX);

	AddShader(RPG_SHADER_DEFAULT_PS_COLOR_NAME, hlslDirPath + "PS_Color.hlsl", RpgShader::TYPE_PIXEL);
	AddShader(RPG_SHADER_DEFAULT_PS_TEXTURE_COLOR_NAME, hlslDirPath + "PS_TextureColor.hlsl", RpgShader::TYPE_PIXEL);
	AddShader(RPG_SHADER_DEFAULT_PS_TEXTURE_FONT_NAME, hlslDirPath + "PS_TextureColor.hlsl", RpgShader::TYPE_PIXEL, { "FONT" });
	AddShader(RPG_SHADER_DEFAULT_PS_PHONG_NAME, hlslDirPath + "PS_ForwardPhong.hlsl", RpgShader::TYPE_PIXEL);
	AddShader(RPG_SHADER_DEFAULT_PS_FULLSCREEN_GAMMA_NAME, hlslDirPath + "PS_FullscreenGamma.hlsl", RpgShader::TYPE_PIXEL);

	AddShader(RPG_SHADER_DEFAULT_CS_SKINNING_NAME, hlslDirPath + "CS_Skinning.hlsl", RpgShader::TYPE_COMPUTE);

	CompileShaders(true);

#else
	// TODO: Read compiled shader code bytes

#endif // !RPG_BUILD_SHIPPING

	bInitialized = true;
}


void RpgShaderManager::Shutdown() noexcept
{
	if (!bInitialized)
	{
		return;
	}

	RPG_Log(RpgLogShader, "Shutdown shader manager");

	bInitialized = false;
}


void RpgShaderManager::AddShader(const RpgName& in_Name, const RpgString& in_HlslFilePath, RpgShader::EType in_Type, RpgShader::FCompileMacros optIn_CompileMacros) noexcept
{
	if (ShaderNames.FindIndexByValue(in_Name) != RPG_INDEX_INVALID)
	{
		RPG_LogWarn(RpgLogShader, "Ignore add shader. Shader with name [%s] already exists!", *in_Name);
		return;
	}

	ShaderNames.AddValue(in_Name);
	ShaderDatas.AddValue({ nullptr, FShaderData::COMPILE_STATE_PENDING });
	
	RpgAsyncTask_CompileShader* task = RpgThreadPool::CreateTask<RpgAsyncTask_CompileShader>();
	task->Name = in_Name;
	task->FilePath = in_HlslFilePath;
	task->Type = in_Type;
	task->CompileMacros = optIn_CompileMacros;

	AsyncTaskCompileShaders.AddValue(task);
}


void RpgShaderManager::CompileShaders(bool bWaitAll) noexcept
{
	RpgArrayInline<RpgThreadTask*, 16> taskToSubmits;

	for (int i = 0; i < ShaderDatas.GetCount(); ++i)
	{
		FShaderData& data = ShaderDatas[i];

		if (data.State == FShaderData::COMPILE_STATE_PENDING)
		{
			data.State = FShaderData::COMPILE_STATE_COMPILING;
			taskToSubmits.AddValue(AsyncTaskCompileShaders[i]);
		}
	}

	if (!taskToSubmits.IsEmpty())
	{
		RpgThreadPool::SubmitTasks(taskToSubmits.GetData(), taskToSubmits.GetCount());
	}

	for (int i = 0; i < ShaderDatas.GetCount(); ++i)
	{
		FShaderData& data = ShaderDatas[i];
		if (data.State != FShaderData::COMPILE_STATE_COMPILING)
		{
			continue;
		}

		RpgAsyncTask_CompileShader* task = AsyncTaskCompileShaders[i];
		bool bCompileDone = task->IsDone();

		if (!bCompileDone && bWaitAll)
		{
			AsyncTaskCompileShaders[i]->Wait();
			bCompileDone = true;
		}

		if (bCompileDone)
		{
			data.CodeBlob = task->GetResult();
			data.State = FShaderData::COMPILE_STATE_COMPILED;
			task->Reset();
		}
	}
}


bool RpgShaderManager::DoesShaderExists(const RpgName& name) noexcept
{
	return ShaderNames.FindIndexByValue(name) != RPG_INDEX_INVALID;
}


IDxcBlob* RpgShaderManager::GetShaderCodeBlob(const RpgName& name) noexcept
{
	const int index = ShaderNames.FindIndexByValue(name);
	if (index == RPG_INDEX_INVALID)
	{
		return nullptr;
	}

	return ShaderDatas[index].CodeBlob.Get();
}
