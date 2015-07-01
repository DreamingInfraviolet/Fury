#pragma once
#include "mesh.h"
#include "texture.h"
#include "scene.h"
#include "input.h"
#include "camera.h"
#include "renderwindow.h"
#include "shader.h"
#include "physics.h"
#include "matrixd.h"


enum Team {WHITE, BLACK};
enum PieceID { PAWN, ROOK, KNIGHT, BISHOP, KING, QUEEN, PIECE_COUNT };

class Coord
{
public:
	int x = 0;
	int y = 0;

	Coord(int x_, int y_) : x(x_), y(y_) {}
};


class Piece
{
public:
	Coord coord;
	Team team;

	StaticMeshInstance* inst;
	DrawableNode* node;

	/**Adds itself to the Scene*/
	Piece(const Coord& c, const Team t, StaticMesh* mesh, Scene* scene, Physics* physics) : coord(c), team(t)
	{
		inst = mesh->createInstance();
		node = scene->createDrawableNode(scene->getRootNode());
		
		node->addDrawable(inst);
		setPos(coord);
		node->scale(0.015f);

		physics->registerObject(node, inst->parent());

		switch (t)
		{
		case BLACK:
			inst->setColour(math::vec4(0.1f, 0.1f, 0.1f, 1));
			break;
		case WHITE:
			inst->setColour(math::vec4(1, 1, 1, 1));
			break;
		default:
			assert(0);
		}
	}

	void setPos(const Coord& c)
	{
		node->setPos((float)c.x, 0.f, (float)c.y);
	}

	Coord getPos()
	{
		math::vec3 pos = node->getPos();
		return Coord(math::iround(pos.x), math::iround(pos.y));
	}
};

class Board
{
public:
	std::vector<Piece> pieces;
	StaticMesh chessMeshes[PIECE_COUNT];

	StaticMesh obj;
	StaticMeshInstance* inst;
	DrawableNode* node;

	bool initialise(Scene* scene, RenderWindow* window, Physics* physics)
	{
		if (!obj.load("Game/Models/Board.vmf"))
			return false;
		obj.upload(window);
		inst = obj.createInstance();
		node = scene->createDrawableNode(scene->getRootNode());
		node->addDrawable(inst);

		//Move board to the centre using chess coordinates, and scale:
		node->setPos(3.5f, 0.f, 3.5f);
		node->scale(8);

		pieces.reserve(32);

		//Draw pawns
		
		for (int i = 0; i < 8; ++i)
		{
			pieces.push_back(Piece(Coord(i, 1), WHITE, &chessMeshes[PAWN], scene, physics));
			pieces.push_back(Piece(Coord(i, 6), BLACK, &chessMeshes[PAWN], scene, physics));
		}

		pieces.push_back(Piece(Coord(0, 0), WHITE, &chessMeshes[ROOK], scene, physics));
		pieces.push_back(Piece(Coord(7, 0), WHITE, &chessMeshes[ROOK], scene, physics));
		pieces.push_back(Piece(Coord(0, 7), BLACK, &chessMeshes[ROOK], scene, physics));
		pieces.push_back(Piece(Coord(7, 7), BLACK, &chessMeshes[ROOK], scene, physics));

		pieces.push_back(Piece(Coord(1, 0), WHITE, &chessMeshes[KNIGHT], scene, physics));
		pieces.back().node->rotate(math::Quaternion().fromAxisRotation(math::vec3(0,1,0), -math::PIHalf));
		pieces.push_back(Piece(Coord(6, 0), WHITE, &chessMeshes[KNIGHT], scene, physics));
		pieces.back().node->rotate(math::Quaternion().fromAxisRotation(math::vec3(0, 1, 0), -math::PIHalf));
		pieces.push_back(Piece(Coord(1, 7), BLACK, &chessMeshes[KNIGHT], scene, physics));
		pieces.back().node->rotate(math::Quaternion().fromAxisRotation(math::vec3(0, 1, 0), math::PIHalf));
		pieces.push_back(Piece(Coord(6, 7), BLACK, &chessMeshes[KNIGHT], scene, physics));
		pieces.back().node->rotate(math::Quaternion().fromAxisRotation(math::vec3(0, 1, 0), math::PIHalf));

		pieces.push_back(Piece(Coord(2, 0), WHITE, &chessMeshes[BISHOP], scene, physics));
		pieces.push_back(Piece(Coord(5, 0), WHITE, &chessMeshes[BISHOP], scene, physics));
		pieces.push_back(Piece(Coord(2, 7), BLACK, &chessMeshes[BISHOP], scene, physics));
		pieces.push_back(Piece(Coord(5, 7), BLACK, &chessMeshes[BISHOP], scene, physics));

		pieces.push_back(Piece(Coord(4, 0), WHITE, &chessMeshes[KING], scene, physics));
		pieces.push_back(Piece(Coord(3, 7), BLACK, &chessMeshes[KING], scene, physics));

		pieces.push_back(Piece(Coord(3, 0), WHITE, &chessMeshes[QUEEN], scene, physics));
		pieces.push_back(Piece(Coord(4, 7), BLACK, &chessMeshes[QUEEN], scene, physics));
		
		return true;
	}

};

