#include<Windows.h>
#include<cstdint>
#include<string>
#include<format>
#include<d3d12.h>
#include<dxgi1_6.h>
#include<cassert>
#include<dxgidebug.h>
#include<dxcapi.h>
#include<fstream>
#include<sstream>
#include"externals/imgui/imgui.h"
#include"externals/imgui/imgui_impl_dx12.h"
#include"externals/imgui/imgui_impl_win32.h"
#include"externals/DirectXTex/DirectXTex.h"

#include"Matrix.h"
#include"Transform.h"
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);


#pragma comment(lib,"d3d12.lib")
#pragma comment(lib,"dxgi.lib")
#pragma comment(lib,"dxguid.lib")
#pragma comment(lib,"dxcompiler.lib")

struct VertexData {
	Vector4 position;
	Vector2 texcoord;
	Vector3 normal;
};

struct MaterialData {
	std::string textureFilePath;
};

struct ModelData {
	std::vector<VertexData> vertices;
	MaterialData material;
};

//ウィンドウプロシージャ
LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {




	if (ImGui_ImplWin32_WndProcHandler(hwnd, msg, wparam, lparam)) {
		return true;
	}

	//メッセージに応じてゲーム固有の処理を行う
	switch (msg) {
		//ウィンドウが破棄された
	case WM_DESTROY:
		//OSに対して、アプリ終了を伝える
		PostQuitMessage(0);
		return 0;
	}

	//　標準のメッセージ処理を行う
	return DefWindowProc(hwnd, msg, wparam, lparam);
}
std::wstring ConvertString(const std::string& str) {
	if (str.empty()) {
		return std::wstring();
	}

	auto sizeNeeded = MultiByteToWideChar(CP_UTF8, 0, reinterpret_cast<const char*>(&str[0]), static_cast<int>(str.size()), NULL, 0);
	if (sizeNeeded == 0) {
		return std::wstring();
	}
	std::wstring result(sizeNeeded, 0);
	MultiByteToWideChar(CP_UTF8, 0, reinterpret_cast<const char*>(&str[0]), static_cast<int>(str.size()), &result[0], sizeNeeded);
	return result;
}

std::string ConvertString(const std::wstring& str) {
	if (str.empty()) {
		return std::string();
	}

	auto sizeNeeded = WideCharToMultiByte(CP_UTF8, 0, str.data(), static_cast<int>(str.size()), NULL, 0, NULL, NULL);
	if (sizeNeeded == 0) {
		return std::string();
	}
	std::string result(sizeNeeded, 0);
	WideCharToMultiByte(CP_UTF8, 0, str.data(), static_cast<int>(str.size()), result.data(), sizeNeeded, NULL, NULL);
	return result;
}

void Log(const std::string& message) {
	OutputDebugStringA(message.c_str());
}

void Log(const std::wstring& message) {
	OutputDebugStringA(ConvertString(message).c_str());
}









IDxcBlob* CompileShader(
	//CompilerするShaderファイルへのパス
	const std::wstring& filePath,
	//Compilerに使用するProfile
	const wchar_t* profile,
	//初期化したものを3つ
	IDxcUtils* dxcUtils,
	IDxcCompiler3* dxcCompiler,
	IDxcIncludeHandler* includeHandler) {
	// ここの中身をこの後書いていく
	// 1.hlslファイルを読み込む
	Log(ConvertString(std::format(L"Begin CompileShader,path:{},profile:{}\n", filePath, profile)));
	// hlslファイルを読む
	IDxcBlobEncoding* shaderSource = nullptr;
	HRESULT hr = dxcUtils->LoadFile(filePath.c_str(), nullptr, &shaderSource);
	//読めなかったら止める
	assert(SUCCEEDED(hr));
	//読み込んだファイルの内容を設定する
	DxcBuffer shaderSourceBuffer;
	shaderSourceBuffer.Ptr = shaderSource->GetBufferPointer();
	shaderSourceBuffer.Size = shaderSource->GetBufferSize();
	shaderSourceBuffer.Encoding = DXC_CP_UTF8; //UTF8の文字コードであることを通知
	// 2.Compileする
	LPCWSTR arguments[] = {
		filePath.c_str(),//コンパイル対象のhlslファイル名
		L"-E",L"main", //エントリーポイントの指定。基本的にmain以外にはしない
		L"-T",profile, //ShaderProfileの設定
		L"-Zi",L"-Qembed_debug", //デバック用の情報を埋め込む
		L"-Od", //最適化を外しておく
		L"-Zpr" //メモリレイアウトは行優先
	};
	//実際にShaderをコンパイルする
	IDxcResult* shaderResult = nullptr;
	hr = dxcCompiler->Compile(
		&shaderSourceBuffer,	//読み込んだファイル
		arguments,				//コンパイルオプション
		_countof(arguments),	//コンパイルオプションの数
		includeHandler,			//includeが含まれた諸々
		IID_PPV_ARGS(&shaderResult)//コンパイル結果
	);
	//コンパイルエラーではなくdxcが起動できないなど致命的な状況
	assert(SUCCEEDED(hr));

	// 3.警告・エラーが出ていないか確認する

	//　警告・エラーが出てたらログを出して止める
	IDxcBlobUtf8* shaderError = nullptr;
	shaderResult->GetOutput(DXC_OUT_ERRORS, IID_PPV_ARGS(&shaderError), nullptr);
	if (shaderError != nullptr && shaderError->GetStringLength() != 0) {
		Log(shaderError->GetStringPointer());
		//警告・エラーダメゼッタイ
		assert(false);
	}

	// 4.Compile結果を受け取って返す

	//コンパイル結果から実行用のバイナリ部分を取得
	IDxcBlob* shaderBlob = nullptr;
	hr = shaderResult->GetOutput(DXC_OUT_OBJECT, IID_PPV_ARGS(&shaderBlob), nullptr);
	assert(SUCCEEDED(hr));
	//成功したログを出す
	Log(ConvertString(std::format(L"Compile Succeeded, path:{}, profile:{}\n", filePath, profile)));
	//　もう使わないリソースを開放
	shaderSource->Release();
	shaderResult->Release();
	//実行用のバイナリを返却
	return shaderBlob;

}

ID3D12Resource* CreateBufferResource(ID3D12Device* device, size_t sizeInDytes) {
	// 頂点リソース用のヒープの設定
	D3D12_HEAP_PROPERTIES uploadHeapProperties{};
	uploadHeapProperties.Type = D3D12_HEAP_TYPE_UPLOAD;//UploadHeapを使う
	//頂点リソースの設定
	D3D12_RESOURCE_DESC vertexResourceDesc{};
	//バッファリソース。テクスチャの場合はまた別の設定をする
	vertexResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	vertexResourceDesc.Width = sizeInDytes;//リソースのサイズ。今回はVector4を3頂点分
	//バッファの場合はこれらは1にする決まり
	vertexResourceDesc.Height = 1;
	vertexResourceDesc.DepthOrArraySize = 1;
	vertexResourceDesc.MipLevels = 1;
	vertexResourceDesc.SampleDesc.Count = 1;
	//バッファの場合はこれにする決まり
	vertexResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;




	//実際に頂点リソースを作る
	ID3D12Resource* resource = nullptr;
	HRESULT hr = device->CreateCommittedResource(&uploadHeapProperties, D3D12_HEAP_FLAG_NONE,
		&vertexResourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr,
		IID_PPV_ARGS(&resource));
	assert(SUCCEEDED(hr));
	return resource;
}

