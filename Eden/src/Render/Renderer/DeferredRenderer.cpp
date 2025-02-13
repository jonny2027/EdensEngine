#include "Render/Renderer/DeferredRenderer.h"

DeferredRenderer::DeferredRenderer(GraphicsManager *graphicsManager)
{
	mGraphicsManager = graphicsManager;

	Direct3DManager *direct3DManager = mGraphicsManager->GetDirect3DManager();

	{
		CD3DX12_DESCRIPTOR_RANGE range;
		CD3DX12_ROOT_PARAMETER parameter;

		range.Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 0);
		parameter.InitAsDescriptorTable(1, &range, D3D12_SHADER_VISIBILITY_VERTEX);

		D3D12_ROOT_SIGNATURE_FLAGS rootSignatureFlags =
			D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT | // Only the input assembler stage needs access to the constant buffer.
			D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS |
			D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS |
			D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS |
			D3D12_ROOT_SIGNATURE_FLAG_DENY_PIXEL_SHADER_ROOT_ACCESS;

		CD3DX12_ROOT_SIGNATURE_DESC descRootSignature;
		descRootSignature.Init(1, &parameter, 0, nullptr, rootSignatureFlags);

		ID3DBlob *pSignature;
		ID3DBlob *pError;
		Direct3DUtils::ThrowIfHRESULTFailed(D3D12SerializeRootSignature(&descRootSignature, D3D_ROOT_SIGNATURE_VERSION_1, &pSignature, &pError));
		Direct3DUtils::ThrowIfHRESULTFailed(direct3DManager->GetDevice()->CreateRootSignature(0, pSignature->GetBufferPointer(), pSignature->GetBufferSize(), IID_PPV_ARGS(&mRootSignature)));

		pSignature->Release();
		pSignature = NULL;

		if (pError)
		{
			pError->Release();
			pSignature = NULL;
		}
	}

	//Direct3DUtils::ThrowIfHRESULTFailed(direct3DManager->GetDevice()->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, direct3DManager->GetCommandAllocator(), NULL, IID_PPV_ARGS(&mCommandList)));
	//mCommandList->Close();
	direct3DManager->WaitForGPU();

}

DeferredRenderer::~DeferredRenderer()
{
	mRootSignature->Release();
	mRootSignature = NULL;

	//mCommandList->Release();
	//mCommandList = NULL;
}

void DeferredRenderer::Render()
{
	Direct3DManager *direct3DManager = mGraphicsManager->GetDirect3DManager();

	ID3D12GraphicsCommandList *commandList = direct3DManager->GetCommandList();

	Direct3DUtils::ThrowIfHRESULTFailed(direct3DManager->GetCommandAllocator()->Reset());

	// The command list can be reset anytime after ExecuteCommandList() is called.
	Direct3DUtils::ThrowIfHRESULTFailed(commandList->Reset(direct3DManager->GetCommandAllocator(), NULL));

	D3D12_VIEWPORT viewport = direct3DManager->GetScreenViewport();
	commandList->RSSetViewports(1, &viewport);
	//mCommandList->RSSetScissorRects(1, &m_scissorRect);

	// Indicate this resource will be in use as a render target.
	CD3DX12_RESOURCE_BARRIER renderTargetResourceBarrier =
		CD3DX12_RESOURCE_BARRIER::Transition(direct3DManager->GetBackBufferTarget(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
	commandList->ResourceBarrier(1, &renderTargetResourceBarrier);

	// Record drawing commands.
	float color[4] = {0.392156899f, 0.584313750f, 0.929411829f, 1.000000000f};

	commandList->ClearRenderTargetView(direct3DManager->GetRenderTargetView(), color, 0, nullptr);

	CD3DX12_RESOURCE_BARRIER presentResourceBarrier =
		CD3DX12_RESOURCE_BARRIER::Transition(direct3DManager->GetBackBufferTarget(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
	commandList->ResourceBarrier(1, &presentResourceBarrier);

	Direct3DUtils::ThrowIfHRESULTFailed(commandList->Close());

	ID3D12CommandList* ppCommandLists[] = { commandList };
	direct3DManager->GetCommandQueue()->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);
}