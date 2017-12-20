#define _WINNT_WIN32 0500
#include<stdio.h>
#include <windows.h>

#define   ACCESS_READ     1
#define   ACCESS_WRITE   2

BOOL   IsAdmin(void)   {
      HANDLE   hToken;
      DWORD     dwStatus;
      DWORD     dwAccessMask;
      DWORD     dwAccessDesired;
      DWORD     dwACLSize;
      DWORD     dwStructureSize   =   sizeof(PRIVILEGE_SET);
      PACL       pACL                         =   NULL;
      PSID       psidAdmin               =   NULL;
      BOOL       bReturn                   =   FALSE;
      PRIVILEGE_SET       ps;
      GENERIC_MAPPING   GenericMapping;
      PSECURITY_DESCRIPTOR           psdAdmin                       =   NULL;
      SID_IDENTIFIER_AUTHORITY   SystemSidAuthority   =   SECURITY_NT_AUTHORITY;

      __try   {
            ImpersonateSelf(SecurityImpersonation);
            if   (!OpenThreadToken(GetCurrentThread(),   TOKEN_QUERY,   FALSE,  
                        &hToken))   {
                  if   (GetLastError()   !=   ERROR_NO_TOKEN)
                        __leave;
                  if   (!OpenProcessToken(GetCurrentProcess(),   TOKEN_QUERY,  
                              &hToken))
                        __leave;
            }
            if   (!AllocateAndInitializeSid(&SystemSidAuthority,   2,  
                        SECURITY_BUILTIN_DOMAIN_RID,   DOMAIN_ALIAS_RID_ADMINS,
                        0,   0,   0,   0,   0,   0,   &psidAdmin))
                  __leave;

            psdAdmin   =   LocalAlloc(LPTR,   SECURITY_DESCRIPTOR_MIN_LENGTH);
            if   (psdAdmin   ==   NULL)
                  __leave;

            if   (!InitializeSecurityDescriptor(psdAdmin,
                        SECURITY_DESCRIPTOR_REVISION))
                  __leave;
            dwACLSize   =   sizeof(ACL)   +   sizeof(ACCESS_ALLOWED_ACE)   +
                        GetLengthSid(psidAdmin)   -   sizeof(DWORD);
            pACL   =   (PACL)LocalAlloc(LPTR,   dwACLSize);
            if   (pACL   ==   NULL)
                  __leave;
            if   (!InitializeAcl(pACL,   dwACLSize,   ACL_REVISION2))
                  __leave;
            dwAccessMask=   ACCESS_READ   |   ACCESS_WRITE;
            if   (!AddAccessAllowedAce(pACL,   ACL_REVISION2,
                        dwAccessMask,   psidAdmin))
                  __leave;
            if   (!SetSecurityDescriptorDacl(psdAdmin,   TRUE,   pACL,   FALSE))
                  __leave;
            SetSecurityDescriptorGroup(psdAdmin,   psidAdmin,   FALSE);
            SetSecurityDescriptorOwner(psdAdmin,   psidAdmin,   FALSE);
            if   (!IsValidSecurityDescriptor(psdAdmin))
                  __leave;
            dwAccessDesired   =   ACCESS_READ;
            GenericMapping.GenericRead         =   ACCESS_READ;
            GenericMapping.GenericWrite       =   ACCESS_WRITE;
            GenericMapping.GenericExecute   =   0;
            GenericMapping.GenericAll           =   ACCESS_READ   |   ACCESS_WRITE;

            if   (!AccessCheck(psdAdmin,   hToken,   dwAccessDesired,  
                        &GenericMapping,   &ps,   &dwStructureSize,   &dwStatus,  
                        &bReturn))   {
                  printf( "AccessCheck()   failed   with   error   %lu\n ",   GetLastError());
                  __leave;
            }
            RevertToSelf();
      }   __finally   {
            if   (pACL)   LocalFree(pACL);
            if   (psdAdmin)   LocalFree(psdAdmin);    
            if   (psidAdmin)   FreeSid(psidAdmin);
      }
      return   bReturn;
}

int main(int argc, char * argv[])
{
      if   (IsAdmin())
            printf( "Run in admin\n");
      else
            printf( "Not run in admin\n");
	  return 0;
}