#include "compiler_6502.h"

compiler::compiler()
{

}

bool compiler::compile(const std::string& in, const std::string& out)
{
	timer tm;
	std::cout << "Compilation started.\n";
	tm.start();
	std::cout << " -- Reading \"" << in << "\"...\n";
	if (!read_txt(in))
	{
		return false;
	}
	std::cout << " -- Resolving defines...\n";
	resolve_defines();
	std::cout << " -- Cleaning up...\n";
	cleanup();
	std::cout << " -- Parsing code...\n";
	for (const source_line& line : source_lines)
	{
		active_source_line = line;
		if (parse_line(line))
		{
			byte_lines.push_back(active_byte_line);
		}
		else
		{
			tm.stop();
			print_errors();
			std::cout << "Compilation FAILED.\n";
			std::cout << "Compilation took " << tm.elapsed_seconds() << " seconds.\n";
			return false;
		}
	}
	tm.stop();
	print_warnings();
	std::cout << "Compilation SUCCESSFUL.\n";
	std::cout << "Compilation took " << tm.elapsed_seconds() << " seconds.\n";
	std::cout << " -- Writing to \"" << out << "\"...\n";
	build_binary(out);
	std::cout << "Compilation finished.\n";
	return true;
}

bool compiler::compile(const std::string& in, ram& out)
{
	timer tm;
	std::cout << "Compilation started.\n";
	tm.start();
	std::cout << " -- Reading \"" << in << "\"...\n";
	if (!read_txt(in))
	{
		return false;
	}
	std::cout << " -- Resolving defines...\n";
	resolve_defines();
	std::cout << " -- Cleaning up...\n";
	cleanup();
	std::cout << " -- Parsing code...\n";
	for (const source_line& line : source_lines)
	{
		active_source_line = line;
		if (parse_line(line))
		{
			byte_lines.push_back(active_byte_line);
		}
		else
		{
			tm.stop();
			print_errors();
			std::cout << "Compilation FAILED.\n";
			std::cout << "Compilation took " << tm.elapsed_seconds() << " seconds.\n";
			return false;
		}
	}
	tm.stop();
	print_warnings();
	std::cout << "Compilation SUCCESSFUL.\n";
	std::cout << "Compilation took " << tm.elapsed_seconds() << " seconds.\n";
	std::cout << " -- Building RAM...\n";
	build_ram(out);
	std::cout << "Compilation finished.\n";
	return true;
}

bool compiler::read_txt(const std::string& path)
{
	std::ifstream fin(path);
	std::string line = {};
	u64 i = 1;

	if (!fin)
	{
		fin.close();
		return false;
	}
	while (std::getline(fin, line))
	{
		source_lines.emplace_back(i, line);
		i++;
	}
	fin.close();
	return true;
}

void compiler::write_txt(const std::string& path) const
{
	std::ofstream fout(path);
	for (const source_line& line : source_lines)
	{
		fout << line.text << '\n';
	}
	fout.close();
}

void compiler::cleanup()
{
	for (auto it = source_lines.begin(); it != source_lines.end();)
	{
		if (std::regex_match(it->text, mask::EMPTY))
		{
			it = source_lines.erase(it);
		}
		else
		{
			it++;
		}
	}
	for (source_line& line : source_lines)
	{
		line.text = std::regex_replace(line.text, mask::COMMENT, "");
		line.text = std::regex_replace(line.text, mask::EXTRA_SPACES, " ");
		line.text = std::regex_replace(line.text, mask::BEGIN_SPACES, "");
		line.text = std::regex_replace(line.text, mask::END_SPACES, "");
	}
}

void compiler::resolve_defines()
{
	u64			i = 0;
	std::smatch define_match = {};
	std::string name = {};
	std::string value = {};

	for (const auto& [number, line, parsed_op] : source_lines)
	{
		if (std::regex_match(line, mask::DEFINE))
		{
			std::regex_match(line, define_match, mask::DEFINE);
			name = define_match.str(1);
			value = define_match.str(2);

			for (u64 j = i + 1; j < source_lines.size(); j++)
			{
				source_lines[j].text = std::regex_replace(source_lines[j].text, std::regex(name), value);
			}
		}
		i++;
	}
}

