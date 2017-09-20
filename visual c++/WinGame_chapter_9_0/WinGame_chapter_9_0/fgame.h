#ifndef _FGAME_H_
#define _FGAME_H_
typedef struct tagCharacter{
	int NowHp;
	int MaxHp;
	int NowMp;
	int MaxMp;
	int level;				//�ȼ�
	int Strength;			//����
	int Intelligence;		//����
	int Agility;			//����
}Character;
enum ActionTypes{
	ACTION_TYPE_NORMAL = 0,		//��ͨ����
	ACTION_TYPE_CRITICAL = 1,	//����һ��
	ACTION_TYPE_MAGIC = 2,		//ħ������
	ACTION_TYPE_MISS = 3,		//�������
	ACTION_TYPE_RECOVER = 4		//�ͷŻָ�ħ��
};

Character hero, boss;			//Ӣ�ۺʹ�Boss�Ľṹ��
ActionTypes hero_action_type, boss_action_type;		//Ӣ�ۺʹ�Boss��Ҫ��ȡ�Ķ���

#endif