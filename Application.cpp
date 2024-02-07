#include "Application.h"


LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    PAINTSTRUCT ps;
    HDC hdc;

    switch (message)
    {
        case WM_PAINT:
            hdc = BeginPaint(hWnd, &ps);
            EndPaint(hWnd, &ps);
            break;

        case WM_DESTROY:
            PostQuitMessage(0);
            break;

        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
    }

    return 0;
}

Application::Application()
{
	_hInst = nullptr;
	_hWnd = nullptr;
	_driverType = D3D_DRIVER_TYPE_NULL;
	_featureLevel = D3D_FEATURE_LEVEL_11_0;
	_pd3dDevice = nullptr;
	_pImmediateContext = nullptr;
	_pSwapChain = nullptr;
	_pRenderTargetView = nullptr;
	_pVertexShader = nullptr;
	_pPixelShader = nullptr;
	_pVertexLayout = nullptr;
	_pVertexBuffer = nullptr;
	_pIndexBuffer = nullptr;
	_pConstantBuffer = nullptr;
}

Application::~Application()
{
	Cleanup();
}

HRESULT Application::Initialise(HINSTANCE hInstance, int nCmdShow)
{
    if (FAILED(InitWindow(hInstance, nCmdShow)))
	{
        return E_FAIL;
	}

    srand(time(NULL));

    RECT rc;
    GetClientRect(_hWnd, &rc);
    _WindowWidth = rc.right - rc.left;
    _WindowHeight = rc.bottom - rc.top;

    if (FAILED(InitDevice()))
    {
        Cleanup();

        return E_FAIL;
    }

	// Initialize the world matrix
	XMStoreFloat4x4(&_world, XMMatrixIdentity());

    // Initialize the view matrix
    XMFLOAT3 Eye = XMFLOAT3(0.0f, 0.0f, -10.0f);
	XMFLOAT3 At =  XMFLOAT3(0.0f, 0.0f, 1.0f);
	XMFLOAT3 Up =  XMFLOAT3(0.0f, 1.0f, 0.0f);

	//XMStoreFloat4x4(&_view, XMMatrixLookAtLH(Eye, At, Up));

    // Initialize the projection matrix
	//XMStoreFloat4x4(&_projection, XMMatrixPerspectiveFovLH(XM_PIDIV2, _WindowWidth / (FLOAT) _WindowHeight, 0.01f, 100.0f));

  

    myCamera = new Camera(cAt,Eye, XMFLOAT3(0,0,-1), Up, _WindowWidth, _WindowHeight, 0.01f, 100.0f);
    _view = myCamera->GetView();
    _projection = myCamera->GetProjection();

    myCamera2 = new Camera(cTo,XMFLOAT3(0.0f, 0.0f,-20.0f), XMFLOAT3(0.0f, 0.0f, 1.0f), Up, _WindowWidth, _WindowHeight, 0.01f, 100.0f);
    _view = myCamera2->GetView();
    _projection = myCamera2->GetProjection();

    //set current camera to camera 2
    currentCamera = myCamera;
    
    // Create GameObjects
    GameObjects.push_back(new Gameobject("Test models/Car/Car.obj", _pd3dDevice, XMFLOAT3(5.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f)));
    GameObjects.push_back(new Gameobject("skymap.dds", _pd3dDevice, XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(50.0f, 50.0f, 50.0f)));


    //Initialize Lights 
    //Diffuse
    DiffuseLight = XMFLOAT4();
    DiffuseMaterial = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
    directionToLight = XMFLOAT3(0.0f, 0.5f, -0.5f);

    //Ambient 
    AmbientLight = XMFLOAT4();
    AmbientMaterial = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

    //Specular 
    SpecularLight = XMFLOAT4();
    SpecularMaterial = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
    SpecularPower = 10.0f;
    EyeWorldPos = currentCamera->GetEye();

    //Xml Parser
    rapidxml::file<> xmlFile("XML/Lighting.xml");
    rapidxml::xml_document<> doc;
    doc.parse<0>(xmlFile.data());

    rapidxml::xml_node<>* game_node = doc.first_node();

    for (rapidxml::xml_node<>* pPnode = game_node->first_node(); pPnode; pPnode = pPnode->next_sibling())
    {
        // gets the name of the node
        std::string nodeName = pPnode->name();
        // attrubute to get access to the names and values
        rapidxml::xml_attribute<>* p_attribute = pPnode->first_attribute();

        if (nodeName == "ambient")
        {
            AmbientLight.x = std::stof(p_attribute->value()); // gets value of x and interprets it as a floating point number
            p_attribute = p_attribute->next_attribute();     
            AmbientLight.y = std::stof(p_attribute->value()); // gets value of y and interprets it as a flaoting point number 
            p_attribute = p_attribute->next_attribute();      
            AmbientLight.z = std::stof(p_attribute->value()); // gets value of z and interprets it as a floating point number
            p_attribute = p_attribute->next_attribute();
            AmbientLight.w = std::stof(p_attribute->value()); // gets value of W and interprets it as a floating point number
        }

        if (nodeName == "diffuse")
        {
            DiffuseLight.x = std::stof(p_attribute->value()); // gets value of x and interprets it as a floating point number
            p_attribute = p_attribute->next_attribute();
            DiffuseLight.y = std::stof(p_attribute->value()); // gets value of y and interprets it as a floating point number
            p_attribute = p_attribute->next_attribute();
            DiffuseLight.z = std::stof(p_attribute->value()); // gets value of z and interprets it as a floating point number
            p_attribute = p_attribute->next_attribute();
            DiffuseLight.w = std::stof(p_attribute->value()); // gets value of w and interprets it as a floating point number
        }

        if (nodeName == "specular")
        {
            SpecularLight.x = std::stof(p_attribute->value());
            p_attribute = p_attribute->next_attribute();
            SpecularLight.y = std::stof(p_attribute->value());
            p_attribute = p_attribute->next_attribute();
            SpecularLight.z = std::stof(p_attribute->value());
            p_attribute = p_attribute->next_attribute();
            SpecularLight.w = std::stof(p_attribute->value());
        }
    }

    doc.clear();
    //create texture from file
    CreateDDSTextureFromFile(_pd3dDevice, L"Crate_COLOR.dds", nullptr, &_pTextureRV);
    CreateDDSTextureFromFile(_pd3dDevice, L"Car_COLOR.dds", nullptr, &_pCarRV);
    CreateDDSTextureFromFile(_pd3dDevice, L"Crate_SPEC.dds", nullptr, &_pTextureSpecRV);
  //  CreateDDSTextureFromFileEx(_pd3dDevice, L"skymap.dds", nullptr, &_pCubeMapRV);
    CreateDDSTextureFromFileEx(_pd3dDevice, L"skymap.dds", 0, D3D11_USAGE_DEFAULT, D3D11_BIND_SHADER_RESOURCE, 0, D3D11_RESOURCE_MISC_TEXTURECUBE, false, nullptr, &_pCubeMapRV);
    
    //bind the texture
/*    _pImmediateContext->PSSetShaderResources(0, 1, &_pTextureRV);
    _pImmediateContext->PSSetShaderResources(1, 1, &_pCarRV)*/;
    _pImmediateContext->PSSetShaderResources(1, 1, &_pTextureSpecRV);
    

    // Define the sample state
    D3D11_SAMPLER_DESC samplerDesc;
    ZeroMemory(&samplerDesc, sizeof(samplerDesc));
    samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    samplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
    samplerDesc.MinLOD = 0;
    samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

    _pd3dDevice->CreateSamplerState(&samplerDesc, &_pSamplerLinear);

    _pImmediateContext->PSSetSamplers(0, 1, &_pSamplerLinear);

    //Define depth state for skybox
    D3D11_DEPTH_STENCIL_DESC skyboxDepthStencilDesc;
    ZeroMemory(&skyboxDepthStencilDesc, sizeof(ID3D11DepthStencilState));
    skyboxDepthStencilDesc.DepthEnable = true;
    skyboxDepthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
    skyboxDepthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
    
    _pd3dDevice->CreateDepthStencilState(&skyboxDepthStencilDesc, &_depthStencilLessEqual);



	return S_OK;
}