ID3D12DescriptorHeap* CreateDescriptorHeap(
	ID3D12Device* device, D3D12_DESCRIPTOR_HEAP_TYPE heapType, UINT numDescriptors, bool shaderVisible) {
	//ディスクリプタヒープの生成
	ID3D12DescriptorHeap* descriptorHeap = nullptr;
	D3D12_DESCRIPTOR_HEAP_DESC descriptorHeapDesc{};
	descriptorHeapDesc.Type = heapType;
	descriptorHeapDesc.NumDescriptors = numDescriptors;
	descriptorHeapDesc.Flags = shaderVisible ? D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE : D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	HRESULT hr = device->CreateDescriptorHeap(&descriptorHeapDesc, IID_PPV_ARGS(&descriptorHeap));
	//ディスクリプタヒープが作れなかったので起動できない
	assert(SUCCEEDED(hr));
	return descriptorHeap;
}


DirectX::ScratchImage LoadTexture(const std::string& filePath) {
	//テクスチャファイルを読んでプログラムで扱えるようにする
	DirectX::ScratchImage image{};
	std::wstring filePathW = ConvertString(filePath);
	HRESULT hr = DirectX::LoadFromWICFile(filePathW.c_str(), DirectX::WIC_FLAGS_FORCE_SRGB, nullptr, image);
	assert(SUCCEEDED(hr));

	//　ミップマップの作成
	DirectX::ScratchImage mipImages{};
	hr = DirectX::GenerateMipMaps(
		image.GetImages(),
		image.GetImageCount(),
		image.GetMetadata(),
		DirectX::TEX_FILTER_SRGB,
		0, mipImages);

	assert(SUCCEEDED(hr));

	//ミップマップ付きのデータを返す
	return mipImages;

}

ID3D12Resource* CreateTextureResource(ID3D12Device* device, const DirectX::TexMetadata& metadata) {
	// 1.matadataを基にResourceの作成
	D3D12_RESOURCE_DESC resourceDesc{};
	resourceDesc.Width = UINT(metadata.width);//Textureの幅
	resourceDesc.Height = UINT(metadata.height);//Textureの高さ
	resourceDesc.MipLevels = UINT16(metadata.mipLevels);//mipmapの高さ
	resourceDesc.DepthOrArraySize = UINT16(metadata.arraySize);//mipmapの数
	resourceDesc.Format = metadata.format;//奥行きor配列Textureの配列数
	resourceDesc.SampleDesc.Count = 1;//TextureのFormat
	resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION(metadata.dimension);//

	// 2.利用するHeapの設定。非常に特殊な運用。02_04exで一般的なケース版がある
	D3D12_HEAP_PROPERTIES heapProperties{};
	heapProperties.Type = D3D12_HEAP_TYPE_CUSTOM;
	heapProperties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_WRITE_BACK;
	heapProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_L0;

	// 3.Resourceを作成する
	ID3D12Resource* resource = nullptr;
	HRESULT hr = device->CreateCommittedResource(
		&heapProperties,					//Heapの設定
		D3D12_HEAP_FLAG_NONE,				//Heapの特殊な設定。特になし。
		&resourceDesc,						//Resourceの設定
		D3D12_RESOURCE_STATE_GENERIC_READ,	//初回のResourceState。Textureは基本読むだけ
		nullptr,							//Clear最適値。使わないのでnullptr
		IID_PPV_ARGS(&resource));			//作成するResourceポインタへのポインタ
	assert(SUCCEEDED(hr));
	return resource;
}

ID3D12Resource* CreateDepthStencilTextureResource(ID3D12Device* device, int32_t width, int32_t height) {

	// 生成するResourceの設定
	D3D12_RESOURCE_DESC resourceDesc{};
	resourceDesc.Width = width;										//Textureの幅
	resourceDesc.Height = height;									//Textureの高さ
	resourceDesc.MipLevels = 1;										//mipmapの数
	resourceDesc.DepthOrArraySize = 1;								//奥行き or 配列Textureの配列数
	resourceDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;			//DepthStencilとして利用可能なフォーマット
	resourceDesc.SampleDesc.Count = 1;								//サンプリングカウント。1固定。
	resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;	//２次元
	resourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;	//DepthStencilとして使う通知

	//利用するHeapの設定
	D3D12_HEAP_PROPERTIES heapProperties{};
	heapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;	//VRAM上に作る

	//深度値のクリア設定
	D3D12_CLEAR_VALUE depthClearValue;
	depthClearValue.DepthStencil.Depth = 1.0f;				 //1.0f(最大値)でクリア
	depthClearValue.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;	 //フォーマット。Resourceと合わせる

	//Resourceの生成
	ID3D12Resource* resource = nullptr;
	HRESULT hr = device->CreateCommittedResource(
		&heapProperties,					//Heapの設定
		D3D12_HEAP_FLAG_NONE,				//Heapの特殊な設定。特になし。
		&resourceDesc,						//Resourceの設定
		D3D12_RESOURCE_STATE_DEPTH_WRITE,	//深度値を書き込む状態にしておく
		&depthClearValue,					//Clear最適値
		IID_PPV_ARGS(&resource)				//作成するResourceポインタへのポインタ
	);
	assert(SUCCEEDED(hr));
	return resource;
}

void UploadTextureData(ID3D12Resource* texture, const DirectX::ScratchImage& mipImages) {
	//Meta情報を取得
	const DirectX::TexMetadata& metadata = mipImages.GetMetadata();
	//全MipMapについて
	for (size_t mipLevel = 0; mipLevel < metadata.mipLevels; ++mipLevel) {
		//MipMapLevelを指定して各Imageを取得
		const DirectX::Image* img = mipImages.GetImage(mipLevel, 0, 0);
		//Textureに転送
		HRESULT hr = texture->WriteToSubresource(
			UINT(mipLevel),
			nullptr,
			img->pixels,
			UINT(img->rowPitch),
			UINT(img->slicePitch)
		);

		assert(SUCCEEDED(hr));
	}
}

MaterialData LoadMaterialTemplateFile(const std::string& directoryPath, const std::string& filename) {
	//1.中で必要となる実数の宣言
	MaterialData materialData;
	std::string line;
	//2.ファイルを開く
	std::ifstream file(directoryPath + "/" + filename);
	assert(file.is_open());

	//3.実際にファイルを読み、MatrialDataを構築していく
	while (std::getline(file, line)) {
		std::string identifier;
		std::istringstream s(line);
		s >> identifier;

		// identifierに応じた処理
		if (identifier == "map_Kd") {
			std::string textureFilename;
			s >> textureFilename;
			//連結してファイルパスにする
			materialData.textureFilePath = directoryPath + "/" + textureFilename;
		}
	}
	//4.MaterialDataを返す
	return materialData;
}



