#include <Windows.h>
#include <system_error>
#include <iostream>

void winapi_assert_or_abort(bool condition, const char* msg) {
	if (condition)
		return;
	else {
		std::cerr << std::system_error(std::error_code(GetLastError(), std::system_category()), msg).what() << std::endl;
		system("pause");
		exit(1);
	}
}