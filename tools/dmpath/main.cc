
#include "dmpath.h"

bool IsRunAsAdmin() {
	BOOL isAdmin = FALSE;
	PSID adminGroup = nullptr;

	// ���䲢��ʼ��һ��SID���ڹ���Ա��
	SID_IDENTIFIER_AUTHORITY NtAuthority = SECURITY_NT_AUTHORITY;
	if (AllocateAndInitializeSid(&NtAuthority, 2, SECURITY_BUILTIN_DOMAIN_RID, DOMAIN_ALIAS_RID_ADMINS, 0, 0, 0, 0, 0, 0, &adminGroup)) {
		// ��鵱ǰ�����Ƿ��ڹ���Ա����
		if (!CheckTokenMembership(nullptr, adminGroup, &isAdmin)) {
			isAdmin = FALSE;
		}
		FreeSid(adminGroup);
	}

	return isAdmin == TRUE;
}

int main() {
	// ʾ������ Python ��װ·����ӵ� PATH
#ifdef _WIN32

	if (!IsRunAsAdmin()) {
		std::cerr << "This program requires administrator privileges. Please run as administrator." << std::endl;
		return 1;
	}

	AddToPath("C:\\Python499\\");
#endif
	return 0;
}