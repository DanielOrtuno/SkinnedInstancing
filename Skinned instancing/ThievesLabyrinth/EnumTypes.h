#pragma once


#pragma region Entity

struct eEntity
{
	enum { PLAYER, ENEMY, PROJECTILE, CAMERA, ROOM, ENVIRONMENT, DOOR, COUNT };
};

struct eEnemyType
{
	enum { KNIGHT, BERSERKER, MAGE, ARCHER, CHICKEN, COUNT };
};

#pragma endregion


#pragma region Event Manager

struct eSystems
{
	enum { DEBUG };
};

#pragma endregion


#pragma region Component

struct eComponent
{
	enum 
	{ 
		TRANSFORM, 
		BOX_COLLIDER, 
		CAPSULE_COLLIDER, 
		RIGIDBODY,
		MESH_RENDERER, 
		ANIMATOR,
		STATS,
		FIRE_DEMON_CONTROLLER, 
		CAMERA_CONTROLLER,
		MAGE_CONTROLLER, 
		PROJECTILE_COMPONENT,
		LIGHT,
		INVENTORY,
		COUNT
	};
};

#pragma endregion


#pragma region Renderer

struct eRenderTargetView
{
	enum { DEFAULT, COUNT };
};

struct eDepthStencilView
{
	enum { DEFAULT, COUNT };
};

struct eDepthStencilState
{
	enum { DEFAULT, COUNT };
};

struct eViewport
{
	enum { DEFAULT, COUNT };
};

struct eRasterizerState
{
	enum { DEFAULT, COUNT };
};

struct eVertexBuffer
{

	enum { MINOTAUR, FIRE_DEMON, ENEMY_MAGE, SPHERE, ROOM, ROOM1, ROOM2, ROOM3, DOOR, PILLAR, ENVIRONMENT, DEBUG_LINES, COUNT};
};

struct eIndexBuffer
{
	enum { MINOTAUR, FIRE_DEMON, ENEMY_MAGE, SPHERE, ROOM, ROOM1, ROOM2, ROOM3, DOOR, PILLAR, ENVIRONMENT, COUNT};
};

struct eConstantBuffer
{
	enum { MODEL, CAMERA, JOINT_MATRICES, LIGHTS, COUNT }; //JOINT_MATRICES, LIGHT,
};

struct eVertexShader
{
	enum { DEFAULT, SKINNED, DEBUG_SHADER, COUNT };
};

struct ePixelShader
{

	enum { DEFAULT, DEBUG_SHADER, COUNT };
};

struct eInputLayout
{
	enum { DEFAULT, SKINNED, DEBUG, COUNT };
};

struct eTexture
{
	enum { MINOTAUR_DIFFUSE, FIRE_DEMON_DIFFUSE, ENEMY_MAGE_DIFFUSE, ROOM_DIFFUSE, ROOM1, ROOM2, ROOM3, DOOR, PILLAR, ENVIRONMENT, NOISE, COUNT };
};

struct eSampler
{
	enum { BORDER, CLAMP, MIRROR, WRAP, ANIMATION_SAMPLER, COUNT};
};

#pragma endregion


#pragma region Animator

struct eAnimator
{
	enum { MAGE, FIRE_DEMON, COUNT };
};

struct eEnemyAnimation
{
	enum { IDLE, WALK, ATTACK, COUNT }; // ATTACK, DEATH,
};

struct ePlayerAnimation
{
	enum { IDLE, WALK, MELEE, COUNT}; //, ATTACK, SPELL_1, SPELL_2, SPELL_3, DEATH,
};

#pragma endregion


#pragma region GUI

struct eSprite
{
	enum { HEALTHBAR, MANABAR, COUNT};
};

struct eMenu
{
	enum { START = 0, START_OPTIONS, SELECT, PAUSE, PAUSE_OPTIONS, INVENTORY, WIN, LOSE, GAME, COUNT };
};

#pragma endregion


#pragma region Audio

struct eAudio
{
	enum { SFX, MUSIC, COUNT};
};

struct eSFX
{
	enum { BUTTON, PLAYERMOVE, HIT, HIT2, HIT3, HIT4, FIREBALL1, FIREBALL2, COUNT};
};

struct eMusic
{
	enum { MAIN, COUNT };
};
#pragma endregion


#pragma region Collision

struct eCollisionState
{
	enum { COLLISION_ENTER, COLLISION_STAY, COLLISION_EXIT, TRIGGER_ENTER, TRIGGER_STAY, TRIGGER_EXIT };
};

#pragma endregion


#pragma region Game State

struct eGameState
{
	enum { MAIN_MENU, LOADING_LEVEL, PLAYING, PAUSED, QUIT };
};

#pragma endregion


#pragma region Room Types

struct eRoomTypes
{
	enum { DEFULT, COUNT };
};

#pragma endregion


#pragma region Collision Types

struct eCollisionTypes
{
	enum { BOX_BOX, CAPSULE_CAPSULE, BOX_CAPSULE };
};

#pragma endregion


#pragma region Items

struct ePassiveItems
{
	enum { NONE = 0, RING, THIMBLE, BELT, GAUNTLET, BOOTS, HASTE, HAT, COIN, COUNT };
};

struct eActiveItems
{
	enum { NONE = 0, SHIELD, HORN, LAVA, BOMB, BREEZE, TELEPORTER, COUNT };
};

struct eConsumableItems
{
	enum { NONE = 0, HEALTH, MANA, DAMAGE, HASTE, RESIST, COUNT };
};

#pragma endregion