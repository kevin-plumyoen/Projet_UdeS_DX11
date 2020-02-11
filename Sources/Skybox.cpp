#include "Skybox.h"
#include "sommetbloc.h"
#include "dispositifD3D11.h"

void Skybox::Draw() {
  // Obtenir le contexte
  ID3D11DeviceContext* pImmediateContext = pDispositif->GetImmediateContext();

  // Choisir la topologie des primitives
  pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

  // Source des sommets
  const UINT stride = sizeof(PM3D::CSommetBloc);
  const UINT offset = 0;
  pImmediateContext->IASetVertexBuffers(0, 1, &pVertexBuffer, &stride, &offset);


  // Source des index
  pImmediateContext->IASetIndexBuffer(pIndexBuffer, DXGI_FORMAT_R16_UINT, 0);

  pImmediateContext->OMSetDepthStencilState(pDispositif->GetDepthStencilStateNoTest(), 1);

  pImmediateContext->DrawIndexed(nb_indices, 0, 0);

  pImmediateContext->OMSetDepthStencilState(pDispositif->GetDepthStencilState(), 1);
}