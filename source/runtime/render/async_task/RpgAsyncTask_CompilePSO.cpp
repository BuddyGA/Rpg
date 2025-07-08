#include "RpgAsyncTask_CompilePSO.h"
#include "../RpgRenderPipeline.h"
#include "shader/RpgShaderManager.h"



RpgAsyncTask_CompilePSO::RpgAsyncTask_CompilePSO() noexcept
{
	RootSignature = nullptr;
}


void RpgAsyncTask_CompilePSO::Reset() noexcept
{
	RpgThreadTask::Reset();

	RootSignature = nullptr;
	PSO.Reset();
}


void RpgAsyncTask_CompilePSO::Execute() noexcept
{
	RPG_LogDebug(RpgLogD3D12, "[ThreadId-%u] Execute compile PSO task for Material: %s", SDL_GetCurrentThreadID(), *MaterialName);

	D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc{};
	psoDesc.NodeMask = 0;
	psoDesc.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;
	psoDesc.pRootSignature = RootSignature;
	psoDesc.PrimitiveTopologyType = (MaterialRenderState.RasterMode == RpgMaterialRasterMode::LINE) ? D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE : D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

	// Vertex input elements
	RpgName vertexShaderName = RPG_SHADER_DEFAULT_VS_FULLSCREEN_NAME;
	RpgArrayInline<D3D12_INPUT_ELEMENT_DESC, 10> vertexInputElements;
	{
		switch (MaterialRenderState.VertexMode)
		{
			case RpgMaterialVertexMode::PRIMITIVE_2D:
			{
				vertexShaderName = RPG_SHADER_DEFAULT_VS_PRIMITIVE_2D_NAME;

				vertexInputElements.AddValue({ "POSITION", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 });
				vertexInputElements.AddValue({ "COLOR", 0, DXGI_FORMAT_R8G8B8A8_UNORM, 0, 8, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 });

				break;
			}

			case RpgMaterialVertexMode::MESH_2D:
			{
				vertexShaderName = RPG_SHADER_DEFAULT_VS_MESH_2D_NAME;

				vertexInputElements.AddValue({ "POSITION", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 });
				vertexInputElements.AddValue({ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 8, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 });
				vertexInputElements.AddValue({ "COLOR", 0, DXGI_FORMAT_R8G8B8A8_UNORM, 0, 16, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 });

				break;
			}

			case RpgMaterialVertexMode::PRIMITIVE:
			{
				vertexShaderName = RPG_SHADER_DEFAULT_VS_PRIMITIVE_NAME;

				vertexInputElements.AddValue({ "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 });
				vertexInputElements.AddValue({ "COLOR", 0, DXGI_FORMAT_R8G8B8A8_UNORM, 0, 16, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 });

				break;
			}

			case RpgMaterialVertexMode::MESH:
			{
				vertexShaderName = RPG_SHADER_DEFAULT_VS_MESH_NAME;

				// Position
				vertexInputElements.AddValue({ "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 });

				// Normal, Tangent
				vertexInputElements.AddValue({ "NORMAL", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 });
				vertexInputElements.AddValue({ "TANGENT", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 16, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 });

				// TexCoord
				vertexInputElements.AddValue({ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 2, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 });

				break;
			}

			case RpgMaterialVertexMode::SKELETAL_MESH:
			{
				RPG_CheckV(0, "Not using this anymore!");

				vertexShaderName = RPG_SHADER_DEFAULT_VS_MESH_SKINNED_NAME;

				// Position
				vertexInputElements.AddValue({ "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 });

				// Normal, Tangent
				vertexInputElements.AddValue({ "NORMAL", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 });
				vertexInputElements.AddValue({ "TANGENT", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 16, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 });

				// TexCoord
				vertexInputElements.AddValue({ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 2, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 });

				// Skins
				vertexInputElements.AddValue({ "BONEWEIGHTS", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 3, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 });
				vertexInputElements.AddValue({ "BONEWEIGHTS", 1, DXGI_FORMAT_R32G32B32A32_FLOAT, 3, 16, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 });
				vertexInputElements.AddValue({ "BONEINDICES", 0, DXGI_FORMAT_R8G8B8A8_UINT, 3, 32, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 });
				vertexInputElements.AddValue({ "BONEINDICES", 1, DXGI_FORMAT_R8G8B8A8_UINT, 3, 36, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 });
				vertexInputElements.AddValue({ "BONECOUNT", 0, DXGI_FORMAT_R8_UINT, 3, 40, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 });

				break;
			}

			default:
				break;
		}

		psoDesc.InputLayout.pInputElementDescs = vertexInputElements.GetData();
		psoDesc.InputLayout.NumElements = static_cast<UINT>(vertexInputElements.GetCount());
	}


	// Shader state
	{
		// Vertex shader
		IDxcBlob* vertexShaderCodeBlob = RpgShaderManager::GetShaderCodeBlob(vertexShaderName);
		RPG_Assert(vertexShaderCodeBlob);
		psoDesc.VS.pShaderBytecode = vertexShaderCodeBlob->GetBufferPointer();
		psoDesc.VS.BytecodeLength = vertexShaderCodeBlob->GetBufferSize();

		// Pixel shader
		if (MaterialRenderState.PixelShaderName.GetLength() > 0)
		{
			IDxcBlob* pixelShaderCodeBlob = RpgShaderManager::GetShaderCodeBlob(MaterialRenderState.PixelShaderName);
			RPG_Assert(pixelShaderCodeBlob);
			psoDesc.PS.pShaderBytecode = pixelShaderCodeBlob->GetBufferPointer();
			psoDesc.PS.BytecodeLength = pixelShaderCodeBlob->GetBufferSize();
		}
	}


	// Rasterizer state
	{
		psoDesc.RasterizerState.CullMode = MaterialRenderState.bTwoSides ? D3D12_CULL_MODE_NONE : D3D12_CULL_MODE_BACK;
		psoDesc.RasterizerState.FrontCounterClockwise = FALSE;
		psoDesc.RasterizerState.DepthBias = static_cast<INT>(MaterialRenderState.DepthBias);
		psoDesc.RasterizerState.SlopeScaledDepthBias = MaterialRenderState.DepthBiasSlope;
		psoDesc.RasterizerState.DepthBiasClamp = MaterialRenderState.DepthBiasClamp;
		psoDesc.RasterizerState.DepthClipEnable = MaterialRenderState.bDepthWrite;
		psoDesc.RasterizerState.AntialiasedLineEnable = MaterialRenderState.RasterMode == RpgMaterialRasterMode::LINE;
		psoDesc.RasterizerState.MultisampleEnable = FALSE;
		psoDesc.RasterizerState.ForcedSampleCount = 0;
		psoDesc.RasterizerState.ConservativeRaster = MaterialRenderState.bConservativeRasterization ? D3D12_CONSERVATIVE_RASTERIZATION_MODE_ON : D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;
		psoDesc.RasterizerState.FillMode = (MaterialRenderState.RasterMode == RpgMaterialRasterMode::SOLID) ? D3D12_FILL_MODE_SOLID : D3D12_FILL_MODE_WIREFRAME;
	}


	// Render target state
	psoDesc.NumRenderTargets = MaterialRenderState.RenderTargetCount;
	psoDesc.BlendState.AlphaToCoverageEnable = FALSE;
	const RpgMaterialBlendMode blendMode = MaterialRenderState.BlendMode;

	for (UINT r = 0; r < psoDesc.NumRenderTargets; ++r)
	{
		psoDesc.RTVFormats[r] = DXGI_FORMAT_R8G8B8A8_UNORM;

		D3D12_RENDER_TARGET_BLEND_DESC& renderTargetBlendDesc = psoDesc.BlendState.RenderTarget[r];
		renderTargetBlendDesc.BlendEnable = FALSE;
		renderTargetBlendDesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

		switch (blendMode)
		{
			case RpgMaterialBlendMode::OPACITY_MASK:
			{
				renderTargetBlendDesc.BlendEnable = TRUE;
				renderTargetBlendDesc.SrcBlend = D3D12_BLEND_SRC_ALPHA;
				renderTargetBlendDesc.BlendOp = D3D12_BLEND_OP_ADD;
				renderTargetBlendDesc.DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
				renderTargetBlendDesc.SrcBlendAlpha = D3D12_BLEND_ONE;
				renderTargetBlendDesc.BlendOpAlpha = D3D12_BLEND_OP_MAX;
				renderTargetBlendDesc.DestBlendAlpha = D3D12_BLEND_ONE;

				break;
			}

			case RpgMaterialBlendMode::FADE:
			{
				renderTargetBlendDesc.BlendEnable = TRUE;
				renderTargetBlendDesc.SrcBlend = D3D12_BLEND_SRC_ALPHA;
				renderTargetBlendDesc.BlendOp = D3D12_BLEND_OP_ADD;
				renderTargetBlendDesc.DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
				renderTargetBlendDesc.SrcBlendAlpha = D3D12_BLEND_ONE;
				renderTargetBlendDesc.BlendOpAlpha = D3D12_BLEND_OP_ADD;
				renderTargetBlendDesc.DestBlendAlpha = D3D12_BLEND_INV_SRC_ALPHA;

				break;
			}

			case RpgMaterialBlendMode::TRANSPARENCY:
			{
				renderTargetBlendDesc.BlendEnable = TRUE;
				renderTargetBlendDesc.SrcBlend = D3D12_BLEND_SRC_ALPHA;
				renderTargetBlendDesc.BlendOp = D3D12_BLEND_OP_ADD;
				renderTargetBlendDesc.DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
				renderTargetBlendDesc.SrcBlendAlpha = D3D12_BLEND_ONE;
				renderTargetBlendDesc.BlendOpAlpha = D3D12_BLEND_OP_ADD;
				renderTargetBlendDesc.DestBlendAlpha = D3D12_BLEND_INV_SRC_ALPHA;

				break;
			}

			default:
				break;
		}
	}


	// Depth stencil state
	{
		psoDesc.DSVFormat = MaterialRenderState.bStencilTest ? DXGI_FORMAT_D24_UNORM_S8_UINT : DXGI_FORMAT_D32_FLOAT;
		psoDesc.DepthStencilState.DepthEnable = MaterialRenderState.bDepthTest;
		psoDesc.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
		psoDesc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;
		psoDesc.DepthStencilState.StencilEnable = MaterialRenderState.bStencilTest;
	}


	// Sample state
	{
		psoDesc.SampleDesc.Count = 1;
		psoDesc.SampleDesc.Quality = 0;
		psoDesc.SampleMask = UINT32_MAX;
	}

	RPG_D3D12_Validate(RpgD3D12::GetDevice()->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&PSO)));
	RPG_D3D12_SetDebugName(PSO, "PSO_%s", *MaterialName);

	RPG_LogDebug(RpgLogD3D12, "[ThreadId-%u] Compiled PSO for material: %s", SDL_GetCurrentThreadID(), *MaterialName);
}