bool compiler::resolve_labels()
{
	std::smatch	label_match = {};
	std::string	label = {};

	for (auto& [number, line, parsed_op] : source_lines)
	{
		if (std::regex_match(line, mask::EMPTY) || std::regex_match(line, mask::DEFINE))
		{
			continue;
		}

		if (std::regex_match(line, label_match, mask::CORRECT_LINE))
		{
			label = label_match.str(1);
			if (label != "")
			{

			}
		}
	}
}

bool compiler::parse_line(const source_line& line)
{
	if (std::regex_match(line.text, mask::EMPTY) || std::regex_match(line.text, mask::DEFINE))
	{
		return true;
	}

	u64			line_number = line.number;
	std::string	line_text = line.text;
	std::string	op_text = {};
	std::string	addr_text = {};
	std::smatch	line_match = {};

	if (!std::regex_match(line_text, mask::CORRECT_LINE))
	{
		std::stringstream error_line;
		error_line << "Compilation ERROR at line: " << line_number << " \"" << line_text << "\".\n";
		error_line << "Incorrect source line.";
		errors.push_back(error_line.str());
		return false;
	}

	std::regex_match(line_text, line_match, mask::CORRECT_LINE);
	op_text = line_match.str(3);
	addr_text = line_match.str(6);

	if (op_text == "BRK" || op_text == "brk")
	{
		if (addr_text == "")
		{
			active_byte_line.size = 1;
			active_byte_line.bytes[0] = op::BRK;
			return true;
		}
		else
		{
			std::stringstream error_line;
			error_line << "Compilation ERROR at line: " << line_number << " \"" << line_text << "\".\n";
			error_line << "Operator \"BRK\" has only implied addressing mode.";
			errors.push_back(error_line.str());
			return false;
		}
	}
	if (op_text == "JMP" || op_text == "jmp")
	{
		std::string addr_num = {};
		u16 parsed_address = {};
		std::smatch addr_match = {};

		if (std::regex_match(addr_text, mask::ABS_IN))
		{
			std::regex_match(addr_text, addr_match, mask::ABS_IN);
			addr_num = addr_match.str(1);
			parsed_address = parse_number(addr_num);

			active_byte_line.size = 3;
			active_byte_line.bytes[0] = op::JMP_IN;
			active_byte_line.bytes[1] = u8(parsed_address & 0xFF);
			active_byte_line.bytes[2] = u8(parsed_address >> BIT_SIZE);
			return true;
		}
		else if (std::regex_match(addr_text, mask::ABS))
		{
			std::regex_match(addr_text, addr_match, mask::ABS);
			addr_num = addr_match.str(1);
			parsed_address = parse_number(addr_num);

			active_byte_line.size = 3;
			active_byte_line.bytes[0] = op::JMP_IN;
			active_byte_line.bytes[1] = u8(parsed_address & 0xFF);
			active_byte_line.bytes[2] = u8(parsed_address >> BIT_SIZE);
			return true;
		}
		else
		{
			std::stringstream error_line;
			error_line << "Compilation ERROR at line: " << line_number << " \"" << line_text << "\".\n";
			error_line << "Operator \"JMP\" has only absolute or indirect addressing modes.";
			errors.push_back(error_line.str());
			return false;
		}
	}

	if (!parse_operator(op_text))
	{
		return false;
	}

	if (!parse_addressing(addr_text))
	{
		return false;
	}

	return true;
}

bool compiler::parse_operator(const std::string& op)
{
	if (compiler::instructions_map.find(op) == compiler::instructions_map.end())
	{
		std::stringstream error_line;
		error_line << "Compilation ERROR at line: " << active_source_line.number << " \"" << active_source_line.text << "\".\n";
		error_line << "Couldn't resolve operator: \"" << op << "\".";
		errors.push_back(error_line.str());
		return false;
	}

	active_operator = compiler::instructions_map.at(op);
	active_source_line.parsed_op = op;
	return true;
}

