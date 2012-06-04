/**************************************************************************
	created		:	2011-06-07
	filename	: 	WindowerCommand.h
	author		:	Xebeth`
	copyright	:	North Edge (2011)
	purpose		:	Windower command and related structures
**************************************************************************/
#ifndef __WINDOWER_COMMAND_H__
#define __WINDOWER_COMMAND_H__

#include "CommandParameter.h"

namespace Windower
{
	class WindowerCommand;
	class ICommandHandler;

	//! result codes of command dispatching
	enum DISPATCHER_RESULT
	{
		DISPATCHER_RESULT_INVALID_CALL = -2,	//!< invalid call
		DISPATCHER_RESULT_INVALID_PARAMETERS,	//!< invalid parameters
		DISPATCHER_RESULT_FAILURE = 0,			//!< failure
		DISPATCHER_RESULT_SUCCESS				//!< success
	};

	//! a hash map of command parameters
	typedef stdext::hash_map<std::string, CommandParameter*> CommandParams;

	//! \brief Windower command
	class WindowerCommand
	{
	public:
		WindowerCommand();
		/*! \brief WindowerCommand constructor
			\param[in] RegistrationKey_in : the key of the plugin registering the command
			\param[in] CmdID_in : the ID of the command
			\param[in] Name_in : the name of the command
			\param[in] Description_in : the description of the command
			\param[in] pHandler_in : the command handler (typically a plugin instance)
			\param[in] Public_in : flag specifying if the command is listed with //help
			\param[in] Restricted_in : flag specifying if the command can be called by the user
			\return true if the command was registered successfully; false otherwise
		*/
		WindowerCommand(DWORD RegistrationKey_in, INT_PTR CmdID_in, const std::string &Name_in,
						const std::string &Description_in, ICommandHandler *pHandler_in,
						bool Public_in = true, bool Restricted_in = false)
			: m_Description(Description_in), m_ID(CmdID_in), m_Name(Name_in), m_Restricted(Restricted_in), 
			  m_MinParamsCount(0), m_MaxParamsCount(0), m_Public(Public_in), m_pHandler(pHandler_in),
			  m_RegistrationKey(RegistrationKey_in), m_RefCount(0) {}
		//! \brief WindowerCommand destructor
		~WindowerCommand() { Clear(); }

		std::string& Output(std::string &Output_out, bool ShowValues_in = false);

		void Clear();

		//! \brief Method called when the command is registered
		void OnRegister() { ++m_RefCount; }
		/*! \brief Method called when the command is unregistered
			\return true if the reference count reaches 0; false otherwise
		*/
		bool OnUnregister() { --m_RefCount; return (m_RefCount == 0); }

		/*! \brief Retrieves the number of parameters
			\return the number of parameters
		*/
		size_t Count() const { return m_Parameters.size(); }
		/*! \brief Checks if there is no parameter
			\return true if there is no parameter; false otherwise
		*/
		bool IsEmpty() const { return m_Parameters.empty(); }

		/*! \brief Retrieves the iterator for the beginning of the parameters map
			\return the iterator for the beginning of the parameters map
		*/
		CommandParams::const_iterator Begin() const { return m_Parameters.begin(); }
		/*! \brief Retrieves the iterator for the end of the parameters map
			\return the iterator for the end of the parameters map
		*/
		CommandParams::const_iterator End() const { return m_Parameters.end(); }

		int Execute(std::string &Feedback_out);
		void Invalidate();

		/*! \brief Checks if the command is public
			\return true if the command is public; false otherwise
		*/
		bool IsPublic() const { return m_Public; }
		/*! \brief Sets the flag specifying if the command is public
			\param[in] Public_in : the flag specifying if the command is public
		*/
		void SetPublic(bool Public_in) { m_Public = Public_in; }

		/*! \brief Checks if the command is restricted
			\return true if the command is restricted; false otherwise
		*/
		bool IsRestricted() const { return m_Restricted; }
		/*! \brief Sets the flag specifying if the command is restricted
			\param[in] Public_in : the flag specifying if the command is restricted
		*/
		void SetRestricted(bool Public_in) { m_Public = Public_in; }

		/*! \brief Retrieves the name of the command
			\return the name of the command
		*/
		const std::string& GetName() const { return m_Name; }
		/*! \brief Sets the name of the command
			\param[in] Name_in : the name of the command
		*/
		void SetName(const std::string &Name_in) { m_Name = Name_in; }
		/*! \brief Retrieves the key used to register the command
			\return the key used to register the command
		*/
		DWORD GetKey() const { return m_RegistrationKey; }
		/*! \brief Retrieves the ID of the command
			\return the ID of the command
		*/
		INT_PTR GetID() const { return m_ID; }

