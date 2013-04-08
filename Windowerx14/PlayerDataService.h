/**************************************************************************
	created		:	2013-04-06
	filename	: 	PlayerDataService.h
	author		:	Xebeth`
	copyright	:	North Edge (2013)
	purpose		:	Player data service
**************************************************************************/
#ifndef __PLAYER_DATA_SERVICE_H__
#define __PLAYER_DATA_SERVICE_H__

namespace Windower
{
	class PlayerDataService : public ModuleService
	{
	public:
		PlayerDataService(const string_t& Name_in, const HookPointers &Hooks_in,
						  bool InvokePermission_in = false);

		void OnTargetPtrChange(const TargetData *pTargetData_in);
		void OnPlayerPtrChange(const TargetData *pPlayerData_in);
		void DestroyContext() {} 
		void CreateContext();

	private:		
		const TargetData *m_pPlayerTarget;
		const TargetData *m_pPlayerData;
	};
}

#endif//__PLAYER_DATA_SERVICE_H__
