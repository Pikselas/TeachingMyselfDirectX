#include<Windows.h>
#include<d3d11.h>
#include<chrono>
#include<wrl.h>
#include<d3dcompiler.h>
#include<sstream>
#include<DirectXMath.h>
#include<chrono>
#pragma comment(lib,"d3d11.lib")
#pragma comment(lib,"D3DCompiler.lib")

LRESULT _stdcall dfWnd(HWND handle, UINT msgCode, WPARAM wparam, LPARAM lparam)
{
	if (msgCode == WM_CLOSE)
	{
		PostQuitMessage(10);
	}
	return DefWindowProc(handle, msgCode, wparam, lparam);
}
int _stdcall WinMain(HINSTANCE hinstance, HINSTANCE hprev, LPSTR lpcmd, int cmdshow)
{
	try
	{
		WNDCLASSEX wc = { 0 };
		wc.cbSize = sizeof(wc);
		wc.style = CS_OWNDC;
		wc.lpfnWndProc = dfWnd;
		wc.cbClsExtra = 0;
		wc.cbWndExtra = 0;
		wc.hInstance = hinstance;
		wc.hIcon = nullptr;
		wc.hCursor = nullptr;
		wc.hbrBackground = nullptr;
		wc.lpszMenuName = nullptr;
		wc.lpszClassName = "ks_Class";
		wc.hIconSm = nullptr;
		RegisterClassEx(&wc);

		RECT rc = {};
		rc.right = 800;
		rc.bottom = 600;

		AdjustWindowRect(&rc, WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU, false);

		HWND handle = CreateWindowEx(0, "ks_Class", "KS_Window", WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU, CW_USEDEFAULT, CW_USEDEFAULT, rc.right - rc.left, rc.bottom - rc.top, nullptr, nullptr, hinstance, nullptr);
		ShowWindow(handle, SW_SHOW);
		MSG msg;
		Microsoft::WRL::ComPtr<ID3D11Device> pDevice;
		Microsoft::WRL::ComPtr<IDXGISwapChain> pSwapChain;
		Microsoft::WRL::ComPtr<ID3D11DeviceContext> pDeviceContext;
		Microsoft::WRL::ComPtr<ID3D11Resource> pBackBuffer;
		Microsoft::WRL::ComPtr<ID3D11RenderTargetView> pTarget;
		DXGI_SWAP_CHAIN_DESC sd = { 0 };
		sd.BufferDesc.Width = 0;  // look at the window and use it's size
		sd.BufferDesc.Height = 0;
		sd.BufferDesc.RefreshRate.Numerator = 0; // pick the default refresh rates
		sd.BufferDesc.RefreshRate.Denominator = 0;
		sd.BufferCount = 1;  // one back buffer -> one back and one front double buffering
		sd.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM; // this is the color format (BGRA) 
		sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED; // not specifying any scaling because we want the renedred frame same as window size
		sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED; // how buffer scaning will be done for copying all to video memory
		sd.Flags = 0; // not setting any flags
		sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT; // use the buffer for render target
		sd.OutputWindow = handle;
		sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD; // discard the effects for swapping frames
		sd.Windowed = TRUE;
		// for antialiasing we don't want it right now
		sd.SampleDesc.Count = 1;
		sd.SampleDesc.Quality = 0;

		if (auto hrcode = D3D11CreateDeviceAndSwapChain(0, D3D_DRIVER_TYPE_HARDWARE, nullptr, 0, nullptr, 0, D3D11_SDK_VERSION, &sd, &pSwapChain, &pDevice, nullptr, &pDeviceContext); FAILED(hrcode))
		{
			throw hrcode;
		}

		pSwapChain->GetBuffer(0, __uuidof(ID3D11Resource), &pBackBuffer);
		pDevice->CreateRenderTargetView(pBackBuffer.Get(), nullptr, &pTarget);

		// base structure for every vertex type
		struct VertexType
		{
			float x, y, z;
			unsigned char r, g, b, a;
		};

		//creating an array of vertices that will be drawn
		//below points are position for a triangle showing at the middle of the screen
		//viewport's range is from -1.0f to 1.0f (x,y,z axis)
		//triangle will be drawn using vertices at clockwise

		VertexType vtx[] = {
			{0.0 , 0.5 , 0.0 , 255 , 0 , 0 },
			{0.5 , -0.5 , 0.0 , 0 , 255 , 0 },
			{-0.5 , -0.5 , 0.0 ,  0 , 0, 255 },
			{-0.5 , 0.0  ,0.0 , 0 , 0 , 255 },
			{0.5 , 0.0  ,0.0 , 0 , 255 , 0},
		};

		//vertex buffer description
		D3D11_BUFFER_DESC bd = { 0 };
		bd.ByteWidth = sizeof(vtx);					//total array size
		bd.Usage = D3D11_USAGE_DEFAULT;				// how buffer data will be used (read/write protections for GPU/CPU)
		bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;	// What type of buffer would it be
		bd.CPUAccessFlags = 0u;						// we don't want any cpu access for now so setting it to 0 for now
		bd.MiscFlags = 0u;							// misscellinious flag for buffer configuration (we don't want it now either)
		bd.StructureByteStride = sizeof(VertexType); // Size of every vertex in the array 

		//holds the data pointer that will be used in vertex buffer
		D3D11_SUBRESOURCE_DATA subd = { 0 };
		subd.pSysMem = vtx;							// pointer to array so that it can copy all the array data to the buffer

		Microsoft::WRL::ComPtr<ID3D11Buffer> VBuffer;
		pDevice->CreateBuffer(&bd, &subd, &VBuffer);
		UINT stride = sizeof(VertexType);			// size of every vertex
		UINT offset = 0u;							// displacement after which the actual data start (so 0 because no displacement is there)
		//statrting slot(from 0) , number of buffers(1 buffer) , pp , 
		pDeviceContext->IASetVertexBuffers(0u, 1u, VBuffer.GetAddressOf(), &stride, &offset);

		Microsoft::WRL::ComPtr<ID3D11VertexShader> vS;// shader pointer
		Microsoft::WRL::ComPtr<ID3DBlob> blb; // holds the compiled shader bytecode
		D3DReadFileToBlob(L"VertexShader.cso", &blb); // reading the bytecode and storing it to blob
		pDevice->CreateVertexShader(blb->GetBufferPointer(), blb->GetBufferSize(), nullptr, &vS); // creating vertex shader
		pDeviceContext->VSSetShader(vS.Get(), nullptr, 0u); // binding vertex shader

		Microsoft::WRL::ComPtr<ID3D11InputLayout> inpl;
		//semantic name , semantic index , format , inputslot , offset , input data class , data step rate

		D3D11_INPUT_ELEMENT_DESC ied[] = {

			//tells that the first 3 * 4 * 8 bits = 32 * 3 = 96 bits of the vertex struct are for positions for every vertex
			{"POSITION",0,DXGI_FORMAT_R32G32B32_FLOAT,0,0,D3D11_INPUT_PER_VERTEX_DATA,0},
			//and next 3 * 1 * 8 = 24 bits are color value for each of those vertex
			// unorm for automaticall convert 0-255 range 0.0-1.0 range
			{"COLOR",0,DXGI_FORMAT_R8G8B8A8_UNORM,0,12u,D3D11_INPUT_PER_VERTEX_DATA,0}
		};
		//creating and setting
		pDevice->CreateInputLayout(ied, (UINT)std::size(ied), blb->GetBufferPointer(), blb->GetBufferSize(), &inpl);
		pDeviceContext->IASetInputLayout(inpl.Get());

		Microsoft::WRL::ComPtr<ID3D11PixelShader> ps; // shader pointer
		D3DReadFileToBlob(L"PixelShader.cso", &blb); // reading file to blob
		pDevice->CreatePixelShader(blb->GetBufferPointer(), blb->GetBufferSize(), nullptr, &ps); // creating
		pDeviceContext->PSSetShader(ps.Get(), nullptr, 0u); // setting

		pDeviceContext->OMSetRenderTargets(1u, pTarget.GetAddressOf(), nullptr);

		D3D11_VIEWPORT vp = {};
		vp.TopLeftX = 0;
		vp.TopLeftY = 0;
		vp.Width = 800;  //screen height
		vp.Height = 600; // screen width
		vp.MaxDepth = 1; // maximum depth for z axis
		vp.MinDepth = 0; // minimum depth for z axis
		pDeviceContext->RSSetViewports(1u, &vp);

		//draws the vertices as a list of traingles 
		pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		const unsigned short indices[] = {

			0 , 1 , 2,
			0 , 2 , 3,
			0 , 4 , 1
		};

		Microsoft::WRL::ComPtr<ID3D11Buffer> IndxBuff;

		D3D11_BUFFER_DESC indesc = { 0 };
		indesc.ByteWidth = sizeof(indices);
		indesc.Usage = D3D11_USAGE_DEFAULT;
		indesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		indesc.StructureByteStride = sizeof(unsigned short);
		indesc.CPUAccessFlags = 0u;
		indesc.MiscFlags = 0u;

		D3D11_SUBRESOURCE_DATA isd = { 0 };
		isd.pSysMem = indices;

		pDevice->CreateBuffer(&indesc, &isd, &IndxBuff);
		pDeviceContext->IASetIndexBuffer(IndxBuff.Get(), DXGI_FORMAT_R16_UINT, 0u);

		auto t_point = std::chrono::system_clock::now();

		float col[] = {1.0f , 1.0f , 1.0f , 1.0f};

		std::chrono::duration<double> d;

		while (true)
		{

			const double angle = (d = std::chrono::system_clock::now() - t_point).count();
			//a rotation around z axis matrix
			
			DirectX::XMMATRIX TransformMatrix = DirectX::XMMatrixRotationZ(angle);

			Microsoft::WRL::ComPtr<ID3D11Buffer> ConstBuff;
			D3D11_BUFFER_DESC cbuffdsc = { 0 };
			cbuffdsc.ByteWidth = sizeof(TransformMatrix);
			cbuffdsc.Usage = D3D11_USAGE_DYNAMIC;  // because we are gonna change it from cpu side (not now but later)
			cbuffdsc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
			cbuffdsc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE; // giving access to cpu so that we can change it

			D3D11_SUBRESOURCE_DATA cbuffdt = { 0 };
			cbuffdt.pSysMem = &TransformMatrix;

			pDevice->CreateBuffer(&cbuffdsc, &cbuffdt, &ConstBuff);
			pDeviceContext->VSSetConstantBuffers(0u, 1u, ConstBuff.GetAddressOf());


			if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE) != 0)
			{
				if (msg.message == WM_QUIT)
				{
					break;
				}
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
			if (FAILED(pSwapChain->Present(1u, 0u)))
			{
				//throw 1;
			}
			pDeviceContext->ClearRenderTargetView(pTarget.Get(), col);
			pDeviceContext->DrawIndexed(std::size(indices), 0u, 0u);
		}
	}
	catch (HRESULT hrcode)
	{
		char* msgBuff;
		FormatMessage(
			FORMAT_MESSAGE_ALLOCATE_BUFFER |
			FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
			nullptr, hrcode, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), reinterpret_cast<LPSTR>(&msgBuff), 0, nullptr);

		MessageBox(nullptr, msgBuff, "Error", MB_ICONERROR);

		LocalFree(msgBuff);
	}
	return 0;
}