#include "compiler_6502.h"

source_line::source_line()
{

}

source_line::source_line(u64 num, const std::string& new_text)
{
	number = num;
	text = new_text;
}


msg msg::err(const source_line& line, const std::string& type, const std::string& add_msg)
{
	msg result = {};

	result.text = "ERROR: " + type + ".\n";
	result.text += "At line #" + std::to_string(line.number) + ": \"" + line.text + "\".\n";
	result.text += add_msg + ".";

	return result;
}

msg msg::wrn(const source_line& line, const std::string& type, const std::string& add_msg)
{
	msg result = {};

	result.text = "WARNING: " + type + ".\n";
	result.text += "At line #" + std::to_string(line.number) + ": \"" + line.text + "\".\n";
	result.text += add_msg + ".";

	return result;
}


compiler::compiler()
{

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
		if (!std::regex_match(line, mask::EMPTY))
		{
			source_lines.emplace_back(i, line);
		}
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

void compiler::print_errors() const
{
	if (errors.size() == 0)
	{
		return;
	}
	std::cout << '\n';
	for (const auto& [error] : errors)
	{
		std::cout << "!> " << error << '\n\n';
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
	for (const auto& [warning] : warnings)
	{
		std::cout << " > " << warning << '\n\n';
	}
	std::cout << '\n';
}

u16 compiler::parse_number(const std::string& num)
{
	u64 parsed_num = 0xFFFFFFFFFFFFFFFF;

	if (std::regex_match(num, mask::HEX))
	{
		std::stringstream ss;
		ss << std::hex << num.substr(1, num.size() - 1);
		ss >> parsed_num;
	}
	else if (std::regex_match(num, mask::DEC))
	{
		parsed_num = std::stoi(num);
	}
	else if (std::regex_match(num, mask::BIN))
	{
		u64 bin_value = 1;
		for (u8 i = 0; i < num.size() - 1; i++)
		{
			if (num[num.size() - i] == '1')
			{
				parsed_num += bin_value;
			}
			bin_value <<= 1;
		}
	}

	if (parsed_num == 0xFFFFFFFFFFFFFFFF)
	{
		errors.push_back(msg::err(*active_line, "Unrecognized integer literal", "Could not parse \"" + num + "\" as integer."));
		return 0;
	}

	if (parsed_num > 0xFFFF)
	{
		warnings.push_back(msg::wrn(*active_line, "Integer literal is too large", "Integer \"" + num + "\" is too large."));
	}

	return u16(parsed_num);
}

u8 compiler::convert_to_byte(u64 number)
{
	if (number > 0xFF)
	{
		warnings.push_back(msg::wrn(*active_line, "Integer literal is too large", "Integer \"" + std::to_string(number) + "\" is too large for one byte addressing modes."));
	}
	return u8(number & 0xFF);
}

bool compiler::compile(const std::string& path)
{
	bool success = true;
	bool unparsed_lines_present = false;
	internal_timer.start();

	std::cout << " -- Reading from \"" << path << "\"...\n";
	if (!read_txt(path))
	{
		success = false;
		std::cout << "FAILURE: Could not read from " << path << ".\n";
		goto finish_compilation;
	}

	std::cout << " -- Resolving defines...\n";
	resolve_defines();

	std::cout << " -- Cleaning up...\n";
	clean_up();

	std::cout << " -- Reading labels...\n";
	read_labels();

	std::cout << " -- Parsing lines...\n";
	for (u64 attempt = 0; attempt < MAX_PARSE_PASSES; attempt++)
	{
		if (errors.size() != 0)
		{
			success = false;
			goto finish_compilation;
		}

		unparsed_lines_present = false;
		parsed_bytes = 0;
		for (source_line& line : source_lines)
		{
			active_line = &line;
			if (!parse_active_line())
			{
				unparsed_lines_present = true;
			}
			parsed_bytes += active_line->byte_size;
		}

		if (!unparsed_lines_present)
		{
			goto finish_compilation;
		}
	}
	std::cout << "Parsing failed after " << MAX_PARSE_PASSES << " tries.\n";
	success = false;


finish_compilation:;
	internal_timer.stop();
	print_warnings();
	if (success)
	{
		std::cout << "Compilation SUCCESSFUL.\n";
	}
	else
	{
		print_errors();
		std::cout << "Compilation FAILED.\n";
	}
	std::cout << "Compilation took " << internal_timer.elapsed_seconds() << " seconds.\n";
	return success;
}

bool compiler::compile_and_build(const std::string& path, cpu& cpu_ref)
{
	if (compile(path))
	{
		u16 addr = 0x0200;

		cpu_ref.mem.clear();
		cpu_ref.mem[0xFFFF] = op::KIL;
		cpu_ref.mem[0xFFFC] = op::JSR_ABS;
		cpu_ref.mem[0xFFFD] = 0x00;
		cpu_ref.mem[0xFFFE] = 0x02;
		for (const source_line& line : source_lines)
		{
			for (u8 i = 0; i < line.byte_size; i++)
			{
				cpu_ref.mem[addr++] = line.bytes[i];
			}
		}
		cpu_ref.mem[addr++] = op::RTS;
		return true;
	}
	return false;
}

void compiler::resolve_defines()
{
	std::smatch define_match = {};
	std::string name = {};
	std::string value = {};

	for (auto it = source_lines.begin(); it != source_lines.end();)
	{
		if (std::regex_match(it->text, define_match, mask::WEAK_DEFINE))
		{
			name = "\\b" + define_match.str(1) + "\\b";
			value = define_match.str(2);

			// fixing string so formatting would not kick in for "$01"
			value = std::regex_replace(value, std::regex("(\\$[a-fA-F\\d]+)"), "$$$1");

			for (auto it_2 = it + 1; it_2 != source_lines.end(); ++it_2)
			{
				it_2->text = std::regex_replace(it_2->text, std::regex(name), value);
			}

			it = source_lines.erase(it);
		}
		else if (std::regex_match(it->text, define_match, mask::STRONG_DEFINE))
		{
			name = define_match.str(1);
			value = define_match.str(2);

			// fixing string so formatting would not kick in for "$01"
			value = std::regex_replace(value, std::regex("(\\$[a-fA-F\\d]+)"), "$$$1");

			for (auto it_2 = it + 1; it_2 != source_lines.end(); ++it_2)
			{
				it_2->text = std::regex_replace(it_2->text, std::regex(name), value);
			}

			it = source_lines.erase(it);
		}
		else
		{
			++it;
		}
	}
}

void compiler::clean_up()
{
	// empty lines were not read

	for (source_line& line : source_lines)
	{
		line.text = std::regex_replace(line.text, mask::COMMENT, "");
		line.text = std::regex_replace(line.text, mask::EXTRA_SPACES, " ");
		line.text = std::regex_replace(line.text, mask::BEGIN_SPACES, "");
		line.text = std::regex_replace(line.text, mask::END_SPACES, "");
	}
}

void compiler::read_labels()
{
	std::smatch label_match = {};
	std::string name = {};

	for (const source_line& line : source_lines)
	{
		if (std::regex_match(line.text, label_match, mask::LABEL_DECL))
		{
			name = label_match.str(1);
			if (op_map.find(name) == op_map.end())
			{
				labels[name] = ABSENT;
			}
		}
		else if (std::regex_match(line.text, label_match, mask::CORRECT_OP))
		{
			name = label_match.str(2);
			if (name != "" && op_map.find(name) == op_map.end())
			{
				labels[name] = ABSENT;
			}
		}
	}
}

bool compiler::parse_active_line()
{
	if (active_line->parsed)
	{
		parsed_bytes += active_line->byte_size;
		return true;
	}

	std::smatch line_match = {};
	std::string label = {};
	std::string op = {};
	std::string addr = {};

	if (std::regex_match(active_line->text, line_match, mask::LABEL_DECL))
	{
		label = line_match.str(1);

		if (op_map.find(label) == op_map.end())
		{
			if (label != "")
			{
				labels.at(label) = parsed_bytes;
			}

			active_line->parsed = true;
			active_line->byte_size = 0;
			return true;
		}
	}

	if (std::regex_match(active_line->text, line_match, mask::CORRECT_OP))
	{
		label	= line_match.str(2);
		op		= line_match.str(3);
		addr	= line_match.str(5);

		// BEQ LOOP
		// LABEL TAX
		// Both look the same, but different
		if (labels.find(op) != labels.end())
		{
			addr = op;
			op = label;
			label = "";
		}

		if (label != "")
		{
			labels[label] = parsed_bytes;
		}

		if (parse_op(op))
		{
			if (resolve_addr_for_op(addr, op))
			{
				return !active_line->unresolved_label;
			}
			else
			{
				return false;
			}
		}
		else
		{
			return false;
		}
	}
	else
	{
		errors.push_back(msg::err(*active_line, "Incorrect source line", "Could not parse source line"));
		return false;
	}
}

bool compiler::parse_op(const std::string& op)
{
	// handle JMP and BRK here

	if (op_map.find(op) != op_map.end())
	{
		active_line->parsed_op = op_map.at(op);
		return true;
	}
	else
	{
		errors.push_back(msg::err(*active_line, "Unknown operator", "Could not parse \"" + op + "\" as valid operator"));
		return false;
	}
}

bool compiler::resolve_addr_for_op(std::string addr, const std::string& op)
{
	u16			parsed_addr = 0;
	std::smatch addr_match = {};
	std::string label = {};
	std::string sign = {};
	std::string value = {};

	// if addr is complex "ADDR + 1" or "ADDR - 1"
	if (std::regex_match(addr, addr_match, mask::COMPLEX))
	{
		std::string label = addr_match.str(1); // left number
		std::string sign = addr_match.str(2);
		std::string value = addr_match.str(3); // right number

		if (sign == "+")
		{
			addr = std::to_string(parse_number(label) + parse_number(value));
		}
		else if (sign == "-")
		{
			addr = std::to_string(parse_number(label) - parse_number(value));
		}
		else
		{
			errors.push_back(msg::err(*active_line, "Incorrect operator for complex address", "\"" + sign + "\" is not a valid operator for complex address"));
			return false;
		}
	}

	// label in use
	if (std::regex_match(addr, addr_match, mask::LABEL_IN_USE))
	{
		label = addr_match.str(1);
		sign = addr_match.str(3);
		value = addr_match.str(4);

		if (labels.find(label) != labels.end())
		{
			if (labels.at(label) != ABSENT)
			{
				if (ops_that_can_use_labels.find(op) == ops_that_can_use_labels.end())
				{
					errors.push_back(msg::err(*active_line, "Label misuse", "Opeator \"" + op + "\" can not be used with labels"));
					return false;
				}

				// ops that use relative addressing
				if (ops_that_can_use_labels.at(op) == 2)
				{
					if (sign != "" || value != "")
					{
						errors.push_back(msg::err(*active_line, "Label misuse", "Opeator that use relative addressing modes can not dereference [LABEL +/- VALUE}"));
						return false;
					}

					i32 relative_addr = labels.at(label) - parsed_bytes - 1;
					if (relative_addr < -128 || relative_addr > 127)
					{
						errors.push_back(msg::err(*active_line, "Brach relative jump is too large", "Brach relative jumps can range in [-128; 127]"));
						return false;
					}

					active_line->parsed = true;
					active_line->unresolved_label = false;
					active_line->byte_size = 2;
					active_line->bytes[0] = active_line->parsed_op.imp;
					active_line->bytes[1] = char(relative_addr);
					return true;
				}
				else // ops that use absolute addressing
				{
					parsed_addr = labels.at(label);

					if (sign != "")
					{

						if (sign == "+")
						{
							if (value == "")
							{
								errors.push_back(msg::err(*active_line, "Missing operand", "Operator \"+\" requirers two operands"));
								return false;
							}
							parsed_addr += parse_number(value);

							active_line->parsed = true;
							active_line->unresolved_label = false;
							active_line->byte_size = 3;
							active_line->bytes[0] = active_line->parsed_op.abs;
							active_line->bytes[1] = parsed_addr & 0xFF;
							active_line->bytes[2] = parsed_addr >> BIT_SIZE;
							return true;
						}
						else if (sign == "-")
						{
							if (value == "")
							{
								errors.push_back(msg::err(*active_line, "Missing operand", "Operator \"-\" requirers two operands"));
								return false;
							}
							parsed_addr -= parse_number(value);

							active_line->parsed = true;
							active_line->unresolved_label = false;
							active_line->byte_size = 3;
							active_line->bytes[0] = active_line->parsed_op.abs;
							active_line->bytes[1] = parsed_addr & 0xFF;
							active_line->bytes[2] = parsed_addr >> BIT_SIZE;
							return true;
						}
						else
						{
							errors.push_back(msg::err(*active_line, "Unknown operator", "Could not parse \"" + sign + "\" as valid operator"));
							return false;
						}
					}
				}
			}
			else
			{
				active_line->unresolved_label = true;
				return true;
			}
		}
		else
		{
			errors.push_back(msg::err(*active_line, "Refernce of undefined label", "Label \"" + label + "\" was referenced, but not defined"));
			return false;
		}
	}

	// implied
	if (addr == "")
	{
		if (active_line->parsed_op.imp == ABSENT)
		{
			errors.push_back(msg::err(*active_line, "Missing addressing mode", "Operator \"" + op + "\" does not have implied addressing mode"));
			return false;
		}
		active_line->byte_size = 1;
		active_line->bytes[0] = active_line->parsed_op.imp;
		active_line->parsed = true;
		return true;
	}

	// immediate
	if (std::regex_match(addr, addr_match, mask::IM))
	{
		if (active_line->parsed_op.im == ABSENT)
		{
			errors.push_back(msg::err(*active_line, "Missing addressing mode", "Operator \"" + op + "\" does not have immediate addressing mode"));
			return false;
		}
		parsed_addr = parse_number(addr_match.str(1));
		active_line->byte_size = 2;
		active_line->bytes[0] = active_line->parsed_op.im;
		active_line->bytes[1] = convert_to_byte(parsed_addr);
		active_line->parsed = true;
		return true;
	}

	// absolute, zero page or relative
	if (std::regex_match(addr, addr_match, mask::ABS))
	{
		parsed_addr = parse_number(addr_match.str(1));

		if (active_line->parsed_op.zp != ABSENT && parsed_addr <= 0xFF)
		{
			active_line->byte_size = 2;
			active_line->bytes[0] = active_line->parsed_op.zp;
			active_line->bytes[1] = convert_to_byte(parsed_addr);
			active_line->parsed = true;
			return true;
		}
		if (active_line->parsed_op.abs != ABSENT)
		{
			active_line->byte_size = 3;
			active_line->bytes[0] = active_line->parsed_op.abs;
			active_line->bytes[1] = parsed_addr & 0xFF;
			active_line->bytes[2] = parsed_addr >> BIT_SIZE;
			active_line->parsed = true;
			return true;
		}
		if (active_line->parsed_op.zp != ABSENT)
		{
			active_line->byte_size = 2;
			active_line->bytes[0] = active_line->parsed_op.zp;
			active_line->bytes[1] = convert_to_byte(parsed_addr);
			active_line->parsed = true;
			return true;
		}

		errors.push_back(msg::err(*active_line, "Missing addressing mode", "Operator \"" + op + "\" does not have absolute, zero page or relative addressing modes"));
		return false;
	}

	// absolute, x or zero page, x
	if (std::regex_match(addr, addr_match, mask::ABS_X))
	{
		parsed_addr = parse_number(addr_match.str(1));

		if (active_line->parsed_op.zp_x != ABSENT && parsed_addr <= 0xFF)
		{
			active_line->byte_size = 2;
			active_line->bytes[0] = active_line->parsed_op.zp_x;
			active_line->bytes[1] = convert_to_byte(parsed_addr);
			active_line->parsed = true;
			return true;
		}
		if (active_line->parsed_op.abs_x != ABSENT)
		{
			active_line->byte_size = 3;
			active_line->bytes[0] = active_line->parsed_op.abs_x;
			active_line->bytes[1] = parsed_addr & 0xFF;
			active_line->bytes[2] = parsed_addr >> BIT_SIZE;
			active_line->parsed = true;
			return true;
		}
		if (active_line->parsed_op.zp_x != ABSENT)
		{
			active_line->byte_size = 2;
			active_line->bytes[0] = active_line->parsed_op.zp_x;
			active_line->bytes[1] = convert_to_byte(parsed_addr);
			active_line->parsed = true;
			return true;
		}

		errors.push_back(msg::err(*active_line, "Missing addressing mode", "Operator \"" + op + "\" does not have (absolute, x) or (zero page, x) addressing modes"));
		return false;
	}

	// absolute, y or zero page, y
	if (std::regex_match(addr, addr_match, mask::ABS_Y))
	{
		parsed_addr = parse_number(addr_match.str(1));

		if (active_line->parsed_op.zp_y != ABSENT && parsed_addr <= 0xFF)
		{
			active_line->byte_size = 2;
			active_line->bytes[0] = active_line->parsed_op.zp_y;
			active_line->bytes[1] = convert_to_byte(parsed_addr);
			active_line->parsed = true;
			return true;
		}
		if (active_line->parsed_op.abs_y != ABSENT)
		{
			active_line->byte_size = 3;
			active_line->bytes[0] = active_line->parsed_op.abs_y;
			active_line->bytes[1] = parsed_addr & 0xFF;
			active_line->bytes[2] = parsed_addr >> BIT_SIZE;
			active_line->parsed = true;
			return true;
		}
		if (active_line->parsed_op.zp_y != ABSENT)
		{
			active_line->byte_size = 2;
			active_line->bytes[0] = active_line->parsed_op.zp_y;
			active_line->bytes[1] = convert_to_byte(parsed_addr);
			active_line->parsed = true;
			return true;
		}

		errors.push_back(msg::err(*active_line, "Missing addressing mode", "Operator \"" + op + "\" does not have (absolute, y) or (zero page, y) addressing modes"));
		return false;
	}

	// indirect, x
	if (std::regex_match(addr, addr_match, mask::IN_X))
	{
		if (active_line->parsed_op.in_x == ABSENT)
		{
			errors.push_back(msg::err(*active_line, "Missing addressing mode", "Operator \"" + op + "\" does not have (indirect, x) addressing mode"));
			return false;
		}
		parsed_addr = parse_number(addr_match.str(1));
		active_line->byte_size = 2;
		active_line->bytes[0] = active_line->parsed_op.in_x;
		active_line->bytes[1] = convert_to_byte(parsed_addr);
		active_line->parsed = true;
		return true;
	}

	// indirect, y
	if (std::regex_match(addr, addr_match, mask::IN_Y))
	{
		if (active_line->parsed_op.in_y == ABSENT)
		{
			errors.push_back(msg::err(*active_line, "Missing addressing mode", "Operator \"" + op + "\" does not have (indirect, y) addressing mode"));
			return false;
		}
		parsed_addr = parse_number(addr_match.str(1));
		active_line->byte_size = 2;
		active_line->bytes[0] = active_line->parsed_op.in_y;
		active_line->bytes[1] = convert_to_byte(parsed_addr);
		active_line->parsed = true;
		return true;
	}

	errors.push_back(msg::err(*active_line, "Unrecognizable addressing mode", "Could not parse \"" + addr + "\" as valid addressing mode"));
	return false;
}