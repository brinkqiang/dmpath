
#include "dmpath.h"
#include "dmutil.h"
#include "dmflags.h"

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

DEFINE_string(addpath, "", "Path to add to the PATH environment variable");
DEFINE_string(removepath, "", "Path to remove from the PATH environment variable");

int main(int argc, char** argv) {
	// ʾ������ Python ��װ·����ӵ� PATH
	DMFLAGS_INIT(argc, argv);
	SetUsageMessage("Usage: dmpath --addpath=<path> or --removepath=<path>");
	if (argc == 1) {
		std::cout << ProgramUsage();
		return 1;
	}
#ifdef _WIN32

	if (!IsRunAsAdmin()) {
		std::cerr << "This program requires administrator privileges. Please run as administrator." << std::endl;
		return 1;
	}

	// ���·��
	if (!FLAGS_addpath.empty()) {
		if (AddToPath(FLAGS_addpath)) {
			std::cout << "Path added successfully: " << FLAGS_addpath << std::endl;
		}
		else {
			std::cerr << "Failed to add path: " << FLAGS_addpath << std::endl;
		}
	}

	// �Ƴ�·��
	if (!FLAGS_removepath.empty()) {
		if (RemoveFromPath(FLAGS_removepath)) {
			std::cout << "Path removed successfully: " << FLAGS_removepath << std::endl;
		}
		else {
			std::cerr << "Failed to remove path: " << FLAGS_removepath << std::endl;
		}
	}
#endif
	return 0;
}