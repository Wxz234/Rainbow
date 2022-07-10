//
// Copyright (c) Wxz.
// Licensed under the MIT License.
//
// https://github.com/Wxz234/Rainbow
//

#include "../Include/Rainbow/Runtime/Scene.h"
#include "../Include/Rainbow/Runtime/ModelObject.h"

//#include "../Shader/VertexShader.h"

#include "LoadGLTF.h"

#include <cassert>

namespace Rainbow {
	void CreateScene(Device* pDevice, Scene** ppScene) {
		assert(pDevice);
		assert(ppScene);
		Scene* pScene = new Scene(pDevice);
		*ppScene = pScene;
	}
	void RemoveScene(Scene* pScene) {
		assert(pScene);	
		delete pScene;
	}

	Scene::Scene(Device* pDevice) {
		this->pDevice = pDevice;
		pGQueue = pDevice->pQueue;
		CreateQueue(pDevice, COMMAND_TYPE_COMPUTE, &pCQueue);

		//D3DX12_MESH_SHADER_PIPELINE_STATE_DESC meshDesc = {};
		//meshDesc.pRootSignature = rootsignature.Get();
		//meshDesc.MS = { TestMeshShader,sizeof(TestMeshShader) };
		//meshDesc.PS = { PostProcessPS,sizeof(PostProcessPS) };
		//meshDesc.NumRenderTargets = 1;
		//meshDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
		//meshDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);    // CW front; cull back
		//meshDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);         // Opaque
		//meshDesc.DepthStencilState.DepthEnable = FALSE;
		//meshDesc.DepthStencilState.StencilEnable = FALSE;
		//meshDesc.SampleMask = UINT_MAX;
		//meshDesc.SampleDesc = DefaultSampleDesc();

		//auto psoStream = CD3DX12_PIPELINE_MESH_STATE_STREAM(meshDesc);

		D3D12_GRAPHICS_PIPELINE_STATE_DESC pipelineDesc = {};

		//D3D12_PIPELINE_STATE_STREAM_DESC streamDesc;
		//streamDesc.pPipelineStateSubobjectStream = &psoStream;
		//streamDesc.SizeInBytes = sizeof(psoStream);
	}

	Scene::~Scene() {
		for (auto &x: mGameObject) {
			delete x;
		}

		RemoveQueue(pCQueue, true);
	}

	void Scene::LoadModelFromFile(const char* file) {
		GLTFLoader loader(file);
		auto submesh_size = loader.GetSubMeshSize();

		ModelObject* mo = new ModelObject;
		for (uint32_t i = 0;i < submesh_size; ++i) {
			mo->AddSubMesh(loader.GetSubMesh(i));
		}

		mGameObject.push_back(mo);
	}

	void Scene::Draw() {

	}
}