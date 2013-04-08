/**************************************************************************
	created		:	2011-07-09
	filename	: 	Crypt.h
	author		:	Xebeth`
	copyright	:	North Edge (2011)
	purpose		:	Crypt function wrapper
**************************************************************************/
#ifndef __CRYPT_H__
#define __CRYPT_H__

#ifndef _STRING_UTILS_SDK_INCLUDE_
	#error Please include the global header file 'StringUtils.h'
#endif

class CryptUtils : public NonCopyable
{
public:
	CryptUtils();

	static string_t CombineHash(const string_t &InputStr1_in,
								const string_t &InputStr2_in);
	static size_t GenerateMachineID(string_t &MachineID_out,
									const TCHAR *pRootPath_in = NULL);

	static void Crypt(const string_t &CryptKey_in,
					  const string_t &InputStr_in,
					  string_t &OuputStr_out);
	static long Hash(const string_t &InputStr_in);

	static void StringToHex(const string_t &Input_in, string_t &Output_out);
	static void HexToString(const string_t &Input_in, string_t &Output_out);
protected:
	typedef std::collate<TCHAR> collate_t;

	static const collate_t &m_Collate;

	static TCHAR m_SubstBox1[256];
	static TCHAR m_SubstBox2[256];
};

#endif//__CRYPT_H__