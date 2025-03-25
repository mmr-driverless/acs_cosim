#include "pch.h"
#include "Parameters.h"
#include "winapi_helpers.hpp"

const std::filesystem::path& Parameters::acs_root_dir() {
	if (not m_acs_root_dir.empty())
		return m_acs_root_dir;

	wchar_t env_var[MAX_PATH];
	DWORD root_dir_len = GetEnvironmentVariableW(L"ACS_ROOT_DIR", env_var, MAX_PATH);
	winapi_assert_or_abort(root_dir_len > 0 && root_dir_len <= sizeof(env_var), "Failed to read the ACS_ROOT_DIR environment variable.");

	m_acs_root_dir = std::filesystem::path(env_var);
	return m_acs_root_dir;
}