#include "Sprite.h"
#include "SpriteCommon.h"
void Sprite::Initialize(SpriteCommon* spriteCommon){
	this->spriteCommon = spriteCommon;
	dxCommon_ = spriteCommon->GetDxCommon();
	


	//vertexResourceSprite = spriteCommon->GetDxCommon()->CreateBufferResource(sizeof(VertexData) * 6);
	//indexResourceSprite = dxCommon->CreateBufferResource(sizeof(uint32_t) * 6);

}
