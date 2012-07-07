/**************************************************************************
	created		:	2011-10-31
	filename	: 	TestHook.h
	author		:	Xebeth`
	copyright	:	North Edge (2011)
	purpose		:	Test hooks
**************************************************************************/
#ifndef __TEST_HOOK_H__
#define __TEST_HOOK_H__


enum eJob
{
	JOB_NONE									= 0,
	JOB_ADVENTURER,
	JOB_PUGILIST, JOB_PUG = JOB_PUGILIST,
	JOB_GLADIATOR, JOB_GLA = JOB_GLADIATOR,
	JOB_MARAUDER, JOB_MRD = JOB_MARAUDER,
	JOB_FENCER,
	JOB_ENFORCER,
	JOB_ARCHER, JOB_ARC = JOB_ARCHER,
	JOB_LANCER, JOB_LNC = JOB_LANCER,
	JOB_MUSKETEER,
	JOB_SENTINEL,								// 10
	JOB_SAMURAI,
	JOB_STAVESMAN,
	JOB_ASSASSIN,
	JOB_FLAYER,
	JOB_MONK, JOB_MNK = JOB_MONK,
	JOB_PALADIN, JOB_PLD = JOB_PALADIN,
	JOB_WARRIOR, JOB_WAR = JOB_WARRIOR,
	JOB_BARD, JOB_BRD = JOB_BARD,
	JOB_DRAGOON, JOB_DRG = JOB_DRAGOON,
	JOB_UNKNW1,									// 20
	JOB_MYSTIC,
	JOB_THAUMATURGE, JOB_THM = JOB_THAUMATURGE,
	JOB_CONJURER, JOB_CNJ = JOB_CONJURER,
	JOB_ARCANIST,
	JOB_BARD2,
	JOB_BLACKMAGE, JOB_BLM = JOB_BLACKMAGE,
	JOB_WHITEMAGE, JOB_WHM = JOB_WHITEMAGE,
	JOB_UNKNW2,
	JOB_CARPENTER, JOB_CRP = JOB_CARPENTER,
	JOB_BLACKSMITH, JOB_BSM = JOB_BLACKSMITH,	// 30
	JOB_ARMORER, JOB_ARM = JOB_ARMORER,
	JOB_GOLDSMITH, JOB_GSM = JOB_GOLDSMITH,
	JOB_LEATHERWORKER, JOB_LTR = JOB_LEATHERWORKER,
	JOB_WEAVER, JOB_WVR = JOB_WEAVER,
	JOB_ALCHEMIST, JOB_ALC = JOB_ALCHEMIST,
	JOB_CULINARIAN, JOB_CUL = JOB_CULINARIAN,
	JOB_UNKNW3,
	JOB_UNKNW4,
	JOB_MINER, JOB_MIN = JOB_MINER,
	JOB_BOTANIST, JOB_BTN = JOB_BOTANIST,		// 40
	JOB_FISHER, JOB_FSH = JOB_FISHER,
	JOB_SHEPHERD,
	JOB_UNKNW5,
	JOB_UNKNW6,
	JOB_COUNT
};

// int __thiscall sub_D2CB80(void *this, eJob JobID_in)
typedef int (WINAPI *fnSetJob)(LPVOID pThis_in_out, int *pJob_in, int Unk1_in, int Unk2_in);
int WINAPI SetJobHook(LPVOID pThis_in_out, int*, int Unk1_in, int Unk2_in);

#define TEST_SET_JOB_OPCODES_SIGNATURE		"@@885110C20C"
#define TEST_SET_JOB_OPCODES_OFFSET			-11
#define TEST_SET_JOB_OPCODES_HOOK_SIZE		9

// int __thiscall sub_7751E0(int this, void *a2, int a3, int a4)
typedef int (WINAPI *fnFatSub)(LPVOID pThis_in_out, void *a2, int a3, int a4);
int WINAPI FatSubHook(LPVOID pThis_in_out, void *a2, int a3, int a4);

#endif//__TEST_HOOK_H__