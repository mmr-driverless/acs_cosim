#pragma once
#include <filesystem>
#include <optional>

class Parameters
{
	std::filesystem::path m_acs_root_dir;

public:
	const std::filesystem::path& acs_root_dir();
};