ModelData LoadObjFile(const std::string& directoryPath, const std::string& filename) {
	//1.中で必要となる変数の宣言
	ModelData modelData;			//構築するModelData
	std::vector<Vector4> positions;	//位置
	std::vector<Vector3> normals;	//法線
	std::vector<Vector2> texcoords;	//テクスチャ座標
	std::string line;				//ファイルから読んだ1行を格納するもの
	//2.ファイルを開く
	std::ifstream file(directoryPath + "/" + filename);	//ファイルを開く
	assert(file.is_open());								//とりあえず開けなかったら止める
	//3.実際にファイルを読み、ModelDataを構築していく
	while (std::getline(file, line)) {
		std::string identifier;
		std::istringstream s(line);
		s >> identifier;	//先頭の識別子を読む

		// identifierに応じた処理

		if (identifier == "v") {
			Vector4 position;
			s >> position.x >> position.y >> position.z;
			position.x *= -1.0f;
			position.w = 1.0f;
			positions.push_back(position);
		} else if (identifier == "vt") {
			Vector2 texcoord;
			s >> texcoord.x >> texcoord.y;
			texcoords.push_back(texcoord);
		} else if (identifier == "vn") {
			Vector3 normal;
			s >> normal.x >> normal.y >> normal.z;
			normal.x *= -1.0f;
			normals.push_back(normal);

			//4.ModelDataを返す
		} else if (identifier == "f") {
			VertexData triangle[3];
			//面は三角形限定。その他は未対応
			for (int32_t faceVertex = 0; faceVertex < 3; ++faceVertex) {
				std::string vertexDefinition;
				s >> vertexDefinition;
				//頂点の要素へのIndexは「位置/UV/法線」で格納されているので、分解してIndexを取得する
				std::istringstream v(vertexDefinition);
				uint32_t elementIndices[3];
				for (int32_t element = 0; element < 3; element++) {
					std::string index;
					std::getline(v, index, '/');
					elementIndices[element] = std::stoi(index);
				}
				//要素へのIndexから、実際の要素の値を取得して、頂点を構築する
				Vector4 position = positions[elementIndices[0] - 1];
				Vector2 texcoord = texcoords[elementIndices[1] - 1];
				Vector3 normal = normals[elementIndices[2] - 1];
				position.x *= -1.0f;
				normal.x *= -1.0f;
				position.y *= -1.0f;
				normal.y *= -1.0f;
				//texcoord.y = 1.0f - texcoord.y;
				VertexData vertex = { position,texcoord,normal };
				modelData.vertices.push_back(vertex);
				triangle[faceVertex] = { position,texcoord,normal };
			}
			//頂点を逆順で登録することで、周り順を逆にする
			modelData.vertices.push_back(triangle[2]);
			modelData.vertices.push_back(triangle[1]);
			modelData.vertices.push_back(triangle[0]);
		} else if (identifier=="mmtllib"){
		//materialTemplateLibraryファイルの名前を取得する
			std::string materialFilename;
			s >> materialFilename;
			//基本的にobjファイルと同一階層にmtlは存在させるので、ディレクトリ名とファイル名を渡す
			modelData.material = LoadMaterialTemplateFile(directoryPath, materialFilename);

		}
	}

	return modelData;

}


//windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
	//COMの初期化
	CoInitializeEx(0, COINIT_MULTITHREADED);

	//出力ウィンドウへの文字出力
	OutputDebugStringA("Hello,DirectX!");

	WNDCLASS wc{};
	//ウィンドウプロシージャ
	wc.lpfnWndProc = WindowProc;
	//ウィンドウクラス名
	wc.lpszClassName = L"CG2WindowClass";
	//インスタンスハンドル
	wc.hInstance = GetModuleHandle(nullptr);
	//カーソル
	wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
	//ウィンドウクラスを登録する
	RegisterClass(&wc);

	const int32_t kClientWidth = 1280;
	const int32_t kClientHeight = 720;
	RECT wrc = { 0,0,kClientWidth,kClientHeight };

	//クライアント領域を元に実際のサイズにwrcを変更してもらう
	AdjustWindowRect(&wrc, WS_OVERLAPPEDWINDOW, false);

	// ウィンドウの生成
	HWND hwnd = CreateWindow(
		wc.lpszClassName,		//利用するクラス名
		L"CG2",					//タイトルバーの文字
		WS_OVERLAPPEDWINDOW,	//ウィンドウスタイル
		CW_USEDEFAULT,			//表示X座標（windowsに任せる）
		CW_USEDEFAULT,			//表示Y座標（windowsOSに任せる）
		wrc.right - wrc.left,	//ウィンドウ横幅
		wrc.bottom - wrc.top,	//ウィンドウ縦幅
		nullptr,				//縦ウィンドウハンドル
		nullptr,				//メニューハンドル
		wc.hInstance,			//インスタンスハンドル
		nullptr					//オプション
	);

	//DebugLayerを表示する
	// 1.デバックコントローラーを初期化する
	// 2.デバックコントローラーに値が入るとif文が作動する
	// 3.デバックレイヤーを有効化する
	// 4.GPU側でもチェックを行うようにする