/**Chess game XD */
class Game
{
private:
	

	Scene scene;
	Camera camera;
	TextureGPU chessTexture;
	RenderWindow* window;
	ShaderProgram shader;
	Board board;

	bool loadObjects()
	{
		assert(PIECE_COUNT == 6);

		std::string prefix = "Game/Models/";

		board.chessMeshes[0].load((prefix + "Pawn.vmf").c_str());
		board.chessMeshes[1].load((prefix + "Rook.vmf").c_str());
		board.chessMeshes[2].load((prefix + "Knight.vmf").c_str());
		board.chessMeshes[3].load((prefix + "Bishop.vmf").c_str());
		board.chessMeshes[4].load((prefix + "King.vmf").c_str());
		board.chessMeshes[5].load((prefix + "Queen.vmf").c_str());


		for (int i = 0; i < PIECE_COUNT; ++i)
			board.chessMeshes[i].upload(window);

		return true;
	}

	bool loadTextures()
	{
		if (!chessTexture.loadFromFile("Game/Textures/test.bmp"))
			return false;
		if (!chessTexture.upload(window->getDirectX().getDevice(), Texture::TextureType::DIFFUSE))
			return false;
		chessTexture.setForRendering(window->getDirectX().getContext());
		return true;
	}


public:

	Game(RenderWindow* window) : scene(window), camera(0.7f, (float)window->getWidth(), (float)window->getHeight(), 0.01f, 20.f)
	{
		this->window = window;
		scene.setActiveCamera(&camera);
	}

	bool initialise(Physics* physics)
	{
		if (
		!loadTextures() ||
		!loadObjects() ||
		!board.initialise(&scene, window, physics))
			return false;

		ShaderProgramDescriptor shaderDesc("Shaders/test.vs", "Shaders/test.fs",
		{
			ShaderProgramDescriptor::T_START_CBUFFER, ShaderProgramDescriptor::T_MATRIX_4,
			ShaderProgramDescriptor::T_MATRIX_4,
			ShaderProgramDescriptor::T_MATRIX_4,
			ShaderProgramDescriptor::T_END_CBUFFER
		},
		{
			ShaderProgramDescriptor::T_START_CBUFFER,
			ShaderProgramDescriptor::T_FLOAT4,
			ShaderProgramDescriptor::T_END_CBUFFER
		});

		if (!shader.load(&shaderDesc, window->getDirectX().getDevice()))
			return false;

		window->setActiveShader(&shader);

		return true;
	}

	/**Returns false if intention is to quit.*/
	bool processInput(float deltaTime)
	{
		Input& input = window->getInput();
		float speed = 6.f*deltaTime;
		float shiftMultiplier = 2.f;

		if (input.onKeyDown(Input::Code::Shift))
			speed *= shiftMultiplier;

		if (input.onKeyDown(Input::Code::Esc))
			return false;
		if (input.onKeyDown(Input::Code::W))
			camera.flyForwards(speed);
		if (input.onKeyDown(Input::Code::A))
			camera.flyLeft(-speed);
		if (input.onKeyDown(Input::Code::S))
			camera.flyBackwards(speed);
		if (input.onKeyDown(Input::Code::D))
			camera.flyRight(-speed);
		if (input.onKeyDown(Input::Code::E))
			camera.flyUp(speed);
		if (input.onKeyDown(Input::Code::Q))
			camera.flyDown(speed);

		auto dmouse = input.getMouseMovement()*1.7f*deltaTime;
		camera.lookLeft(dmouse.x);
		camera.lookDown(dmouse.y);
		camera.generateMatrix();

		return true;
	}

	void frame(float deltaTime)
	{

	}

	void draw()
	{
		window->clear();
		window->draw(&scene);
	}

	void shutdown()
	{

	}
};