/**************************************************************************
	created		:	2012-07-08
	filename	: 	ReadConfigHook.h
	author		:	Xebeth`
	copyright	:	North Edge (2012)
	purpose		:	Load configuration hook (config.sys)
**************************************************************************/
#ifndef __READCONFIG_HOOK_H__
#define __READCONFIG_HOOK_H__

#define CONFIG_FULLSCREEN_OFFSET 16
#define CONFIG_RESX_OFFSET 20
#define CONFIG_RESY_OFFSET 24

// int __cdecl sub_405BD0(int a1) (search for config.sys)
typedef int (__cdecl *fnReadConfig)(BYTE *pConfigData_out);
int __cdecl ReadConfigHook(BYTE *pConfigData_out);

#define READ_CONFIG_OPCODES_SIGNATURE	"@@33DBBF010000008D4E60BD02000000"
#define READ_CONFIG_OPCODES_OFFSET		-81

#endif//__READCONFIG_HOOK_H__
