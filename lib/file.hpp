#pragma once

#include <filesystem>
#include <string>

namespace Cda
{

	class File
	{
	public:
		static File readFromFileSystem(std::wstring file_name, const std::filesystem::path& file);

		~File() = default;

	private:
		explicit File(std::wstring file_name);


		std::wstring m_file_name;
	};

}
