#include "deferred_rendertarget.h"

bool GBufferRT::Init(ID3D11Device* device, float screenNear, float screenDepth)
{
	if (!this->InitNormals(device))
		return false;

	if (!this->InitDepth(device))
		return false;

	if (!this->InitAmbient(device))
		return false;

	if (!this->InitTexture(device))
		return false;
	this->InitProjMatrix(screenNear, screenDepth);

	return true;
}