#ifdef _DEBUG
	ID3D12Debug1* debugController = nullptr;//1.end

	if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController)))) {//2.end
		//デバックレイヤーを有効化する。
		debugController->EnableDebugLayer();//3.end

		//さらにGPU側でもチェックを行うようにする
		debugController->SetEnableSynchronizedCommandQueueValidation(TRUE);//4.end

#endif // DEBUG





		//文字列を格納する
		std::string str0{ "STRING!!!" };

		//整数を文字列にする
		std::string str1{ std::to_string(10) };



		//ウィンドウを表示する
		ShowWindow(hwnd, SW_SHOW);
		//ここから下に05の資料を書いていく

		//DXGIファクトリーの作成
		IDXGIFactory7* dxgiFactory = nullptr;
		//HRESULTはWindows系のエラーコードであり、
		//関数が成功したかどうかをSUCCEEDEDマクロで判断できる
		HRESULT hr = CreateDXGIFactory(IID_PPV_ARGS(&dxgiFactory));
		//初期化の根本的な問題でエラーが出た場合はプログラムが間違っているか、どうにもできない場合が多いのでassertにしておく
		assert(SUCCEEDED(hr));

		//使用するアダプタ用の変数。最初にnullptrを入れておく
		IDXGIAdapter4* useAdapter = nullptr;
		//良い順にアダプタを頼む
		for (UINT i = 0; dxgiFactory->EnumAdapterByGpuPreference(
			i, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS(&useAdapter)) !=
			DXGI_ERROR_NOT_FOUND; i++) {
			//アダプターの情報を取得する
			DXGI_ADAPTER_DESC3 adapterDesc{};
			hr = useAdapter->GetDesc3(&adapterDesc);
			assert(SUCCEEDED(hr)); //取得できないのは一大事
			//ソフトウェアアダプタでなければ採用！
			if (!(adapterDesc.Flags & DXGI_ADAPTER_FLAG3_SOFTWARE)) {
				//採用したアダプタの情報をログに出力。wstringのほうなので注意
				Log(std::format(L"Use Adapater;{}\n", adapterDesc.Description));
				break;
			}
			useAdapter = nullptr;
		}
		//適切なアダプタが見つからなかったので起動できない
		assert(useAdapter != nullptr);

		ID3D12Device* device = nullptr;
		//機能レベルとログ出力用の文字列
		D3D_FEATURE_LEVEL featureLevels[] = {
			D3D_FEATURE_LEVEL_12_2,D3D_FEATURE_LEVEL_12_1,D3D_FEATURE_LEVEL_12_0
		};
		const char* featureLevelStrings[] = { "12.2", "12.1", "12.0" };
		//高い順に生成できるか試していく
		for (size_t i = 0; i < _countof(featureLevels); i++) {
			hr = D3D12CreateDevice(useAdapter, featureLevels[i], IID_PPV_ARGS(&device));
			//指定した機能レベルでデバイスが生成できたか確認
			if (SUCCEEDED(hr)) {
				//生成できたのでログ出力を行ってループを抜ける
				Log(std::format("FeatureLevel ] {}\n", featureLevelStrings[i]));
				break;
			}
		}
		//デバイスの生成がうまくいかなかったので起動できない
		assert(device != nullptr);
		Log("Complete create D3D12Device!!!\n");//初期化完了ログを出す


		//段階的に分けてエラーと警告を表示し、停止する。
		// 1.インフォキューを生成する
		// 2.インフォキューに値が入ったらif文が作動する
		// 3.やばいエラーの時に作動する
		// 4.エラーの時に止まる
		// 5.警告時に止まる
		// 6.何もなかったら開放する。

#ifdef _DEBUG
		ID3D12InfoQueue* infoQueue = nullptr;//1.end
		if (SUCCEEDED(device->QueryInterface(IID_PPV_ARGS(&infoQueue)))) {//2.end
			//やばいエラーの時に止まる
			infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, true);

			//エラーの時に止まる
			infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, true);

			////警告時に止まる
			//infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, true);

			//開放
			infoQueue->Release();

			// エラーと警告の抑制（windowsの不具合によるエラー表示などを無視するための設定をする）
			// 1.抑制するメッセージのIDを出す
			// 2.抑制するレベルを設定する
			// 3.指定したメッセージの表示を抑制する

			//抑制するメッセージのID
			D3D12_MESSAGE_ID denyIds[] = {
				//Windows11でのDXGIデバックレイヤーとDXGIデバックレイヤーの相互作用バグによるエラーメッセージ
				D3D12_MESSAGE_ID_RESOURCE_BARRIER_MISMATCHING_COMMAND_LIST_TYPE
			};//1.end

			//抑制するレベル
			D3D12_MESSAGE_SEVERITY severities[] = { D3D12_MESSAGE_SEVERITY_INFO };
			D3D12_INFO_QUEUE_FILTER filter{};
			filter.DenyList.NumIDs = _countof(denyIds);
			filter.DenyList.pIDList = denyIds;
			filter.DenyList.NumSeverities = _countof(severities);
			filter.DenyList.pSeverityList = severities;//2.end

			//指定したメッセージの表示を抑制する
			infoQueue->PushStorageFilter(&filter);

		}
