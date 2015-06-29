#pragma once


struct VertexBufferStaticmesh
{
	math::mat4 world;
	math::mat4 view;
	math::mat4 projection;
};

struct PixelBufferStaticmesh
{
	math::vec4 colour;
};