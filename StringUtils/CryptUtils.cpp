/**************************************************************************
	created		:	2011-07-09
	filename	: 	Crypt.cpp
	author		:	Xebeth`
	copyright	:	North Edge (2011)
	purpose		:	Crypt function wrapper
**************************************************************************/
#include "stdafx.h"
#include <locale>

#include "string_t.h"
#include "NonCopyable.h"
#include "CryptUtils.h"

const CryptUtils::collate_t &CryptUtils::m_Collate = std::use_facet<collate_t>(std::locale());

TCHAR CryptUtils::m_SubstBox1[256] = { 0 };
TCHAR CryptUtils::m_SubstBox2[256] = { 0 };

CryptUtils::CryptUtils()
{
	// always initialize the arrays with zero
	memset(m_SubstBox1, 0, sizeof(m_SubstBox1));
	memset(m_SubstBox2, 0, sizeof(m_SubstBox2));
}

void CryptUtils::Crypt(const string_t &CryptKey_in, const string_t &InputStr_in, string_t &OuputStr_out)
{
	TCHAR *pTempStr = _tcsdup(InputStr_in.c_str());
	const TCHAR *pKeyData = CryptKey_in.data();
	size_t InputLength = InputStr_in.size();
	unsigned long i, j, k, t;
	TCHAR temp = NULL;

	OuputStr_out.clear();

	for(i = 0UL; i < 256UL; ++i)
		m_SubstBox1[i] = (TCHAR)i;

	// initialize the substitution box with the user key
	for(i = j = 0UL; i < 256UL; ++i)
	{
		if(j == InputLength)
			j = 0UL;

		m_SubstBox2[i] = pKeyData[++j];
	}

	// scramble sbox1 with sbox2
	for(i = j = 0UL; i < 256UL; i++)
	{
		j = (j + m_SubstBox1[i] + m_SubstBox2[i]) % 256UL;
		temp = m_SubstBox1[i];                             
		m_SubstBox1[i] = m_SubstBox1[j];
		m_SubstBox1[j] = temp;
	}

	for(i = j = k = 0UL; k < InputLength; ++k)
	{
		do
		{
			// increment i
			i = (i + 1UL) % 256UL;
			// increment j
			j = (j + (unsigned long) m_SubstBox1[i]) % 256UL;

			// Scramble SBox #1 further so encryption routine
			// will repeat itself at greater interval
			temp = m_SubstBox1[i];
			m_SubstBox1[i] = m_SubstBox1[j];
			m_SubstBox1[j] = temp;

			// Get ready to create pseudo random   byte for encryption key
			t = ((unsigned long) m_SubstBox1[i] + (unsigned long) m_SubstBox1[j]) % 256UL;
			// xor with the data and done
			temp = (pTempStr[k] ^ m_SubstBox1[t]);
			// fix corruption in some cases
			if (temp == NULL)
				temp = pTempStr[k];
		}
		while(temp == NULL);

		pTempStr[k] = temp;
	}

	OuputStr_out.assign(pTempStr);
	free(pTempStr);
}

long CryptUtils::Hash(const string_t &InputStr_in)
{
	const TCHAR *pData = InputStr_in.data();

	return m_Collate.hash(pData, pData + InputStr_in.size());
}

string_t CryptUtils::CombineHash(const string_t &InputStr1_in, const string_t &InputStr2_in)
{
	long Hash1 = Hash(InputStr1_in);
	long Hash2 = Hash(InputStr2_in);

	return format(_T("%08X%08X"), Hash1, Hash2);
}

size_t CryptUtils::GenerateMachineID(string_t &MachineID_out, const TCHAR *pRootPath_in)
{
	TCHAR VolumeName[_MAX_PATH], FileSystem[_MAX_PATH];
	unsigned long VolumeSerialNumber;
	HW_PROFILE_INFO hwProfileInfo;
	string_t RootPath;

	if (pRootPath_in != NULL)
	{
		RootPath = pRootPath_in;
		// normalize the path
		if (RootPath.back() != '\\')
			RootPath += '\\';

		pRootPath_in = RootPath.c_str();
	}

	MachineID_out.clear();

	if (GetVolumeInformation(pRootPath_in, VolumeName, _MAX_PATH,
		&VolumeSerialNumber, NULL, NULL, FileSystem, _MAX_PATH)
		&& GetCurrentHwProfile(&hwProfileInfo))
	{
		string_t SerialNumber;
		int hex[4];

		hex[0] = ((VolumeSerialNumber >> 24) & 0xff);
		hex[1] = ((VolumeSerialNumber >> 16) & 0xff);
		hex[2] = ((VolumeSerialNumber >> 8) & 0xff);
		hex[3] = (VolumeSerialNumber & 0xff);

		format(SerialNumber, _T("%02X:%02X:%02X:%02X"),
			   hex[0], hex[1], hex[2], hex[3]);

		MachineID_out = CombineHash(SerialNumber, hwProfileInfo.szHwProfileGuid);

		return MachineID_out.size() - 1;
	}

	return 0;
}

void CryptUtils::StringToHex(const string_t &Input_in, string_t &Output_out)
{
	size_t Length = Input_in.size() * sizeof(TCHAR);
	const BYTE *pData = (BYTE*)Input_in.data();

	Output_out.clear();

	for (size_t i = 0; i < Length; ++i)
		append_format(Output_out, _T("%02X"), *(pData + i));
}

void CryptUtils::HexToString(const string_t &Input_in, string_t &Output_out)
{
	// number of bytes per character
	size_t CharSize = sizeof(TCHAR);
	size_t Length = Input_in.size() / CharSize;
	TCHAR *pCurrentChar, *pTempStr = new TCHAR[Length + CharSize];
	string_t HexBlock;
	long Hex;

	memset(pTempStr, 0, Length + CharSize);
	pCurrentChar = pTempStr;
	
	for (size_t i = 0; i < Length; i += CharSize)
	{
		HexBlock = Input_in.substr((i + 1) * CharSize, CharSize);
		HexBlock += Input_in.substr(i * CharSize, CharSize);
		Hex = _tcstol(HexBlock.c_str(), NULL, 16);
		*(pCurrentChar++) = (TCHAR)Hex;
	}

	Output_out.assign(pTempStr);
	delete[] pTempStr;
}