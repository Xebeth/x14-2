// requires Windows 7, Windows 7 Service Pack 1, Windows Server 2003 Service Pack 2, Windows Server 2008, Windows Server 2008 R2, Windows Server 2008 R2 SP1, Windows Vista Service Pack 1, Windows XP Service Pack 3
// requires Windows Installer 3.1 or later
// requires Internet Explorer 5.01 or later

[CustomMessages]
vcredist2015_x86_title=Visual 2015 C++ Redistributable (x86)
en.vcredist2015_x86_size=13.1 MB
en.vcredist2015_x86_lcid=''

vcredist2015_x64_title=Visual C++ 2015 Redistributable (x64)
en.vcredist2015_x64_size=13.9 MB
en.vcredist2015_x64_lcid=''

vcredist2015_sp2_x86_title=Visual C++ 2015 Update 2 Redistributable (x86)
en.vcredist2015_sp2_x86_size=13.3 MB
en.vcredist2015_sp2_x86_lcid=''

vcredist2015_sp2_x64_title=Visual C++ 2015 Update 2 Redistributable (x64)
en.vcredist2015_sp2_x64_size=14.0 MB
en.vcredist2015_sp2_x64_lcid=''

vcredist2015_sp3_x86_title=Visual C++ 2015 Update 3 Redistributable (x86)
en.vcredist2015_sp3_x86_size=13.7 MB
en.vcredist2015_sp3_x86_lcid=''

vcredist2015_sp3_x64_title=Visual C++ 2015 Update 3 Redistributable (x64)
en.vcredist2015_sp3_x64_size=14.5 MB
en.vcredist2015_sp3_x64_lcid=''

[Code]
#IFDEF UNICODE
  #DEFINE AW "W"
#ELSE
  #DEFINE AW "A"
#ENDIF
type
  INSTALLSTATE = Longint;
