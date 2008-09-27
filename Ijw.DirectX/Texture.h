#pragma once

namespace Ijw { namespace DirectX
{
	public ref class Texture
	{
	private:
		Texture( IDirect3DBaseTexture9* texture ) : texture(texture), surf(0) {}

	internal:
		IDirect3DBaseTexture9* texture;
		IDirect3DSurface9* surf;

	public:
		IntPtr GetSurface()
		{
			if (!surf)
			{
				IDirect3DSurface9* _surf;
				HRESULT hr = ((IDirect3DTexture9*)texture)->GetSurfaceLevel( 0, &_surf );
				if (FAILED( hr ))
					throw gcnew InvalidOperationException( "Cant get surface. Wrong texture type?" );
				surf = _surf;
			}
			return IntPtr(surf);
		}

		static Texture^ Create( Stream^ stream, GraphicsDevice^ device )
		{
			stream->Position = 0;
			array<unsigned char>^ data = gcnew array<unsigned char>( (int)stream->Length );
			stream->Read( data, 0, data->Length );

			HRESULT hr;
			IDirect3DBaseTexture9* tex;

			pin_ptr<unsigned char> p = &data[0];

			if (FAILED( hr = D3DXCreateTextureFromFileInMemory( device->device, p, data->Length, (IDirect3DTexture9**)&tex ) ))
				throw gcnew InvalidOperationException("Texture load failed");

			return gcnew Texture( tex );
		}

		static Texture^ CreateCube( Stream^ stream, GraphicsDevice^ device )
		{
			stream->Position = 0;
			array<unsigned char>^ data = gcnew array<unsigned char>((int)stream->Length);
			stream->Read( data, 0, data->Length );

			HRESULT hr;
			IDirect3DBaseTexture9* tex;

			pin_ptr<unsigned char> p = &data[0];

			if (FAILED(hr = D3DXCreateCubeTextureFromFileInMemory( device->device, p, data->Length,
				(IDirect3DCubeTexture9**)&tex )))
				throw gcnew InvalidOperationException("Texture load failed");

			tex->GenerateMipSubLevels();

			return gcnew Texture( tex );
		}

		void SetLod( int n )
		{
			texture->SetLOD(n);
		}

		static Texture^ CreateRenderTarget( GraphicsDevice^ device, int width, int height )
		{
			HRESULT hr;
			IDirect3DBaseTexture9* tex;
			if (FAILED( hr = D3DXCreateTexture( device->device, width, height, 1, D3DUSAGE_RENDERTARGET, D3DFMT_R32F, D3DPOOL_DEFAULT,
				(IDirect3DTexture9**) &tex ) ))
				throw gcnew InvalidOperationException("Failed creating render target");

			return gcnew Texture( tex );
		}
	};
}
}