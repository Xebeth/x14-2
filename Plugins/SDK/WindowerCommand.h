/**************************************************************************
	created		:	2011-06-07
	filename	: 	WindowerCommand.h
	author		:	Xebeth`
	copyright	:	North Edge (2011)
	purpose		:	Windower command and related structures
**************************************************************************/
#ifndef __WINDOWER_COMMAND_H__
#define __WINDOWER_COMMAND_H__

#ifndef _WINDOWER_PLUGIN_SDK_INCLUDE_
	#error Please include the global header file 'PluginSDK.h'
#endif

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
	typedef hash_map<char, CommandParameter*> CommandParams;

	//! \brief Windower command
	class WindowerCommand
	{
	public:
		//! \brief WindowerCommand default constructor
		WindowerCommand();
		/*! \brief WindowerCommand copy constructor
			\param[in] Command_in : the command to copy
		*/
		WindowerCommand(const WindowerCommand &Command_in);
		/*! \brief WindowerCommand constructor
			\param[in] RegistrationKey_in : the key of the plugin registering the command
			\param[in] CmdID_in : the ID of the command
			\param[in] Name_in : the name of the command
			\param[in] Description_in : the description of the command
			\param[in] pHandler_in : the command handler (typically a plugin instance)
			\param[in] Public_in : flag specifying if the command is listed with //help
			\param[in] Restricted_in : flag specifying if the command can be called by the user
		*/
		WindowerCommand(DWORD RegistrationKey_in, INT_PTR CmdID_in, const std::string &Name_in,
						const std::string &Description_in, ICommandHandler *pHandler_in,
						bool Public_in = true, bool Restricted_in = false);
		//! \brief WindowerCommand destructor
		~WindowerCommand();

		/*! \brief Formats the help message for the command
			\param[out] Help_out : a string receiving the formatted help message
			\param[in] ShowValues_in : flag specifying if the description or value is displayed
			\return the formatted help message
		*/
		std::string& Output(std::string &Output_out, bool ShowValues_in = false);
		/*! \brief Copies the specified command
			\param[in] Command_in : the command to copy
		*/
		void Copy(const WindowerCommand &Command_in);
		//! \brief Removes all the parameters
		void Clear();

		//! \brief Method called when the command is registered
		void OnRegister();
		/*! \brief Method called when the command is unregistered
			\return true if the reference count reaches 0; false otherwise
		*/
		bool OnUnregister();

		/*! \brief Retrieves the number of parameters
			\return the number of parameters
		*/
		size_t Count() const;
		/*! \brief Checks if there is no parameter
			\return true if there is no parameter; false otherwise
		*/
		bool IsEmpty() const;

		/*! \brief Retrieves the iterator for the beginning of the parameters map
			\return the iterator for the beginning of the parameters map
		*/
		CommandParams::const_iterator Begin() const;
		/*! \brief Retrieves the iterator for the end of the parameters map
			\return the iterator for the end of the parameters map
		*/
		CommandParams::const_iterator End() const;

		/*! \brief Executes the command by invoking its callback
			\param[out] Feedback_out : the result message of the execution
			\return DISPATCHER_RESULT_SUCCESS if successful; an error code otherwise
		*/
		int Execute(std::string &Feedback_out);
		//! \brief Sets a command to its default state
		void Invalidate();

		/*! \brief Checks if the command is public
			\return true if the command is public; false otherwise
		*/
		bool IsPublic() const;
		/*! \brief Sets the flag specifying if the command is public
			\param[in] Public_in : the flag specifying if the command is public
		*/
		void SetPublic(bool Public_in);

		/*! \brief Checks if the command is restricted
			\return true if the command is restricted; false otherwise
		*/
		bool IsRestricted() const;
		/*! \brief Sets the flag specifying if the command is restricted
			\param[in] Public_in : the flag specifying if the command is restricted
		*/
		void SetRestricted(bool Public_in);

		/*! \brief Retrieves the name of the command
			\return the name of the command
		*/
		const std::string& GetName() const;
		/*! \brief Sets the name of the command
			\param[in] Name_in : the name of the command
		*/
		void SetName(const std::string &Name_in);
		/*! \brief Retrieves the key used to register the command
			\return the key used to register the command
		*/
		DWORD GetKey() const;
		/*! \brief Retrieves the ID of the command
			\return the ID of the command
		*/
		INT_PTR GetID() const;

		/*! \brief Checks if the specified key matches the command one
			\return true if the keys match; false otherwise
		*/
		bool IsKeyMatching(DWORD Key_in) const;
		
		/*! \brief Checks if the parameters of the command are consistent
			return true if the parameters are correct; false otherwise
		*/
		bool ValidateParameters() const;
		
		/*! \brief Retrieves the parameters of the command
			\return the parameters of the command
		*/
		const CommandParams& GetParameters() const;
		/*! \brief Retrieves the minimum number of parameters of the command
			\return the minimum number of parameters of the command
		*/
		size_t GetMinParams() const;
		/*! \brief Retrieves the maximum number of parameters of the command
			\return the maximum number of parameters of the command
		*/
		size_t GetMaxParams() const;
		/*! \brief Retrieves the number of parameters of the command
			\return the number of parameters of the command
		*/
		size_t GetParametersCount() const;

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

		/*! \brief Sets the string value of a parameter
			\param[in] Name_in : the name of the parameter
			\param[in] pDefaultValue_in : the string value of a parameter
			\return true if the parameter value was set; false otherwise
		*/
		bool SetStringValue(const std::string &Name_in, const char *pDefaultValue_in = "");
		/*! \brief Sets the pointer value of a parameter
			\param[in] Name_in : the name of the parameter
			\param[in] DefaultValue_in : the pointer value of a parameter
			\return true if the parameter value was set; false otherwise
		*/
		bool SetPointerValue(const std::string &Name_in, long DefaultValue_in = 0L);
		/*! \brief Sets the integer value of a parameter
			\param[in] Name_in : the name of the parameter
			\param[in] DefaultValue_in : the integer value of a parameter
			\return true if the parameter value was set; false otherwise
		*/
		bool SetIntegerValue(const std::string &Name_in, long DefaultValue_in = 0L);
		/*! \brief Sets the float value of a parameter
			\param[in] Name_in : the name of the parameter
			\param[in] DefaultValue_in : the float value of a parameter
			\return true if the parameter value was set; false otherwise
		*/
		bool SetFloatValue(const std::string &Name_in, double DefaultValue_in = 0.);

		/*! \brief Retrieves the string value of a parameter
			\param[in] Name_in : the name of the parameter
			\return the string value of a parameter
		*/
		const std::string GetStringValue(const std::string &Name_in) const;
		/*! \brief Retrieves the string value of a parameter as unicode
			\param[in] Name_in : the name of the parameter
			\return the string value of a parameter as unicode
		*/
		const string_t GetWideStringValue(const std::string &Name_in) const;
		/*! \brief Retrieves the integer value of a parameter formatted as hexadecimal
			\param[in] Name_in : the name of the parameter
			\return the pointer value of a parameter
		*/
		long GetPointerValue(const std::string &Name_in) const;
		/*! \brief Retrieves the integer value of a parameter
			\param[in] Name_in : the name of the parameter
			\return the integer value of a parameter
		*/
		long GetIntegerValue(const std::string &Name_in) const;
		/*! \brief Retrieves the float value of a parameter
			\param[in] Name_in : the name of the parameter
			\return the float value of a parameter
		*/
		double GetFloatValue(const std::string &Name_in) const;

	protected:
		/*! \brief Retrieves a parameter given its name
			\param[in] Name_in : the name of the parameter
			\return a pointer to the parameter if it exists; NULL otherwise
		*/
		CommandParameter* GetParameter(const std::string &Name_in) const;
		/*! \brief Creates a parameter of the specified type
			\param[in] Name_in : the name of the parameter
			\param[in] bOptional_in : flag specifying if the parameter is optional
			\param[in] Desc_in : the description of the parameter
			\param[in] Type_in : the type of the parameter
			\return true a pointer to the parameter
		*/
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