		/*! \brief Checks if the specified key matches the command one
			\return true if the keys match; false otherwise
		*/
		bool IsKeyMatching(DWORD Key_in) const { return (m_RegistrationKey == Key_in); }
		
		/*! \brief Checks if the parameters of the command are consistent
			return true if the parameters are correct; false otherwise
		*/
		bool ValidateParameters() const { return (m_MinParamsCount <= m_MaxParamsCount	\
											   && m_MinParamsCount >= m_MaxParamsCount	\
											   && m_Parameters.size() >= m_MaxParamsCount); }
		
		/*! \brief Retrieves the parameters of the command
			\return the parameters of the command
		*/
		const CommandParams& GetParameters() const { return m_Parameters; }
		/*! \brief Retrieves the minimum number of parameters of the command
			\return the minimum number of parameters of the command
		*/
		size_t GetMinParams() const { return m_MinParamsCount; }
		/*! \brief Retrieves the maximum number of parameters of the command
			\return the maximum number of parameters of the command
		*/
		size_t GetMaxParams() const { return m_MaxParamsCount; }
		/*! \brief Retrieves the number of parameters of the command
			\return the number of parameters of the command
		*/
		size_t GetParametersCount() const { return m_Parameters.size(); }

		bool AddStringParam(const std::string &Name_in, bool bOptional = false,
							const char *pDefaultValue_in = "",
							const char *pDesc_in = "a string value");
		bool AddPointerParam(const std::string &Name_in, bool bOptional = false,
							 long DefaultValue_in = 0L,
							 const char *pDesc_in = "a pointer value");
		bool AddIntegerParam(const std::string &Name_in, bool bOptional = false,
							 long DefaultValue_in = 0L,
							 const char *pDesc_in = "an integer value");
		bool AddFloatParam(const std::string &Name_in, bool bOptional = false,
						   double DefaultValue_in = 0.,
						   const char *pDesc_in = "a float value");

		bool SetStringValue(const std::string &Name_in, const char *pDefaultValue_in = "");
		bool SetPointerValue(const std::string &Name_in, long DefaultValue_in = 0L);
		bool SetIntegerValue(const std::string &Name_in, long DefaultValue_in = 0L);
		bool SetFloatValue(const std::string &Name_in, double DefaultValue_in = 0.);

		bool SetStringValue(CommandParameter *pParam_in, const std::string &DefaultValue_in);
		bool SetPointerValue(CommandParameter *pParam_in, long DefaultValue_in = 0L);
		bool SetIntegerValue(CommandParameter *pParam_in, long DefaultValue_in = 0L);
		bool SetFloatValue(CommandParameter *pParam_in, double DefaultValue_in = 0.);

		const std::string GetStringValue(const std::string &Name_in) const;
		const string_t GetWideStringValue(const std::string &Name_in) const;
		long GetPointerValue(const std::string &Name_in) const;
		long GetIntegerValue(const std::string &Name_in) const;
		double GetFloatValue(const std::string &Name_in) const;

	protected:
		CommandParameter* GetParameter(const std::string &Name_in) const;

		CommandParameter* CreateParameter(const std::string &Name_in,
										  bool bOptional_in,
										  const std::string &Desc_in,
										  COMMAND_PARAM_TYPE Type_in);
		//! flag specifying if the command is listed with //help
		bool m_Public;
		//! flag specifying if the command can be called by the user
		bool m_Restricted;
		//! the ID of the command
		INT_PTR m_ID;
		//! the name of the command
		std::string m_Name;
		//! the description of the command
		std::string m_Description;
		//! minimal number of parameters for the command
		unsigned int m_MinParamsCount;
		//! maximal number of parameters for the command
		unsigned int m_MaxParamsCount;
		//! the key of the plugin registering the command
		DWORD m_RegistrationKey;
		//! the command handler (typically a plugin instance)
		ICommandHandler *m_pHandler;
		//! a collection of parameters
		CommandParams m_Parameters;

	private:
		//! array of strings corresponding to the parameter types
		static char* CommandParamTypes[COMMAND_PARAM_TYPE_COUNT];
		//! reference count
		UINT m_RefCount;
	};
}

#endif//__WINDOWER_COMMAND_H__