HRESULT Application::InitShadersAndInputLayout()
{
	HRESULT hr;

    // Compile the vertex shader
    ID3DBlob* pVSBlob = nullptr;

    //Compile shaders from file
    hr = CompileShaderFromFile(L"DX11 Framework.hlsl", "VS", "vs_4_0", &pVSBlob);
   
    //check if compiling shaders from file failed
    if (FAILED(hr))
    {
        MessageBox(nullptr,
                   L"The HLSL file cannot be compiled. Check VS Outpot for Error Log.", L"Error", MB_OK);
        return hr;
    }
	// Create the vertex shader
	hr = _pd3dDevice->CreateVertexShader(pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), nullptr, &_pVertexShader);
    // Compile the Skybox Vertex Shader
    hr = CompileShaderFromFile(L"DX11 Framework.hlsl", "SKYBOXVS", "vs_4_0", &pVSBlob);

    if (FAILED(hr))
    {
        MessageBox(nullptr,
            L"The HLSL file cannot be compiled. Check VS Outpot for Error Log.", L"Error", MB_OK);
        return hr;
    }

    // Create the Skybox vertex shader
    hr = _pd3dDevice->CreateVertexShader(pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), nullptr, &_pSkyboxVertexShader);


    if (FAILED(hr))
    {
        pVSBlob->Release();
        return hr;
    }
	// Compile the pixel shader
	ID3DBlob* pPSBlob = nullptr;
    hr = CompileShaderFromFile(L"DX11 Framework.hlsl", "PS", "ps_4_0", &pPSBlob);

    if (FAILED(hr))
    {
        MessageBox(nullptr,
            L"The HLSL file cannot be compiled. Check VS Outpot for Error Log.", L"Error", MB_OK);
        return hr;
    }
	hr = _pd3dDevice->CreatePixelShader(pPSBlob->GetBufferPointer(), pPSBlob->GetBufferSize(), nullptr, &_pPixelShader);

   hr = CompileShaderFromFile(L"DX11 Framework.hlsl", "SKYBOXPS", "ps_4_0", &pPSBlob);
    if (FAILED(hr))
    {
        MessageBox(nullptr,
            L"The HLSL file cannot be compiled. Check VS Outpot for Error Log.", L"Error", MB_OK);
        return hr;
    };

	// Create the pixel shader
	hr = _pd3dDevice->CreatePixelShader(pPSBlob->GetBufferPointer(), pPSBlob->GetBufferSize(), nullptr, &_pSkyboxPixelShader);
	pPSBlob->Release();

    if (FAILED(hr))
        return hr;

    // Define the input layout
    D3D11_INPUT_ELEMENT_DESC layout[] =
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "NORMAL", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
       { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0},
	};
    
	UINT numElements = ARRAYSIZE(layout);

    // Create the input layout
	hr = _pd3dDevice->CreateInputLayout(layout, numElements, pVSBlob->GetBufferPointer(),
                                        pVSBlob->GetBufferSize(), &_pVertexLayout);
	pVSBlob->Release();

	if (FAILED(hr))
        return hr;
    
    // Set the input layout
    _pImmediateContext->IASetInputLayout(_pVertexLayout);

	return hr;
}

