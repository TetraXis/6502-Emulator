#include "compiler_6502.h"

compiler::compiler()
{

}

bool compiler::compile(const std::string in, const std::string out)
{
	if (!read_txt(in))
	{
		return false;
	}
	remove_comments();
	write_txt(out);
	return true;
}

bool compiler::read_txt(const std::string path)
{
	std::ifstream fin(path);
	std::string line = {};
	u64 i = 0;

	if (!fin)
	{
		fin.close();
		return false;
	}
	while (std::getline(fin, line))
	{
		lines.emplace_back(i, line);
		i++;
	}
	fin.close();
	return true;
}

void compiler::write_txt(const std::string path) const
{
	std::ofstream fout(path);
	for (const source_line& line : lines)
	{
		fout << line.text << '\n';
	}
	fout.close();
}

void compiler::remove_comments()
{
	for (auto it = lines.begin(); it != lines.end();)
	{
		if (std::regex_match(it->text, masks::EMPTY))
		{
			lines.erase(it);
		}
		else
		{
			it++;
		}
	}
	for (source_line& line : lines)
	{
		std::regex_replace(line.text, masks::COMMENT, "");
	}
}

bool compiler::parse_line(const source_line& line) const
{

	return false;
}