bool compiler::parse_addressing(const std::string& addr)
{
	u16			parsed_address = {};
	std::string	addr_num = {};
	std::smatch	addr_match = {};

	if (addr == "") // implied
	{
		if (active_operator.imp == ABSENT)
		{
			std::stringstream error_line;
			error_line << "Compilation ERROR at line: " << active_source_line.number << " \"" << active_source_line.text << "\".\n";
			error_line << "Operator \"" << active_source_line.parsed_op << "\" doesn't have implied addressing mode.";
			errors.push_back(error_line.str());
			return false;
		}
		active_byte_line.size = 1;
		active_byte_line.bytes[0] = active_operator.imp;
		return true;
	}

	if (std::regex_match(addr, mask::IM)) // immediate
	{
		if (active_operator.im == ABSENT)
		{
			std::stringstream error_line;
			error_line << "Compilation ERROR at line: " << active_source_line.number << " \"" << active_source_line.text << "\".\n";
			error_line << "Operator \"" << active_source_line.parsed_op << "\" doesn't have immediate addressing mode.";
			errors.push_back(error_line.str());
			return false;
		}
		std::regex_match(addr, addr_match, mask::IM);
		addr_num = addr_match.str(1);
		parsed_address = parse_number(addr_num);
		if (parsed_address > 0xFF)
		{
			std::stringstream warning_line;
			warning_line << "WARNING at line: " << active_source_line.number << " \"" << active_source_line.text << "\".\n";
			warning_line << "Integer \"" << addr_num << "\" is too large for immediate addressing mode.";
			warnings.push_back(warning_line.str());
		}
		active_byte_line.size = 2;
		active_byte_line.bytes[0] = active_operator.im;
		active_byte_line.bytes[1] = u8(parsed_address & 0xFF);
		return true;
	}

	if (std::regex_match(addr, mask::ABS)) // absolute, zero page or relative
	{
		std::regex_match(addr, addr_match, mask::ABS);
		addr_num = addr_match.str(1);
		parsed_address = parse_number(addr_num);

		if (active_operator.zp != ABSENT && parsed_address < 0xFF) // zero page or relative
		{
			active_byte_line.size = 2;
			active_byte_line.bytes[0] = active_operator.zp;
			active_byte_line.bytes[1] = u8(parsed_address & 0xFF);
			return true;
		}
		if (active_operator.abs != ABSENT) // absolute
		{
			active_byte_line.size = 3;
			active_byte_line.bytes[0] = active_operator.abs;
			active_byte_line.bytes[1] = u8(parsed_address & 0xFF);
			active_byte_line.bytes[2] = u8(parsed_address >> BIT_SIZE);
			return true;
		}
		if (active_operator.zp != ABSENT) // zero page or relative
		{
			if (parsed_address > 0xFF)
			{
				std::stringstream warning_line;
				warning_line << "WARNING at line: " << active_source_line.number << " \"" << active_source_line.text << "\".\n";
				warning_line << "Integer \"" << addr_num << "\" is too large for zero page or relative addressing modes.";
				warnings.push_back(warning_line.str());
			}
			active_byte_line.size = 2;
			active_byte_line.bytes[0] = active_operator.zp;
			active_byte_line.bytes[1] = u8(parsed_address & 0xFF);
			return true;
		}
		std::stringstream error_line;
		error_line << "Compilation ERROR at line: " << active_source_line.number << " \"" << active_source_line.text << "\".\n";
		error_line << "Operator \"" << active_source_line.parsed_op << "\" doesn't have absolute, zero page or relative addressing modes.";
		errors.push_back(error_line.str());
		return false;
	}

	if (std::regex_match(addr, mask::ABS_X)) // absolute, x or zero page, x
	{
		std::regex_match(addr, addr_match, mask::ABS_X);
		addr_num = addr_match.str(1);
		parsed_address = parse_number(addr_num);

		if (active_operator.zp_x != ABSENT && parsed_address < 0xFF) // zero page, x
		{
			active_byte_line.size = 2;
			active_byte_line.bytes[0] = active_operator.zp_x;
			active_byte_line.bytes[1] = u8(parsed_address & 0xFF);
			return true;
		}
		if (active_operator.abs_x != ABSENT) // absolute, x
		{
			active_byte_line.size = 3;
			active_byte_line.bytes[0] = active_operator.abs_x;
			active_byte_line.bytes[1] = u8(parsed_address & 0xFF);
			active_byte_line.bytes[2] = u8(parsed_address >> BIT_SIZE);
			return true;
		}
		if (active_operator.zp_x != ABSENT) // zero page, x
		{
			if (parsed_address > 0xFF)
			{
				std::stringstream warning_line;
				warning_line << "WARNING at line: " << active_source_line.number << " \"" << active_source_line.text << "\".\n";
				warning_line << "Integer \"" << addr_num << "\" is too large for (zero page, x) addressing mode.";
				warnings.push_back(warning_line.str());
			}
			active_byte_line.size = 2;
			active_byte_line.bytes[0] = active_operator.zp_x;
			active_byte_line.bytes[1] = u8(parsed_address & 0xFF);
			return true;
		}
		std::stringstream error_line;
		error_line << "Compilation ERROR at line: " << active_source_line.number << " \"" << active_source_line.text << "\".\n";
		error_line << "Operator \"" << active_source_line.parsed_op << "\" doesn't have (absolute, x) or (zero page, x) addressing modes.";
		errors.push_back(error_line.str());
		return false;
	}

	if (std::regex_match(addr, mask::ABS_Y)) // absolute, y or zero page, y
	{
		std::regex_match(addr, addr_match, mask::ABS_Y);
		addr_num = addr_match.str(1);
		parsed_address = parse_number(addr_num);

		if (active_operator.zp_y != ABSENT && parsed_address < 0xFF) // zero page, y
		{
			active_byte_line.size = 2;
			active_byte_line.bytes[0] = active_operator.zp_y;
			active_byte_line.bytes[1] = u8(parsed_address & 0xFF);
			return true;
		}
		if (active_operator.abs_y != ABSENT) // absolute, y
		{
			active_byte_line.size = 3;
			active_byte_line.bytes[0] = active_operator.abs_y;
			active_byte_line.bytes[1] = u8(parsed_address & 0xFF);
			active_byte_line.bytes[2] = u8(parsed_address >> BIT_SIZE);
			return true;
		}
		if (active_operator.zp_y != ABSENT) // zero page, y
		{
			if (parsed_address > 0xFF)
			{
				std::stringstream warning_line;
				warning_line << "WARNING at line: " << active_source_line.number << " \"" << active_source_line.text << "\".\n";
				warning_line << "Integer \"" << addr_num << "\" is too large for (zero page, y) addressing mode.";
				warnings.push_back(warning_line.str());
			}
			active_byte_line.size = 2;
			active_byte_line.bytes[0] = active_operator.zp_y;
			active_byte_line.bytes[1] = u8(parsed_address & 0xFF);
			return true;
		}
		std::stringstream error_line;
		error_line << "Compilation ERROR at line: " << active_source_line.number << " \"" << active_source_line.text << "\".\n";
		error_line << "Operator \"" << active_source_line.parsed_op << "\" doesn't have (absolute, y) or (zero page, y) addressing modes.";
		errors.push_back(error_line.str());
		return false;
	}

	if (std::regex_match(addr, mask::IN_X)) // indirect, x
	{
		if (active_operator.in_x != ABSENT) // indirect, x
		{
			std::regex_match(addr, addr_match, mask::IN_X);
			addr_num = addr_match.str(1);
			parsed_address = parse_number(addr_num);
			if (parsed_address > 0xFF)
			{
				std::stringstream warning_line;
				warning_line << "WARNING at line: " << active_source_line.number << " \"" << active_source_line.text << "\".\n";
				warning_line << "Integer \"" << addr_num << "\" is too large for (indirect, x) addressing mode.";
				warnings.push_back(warning_line.str());
			}

			active_byte_line.size = 2;
			active_byte_line.bytes[0] = active_operator.in_x;
			active_byte_line.bytes[1] = u8(parsed_address & 0xFF);
			return true;
		}
	}

	if (std::regex_match(addr, mask::IN_Y)) // indirect, y
	{
		if (active_operator.in_y != ABSENT) // indirect, y
		{
			std::regex_match(addr, addr_match, mask::IN_Y);
			addr_num = addr_match.str(1);
			parsed_address = parse_number(addr_num);
			if (parsed_address > 0xFF)
			{
				std::stringstream warning_line;
				warning_line << "WARNING at line: " << active_source_line.number << " \"" << active_source_line.text << "\".\n";
				warning_line << "Integer \"" << addr_num << "\" is too large for (indirect, y) addressing mode.";
				warnings.push_back(warning_line.str());
			}

			active_byte_line.size = 2;
			active_byte_line.bytes[0] = active_operator.in_y;
			active_byte_line.bytes[1] = u8(parsed_address & 0xFF);
			return true;
		}
	}


	std::stringstream error_line;
	error_line << "Compilation ERROR at line: " << active_source_line.number << " \"" << active_source_line.text << "\".\n";
	error_line << "Couldn't parse \"" << addr << "\" as addressing mode.\n";
	errors.push_back(error_line.str());
	return false;
}