HRESULT Application::InitVertexBuffer()
{
    HRESULT hr = 0;

    // Create vertex buffer
    SimpleVertex vertices[] =
    {
 
        { XMFLOAT3(-1.0f, 1.0f, -1.0f),    XMFLOAT3(-1.0f, 1.0f, -1.0f),      XMFLOAT2(0,0)},
        { XMFLOAT3(1.0f, 1.0f, -1.0f),     XMFLOAT3(1.0f, 1.0f, -1.0f),       XMFLOAT2(1,0)},
        { XMFLOAT3(-1.0f, -1.0f, -1.0f),   XMFLOAT3(-1.0f, -1.0f, -1.0f),     XMFLOAT2(0,1)},

        { XMFLOAT3(1.0f, -1.0f, -1.0f),    XMFLOAT3(1.0f, -1.0f, -1.0f),      XMFLOAT2(1,1)},

        { XMFLOAT3(-1.0f, 1.0f, 1.0f),     XMFLOAT3(-1.0f, 1.0f, 1.0f),       XMFLOAT2(0,0)},

        { XMFLOAT3(1.0f, 1.0f, 1.0f),      XMFLOAT3(1.0f, 1.0f, 1.0f),        XMFLOAT2(1,0)},

        { XMFLOAT3(-1.0f, -1.0f, 1.0f),    XMFLOAT3(-1.0f, -1.0f, 1.0f),      XMFLOAT2(0,1)},
        { XMFLOAT3(1.0f, -1.0f, 1.0f),     XMFLOAT3(1.0f, -1.0f, 1.0f),       XMFLOAT2(1,1)},

        //top
        { XMFLOAT3(-1.0f, 1.0f, -1.0f),    XMFLOAT3(-1.0f, 1.0f, -1.0f),      XMFLOAT2(0,1)},
        { XMFLOAT3(1.0f, 1.0f, -1.0f),     XMFLOAT3(1.0f, 1.0f, -1.0f),       XMFLOAT2(1,1)},
        
       //bot
       { XMFLOAT3(-1.0f, -1.0f, 1.0f),     XMFLOAT3(-1.0f, -1.0f, 1.0f),      XMFLOAT2(0,0)},
       {XMFLOAT3(1.0f, -1.0f, 1.0f),       XMFLOAT3(1.0f, -1.0f, 1.0f),       XMFLOAT2(1,0)},
     
        //left
        { XMFLOAT3(-1.0f, 1.0f, -1.0f),    XMFLOAT3(-1.0f, 1.0f, -1.0f),      XMFLOAT2(1,0)},
        { XMFLOAT3(-1.0f, -1.0f, -1.0f),   XMFLOAT3(-1.0f, -1.0f, -1.0f),     XMFLOAT2(1,1)},

        //right
        { XMFLOAT3(1.0f, -1.0f, -1.0f),    XMFLOAT3(1.0f, -1.0f, -1.0f),      XMFLOAT2(0,1)},
        { XMFLOAT3(1.0f, 1.0f, -1.0f),     XMFLOAT3(1.0f, 1.0f, -1.0f),       XMFLOAT2(0,0)},
        { XMFLOAT3(1.0f, 1.0f, 1.0f),      XMFLOAT3(1.0f, 1.0f, 1.0f),        XMFLOAT2(1,0)},



    };

    //buffer description
    D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = sizeof(SimpleVertex) * 17; // the amount of vertices in our vertex buffer
    bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;

    //subresource data
    D3D11_SUBRESOURCE_DATA InitData;
	ZeroMemory(&InitData, sizeof(InitData));
    InitData.pSysMem = vertices;

    //create buffer
    hr = _pd3dDevice->CreateBuffer(&bd, &InitData, &_pVertexBuffer);


    if (FAILED(hr))
        return hr;

    // Create pyramid buffer
    SimpleVertex Pyramidvertices[] =
    {
            //bottom
           { XMFLOAT3(-1.0f, -1.0f, 1.0f), XMFLOAT3(-1.0f, -1.0f, 1.0f),XMFLOAT2(0,0)},
           { XMFLOAT3(1.0f, -1.0f, 1.0f),XMFLOAT3(1.0f, -1.0f, 1.0f),XMFLOAT2(1,0)},
           { XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT3(-1.0f, -1.0f, -1.0f),XMFLOAT2(0,1)},
           { XMFLOAT3(1.0f, -1.0f, -1.0f),XMFLOAT3(1.0f, -1.0f, -1.0f),XMFLOAT2(1,1)},

           //top
           { XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT3(0.0f,1.0f,0.0f),XMFLOAT2(0.5,0)},
           
           //back
           { XMFLOAT3(-1.0f, -1.0f, 1.0f), XMFLOAT3(-1.0f, -1.0f, 1.0f),XMFLOAT2(1,1)},
           { XMFLOAT3(1.0f, -1.0f, 1.0f),XMFLOAT3(1.0f, -1.0f, 1.0f),XMFLOAT2(0,1)},

           //front
           { XMFLOAT3(1.0f, -1.0f, -1.0f),XMFLOAT3(1.0f, -1.0f, -1.0f),XMFLOAT2(1,1)},
           { XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT3(-1.0f, -1.0f, -1.0f),XMFLOAT2(0,1)},
           
           //right
           { XMFLOAT3(1.0f, -1.0f, 1.0f),XMFLOAT3(1.0f, -1.0f, 1.0f),XMFLOAT2(1,1)},
           { XMFLOAT3(1.0f, -1.0f, -1.0f),XMFLOAT3(1.0f, -1.0f, -1.0f),XMFLOAT2(0,1)},
           //left
           { XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT3(-1.0f, -1.0f, -1.0f),XMFLOAT2(1,1)},
           { XMFLOAT3(-1.0f, -1.0f, 1.0f), XMFLOAT3(-1.0f, -1.0f, 1.0f),XMFLOAT2(0,1)},
    };
    // Create Pyramid Buffer Desc
    D3D11_BUFFER_DESC pyramidbd;
    ZeroMemory(&pyramidbd, sizeof(pyramidbd));
    pyramidbd.Usage = D3D11_USAGE_DEFAULT;
    pyramidbd.ByteWidth = sizeof(SimpleVertex) * 13; // the amount of vertices in our vertex buffer
    pyramidbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    pyramidbd.CPUAccessFlags = 0;
    // Create Pyramid Subresource Data
    D3D11_SUBRESOURCE_DATA PyramidInitData;
    ZeroMemory(&PyramidInitData, sizeof(PyramidInitData));
    PyramidInitData.pSysMem = Pyramidvertices;
    // Create Pyramid Buffer
    hr = _pd3dDevice->CreateBuffer(&pyramidbd, &PyramidInitData, &_pPyramidVertexBuffer);

    if (FAILED(hr))
        return hr;

	return S_OK;
}

