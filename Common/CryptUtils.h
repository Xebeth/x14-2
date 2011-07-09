/**************************************************************************
	created		:	2011-07-09
	filename	: 	Crypt.h
	author		:	Xebeth`
	copyright	:	North Edge (2011)
	purpose		:	Crypt function wrapper
**************************************************************************/
#ifndef __CRYPT_H__
#define __CRYPT_H__

class CryptUtils
{
public:
	CryptUtils();

	string_t CombineHash(const string_t &InputStr1_in,
						 const string_t &InputStr2_in);
	size_t GenerateMachineID(string_t &MachineID_out);

	void Crypt(const string_t &CryptKey_in,
			   const string_t &InputStr_in,
			   string_t &OuputStr_out);
	long Hash(const string_t &InputStr_in);

	void StringToHex(const string_t &Input_in, string_t &Output_out);
	void HexToString(const string_t &Input_in, string_t &Output_out);
protected:
	typedef std::collate<TCHAR> collate_t;

	const collate_t& m_Collate;

	TCHAR m_SubstBox1[256];
	TCHAR m_SubstBox2[256];
};

#endif//__CRYPT_H__