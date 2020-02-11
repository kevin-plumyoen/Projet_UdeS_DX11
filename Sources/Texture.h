#pragma once

#include <string>

struct ID3D11ShaderResourceView;

namespace PM3D
{

class CDispositifD3D11;

class CTexture
{
public:
	CTexture(const std::wstring& filename, CDispositifD3D11* pDispositif);
    // Copy is disabled
    CTexture(const CTexture&) = delete;
    CTexture& operator=(const CTexture&) = delete;
	~CTexture();

	const std::wstring& GetFilename() const { return m_Filename; }
	ID3D11ShaderResourceView* GetD3DTexture() { return m_Texture; }

private:
	std::wstring m_Filename;
	ID3D11ShaderResourceView* m_Texture;
};

} // namespace PM3D