HRESULT Application::InitIndexBuffer()
{
	HRESULT hr;

    // Create index buffer
    WORD indices[] =
    {
        0, 1, 2,    // front
        2, 1, 3,

        4, 12, 13,    // left
        13, 6, 4,

        5,4,6,          // back
        6,7,5,

        14, 15, 7,    // right
        7, 15, 16,

        4, 5, 8,    // top
        8, 5, 9,

        10, 2, 3,    // bot
        3, 11, 10,



    };

    //Buffer description
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = sizeof(WORD) * 36;     
    bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bd.CPUAccessFlags = 0;

    //subresource data
	D3D11_SUBRESOURCE_DATA InitData;
	ZeroMemory(&InitData, sizeof(InitData));
    InitData.pSysMem = indices;

    //create buffer
    hr = _pd3dDevice->CreateBuffer(&bd, &InitData, &_pIndexBuffer);

    if (FAILED(hr))
        return hr;

    WORD SkyboxIndices[] =
    {
        2, 1, 0,
        3, 1, 2,

        13, 12, 4,
        4,  6, 13,

        6, 4, 5,
        5, 7, 6,

        7, 15, 14,
        16, 15, 7,

        8, 5, 4,
        9, 5, 8,

        3, 2, 10,
        10, 11, 3,
    };

    //Buffer description
    D3D11_BUFFER_DESC skyboxbd;
    ZeroMemory(&skyboxbd, sizeof(skyboxbd));
    skyboxbd.Usage = D3D11_USAGE_DEFAULT;
    skyboxbd.ByteWidth = sizeof(WORD) * 36;
    skyboxbd.BindFlags = D3D11_BIND_INDEX_BUFFER;
    skyboxbd.CPUAccessFlags = 0;

    //subresource data
    D3D11_SUBRESOURCE_DATA skyboxInitData;
    ZeroMemory(&skyboxInitData, sizeof(skyboxInitData));
    skyboxInitData.pSysMem = SkyboxIndices;

    //create buffer
    hr = _pd3dDevice->CreateBuffer(&skyboxbd, &skyboxInitData, &_pSkyboxIndexBuffer);

    if (FAILED(hr))
        return hr;


    // Create index buffer
    WORD Pyramidindices[] =
    {
       0, 2, 1,    // bot
       1, 2, 3,

       5, 6, 4,    // front

       9, 10, 4,    // right

       7, 8, 4,    // back

       11, 12, 4,    // left
    };

    // Pyramid Index Buffer Desc
    D3D11_BUFFER_DESC pyramidbd;
    ZeroMemory(&pyramidbd, sizeof(pyramidbd));
    pyramidbd.Usage = D3D11_USAGE_DEFAULT;
    pyramidbd.ByteWidth = sizeof(WORD) * 18;
    pyramidbd.BindFlags = D3D11_BIND_INDEX_BUFFER;
    pyramidbd.CPUAccessFlags = 0;
    // Pyramid Index Subresource Data
    D3D11_SUBRESOURCE_DATA pyramidInitData;
    ZeroMemory(&pyramidInitData, sizeof(pyramidInitData));
    pyramidInitData.pSysMem = Pyramidindices;
    // Create Pyramid Index Buffer
    hr = _pd3dDevice->CreateBuffer(&pyramidbd, &pyramidInitData, &_pPyramidIndexBuffer);

    if (FAILED(hr))
        return hr;

	return S_OK;
}

