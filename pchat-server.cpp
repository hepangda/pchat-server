#include<service.h>
#include<spkg.h>
#include<rvpkg.h>
#include<dpkg.h>
#include<watchdog.h>
#include<unistd.h>
#include<thread>
using namespace std;

EXTERN_WATCHDOGS;

int main() {
	database_init();
	thread thread_spkg(spkg_init),
		   thread_dpkg(dpkg_init),
		   thread_rvpkg(rvpkg_init);

	thread_spkg.join();
	thread_dpkg.join();
	thread_rvpkg.join();
    return 0;
}
