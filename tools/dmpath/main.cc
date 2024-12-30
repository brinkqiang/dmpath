
#include "dmpath.h"
#include "dmutil.h"
#include "dmflags.h"

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
#endif

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

	return 0;
}