HRESULT Application::InitWindow(HINSTANCE hInstance, int nCmdShow)
{
    // Register class
    WNDCLASSEX wcex;
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(hInstance, (LPCTSTR)IDI_TUTORIAL1);
    wcex.hCursor = LoadCursor(NULL, IDC_ARROW );
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = nullptr;
    wcex.lpszClassName = L"TutorialWindowClass";
    wcex.hIconSm = LoadIcon(wcex.hInstance, (LPCTSTR)IDI_TUTORIAL1);
    if (!RegisterClassEx(&wcex))
        return E_FAIL;

    // Create window
    _hInst = hInstance;
    RECT rc = {0, 0, 640, 480};
    AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);
    _hWnd = CreateWindow(L"TutorialWindowClass", L"DX11 Framework", WS_OVERLAPPEDWINDOW,
                         CW_USEDEFAULT, CW_USEDEFAULT, rc.right - rc.left, rc.bottom - rc.top, nullptr, nullptr, hInstance,
                         nullptr);
    if (!_hWnd)
		return E_FAIL;

    ShowWindow(_hWnd, nCmdShow);

    return S_OK;
}

HRESULT Application::CompileShaderFromFile(WCHAR* szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob** ppBlobOut)
{
    HRESULT hr = S_OK;

    DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined(DEBUG) || defined(_DEBUG)
    // Set the D3DCOMPILE_DEBUG flag to embed debug information in the shaders.
    // Setting this flag improves the shader debugging experience, but still allows 
    // the shaders to be optimized and to run exactly the way they will run in 
    // the release configuration of this program.
    dwShaderFlags |= D3DCOMPILE_DEBUG;
#endif

    ID3DBlob* pErrorBlob;
    hr = D3DCompileFromFile(szFileName, nullptr, nullptr, szEntryPoint, szShaderModel, 
        dwShaderFlags, 0, ppBlobOut, &pErrorBlob);

    if (FAILED(hr))
    {
        if (pErrorBlob != nullptr)
            OutputDebugStringA((char*)pErrorBlob->GetBufferPointer());

        if (pErrorBlob) pErrorBlob->Release();

        return hr;
    }

    if (pErrorBlob) pErrorBlob->Release();

    return S_OK;
}

