#pragma once

#include "core/RpgString.h"

#include <wrl/client.h>
using namespace Microsoft::WRL;

#include <dxcapi.h>



namespace RpgShader
{
    typedef RpgArrayInline<RpgName, 8> FCompileMacros;


    enum EType : uint8_t
    {
        TYPE_NONE = 0,
        TYPE_VERTEX,
        TYPE_AMPLIFICATION,
        TYPE_MESH,
        TYPE_GEOMETRY,
        TYPE_PIXEL,
        TYPE_COMPUTE,
        TYPE_MAX_COUNT
    };
};



namespace RpgShaderManager
{
	extern void Initialize() noexcept;
	extern void Shutdown() noexcept;

	extern void AddShader(const RpgName& in_Name, const RpgString& in_HlslFilePath, RpgShader::EType in_Type, RpgShader::FCompileMacros optIn_CompileMacros = RpgShader::FCompileMacros()) noexcept;
	extern void CompileShaders(bool bWaitAll) noexcept;

	[[nodiscard]] extern bool DoesShaderExists(const RpgName& name) noexcept;
	[[nodiscard]] extern IDxcBlob* GetShaderCodeBlob(const RpgName& name) noexcept;

};
