#pragma once
#include <DirectXMath.h>


enum class EntityType
{
	Player,
	Spell,
	Wall,
	emitter
};

enum Button
{
	A, B, X, Y,
	Rb, Rt, Lb, Lt,
	Up, Down, Left, Right,
	Lstick, Rstick,
	Start, Back
};

enum GameState {
	MainMenu,		//choose amount of players
	ChoosePowers,	//choose with "class" you want to play as
	Playing,		//main game loop
	EndRound,		//upgrading abilities
	EndGame			//showing results (stats maybe) and retun to main menu
};

struct Particle
{
	DirectX::XMFLOAT3 position;
	DirectX::XMFLOAT3 velocity;
	float age;
	int type;
};

struct Emitterlocation
{
	DirectX::XMFLOAT4 randomVector;
	DirectX::XMFLOAT3 position;
	int particleType;
};

struct dirLight
{
	DirectX::XMFLOAT4 lightDirection;
	DirectX::XMFLOAT4 lightColor;
};

struct pointLight
{
	DirectX::XMFLOAT4 lightColor;
	DirectX::XMFLOAT3 lightPos;
	float range;
};

struct CooldownStruct
{
	int p1Cd[4];
	int p2Cd[4];
	int p3Cd[4];
	int p4Cd[4];
};

enum spellSounds
{
	fireProjectile,
	fireExplotion,
	arcaneProjectile,
	earthProjectile,
	windProjectile,
	waterProjectile,
	fireStomp,
	arcaneStomp, 
	earthStomp,
	windStomp,
	waterStomp,
	fireDash,
	arcaneDash,
	earthDash,
	windDash,
	waterDash,
	fireWall,
	arcaneWall, 
	earthWall,
	windWall,
	waterWall
};

struct debuf
{
	float speed;
	float dot;
	float duration;
};

struct chooseUpgradesVertex
{
	DirectX::XMFLOAT4 pos;
	DirectX::XMFLOAT2 uv;
	int index;
};