HRESULT Application::InitDevice()
{
    HRESULT hr = S_OK;

    UINT createDeviceFlags = 0;

#ifdef _DEBUG
    createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

    D3D_DRIVER_TYPE driverTypes[] =
    {
        D3D_DRIVER_TYPE_HARDWARE,
        D3D_DRIVER_TYPE_WARP,
        D3D_DRIVER_TYPE_REFERENCE,
    };

    UINT numDriverTypes = ARRAYSIZE(driverTypes);

    D3D_FEATURE_LEVEL featureLevels[] =
    {
        D3D_FEATURE_LEVEL_11_0,
        D3D_FEATURE_LEVEL_10_1,
        D3D_FEATURE_LEVEL_10_0,
    };

	UINT numFeatureLevels = ARRAYSIZE(featureLevels);

    DXGI_SWAP_CHAIN_DESC sd;
    ZeroMemory(&sd, sizeof(sd));
    sd.BufferCount = 1;
    sd.BufferDesc.Width = _WindowWidth;
    sd.BufferDesc.Height = _WindowHeight;
    sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    sd.BufferDesc.RefreshRate.Numerator = 60;
    sd.BufferDesc.RefreshRate.Denominator = 1;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.OutputWindow = _hWnd;
    sd.SampleDesc.Count = 1;
    sd.SampleDesc.Quality = 0;
    sd.Windowed = TRUE;

    for (UINT driverTypeIndex = 0; driverTypeIndex < numDriverTypes; driverTypeIndex++)
    {
        _driverType = driverTypes[driverTypeIndex];
        hr = D3D11CreateDeviceAndSwapChain(nullptr, _driverType, nullptr, createDeviceFlags, featureLevels, numFeatureLevels,
                                           D3D11_SDK_VERSION, &sd, &_pSwapChain, &_pd3dDevice, &_featureLevel, &_pImmediateContext);
        if (SUCCEEDED(hr))
            break;
    }

    if (FAILED(hr))
        return hr;

    // Create a render target view
    ID3D11Texture2D* pBackBuffer = nullptr;
    hr = _pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);

    if (FAILED(hr))
        return hr;

    //Define depth/stencil buffer
    D3D11_TEXTURE2D_DESC depthStencilDesc;
    depthStencilDesc.Width = _WindowWidth;
    depthStencilDesc.Height = _WindowHeight;
    depthStencilDesc.MipLevels = 1;
    depthStencilDesc.ArraySize = 1;
    depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    depthStencilDesc.SampleDesc.Count = 1;
    depthStencilDesc.SampleDesc.Quality = 0;
    depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
    depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    depthStencilDesc.CPUAccessFlags = 0;
    depthStencilDesc.MiscFlags = 0;

    _pd3dDevice->CreateTexture2D(&depthStencilDesc, nullptr, &_depthStencilBuffer);
    _pd3dDevice->CreateDepthStencilView(_depthStencilBuffer, nullptr, &_depthStencilView);

    hr = _pd3dDevice->CreateRenderTargetView(pBackBuffer, nullptr, &_pRenderTargetView);
    pBackBuffer->Release();

    if (FAILED(hr))
        return hr;

    _pImmediateContext->OMSetRenderTargets(1, &_pRenderTargetView, _depthStencilView);

    // Setup the viewport
    D3D11_VIEWPORT vp;
    vp.Width = (FLOAT)_WindowWidth;
    vp.Height = (FLOAT)_WindowHeight;
    vp.MinDepth = 0.0f;
    vp.MaxDepth = 1.0f;
    vp.TopLeftX = 0;
    vp.TopLeftY = 0;
    _pImmediateContext->RSSetViewports(1, &vp);

    
	hr = InitShadersAndInputLayout();
    if (FAILED(hr)) return S_FALSE;


    // Set vertex buffer
	InitVertexBuffer();
    UINT stride = sizeof(SimpleVertex);
    UINT offset = 0;
    _pImmediateContext->IASetVertexBuffers(0, 1, &_pVertexBuffer, &stride, &offset);

    
    // Set index buffer
	InitIndexBuffer();
    _pImmediateContext->IASetIndexBuffer(_pIndexBuffer, DXGI_FORMAT_R16_UINT, 0);


    // Set primitive topology
    _pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// Create the constant buffer desc
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(ConstantBuffer);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = 0;
    hr = _pd3dDevice->CreateBuffer(&bd, nullptr, &_pConstantBuffer);

    //Rasterizer States
    D3D11_RASTERIZER_DESC wfDesc;                   //wireframe state
    ZeroMemory(&wfDesc, sizeof(D3D11_RASTERIZER_DESC));
    wfDesc.FillMode = D3D11_FILL_WIREFRAME;
    wfDesc.CullMode = D3D11_CULL_NONE;
    hr = _pd3dDevice->CreateRasterizerState(&wfDesc, &_wireFrame);

    //_pImmediateContext->RSSetState(_wireFrame);

    D3D11_RASTERIZER_DESC solidDesc;
    ZeroMemory(&solidDesc, sizeof(D3D11_RASTERIZER_DESC));
    solidDesc.FillMode = D3D11_FILL_SOLID;
    solidDesc.CullMode = D3D11_CULL_BACK;
    hr = _pd3dDevice->CreateRasterizerState(&solidDesc, &_solid);

    //bind it to the RS state of the pipeline
    //_pImmediateContext->RSSetState(_solid);

    //Mesh Datas
   // carMeshData = OBJLoader::Load("Test Models/Car/Car.obj", _pd3dDevice);
   // skyboxMeshData = OBJLoader::Load("cube.obj", _pd3dDevice);

    D3D11_BLEND_DESC blendDesc;
    ZeroMemory(&blendDesc, sizeof(blendDesc));

    D3D11_RENDER_TARGET_BLEND_DESC rtbd;
    ZeroMemory(&rtbd, sizeof(rtbd));

    rtbd.BlendEnable = true;
    rtbd.SrcBlend = D3D11_BLEND_SRC_COLOR;
    rtbd.DestBlend = D3D11_BLEND_BLEND_FACTOR;
    rtbd.BlendOp = D3D11_BLEND_OP_ADD;
    rtbd.SrcBlendAlpha = D3D11_BLEND_ONE;
    rtbd.DestBlendAlpha = D3D11_BLEND_ZERO;
    rtbd.BlendOpAlpha = D3D11_BLEND_OP_ADD;
    rtbd.RenderTargetWriteMask = D3D10_COLOR_WRITE_ENABLE_ALL;

    blendDesc.AlphaToCoverageEnable = false;
  //  blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
   // blendDesc.RenderTarget[0].DestBlend= D3D11_BLEND_INV_SRC_ALPHA;
    blendDesc.RenderTarget[0] = rtbd;


    _pd3dDevice->CreateBlendState(&blendDesc, &Transparency);

    if (FAILED(hr))
        return hr;

    return S_OK;
}

void Application::Cleanup()
{
    if (_pImmediateContext) _pImmediateContext->ClearState();

    if (_pConstantBuffer) _pConstantBuffer->Release();
    if (_pVertexBuffer) _pVertexBuffer->Release();
    if (_pIndexBuffer) _pIndexBuffer->Release();
    if (_pVertexLayout) _pVertexLayout->Release();
    if (_pVertexShader) _pVertexShader->Release();
    if (_pPixelShader) _pPixelShader->Release();
    if (_pRenderTargetView) _pRenderTargetView->Release();
    if (_pSwapChain) _pSwapChain->Release();
    if (_pImmediateContext) _pImmediateContext->Release();
    if (_pd3dDevice) _pd3dDevice->Release();
    if (_wireFrame) _wireFrame->Release();
    if (_solid) _solid->Release();
    if (_depthStencilView)_depthStencilView->Release();
    if (_depthStencilBuffer)_depthStencilBuffer->Release();
    if (_pTextureRV)_pTextureRV->Release();
    if (_pTextureSpecRV)_pTextureSpecRV->Release();
    if (Transparency)Transparency->Release();
}

