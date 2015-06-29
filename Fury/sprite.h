#pragma once
class Sprite : public Movable, public Drawable
{
protected:
TextureGPU* mTexture;

public:
Sprite();
Sprite(TextureGPU* texture);
~Sprite();

virtual void draw() override
{

}
};