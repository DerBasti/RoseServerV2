
#include "WorldServer.h"
#include "FileTypes\VFS.h"
#include "FileTypes\AIP.h"

int main() {
	MYSQL mysql;
	WorldServer ws("127.0.0.1", 29200, &mysql);
	return 0;
}