void Application::Update()
{
    // Update our time
    static float t = 0.0f;

    if (_driverType == D3D_DRIVER_TYPE_REFERENCE)
    {
        t += (float) XM_PI * 0.0125f;
    }
    else
    {
        static DWORD dwTimeStart = 0;
        DWORD dwTimeCur = GetTickCount();

        if (dwTimeStart == 0)
            dwTimeStart = dwTimeCur;

        t = (dwTimeCur - dwTimeStart) / 1000.0f;
    }
   // set the fill mode to wireframe if pressed left button
    if (GetAsyncKeyState(VK_LBUTTON)) 
    {
        _pImmediateContext->RSSetState(_wireFrame);
    }
    //set the fill mode to solid if pressed right button
    if (GetAsyncKeyState(VK_RBUTTON))
    {
        _pImmediateContext->RSSetState(_solid);
        _pImmediateContext->VSSetShader(_pVertexShader, nullptr, 0);
        _pImmediateContext->PSSetShader(_pPixelShader, nullptr, 0);
    }

    if (GetAsyncKeyState(VK_NUMPAD0))
    {
        //set current camera to cam1
        currentCamera = myCamera;
        //_view = myCamera->GetView();
        _projection = myCamera->GetProjection();
      
       
    }
    if (GetAsyncKeyState(VK_NUMPAD1))
    {
        //set current camera to cam2
        currentCamera = myCamera2;
        //_view = myCamera2->GetView();
        _projection= myCamera2->GetProjection();
        
    }
    currentCamera->Update();


    //
    // Animate the cube
    
 //   //First Cube
	XMStoreFloat4x4(&_world, XMMatrixRotationY(t * 0.5));

    XMStoreFloat4x4(&_pyramid, XMMatrixRotationY(t) * XMMatrixTranslation(-3.0f, 0, 0));
    _carMesh = GameObjects[0]->GetWorldMatrix();
   _skyboxMesh = GameObjects[1]->GetWorldMatrix();





    //Sun Cube
    //XMStoreFloat4x4(&_world2, XMMatrixRotationY(t) * XMMatrixTranslation(-2.0f, 0, 2.0f) * XMMatrixRotationY(t)); // second cube
    //XMStoreFloat4x4(&_world2, XMMatrixScaling(0.25f, 0.25f, 0.25f) * XMMatrixRotationY(t) * XMMatrixTranslation(-2.0f, 0, 0) * XMMatrixRotationY(t));

    ////Planet which is spinning around the sun cube
    //XMStoreFloat4x4(&_world3,  XMMatrixRotationY(t) * XMMatrixTranslation(-5.0f, 0, 0) * XMMatrixScaling(0.5f, 0.5f,0.5f)  * XMMatrixRotationY(t) * XMLoadFloat4x4(&_world2)); 

 //   //Moon which is rotating around the cube which is spinning around the sun
 //   XMStoreFloat4x4(&_world4,  XMMatrixRotationY(t) * XMMatrixTranslation(-7.0f, 0, 0) * XMMatrixScaling(0.3f, 0.3f, 0.3f) * XMMatrixRotationZ(t) * XMLoadFloat4x4(&_world3));
 //
   
  
    //for (int i = 0; i < 10; i++)
    //{

    //    XMStoreFloat4x4(&asteroidWorlds[i], XMMatrixRotationY(t) * XMMatrixTranslation(10, 0, i * 3.5f) * XMMatrixScaling(0.25f, 0.25f, 0.25f) * XMMatrixRotationY(t) * XMLoadFloat4x4(&_world));
    //}

}