const
  INSTALLSTATE_INVALIDARG = -2;  // An invalid parameter was passed to the function.
  INSTALLSTATE_UNKNOWN = -1;     // The product is neither advertised or installed.
  INSTALLSTATE_ADVERTISED = 1;   // The product is advertised but not installed.
  INSTALLSTATE_ABSENT = 2;       // The product is installed for a different user.
  INSTALLSTATE_DEFAULT = 5;      // The product is installed for the current user.

  VC_2005_REDIST_X86 = '{A49F249F-0C91-497F-86DF-B2585E8E76B7}';
  VC_2005_REDIST_X64 = '{6E8E85E8-CE4B-4FF5-91F7-04999C9FAE6A}';
  VC_2005_REDIST_IA64 = '{03ED71EA-F531-4927-AABD-1C31BCE8E187}';
  VC_2005_SP1_REDIST_X86 = '{7299052B-02A4-4627-81F2-1818DA5D550D}';
  VC_2005_SP1_REDIST_X64 = '{071C9B48-7C32-4621-A0AC-3F809523288F}';
  VC_2005_SP1_REDIST_IA64 = '{0F8FB34E-675E-42ED-850B-29D98C2ECE08}';
  VC_2005_SP1_ATL_SEC_UPD_REDIST_X86 = '{837B34E3-7C30-493C-8F6A-2B0F04E2912C}';
  VC_2005_SP1_ATL_SEC_UPD_REDIST_X64 = '{6CE5BAE9-D3CA-4B99-891A-1DC6C118A5FC}';
  VC_2005_SP1_ATL_SEC_UPD_REDIST_IA64 = '{85025851-A784-46D8-950D-05CB3CA43A13}';

  VC_2008_REDIST_X86 = '{FF66E9F6-83E7-3A3E-AF14-8DE9A809A6A4}';
  VC_2008_REDIST_X64 = '{350AA351-21FA-3270-8B7A-835434E766AD}';
  VC_2008_REDIST_IA64 = '{2B547B43-DB50-3139-9EBE-37D419E0F5FA}';
  VC_2008_SP1_REDIST_X86 = '{9A25302D-30C0-39D9-BD6F-21E6EC160475}';
  VC_2008_SP1_REDIST_X64 = '{8220EEFE-38CD-377E-8595-13398D740ACE}';
  VC_2008_SP1_REDIST_IA64 = '{5827ECE1-AEB0-328E-B813-6FC68622C1F9}';
  VC_2008_SP1_ATL_SEC_UPD_REDIST_X86 = '{1F1C2DFC-2D24-3E06-BCB8-725134ADF989}';
  VC_2008_SP1_ATL_SEC_UPD_REDIST_X64 = '{4B6C7001-C7D6-3710-913E-5BC23FCE91E6}';
  VC_2008_SP1_ATL_SEC_UPD_REDIST_IA64 = '{977AD349-C2A8-39DD-9273-285C08987C7B}';
  VC_2008_SP1_MFC_SEC_UPD_REDIST_X86 = '{9BE518E6-ECC6-35A9-88E4-87755C07200F}';
  VC_2008_SP1_MFC_SEC_UPD_REDIST_X64 = '{5FCE6D76-F5DC-37AB-B2B8-22AB8CEDB1D4}';
  VC_2008_SP1_MFC_SEC_UPD_REDIST_IA64 = '{515643D1-4E9E-342F-A75A-D1F16448DC04}';

  VC_2010_REDIST_X86 = '{196BB40D-1578-3D01-B289-BEFC77A11A1E}';
  VC_2010_REDIST_X64 = '{DA5E371C-6333-3D8A-93A4-6FD5B20BCC6E}';
  VC_2010_REDIST_IA64 = '{C1A35166-4301-38E9-BA67-02823AD72A1B}';
  VC_2010_SP1_REDIST_X86 = '{F0C3E5D1-1ADE-321E-8167-68EF0DE699A5}';
  VC_2010_SP1_REDIST_X64 = '{1D8E6291-B0D5-35EC-8441-6616F567A0F7}';
  VC_2010_SP1_REDIST_IA64 = '{88C73C1C-2DE5-3B01-AFB8-B46EF4AB41CD}';

  VC_2013_REDIST_X86 = '{13A4EE12-23EA-3371-91EE-EFB36DDFFF3E}';
  VC_2013_REDIST_X64 = '{A749D8E6-B613-3BE3-8F5F-045C84EBA29B}';
    
  VC_2015_REDIST_X86 = '{A2563E55-3BEC-3828-8D67-E5E8B9E8B675}';
  VC_2015_REDIST_X64 = '{0D3E9E15-DE7A-300B-96F1-B4AF12B96488}';
  
  VC_2015_SP2_REDIST_X86 = '{B5FC62F5-A367-37A5-9FD2-A6E137C0096F}';
  VC_2015_SP2_REDIST_X64 = '{7B50D081-E670-3B43-A460-0E2CDB5CE984}';
      
  VC_2015_SP3_REDIST_X86 = '{e2803110-78b3-4664-a479-3611a381656a}';
  VC_2015_SP3_REDIST_X64 = '{d992c12e-cab2-426f-bde3-fb8c53950b0d}';
  
	vcredist2015_x86_url = 'http://download.microsoft.com/download/9/3/F/93FCF1E7-E6A4-478B-96E7-D4B285925B00/vc_redist.x86.exe';
	vcredist2015_x64_url = 'http://download.microsoft.com/download/9/3/F/93FCF1E7-E6A4-478B-96E7-D4B285925B00/vc_redist.x64.exe';
	
	vcredist2015_sp2_x86_url = 'http://download.microsoft.com/download/9/b/3/9b3d2920-49f7-4e76-a55c-d72b51e44537/vc_redist.x86.exe';
	vcredist2015_sp2_x64_url = 'http://download.microsoft.com/download/8/c/b/8cb4af84-165e-4b36-978d-e867e07fc707/vc_redist.x64.exe';
	
	vcredist2015_sp3_x86_url = 'http://download.microsoft.com/download/6/A/A/6AA4EDFF-645B-48C5-81CC-ED5963AEAD48/vc_redist.x86.exe';
	vcredist2015_sp3_x64_url = 'http://download.microsoft.com/download/6/A/A/6AA4EDFF-645B-48C5-81CC-ED5963AEAD48/vc_redist.x64.exe';
	
function MsiQueryProductState(szProduct: string): INSTALLSTATE; 
  external 'MsiQueryProductState{#AW}@msi.dll stdcall';

function VCVersionInstalled(const ProductID: string): Boolean;
var
  State: Integer;
begin
  State := MsiQueryProductState(ProductID);
//MsgBox('Product ' + ProductID + ' state: ' + IntToStr(State), mbInformation, MB_OK);    
  Result := State = INSTALLSTATE_DEFAULT;
end;
  
procedure vcredist2015();
begin
	if (not VCVersionInstalled(VC_2015_SP3_REDIST_X86)) then
		AddProduct('vcredist2015.x86.exe',
               CustomMessage('vcredist2015_sp3_x86_lcid') + '/passive /norestart',
               CustomMessage('vcredist2015_sp3_x86_title'),
               CustomMessage('vcredist2015_sp3_x86_size'),
               vcredist2015_sp3_x86_url, false, false);
  if (IsWin64 and not VCVersionInstalled(VC_2015_SP3_REDIST_X64)) then
  	AddProduct('vcredist2015.x64.exe',
               CustomMessage('vcredist2015_sp3_x64_lcid') + '/passive /norestart',
               CustomMessage('vcredist2015_sp3_x64_title'),
               CustomMessage('vcredist2015_sp3_x64_size'),
               vcredist2015_sp3_x64_url, false, false);
end;