#endif // _DEBUG


		//コマンドキューを生成する
		ID3D12CommandQueue* commandQueue = nullptr;
		D3D12_COMMAND_QUEUE_DESC commandQueueDesc{};
		hr = device->CreateCommandQueue(&commandQueueDesc,
			IID_PPV_ARGS(&commandQueue));
		//コマンドキューの生成がうまくいかなかったので起動できない
		assert(SUCCEEDED(hr));

		//コマンドアロケータを生成する
		ID3D12CommandAllocator* commandAllocator = nullptr;
		hr = device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&commandAllocator));
		//コマンドアロケータの生成がうまくいかなかったので起動できない
		assert(SUCCEEDED(hr));

		//コマンドリストを生成する
		ID3D12GraphicsCommandList* commandList = nullptr;
		hr = device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, commandAllocator, nullptr, IID_PPV_ARGS(&commandList));
		//コマンドリストの生成がうまくいかなかったので起動できない
		assert(SUCCEEDED(hr));

		//01_00の12ページから始まる4/17
		// スワップチェーンを作成する
		IDXGISwapChain4* swapChain = nullptr;
		DXGI_SWAP_CHAIN_DESC1 swapChainDesc{};
		swapChainDesc.Width = kClientWidth;		//画面の幅。ウィンドウのクライアント領域を同じものにしておく。
		swapChainDesc.Height = kClientHeight;	//画面の高さ。ウィンドウのクライアント領域を同じものにしておく。
		swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;//色の形式
		swapChainDesc.SampleDesc.Count = 1;//マルチサンプルしない
		swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;//描画のターゲットとして利用する
		swapChainDesc.BufferCount = 2;//ダブルバッファ
		swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;//モニタにうつしたら中身を破棄
		//コマンドキュー、ウィンドウハンドル設定を渡して生成する
		hr = dxgiFactory->CreateSwapChainForHwnd(commandQueue, hwnd, &swapChainDesc, nullptr, nullptr, reinterpret_cast<IDXGISwapChain1**>(&swapChain));
		assert(SUCCEEDED(hr));

		//RTV用のヒープでディスクリプタの数は2．RTVはShader内で触るものではないため、ShaderVisibleはfalse
		ID3D12DescriptorHeap* rtvDescriptorHeap = CreateDescriptorHeap(device, D3D12_DESCRIPTOR_HEAP_TYPE_RTV, 2, false);
		//SRV用のヒープでディスクリプタの数は128。SRVはshader内で触るものなので、shaderVisibleはtrue
		ID3D12DescriptorHeap* srvDescriptorHeap = CreateDescriptorHeap(device, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, 128, true);
		//DSV用のヒープでディスクリプタの数は1。DSVはShader内で触るものではないので、ShaderVisibleはfalse
		ID3D12DescriptorHeap* dsvDescriptorHeap = CreateDescriptorHeap(device, D3D12_DESCRIPTOR_HEAP_TYPE_DSV, 1, false);


		//SwapChainからResourceを引っ張ってくる
		ID3D12Resource* swapChainResources[2] = { nullptr };
		hr = swapChain->GetBuffer(0, IID_PPV_ARGS(&swapChainResources[0]));
		//うまく取得できなければ起動できない
		assert(SUCCEEDED(hr));
		hr = swapChain->GetBuffer(1, IID_PPV_ARGS(&swapChainResources[1]));
		assert(SUCCEEDED(hr));


		//01_00の20ページから始まる4/18

		//RTVの設定
		D3D12_RENDER_TARGET_VIEW_DESC rtvDesc{};
		rtvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;//出力結果をSRGBに変換して書き込む
		rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;//2dテクスチャとして書き込む

		//ディスクリプタの先頭を取得する
		D3D12_CPU_DESCRIPTOR_HANDLE rtvStartHandle = rtvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();

		//RTVを２つ作るのでディスクリプタを２つ用意
		D3D12_CPU_DESCRIPTOR_HANDLE rtvHandles[2];

		//まず１つ目を作る。一つ目は最初の所に作る。作る場所をこちらで指定してあげる必要がある。
		rtvHandles[0] = rtvStartHandle;
		device->CreateRenderTargetView(swapChainResources[0], &rtvDesc, rtvHandles[0]);

		//２つ目のディスクリプタハンドルを得る(自力で)
		//ポインタの位置をずらすみたいに大きくする。
		rtvHandles[1].ptr = rtvHandles[0].ptr + device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

		//２つ目を作る
		device->CreateRenderTargetView(swapChainResources[1], &rtvDesc, rtvHandles[1]);

		//Fenceを作る
		//初期値0でFenceを作る
		ID3D12Fence* fence = nullptr;
		uint64_t fenceValue = 0;
		hr = device->CreateFence(fenceValue, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence));
		assert(SUCCEEDED(hr));

		//FenceのSignalを待つためのイベントを作成する
		HANDLE fenceEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
		assert(fenceEvent != nullptr);

		// dxcCompilerを初期化
		IDxcUtils* dxcUtils = nullptr;
		IDxcCompiler3* dxcCompiler = nullptr;
		hr = DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(&dxcUtils));
		assert(SUCCEEDED(hr));
		hr = DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(&dxcCompiler));
		assert(SUCCEEDED(hr));


		//現時点でincludeはしないが、includeに対応するための設定を行っておく
		IDxcIncludeHandler* includeHandler = nullptr;
		hr = dxcUtils->CreateDefaultIncludeHandler(&includeHandler);
		assert(SUCCEEDED(hr));

		//02_00_29ページの内容

		// RootSignature作成
		D3D12_ROOT_SIGNATURE_DESC descriptionRootSignature{};
		descriptionRootSignature.Flags =
			D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

		D3D12_STATIC_SAMPLER_DESC staticSamplers[1] = {};
		staticSamplers[0].Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;//
		staticSamplers[0].AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;//
		staticSamplers[0].AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
		staticSamplers[0].AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
		staticSamplers[0].ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;//
		staticSamplers[0].MaxLOD = D3D12_FLOAT32_MAX;//
		staticSamplers[0].ShaderRegister = 0;
		staticSamplers[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
		descriptionRootSignature.pStaticSamplers = staticSamplers;
		descriptionRootSignature.NumStaticSamplers = _countof(staticSamplers);



		D3D12_DESCRIPTOR_RANGE descriptorRange[1] = {};
		descriptorRange[0].BaseShaderRegister = 0; //0から始まる
		descriptorRange[0].NumDescriptors = 1;//数は１つ
		descriptorRange[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;//SRVを使う
		descriptorRange[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;//Offsetを自動計算


		//RootParameter作成。複数設定できるので配列。今回は結果１つだけなので長さ１の配列
		D3D12_ROOT_PARAMETER rootParameters[3] = {};
		rootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;//CBVを使う
		rootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;//PixelShaderで使う
		rootParameters[0].Descriptor.ShaderRegister = 0;//レジスタ番号0とバインド

		rootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;//CBVを使う
		rootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;//VertexShaderで使う
		rootParameters[1].Descriptor.ShaderRegister = 0;//レジスタ番号0とバインド

		rootParameters[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;//DescriptorTableを使う
		rootParameters[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;//VertexShaderで使う
		rootParameters[2].DescriptorTable.pDescriptorRanges = descriptorRange;//Tableの中身の配列を指定
		rootParameters[2].DescriptorTable.NumDescriptorRanges = _countof(descriptorRange);//Tableで利用する数

		descriptionRootSignature.pParameters = rootParameters;//ルートパラメータ配列へのポインタ
		descriptionRootSignature.NumParameters = _countof(rootParameters);//配列の長さ


		//シリアライズしてバイナリする
		ID3DBlob* signatureBlob = nullptr;
		ID3DBlob* errorBlob = nullptr;
		hr = D3D12SerializeRootSignature(&descriptionRootSignature,
			D3D_ROOT_SIGNATURE_VERSION_1, &signatureBlob, &errorBlob);
		if (FAILED(hr)) {
			Log(reinterpret_cast<char*>(errorBlob->GetBufferPointer()));
			assert(false);
		}
		//バイナリを元に生成
		ID3D12RootSignature* rootSignaturre = nullptr;
		hr = device->CreateRootSignature(0, signatureBlob->GetBufferPointer(),
			signatureBlob->GetBufferSize(), IID_PPV_ARGS(&rootSignaturre));
		assert(SUCCEEDED(hr));

		//InputLayout
		D3D12_INPUT_ELEMENT_DESC inputElementDescs[2] = {};
		inputElementDescs[0].SemanticName = "POSITION";
		inputElementDescs[0].SemanticIndex = 0;
		inputElementDescs[0].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		inputElementDescs[0].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
		inputElementDescs[1].SemanticName = "TEXCOORD";
		inputElementDescs[1].SemanticIndex = 0;
		inputElementDescs[1].Format = DXGI_FORMAT_R32G32_FLOAT;
		inputElementDescs[1].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
		D3D12_INPUT_LAYOUT_DESC inputLayoutDesc{};
		inputLayoutDesc.pInputElementDescs = inputElementDescs;
		inputLayoutDesc.NumElements = _countof(inputElementDescs);

		//BlendStateの設定
		D3D12_BLEND_DESC blendDesc{};
		//すべての色要素を書き込む
		blendDesc.RenderTarget[0].RenderTargetWriteMask =
			D3D12_COLOR_WRITE_ENABLE_ALL;

		//RasiterzerStateの設定
		D3D12_RASTERIZER_DESC resterizerDesc{};
		//裏側(時計回り)を表示しない
		resterizerDesc.CullMode = D3D12_CULL_MODE_BACK;
		//三角形の中を塗りつぶす
		resterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;

		//Shaderをコンパイルする
		IDxcBlob* vertexShaderBlob = CompileShader(L"Object3D.VS.hlsl",
			L"vs_6_0", dxcUtils, dxcCompiler, includeHandler
		);
		assert(vertexShaderBlob != nullptr);

		IDxcBlob* pixelShaderBlob = CompileShader(L"Object3D.PS.hlsl",
			L"ps_6_0", dxcUtils, dxcCompiler, includeHandler
		);
		assert(pixelShaderBlob != nullptr);

		D3D12_GRAPHICS_PIPELINE_STATE_DESC graphicsPipelineStateDesc{};
		graphicsPipelineStateDesc.pRootSignature = rootSignaturre;//RootSignature
		graphicsPipelineStateDesc.InputLayout = inputLayoutDesc;//InputLayout
		graphicsPipelineStateDesc.VS = {
			vertexShaderBlob->GetBufferPointer(),vertexShaderBlob->GetBufferSize()
		};//vertexShader
		graphicsPipelineStateDesc.PS = {
			pixelShaderBlob->GetBufferPointer(),pixelShaderBlob->GetBufferSize()
		};//PixelShader
		graphicsPipelineStateDesc.BlendState = blendDesc;//BlendState
		graphicsPipelineStateDesc.RasterizerState = resterizerDesc;//RasterizeState
		//書き込むRTVの情報
		graphicsPipelineStateDesc.NumRenderTargets = 1;
		graphicsPipelineStateDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
		//利用するトポロジ(形状)のタイプ。三角形
		graphicsPipelineStateDesc.PrimitiveTopologyType =
			D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		//どのように画面に色を打ち込むかの設定(気にしなくて良い)
		graphicsPipelineStateDesc.SampleDesc.Count = 1;
		graphicsPipelineStateDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;

		//DepthStencilTextureをウィンドウのサイズで作成
		ID3D12Resource* depthStencilResorce = CreateDepthStencilTextureResource(device, kClientWidth, kClientHeight);

		// DSVの設定
		D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc{};
		dsvDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;//Format。基本的にResourceに合わせる
		dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;// 2dTexture
		//DSVHeapの先頭にDSVを作る
		device->CreateDepthStencilView(depthStencilResorce, &dsvDesc, dsvDescriptorHeap->GetCPUDescriptorHandleForHeapStart());

		//DepthStencilStateの設定
		D3D12_DEPTH_STENCIL_DESC depthStencilDesc{};
		//Depthの機能を有効化する
		depthStencilDesc.DepthEnable = true;
		//書き込みします
		depthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
		//比較関数はLessEqual。つまり、近ければ描画される
		depthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;

		// DepthStencilの設定
		graphicsPipelineStateDesc.DepthStencilState = depthStencilDesc;
		graphicsPipelineStateDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;


		//実際に作成
		ID3D12PipelineState* graphicsPipelineState = nullptr;
		hr = device->CreateGraphicsPipelineState(&graphicsPipelineStateDesc,
			IID_PPV_ARGS(&graphicsPipelineState));
		assert(SUCCEEDED(hr));

		//モデル読み込み
		ModelData modelData = LoadObjFile("resources/model", "plane.obj");

		//頂点リソースを作る
		ID3D12Resource* vertexResource = CreateBufferResource(device, sizeof(VertexData) * modelData.vertices.size());

		//Sprite用の頂点リソースを作る
		ID3D12Resource* vertexResourceSprite = CreateBufferResource(device, sizeof(VertexData) * 6);

		ID3D12Resource* indexResourceSprite = CreateBufferResource(device, sizeof(uint32_t) * 6);

		//マテリアル用のリソースを作る。今回はcolor１つ分のサイズを用意する
		ID3D12Resource* materialResource = CreateBufferResource(device, sizeof(Vector4));



		//WVP用のリソースを作る。Matrix4x4 1つ分のサイズを用意する
		ID3D12Resource* wvpResource = CreateBufferResource(device, sizeof(Matrix4x4));
		//データを読み込む
		Matrix4x4* wvpData = nullptr;
		//書き込むためのアドレスを取得
		wvpResource->Map(0, nullptr, reinterpret_cast<void**>(&wvpData));
		//単位行列を書き込んでおく
		*wvpData = MakeIdentity4x4();

		//Sprite用のTransformationMatrix用のリソースを作る。Matrix4x4 1つ分のサイズを用意する
		ID3D12Resource* transformationMatrixResourceSprite = CreateBufferResource(device, sizeof(Matrix4x4));
		//データを読み込む
		Matrix4x4* transformtionMatrixDataSprite = nullptr;
		//書き込むためのアドレスを取得
		transformationMatrixResourceSprite->Map(0, nullptr, reinterpret_cast<void**>(&transformtionMatrixDataSprite));
		//単位行列を書き込んでおく
		*transformtionMatrixDataSprite = MakeIdentity4x4();

		//マテリアルにデータを書き込む
		Vector4* materialData = nullptr;
		//
		Matrix4x4* transformationMatrixDate = nullptr;

		//書き込むためのアドレスを取得
		materialResource->Map(0, nullptr, reinterpret_cast<void**>(&materialData));
		wvpResource->Map(0, nullptr, reinterpret_cast<void**>(&transformationMatrixDate));

		//今回は赤を書き込んでみる
		*materialData = Vector4(1.0f, 0.0f, 0.0f, 1.0f);

		// 頂点バッファビューを作成する
		D3D12_VERTEX_BUFFER_VIEW vertexBufferView{};
		// リソースの先頭のアドレスから使う
		vertexBufferView.BufferLocation = vertexResource->GetGPUVirtualAddress();
		//　使用するリソースのサイズは頂点３つ分のサイズ
		vertexBufferView.SizeInBytes = UINT(sizeof(VertexData) * modelData.vertices.size());
		//　1頂点あたりのサイズ
		vertexBufferView.StrideInBytes = sizeof(VertexData);

		//以下の4つはSprite用
		//頂点バッファビューを作成する
		D3D12_VERTEX_BUFFER_VIEW vertexBufferViewSprite{};
		//リソースの先頭のアドレスから使う
		vertexBufferViewSprite.BufferLocation = vertexResourceSprite->GetGPUVirtualAddress();
		//使用するリソースのサイズは頂点6つ分のサイズ
		vertexBufferViewSprite.SizeInBytes = sizeof(VertexData) * 6;
		//1頂点あたりのサイズ
		vertexBufferViewSprite.StrideInBytes = sizeof(VertexData);

		D3D12_INDEX_BUFFER_VIEW indexBufferViewSprite{};
		//リソースの先頭のアドレスから使う
		indexBufferViewSprite.BufferLocation = indexResourceSprite->GetGPUVirtualAddress();
		//使用するリソースのサイズはインデックス6つ分のサイズ
		indexBufferViewSprite.SizeInBytes = sizeof(uint32_t) * 6;
		//インデックスはuint32_tとする
		indexBufferViewSprite.Format = DXGI_FORMAT_R32_UINT;


		//　頂点リソースにデータを書き込む
		VertexData* vertexData = nullptr;
		//書き込むためのアドレスを取得
		vertexResource->Map(0, nullptr, reinterpret_cast<void**>(&vertexData));//書き込むためのアドレスを取得
		std::memcpy(vertexData, modelData.vertices.data(), sizeof(VertexData)* modelData.vertices.size());//頂点データをリソースにコピー
		////左下
		//vertexData[0].position = { -0.5f,-0.5f,0.0f,1.0f };
		//vertexData[0].texcoord = { 0.0f,1.0f };
		////上
		//vertexData[1].position = { 0.0f,0.5f,0.0f,1.0f };
		//vertexData[1].texcoord = { 0.5f,0.0f };
		////右下
		//vertexData[2].position = { 0.5f,-0.5f,0.0f,1.0f };
		//vertexData[2].texcoord = { 1.0f,1.0f };

		////左下2
		//vertexData[3].position = { -0.5f,-0.5f,0.5f,1.0f };
		//vertexData[3].texcoord = { 0.0f,1.0f };
		////上2
		//vertexData[4].position = { 0.0f,0.0f,0.0f,1.0f };
		//vertexData[4].texcoord = { 0.5f,0.0f };
		////右下2
		//vertexData[5].position = { 0.5f,-0.5f,-0.5f,1.0f };
		//vertexData[5].texcoord = { 1.0f,1.0f };

		//頂点リソースのにデータを書き込む(Sprite)
		VertexData* vertexDataSprite = nullptr;
		//書き込むためのアドレスを取得(Sprite)
		vertexResourceSprite->Map(0, nullptr, reinterpret_cast<void**>(&vertexDataSprite));

		//1枚目の三角形
		vertexDataSprite[0].position = { 0.0f,360.0f,0.0f,1.0f };//左下
		vertexDataSprite[0].texcoord = { 0.0f,1.0f };
		vertexDataSprite[1].position = { 0.0f,0.0f,0.0f,1.0f };//左上
		vertexDataSprite[1].texcoord = { 0.0f,0.0f };
		vertexDataSprite[2].position = { 640.0f,360.0f,0.0f,1.0f };//右下
		vertexDataSprite[2].texcoord = { 1.0f,1.0f };
		//2枚目の三角形
		vertexDataSprite[3].position = { 0.0f,0.0f,0.0f,1.0f };//左上
		vertexDataSprite[3].texcoord = { 0.0f,0.0f };
		vertexDataSprite[4].position = { 640.0f,0.0f,0.0f,1.0f };//右上
		vertexDataSprite[4].texcoord = { 1.0f,0.0f };
		vertexDataSprite[5].position = { 640.0f,360.0f,0.0f,1.0f };//右下
		vertexDataSprite[5].texcoord = { 1.0f,1.0f };


		//インデックスリソースにデータを書き込む
		uint32_t* indexDataSprite = nullptr;
		indexResourceSprite->Map(0, nullptr, reinterpret_cast<void**>(&indexDataSprite));
		indexDataSprite[0] = 0;
		indexDataSprite[1] = 1;
		indexDataSprite[2] = 2;
		indexDataSprite[3] = 1;
		indexDataSprite[4] = 3;
		indexDataSprite[5] = 2;

		//ビューポート
		D3D12_VIEWPORT viewport{};
		//クライアント領域のサイズと一緒にして画面全体を表示
		viewport.Width = kClientWidth;
		viewport.Height = kClientHeight;
		viewport.TopLeftX = 0;
		viewport.TopLeftY = 0;
		viewport.MinDepth = 0.0f;
		viewport.MaxDepth = 1.0f;

		//シザー矩形
		D3D12_RECT scissorRect{};
		//基本的にビューポートと同じく系で構成されるようにする
		scissorRect.left = 0;
		scissorRect.right = kClientWidth;
		scissorRect.top = 0;
		scissorRect.bottom = kClientHeight;

		//　ImGuiの初期化。詳細はさして重要ではないため解説は省略する。
		//　こうゆうもんである
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGui::StyleColorsDark();
		ImGui_ImplWin32_Init(hwnd);
		ImGui_ImplDX12_Init(device,
			swapChainDesc.BufferCount,
			rtvDesc.Format,
			srvDescriptorHeap,
			srvDescriptorHeap->GetCPUDescriptorHandleForHeapStart(),
			srvDescriptorHeap->GetGPUDescriptorHandleForHeapStart()
		);

		//Transform変数を作る
		Transform transform{ {1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f},{0.0f,0.0f,0.0f} };
		Transform cameraTransform{ {1.0f,1.0f,1.0f}, {0.0f,0.0f,0.0f},{0.0f,0.0f,-5.0f} };

		Transform transformSprite{ {1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f},{0.0f,0.0f,0.0f} };

		//Textureを読んで転送する
		DirectX::ScratchImage mipImages = LoadTexture("resources/uvChecker.png");
		const DirectX::TexMetadata& metadata = mipImages.GetMetadata();
		ID3D12Resource* textureResource = CreateTextureResource(device, metadata);
		UploadTextureData(textureResource, mipImages);

		//DirectX::ScratchImage mipImages2 = LoadTexture(modelData.material.textureFilePath);

		


		// metaDataを基にSRVの設定
		D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
		srvDesc.Format = metadata.format;
		srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;//2Dテクスチャ
		srvDesc.Texture2D.MipLevels = UINT(metadata.mipLevels);

		//SRVを作成するDescriptorHeapの場所を決める
		D3D12_CPU_DESCRIPTOR_HANDLE textureSrvHandleCPU = srvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
		D3D12_GPU_DESCRIPTOR_HANDLE textureSrvHandleGPU = srvDescriptorHeap->GetGPUDescriptorHandleForHeapStart();
		//先頭はImGuiを使っているのでその次を使う
		textureSrvHandleCPU.ptr += device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
		textureSrvHandleGPU.ptr += device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
		// SRVの生成
		device->CreateShaderResourceView(textureResource, &srvDesc, textureSrvHandleCPU);



		MSG msg{};
		//ウィンドウのxボタンが押されるまでループ
		while (msg.message != WM_QUIT) {
			//windowにメッセージが来てたら最優先で処理させる
			if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			} else {
				//ゲームの処理

				ImGui_ImplDX12_NewFrame();
				ImGui_ImplWin32_NewFrame();
				ImGui::NewFrame();
				//開発用UIの処理。実際に開発用のUIを出す場合はここをゲーム固有の処理に置き換える
				//ImGui::ShowDemoWindow();
				ImGui::Begin("Window");
				//変える変数の名前,変えるデータ,変える速度
				ImGui::ColorEdit3("color", &materialData->x);
				//ImGui::DragFloat3("CameraTranslate", &cameraTransform, 0.01f);
				//ImGui::DragFloat3("CameraTranslate", &cameraRotate.x, 0.01f);
				//ImGui::DragFloat("SphereRadius", &sphere.radius, 0.01f);
				ImGui::DragFloat3("spriteRotato", &transformSprite.rotate.x, 0.01f);
				ImGui::DragFloat3("spriteScale", &transformSprite.scale.x, 0.01f);
				ImGui::DragFloat3("spriteTranslate", &transformSprite.translate.x, 0.01f);
				ImGui::DragFloat3("modelRotato", &transform.rotate.x, 0.01f);
				ImGui::DragFloat3("modelScale", &transform.scale.x, 0.01f);
				ImGui::DragFloat3("modelTranslate", &transform.translate.x, 0.01f);
				ImGui::End();

				transform.rotate.y += 0.03f;
				Matrix4x4 worldMatrix = MakeAffineMatrix(transform.scale, transform.rotate, transform.translate);
				*wvpData = worldMatrix;

				Matrix4x4 cameraMatrix = MakeAffineMatrix(cameraTransform.scale, cameraTransform.rotate, cameraTransform.translate);
				Matrix4x4 viewMatrix = Inverse(cameraMatrix);
				Matrix4x4 projectionMatrix = MakePerspectiveFovMatrix(0.45f, float(kClientWidth) / float(kClientHeight), 0.1f, 100.0f);
				//WVPMatrixを作る
				Matrix4x4 worldViewProjectionMatrix = Multply(worldMatrix, Multply(viewMatrix, projectionMatrix));
				*transformationMatrixDate = worldViewProjectionMatrix;

				//Sprite用のworldViewProjectionMatrixを作る
				Matrix4x4 worldMatrixSprite = MakeAffineMatrix(transformSprite.scale, transformSprite.rotate, transformSprite.translate);
				Matrix4x4 viewMatrixSprite = MakeIdentity4x4();
				Matrix4x4 projectionMatrixSprite = MakeOrthographicMatrix(0.0f, 0.0f, float(kClientWidth), float(kClientHeight), 0.0f, 100.0f);
				Matrix4x4 worldViewProjectionMatrixSprite = Multply(worldMatrixSprite, Multply(viewMatrixSprite, projectionMatrixSprite));
				*transformtionMatrixDataSprite = worldViewProjectionMatrixSprite;


				//コマンドを積み込んで確定させる
				// 1.2つあるResourceのうち、どちらが今BackBufferなのかをSwapChainに問い合わせる
				// 2.CommandListに今から描画するRTVを設定する
				// 3.RTVに対して指定した色で画面をクリアする
				// 4.CommandListを閉じて内容を確定させる。

				//ImGuiの内部コマンドを生成する
				ImGui::Render();


				//これから書き込むバックバッファのインデックスを取得
				UINT backBufferIndex = swapChain->GetCurrentBackBufferIndex();//1.end

				/***TransitionBarrierを張る***/

				// TransitionBarrierの設定
				D3D12_RESOURCE_BARRIER barrier{};
				// 今回のバリアはTransition
				barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
				// Noneにしておく。
				barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
				// バリアを張る対象のリソース。現在のバックバッファに対して行う。
				barrier.Transition.pResource = swapChainResources[backBufferIndex];
				// 遷移前（現在）のResourceState
				barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
				// 遷移後のResourceState
				barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
				// TransitionBarrierを張る
				commandList->ResourceBarrier(1, &barrier);

				//描画先のRTVを設定する。
				D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = dsvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
				commandList->OMSetRenderTargets(1, &rtvHandles[backBufferIndex], false, &dsvHandle);//2.end

				//指定した色で画面全体をクリアする
				float clearColor[] = { 0.1f,0.25f,0.5f,1.0f };//青っぽい色。RGBAの順
				commandList->ClearRenderTargetView(rtvHandles[backBufferIndex], clearColor, 0, nullptr);//3.end

				//描画用のDescriptorHeapの設定
				ID3D12DescriptorHeap* descriptorHeaps[] = { srvDescriptorHeap };
				commandList->SetDescriptorHeaps(1, descriptorHeaps);

				//　コマンドを積む(三角形の描画)
				commandList->RSSetViewports(1, &viewport);//Viewportを設定
				commandList->RSSetScissorRects(1, &scissorRect);//Scirssorを設定
				//　RootSignatureを設定。PSOに設定しているけど別途設定が必要
				commandList->SetGraphicsRootSignature(rootSignaturre);
				commandList->SetPipelineState(graphicsPipelineState);//PSOを設定
				commandList->IASetVertexBuffers(0, 1, &vertexBufferView);//VBVを設定
				//　形状を設定。PSOに設定しているものとはまた別。同じものを設定すると考えておけば良い
				commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
				//マテリアルCBufferの場所を設定
				commandList->SetGraphicsRootConstantBufferView(0, materialResource->GetGPUVirtualAddress());
				//wvp用のCBufferの場所を設定
				commandList->SetGraphicsRootConstantBufferView(1, wvpResource->GetGPUVirtualAddress());
				// SRVのDescriptorTableの先頭を設定。2はrootParameter[2]である。
				commandList->SetGraphicsRootDescriptorTable(2, textureSrvHandleGPU);

				//指定した深度で画面全体をクリアする
				commandList->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);

				////　描画！！(DrawCall/ドローコール)。３頂点で１つのインスタンス。インスタンスについては今度
				commandList->DrawInstanced(UINT(modelData.vertices.size()), 1, 0, 0);


				//Spriteの描画
				commandList->IASetVertexBuffers(0, 1, &vertexBufferViewSprite);
				//TransformationMatrixCBufferの場所を設定
				commandList->SetGraphicsRootConstantBufferView(1, transformationMatrixResourceSprite->GetGPUVirtualAddress());
				////描画!(DrawCall/ドローコール)
				commandList->DrawInstanced(6, 1, 0, 0);

				//頂点インデックスの描画
				commandList->IASetIndexBuffer(&indexBufferViewSprite);
				////描画！(DrawCall/ドローコール)6このインデックスを使用し1つのインスタンスを描画。その他は当面0でよい
				commandList->DrawIndexedInstanced(6, 1, 0, 0, 0);


				//　実際のcommandListのImGuiの描画コマンドを積む
				ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), commandList);


				// 画面に書く処理はすべて終わり、画面に映すので、状態を遷移
				// 今回はRenderTargetからPresentにする
				barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
				barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
				//TransitionBarrierを張る
				commandList->ResourceBarrier(1, &barrier);

				//コマンドリストの内容を確定させる。すべてのコマンドを積んでからcloseすること
				hr = commandList->Close();
				assert(SUCCEEDED(hr));//4.end


				//コマンドをキックする
				// 1.CommandListが完成したので、CommandQueueを使ってGPUにキックする
				// 2.実行が終わったら、画面が完成したので画面の交換をしてもらう
				//	a.これは、SwapChain作成時に指定したCommandQueueを介して行われる
				//	b.画面交換用のExecuteCommandListを行っていると考えると良い
				// 3.画面の交換をしたら次のフレームの準備をする
				//	a.実際に保存する場所を管理しているAllocatorとCommandListの両方をResetする

				//GPUにコマンドリストの実行を行わせる
				ID3D12CommandList* commandLists[] = { commandList };
				commandQueue->ExecuteCommandLists(1, commandLists);//1.end

				//GPUとOSに画面の交換を行うよう通知する
				swapChain->Present(1, 0);//2.end

				//Signalを送る
				// 1.実行が完了したタイミングでFenceに指定した値を書き込んでもらう
				// 2.CPUではFenceに指定した値が書き込まれているかを確認する
				// 3.指定した値が書き込まれていないのであれば、書き込まれるまで待つ

				//Fenceの値を更新
				fenceValue++;

				//GPUがここまでたどり着いたときに、Fenceの値を指定した値に代入するようにSignalを送る
				commandQueue->Signal(fence, fenceValue);

				//Fenceの値が指定したSignal値にたどり着いているか確認する
				//GetCompletedValueの初期値はFence作成時に渡した初期値
				if (fence->GetCompletedValue() < fenceValue) {
					//指定したSignalにたどり着いていないので、たどり着くまで待つようにイベントを設定する
					fence->SetEventOnCompletion(fenceValue, fenceEvent);
					//イベントを待つ
					WaitForSingleObject(fenceEvent, INFINITE);
				}

				//次のフレーム用のコマンドリストを準備
				hr = commandAllocator->Reset();
				assert(SUCCEEDED(hr));
				hr = commandList->Reset(commandAllocator, nullptr);
				assert(SUCCEEDED(hr));//3.end



			}

			//COMの終了処理
			CoUninitialize();

		}

		//開放処理
		CloseHandle(fenceEvent);
		fence->Release();
		rtvDescriptorHeap->Release();
		swapChainResources[0]->Release();
		swapChainResources[1]->Release();
		swapChain->Release();
		commandList->Release();
		commandAllocator->Release();
		commandQueue->Release();
		device->Release();
		useAdapter->Release();
		dxgiFactory->Release();

		vertexResource->Release();
		graphicsPipelineState->Release();
		signatureBlob->Release();
		if (errorBlob) {
			errorBlob->Release();
		}
		rootSignaturre->Release();
		pixelShaderBlob->Release();
		vertexShaderBlob->Release();
		materialResource->Release();

#ifdef _DEBUG
		debugController->Release();
#endif // _DEBUG
		CloseWindow(hwnd);

		//ImGuiの終了処理。詳細はさして重要ではないので解説は省略する。
		//こういうもんである。初期化と逆順に行う
		ImGui_ImplDX12_Shutdown();
		ImGui_ImplWin32_Shutdown();
		ImGui::DestroyContext();


		//リソースリークチェック
		IDXGIDebug1* debug;
		if (SUCCEEDED(DXGIGetDebugInterface1(0, IID_PPV_ARGS(&debug)))) {
			debug->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_ALL);
			debug->ReportLiveObjects(DXGI_DEBUG_APP, DXGI_DEBUG_RLO_ALL);
			debug->ReportLiveObjects(DXGI_DEBUG_D3D12, DXGI_DEBUG_RLO_ALL);
			debug->Release();
		}


	}
	return 0;
}