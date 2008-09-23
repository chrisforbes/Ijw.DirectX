#pragma once

using namespace Ijw::Math;

namespace Ijw { namespace DirectX
{
	public value class TraceResult
	{
	public:
		bool hit;
		float t;
		float u;
		float v;
		DWORD faceIndex;
	};

	generic< typename T > where T : value class
	public ref class Mesh
	{
	internal:
		ID3DXMesh* mesh;

	public:
		Mesh( GraphicsDevice^ device, VertexFormat vertexFormat, array<T>^ vertices, array<short>^ indices, 
			array<int>^ attributes )
		{
			ID3DXMesh* m;
			HRESULT hr;

			if (FAILED( hr = 
				D3DXCreateMeshFVF( indices->Length / 3, vertices->Length, D3DXMESH_MANAGED, (DWORD)vertexFormat,
				device->device, &m)))
				ThrowHelper::Hr(hr);

			mesh = m;

			{
				void* vb;
				mesh->LockVertexBuffer(0, &vb);
				pin_ptr<T> t = &vertices[0];
				memcpy( vb, t, vertices->Length * sizeof(T) );
				mesh->UnlockVertexBuffer();
			}

			{
				void* ib;
				mesh->LockIndexBuffer(0, &ib);
				pin_ptr<short> t = &indices[0];
				memcpy( ib, t, indices->Length * sizeof(short) );
				mesh->UnlockIndexBuffer();
			}

			{
				DWORD* ab;
				mesh->LockAttributeBuffer(0, &ab);
				pin_ptr<int> t = &attributes[0];
				memcpy( ab, t, attributes->Length * sizeof(int) );
				mesh->UnlockAttributeBuffer();
			}

			DWORD* adjacency = new DWORD[ indices->Length ];
			mesh->GenerateAdjacency(1e-3f, adjacency);
			mesh->OptimizeInplace(D3DXMESHOPT_ATTRSORT, adjacency, NULL, NULL, NULL);
			delete[] adjacency;
		}

		void DrawSubset( int attributeId )
		{
			mesh->DrawSubset( attributeId );
		}

		TraceResult Intersect( Ijw::Math::Vector3 rayStart, Ijw::Math::Vector3 rayDir)
		{
			TraceResult tr;
			ID3DXBuffer * crap = 0;
			DWORD amountOfCrap;
			BOOL hit;

			if (FAILED(D3DXIntersect( mesh, (D3DXVECTOR3 const *) &rayStart, (D3DXVECTOR3 const *) &rayDir,
				&hit, &tr.faceIndex, &tr.u, &tr.v, &tr.t, &crap, &amountOfCrap )))
				tr.hit = false;
			else
				tr.hit = (bool)hit;

			safe_release( crap )

			return tr;
		}

		~Mesh()
		{
			safe_release( mesh );
		}
	};
}}
