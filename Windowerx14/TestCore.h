/**************************************************************************
	created		:	2011-10-31
	filename	: 	TestCore.h
	author		:	Xebeth`
	copyright	:	North Edge (2011)
	purpose		:	Test core module
**************************************************************************/
#ifndef __TEST_CORE_H__
#define __TEST_CORE_H__

namespace Windower
{
	typedef HookEngineLib::IHookManager IHookManager;

	//! \brief Test core module
	class TestCore : public WindowerCore
	{
	public:
		TestCore(WindowerEngine &Engine_in_out);

		// ICoreModule interface implementation
		void RegisterHooks(IHookManager &HookManager_in);
		void OnHookInstall(IHookManager &HookManager_in);

	protected:
		
	};
}

#endif//__TEST_CORE_H__