void Application::Draw()
{
    
    
    // Clear the back buffer
    float ClearColor[4] = {0.0f, 0.125f, 0.3f, 1.0f}; // red,green,blue,alpha
    _pImmediateContext->ClearRenderTargetView(_pRenderTargetView, ClearColor);

    _pImmediateContext->ClearDepthStencilView(_depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);


	XMMATRIX world = XMLoadFloat4x4(&_world);
	XMMATRIX view = XMLoadFloat4x4(&currentCamera->GetView());
	XMMATRIX projection = XMLoadFloat4x4(&_projection);

   // EyeWorldPos = currentCamera->GetEye();

    // Update variables
    ConstantBuffer cb;
	cb.mWorld = XMMatrixTranspose(world);
	cb.mView = XMMatrixTranspose(view);
	cb.mProjection = XMMatrixTranspose(projection);
    cb.DiffLight = DiffuseLight;
    cb.DiffMat = DiffuseMaterial;
    cb.AmbLight =  AmbientLight;
    cb.AmbMat = AmbientMaterial;
    cb.DirToLight = directionToLight;
    cb.SpecLight = SpecularLight;
    cb.SpecMat = SpecularMaterial;
    cb.SpecPower = SpecularPower;
    cb.EyeWorldPos = currentCamera->GetEye();
    cb.fogColor = FogColor;
    cb.fogStart = FogStart;
    cb.fogEnd = FogEnd;
    
    //cb.SpecularEnabled = SpecEnable;

    //update buffer with the most recent information
	_pImmediateContext->UpdateSubresource(_pConstantBuffer, 0, nullptr, &cb, 0, 0);

 //   // Renders a triangle
	_pImmediateContext->VSSetShader(_pVertexShader, nullptr, 0);
    _pImmediateContext->PSSetShader(_pPixelShader, nullptr, 0);
 
	_pImmediateContext->VSSetConstantBuffers(0, 1, &_pConstantBuffer);
    _pImmediateContext->PSSetConstantBuffers(0, 1, &_pConstantBuffer);
	

    UINT stride = sizeof(SimpleVertex);
    UINT offset = 0;


    float blendFactor[] = { 0.75f, 0.75f, 0.75f, 1.0f };


    //Set blend state for transparent objects

    _pImmediateContext->OMSetBlendState(Transparency, blendFactor, 0xffffffff);
    //Set Cube Texture
    GameObjects[0]->SetShaderResource(_pTextureRV);
    _pImmediateContext->PSSetShaderResources(0, 1, GameObjects[0]->GetShaderResource());
    // Set Cube Vertex Buffer
    _pImmediateContext->IASetVertexBuffers(0, 1, &_pVertexBuffer, &stride, &offset);
    // Set Cube Index Buffer
    _pImmediateContext->IASetIndexBuffer(_pIndexBuffer, DXGI_FORMAT_R16_UINT, 0);
    // Draw Cube Indices
    _pImmediateContext->DrawIndexed(36, 0, 0);


    _pImmediateContext->OMSetBlendState(0, 0, 0xffffffff);
    // Set Pyramid Vertex Buffer
    _pImmediateContext->IASetVertexBuffers(0, 1, &_pPyramidVertexBuffer, &stride, &offset);
    // Set Pyramid Index Buffer
    _pImmediateContext->IASetIndexBuffer(_pPyramidIndexBuffer, DXGI_FORMAT_R16_UINT, 0);
 
    world = XMLoadFloat4x4(&_pyramid);
    cb.mWorld = XMMatrixTranspose(world);
    _pImmediateContext->UpdateSubresource(_pConstantBuffer, 0, nullptr, &cb, 0, 0);
    _pImmediateContext->DrawIndexed(18, 0, 0);

    // Set Car Mesh Vertex Buffer
    _pImmediateContext->IASetVertexBuffers(0, 1, &GameObjects[0]->GetMeshData()->VertexBuffer, &GameObjects[0]->GetMeshData()->VBStride, &GameObjects[0]->GetMeshData()->VBOffset);
    // Set Car Mesh Index Buffer
    _pImmediateContext->IASetIndexBuffer(GameObjects[0]->GetMeshData()->IndexBuffer, DXGI_FORMAT_R16_UINT, 0);
    // Draw Car Mesh Indices
    _pImmediateContext->PSSetShaderResources(0, 1, &_pCarRV);
    world = XMLoadFloat4x4(&_carMesh);
    cb.mWorld = XMMatrixTranspose(world);
    _pImmediateContext->UpdateSubresource(_pConstantBuffer, 0, nullptr, &cb, 0, 0);
    _pImmediateContext->DrawIndexed(GameObjects[0]->GetMeshData()->IndexCount, 0, 0);



    //Skybox 
  
   _pImmediateContext->VSSetShader(_pSkyboxVertexShader, nullptr, 0);
   _pImmediateContext->PSSetShader(_pSkyboxPixelShader, nullptr, 0);

    _pImmediateContext->PSSetShaderResources(2, 1, &_pCubeMapRV);
    _pImmediateContext->IASetVertexBuffers(0, 1, &_pVertexBuffer, &stride, &offset);
    _pImmediateContext->IASetIndexBuffer(_pSkyboxIndexBuffer, DXGI_FORMAT_R16_UINT, 0);
     world = XMLoadFloat4x4(&_skyboxMesh);
    cb.mWorld = XMMatrixTranspose(world);
    _pImmediateContext->UpdateSubresource(_pConstantBuffer, 0, nullptr, &cb, 0, 0);
    _pImmediateContext->DrawIndexed(36, 0, 0);

 
    ////Sun
    //world = XMLoadFloat4x4(&_world2);
    //cb.mWorld = XMMatrixTranspose(world);
    //_pImmediateContext->UpdateSubresource(_pConstantBuffer, 0, nullptr, &cb, 0, 0);
    //_pImmediateContext->DrawIndexed(36, 0, 0); // draws index with a new world matrix

    ////Planet spinning around the sun
    //world  = XMLoadFloat4x4(&_world3);
    //cb.mWorld = XMMatrixTranspose(world);
    //_pImmediateContext->UpdateSubresource(_pConstantBuffer, 0, nullptr, &cb, 0, 0);
    //_pImmediateContext->DrawIndexed(36, 0, 0); // draws index with a new world matrix

    //world = XMLoadFloat4x4(&_world4);
    //cb.mWorld = XMMatrixTranspose(world);
    //_pImmediateContext->UpdateSubresource(_pConstantBuffer, 0, nullptr, &cb, 0, 0);
    //_pImmediateContext->DrawIndexed(36, 0, 0); // draws index with a new world matrix


    //for (int i = 0; i < 10; i++)
    //{
    //    world = XMLoadFloat4x4(&asteroidWorlds[i]);
    //    cb.mWorld = XMMatrixTranspose(world);
    //    _pImmediateContext->UpdateSubresource(_pConstantBuffer, 0, nullptr, &cb, 0, 0);
    //    _pImmediateContext->DrawIndexed(36, 0, 0); // draws index with a new world matrix
    //}

    
    // Present our back buffer to our front buffer
    //
    _pSwapChain->Present(0, 0);
}