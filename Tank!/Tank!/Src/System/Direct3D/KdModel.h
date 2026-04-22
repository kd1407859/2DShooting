#pragma once

//==========================================================
// メッシュ用 頂点情報
//==========================================================
struct MeshVertex
{
	Math::Vector3	Pos;		// 座標
	Math::Vector2	UV;			// UV
	Math::Vector3	Normal;		// 法線
};

//==========================================================
// メッシュ用 面情報
//==========================================================
struct MeshFace
{
	UINT Idx[3];				// 三角形を構成する頂点のIndex
};

//==========================================================
// メッシュ用 サブセット情報
//==========================================================
struct MeshSubset
{
//	std::shared_ptr<Material>	MaterialData;		// マテリアルデータへの参照
	UINT		MaterialNo = 0;		// マテリアルNo

	UINT		FaceStart = 0;		// 面Index　このマテリアルで使用されている最初の面のIndex
	UINT		FaceCount = 0;		// 面数　FaceStartから、何枚の面が使用されているかの
};

//==========================================================
//
// メッシュクラス
//
//==========================================================
class KdMesh
{
public:

	//=================================================
	// 取得・設定
	//=================================================

	// サブセット情報配列を取得
	const std::vector<MeshSubset>&		GetSubsets() const { return m_subsets; }

	// 頂点形式取得
//	const VertexDecl&					GetVertexDecl() const { return m_vertexDecl; }

	// 頂点の座標配列を取得
	const std::vector<Math::Vector3>&	GetVertexPositions() const { return m_positions; }
	// 面の配列を取得
	const std::vector<MeshFace>&		GetFaces() const { return m_faces; }

	// 軸平行境界ボックス取得
	const DirectX::BoundingBox&			GetBoundingBox() const { return m_aabb; }
	// 境界球取得
	const DirectX::BoundingSphere&		GetBoundingSphere() const { return m_bs; }

	// メッシュデータをデバイスへセットする
	void SetToDevice() const;


	//=================================================
	// 作成・解放
	//=================================================

	// メッシュ作成
	// ・vertices		… 頂点配列
	// ・faces			… 面インデックス情報配列
	// ・subsets		… サブセット情報配列
	// 戻り値			… 成功：true
	bool Create(const std::vector<MeshVertex>& vertices, const std::vector<MeshFace>& faces, const std::vector<MeshSubset>& subsets);

	// 解放
	void Release()
	{
		m_vb.Release();
		m_ib.Release();
		m_subsets.clear();
		m_positions.clear();
		m_faces.clear();
	}

	~KdMesh()
	{
		Release();
	}

	//=================================================
	// 処理
	//=================================================

	// 指定サブセットを描画
	void DrawSubset(int subsetNo) const;

	// 
	KdMesh() {}

private:

	// 頂点バッファ
	KdBuffer						m_vb;
	// インデックスバッファ
	KdBuffer						m_ib;
	// 頂点形式
//	VertexDecl					m_vertexDecl;

	// サブセット情報
	std::vector<MeshSubset>		m_subsets;

	// 境界データ
	DirectX::BoundingBox		m_aabb;	// 軸平行境界ボックス
	DirectX::BoundingSphere		m_bs;	// 境界球

	// 座標のみの配列(複製)
	std::vector<Math::Vector3>	m_positions;
	// 面情報のみの配列(複製)
	std::vector<MeshFace>		m_faces;

private:
	// コピー禁止用
	KdMesh(const KdMesh& src) = delete;
	void operator=(const KdMesh& src) = delete;
};

//==========================================================
// マテリアル(glTFベースのPBRマテリアル)
//==========================================================
struct KdMaterial
{
	//---------------------------------------
	// 材質データ
	//---------------------------------------

	// 名前
	std::string					Name;

	// 基本色
	std::shared_ptr<KdTexture>	BaseColorTex;				// 基本色テクスチャ
	Math::Vector4				BaseColor = { 1,1,1,1 };	// 基本色のスケーリング係数(RGBA)

	// 金属性、粗さ
	std::shared_ptr<KdTexture>	MetallicRoughnessTex;		// B:金属製 G:粗さ
	float						Metallic = 0.0f;			// 金属性のスケーリング係数
	float						Roughness = 1.0f;			// 粗さのスケーリング係数

	// 自己発光
	std::shared_ptr<KdTexture>	EmissiveTex;				// 自己発光テクスチャ
	Math::Vector3				Emissive = { 0,0,0 };		// 自己発光のスケーリング係数(RGB)

	// 法線マップ
	std::shared_ptr<KdTexture>	NormalTex;

	KdMaterial()
	{
		BaseColorTex			= D3D.GetWhiteTex();
		MetallicRoughnessTex	= D3D.GetWhiteTex();
		EmissiveTex				= D3D.GetWhiteTex();
	}
};

//==========================================================
//
// ノード配列やマテリアル配列を組み合わせ、１つのモデルデータとしたクラス
//
//==========================================================
class KdModel
{
public:

	//==========================================================
	// 取得
	//==========================================================

	// メッシュ取得
	const std::shared_ptr<KdMesh>&	GetMesh() const { return m_mesh; }
	// マテリアル配列取得
	const std::vector<KdMaterial>&	GetMaterials() const { return m_materials; }

	//==========================================================
	// 作成・解放
	//==========================================================

	// glTF形式の3Dモデルをロードする
	// ・filename		… ファイル名
	// 戻り値			… 成功：true
	bool Load(const std::string& filename);

	// 解放
	void Release()
	{
//		m_allNodes.clear();
//		m_onlyRootNodes.clear();
		m_mesh = nullptr;
		m_materials.clear();
	}

	~KdModel() {
		Release();
	}

	// 
	KdModel() {}

private:

	// メッシュ
	std::shared_ptr<KdMesh>		m_mesh;

	// マテリアル配列
	std::vector<KdMaterial>		m_materials;

private:
	// コピー禁止用
	KdModel(const KdModel& src) = delete;
	void operator=(const KdModel& src) = delete;
};