void compiler::print_errors() const
{
	if (errors.size() == 0)
	{
		return;
	}
	std::cout << '\n';
	for (const std::string& error : errors)
	{
		std::cout << "!> " << error << '\n';
	}
	std::cout << '\n';
}

void compiler::print_warnings() const
{
	if (warnings.size() == 0)
	{
		return;
	}
	std::cout << '\n';
	for (const std::string& warning : warnings)
	{
		std::cout << " > " << warning << '\n';
	}
	std::cout << '\n';
}

u16 compiler::parse_number(const std::string& num)
{
	u64 parsed_num = {};

	if (std::regex_match(num, mask::DEC))
	{
		parsed_num = std::stoi(num);
		if (parsed_num > 0xFFFF)
		{
			std::stringstream warning_line;
			warning_line << "WARNING at line: " << active_source_line.number << " \"" << active_source_line.text << "\".\n";
			warning_line << "Integer \"" << num << "\" is too large.";
			warnings.push_back(warning_line.str());
		}
		return u16(parsed_num);
	}

	if (std::regex_match(num, mask::BIN))
	{
		//               vv 16 bits + '%'
		if (num.size() > 17)
		{
			std::stringstream warning_line;
			warning_line << "WARNING at line: " << active_source_line.number << " \"" << active_source_line.text << "\".\n";
			warning_line << "Integer \"" << num << "\" is too large.";
			warnings.push_back(warning_line.str());
		}
		u32 bin_value = 1;
		for (u8 i = 0; i < 16; i++)
		{
			if (num[num.size() - i] == '1')
			{
				parsed_num += bin_value;
			}
			bin_value <<= 1;
		}
		return u16(parsed_num);
	}

	if (std::regex_match(num, mask::HEX))
	{
		//               vv 4 hexes + '$'
		if (num.size() > 5)
		{
			std::stringstream warning_line;
			warning_line << "WARNING at line: " << active_source_line.number << " \"" << active_source_line.text << "\".\n";
			warning_line << "Integer \"" << num << "\" is too large.";
			warnings.push_back(warning_line.str());
		}
		std::stringstream ss;
		ss << std::hex << num.substr(1, num.size() - 1);
		ss >> parsed_num;
		return u16(parsed_num);
	}


	std::stringstream error_line;
	error_line << "Compilation ERROR at line: " << active_source_line.number << " \"" << active_source_line.text << "\".\n";
	error_line << "Couldn't resolve number \"" << num << "\".";
	errors.push_back(error_line.str());

	return 0;
}

void compiler::build_binary(const std::string& out) const
{
	std::ofstream fout(out, std::ios::binary);
	for (const byte_line& line : byte_lines)
	{
		for (u8 i = 0; i < line.size; i++)
		{
			fout << line.bytes[i];
		}
	}
	fout.close();
}

void compiler::build_ram(ram& out) const
{
	out.clear();

	out[0xFFFC] = op::JSR_ABS;
	out[0xFFFD] = 0x00;
	out[0xFFFE] = 0x02;
	out[0xFFFF] = op::KIL;

	u16 addr = 0x200;
	for (const byte_line& line : byte_lines)
	{
		for (u8 i = 0; i < line.size; i++)
		{
			out[addr++] = line.bytes[i];
		}
	}
	out[addr] = op::RTS;
}
