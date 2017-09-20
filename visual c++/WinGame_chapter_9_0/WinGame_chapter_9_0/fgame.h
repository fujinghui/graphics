#ifndef _FGAME_H_
#define _FGAME_H_
typedef struct tagCharacter{
	int NowHp;
	int MaxHp;
	int NowMp;
	int MaxMp;
	int level;				//等级
	int Strength;			//力量
	int Intelligence;		//智力
	int Agility;			//敏捷
}Character;
enum ActionTypes{
	ACTION_TYPE_NORMAL = 0,		//普通攻击
	ACTION_TYPE_CRITICAL = 1,	//致命一击
	ACTION_TYPE_MAGIC = 2,		//魔法攻击
	ACTION_TYPE_MISS = 3,		//攻击落空
	ACTION_TYPE_RECOVER = 4		//释放恢复魔法
};

Character hero, boss;			//英雄和大Boss的结构体
ActionTypes hero_action_type, boss_action_type;		//英雄和大Boss所要采取的动作

#endif