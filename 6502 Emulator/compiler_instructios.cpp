#include "compiler_6502.h"

std::unordered_map<std::string, op_modes> compiler::op_map =
{
	{
		"LDA",				// name
		{
			ABSENT_OP,			// implied
			op::LDA_IM,		// immediate
			op::LDA_ZP,		// zero page or relative
			op::LDA_ZP_X,	// zero page, x
			ABSENT_OP,			// zero page, y
			op::LDA_ABS,	// absolute
			op::LDA_ABS_X,	// absolute, x
			op::LDA_ABS_Y,	// absolute, y
			op::LDA_IN_X,	// indirect, x
			op::LDA_IN_Y	// indirect, y
		}
	},
	{
		"STA",				// name
		{
			ABSENT_OP,			// implied
			ABSENT_OP,			// immediate
			op::STA_ZP,		// zero page or relative
			op::STA_ZP_X,	// zero page, x
			ABSENT_OP,			// zero page, y
			op::STA_ABS,	// absolute
			op::STA_ABS_X,	// absolute, x
			op::STA_ABS_Y,	// absolute, y
			op::STA_IN_X,	// indirect, x
			op::STA_IN_Y	// indirect, y
		}
	},
	{
		"LDX",				// name
		{
			ABSENT_OP,			// implied
			op::LDX_IM,		// immediate
			op::LDX_ZP,		// zero page or relative
			ABSENT_OP,			// zero page, x
			op::LDX_ZP_Y,	// zero page, y
			op::LDX_ABS,	// absolute
			ABSENT_OP,			// absolute, x
			op::LDX_ABS_Y,	// absolute, y
			ABSENT_OP,			// indirect, x
			ABSENT_OP			// indirect, y
		}
	},
	{
		"STX",				// name
		{
			ABSENT_OP,			// implied
			ABSENT_OP,			// immediate
			op::STX_ZP,		// zero page or relative
			ABSENT_OP,			// zero page, x
			op::STX_ZP_Y,	// zero page, y
			op::STX_ABS,	// absolute
			ABSENT_OP,			// absolute, x
			ABSENT_OP,			// absolute, y
			ABSENT_OP,			// indirect, x
			ABSENT_OP			// indirect, y
		}
	},
	{
		"LDY",				// name
		{
			ABSENT_OP,			// implied
			op::LDY_IM,		// immediate
			op::LDY_ZP,		// zero page or relative
			op::LDY_ZP_X,	// zero page, x
			ABSENT_OP,			// zero page, y
			op::LDY_ABS,	// absolute
			op::LDY_ABS_X,	// absolute, x
			ABSENT_OP,			// absolute, y
			ABSENT_OP,			// indirect, x
			ABSENT_OP			// indirect, y
		}
	},
	{
		"STY",				// name
		{
			ABSENT_OP,			// implied
			ABSENT_OP,			// immediate
			op::STY_ZP,		// zero page or relative
			op::STY_ZP_X,	// zero page, x
			ABSENT_OP,			// zero page, y
			op::STY_ABS,	// absolute
			ABSENT_OP,			// absolute, x
			ABSENT_OP,			// absolute, y
			ABSENT_OP,			// indirect, x
			ABSENT_OP			// indirect, y
		}
	},
	{
		"TAX",				// name
		{
			op::TAX,		// implied
			ABSENT_OP,			// immediate
			ABSENT_OP,			// zero page or relative
			ABSENT_OP,			// zero page, x
			ABSENT_OP,			// zero page, y
			ABSENT_OP,			// absolute
			ABSENT_OP,			// absolute, x
			ABSENT_OP,			// absolute, y
			ABSENT_OP,			// indirect, x
			ABSENT_OP 			// indirect, y
		}
	},
	{
		"TAY",				// name
		{
			op::TAY,		// implied
			ABSENT_OP,			// immediate
			ABSENT_OP,			// zero page or relative
			ABSENT_OP,			// zero page, x
			ABSENT_OP,			// zero page, y
			ABSENT_OP,			// absolute
			ABSENT_OP,			// absolute, x
			ABSENT_OP,			// absolute, y
			ABSENT_OP,			// indirect, x
			ABSENT_OP			// indirect, y
		}
	},
	{
		"TXA",				// name
		{
			op::TXA,		// implied
			ABSENT_OP,			// immediate
			ABSENT_OP,			// zero page or relative
			ABSENT_OP,			// zero page, x
			ABSENT_OP,			// zero page, y
			ABSENT_OP,			// absolute
			ABSENT_OP,			// absolute, x
			ABSENT_OP,			// absolute, y
			ABSENT_OP,			// indirect, x
			ABSENT_OP			// indirect, y
		}
	},
	{
		"TYA",				// name
		{
			op::TYA,		// implied
			ABSENT_OP,			// immediate
			ABSENT_OP,			// zero page or relative
			ABSENT_OP,			// zero page, x
			ABSENT_OP,			// zero page, y
			ABSENT_OP,			// absolute
			ABSENT_OP,			// absolute, x
			ABSENT_OP,			// absolute, y
			ABSENT_OP,			// indirect, x
			ABSENT_OP			// indirect, y
		}
	},
	{
		"TSX",				// name
		{
			op::TSX,		// implied
			ABSENT_OP,			// immediate
			ABSENT_OP,			// zero page or relative
			ABSENT_OP,			// zero page, x
			ABSENT_OP,			// zero page, y
			ABSENT_OP,			// absolute
			ABSENT_OP,			// absolute, x
			ABSENT_OP,			// absolute, y
			ABSENT_OP,			// indirect, x
			ABSENT_OP			// indirect, y
		}
	},
	{
		"TXS",				// name
		{
			op::TXS,		// implied
			ABSENT_OP,			// immediate
			ABSENT_OP,			// zero page or relative
			ABSENT_OP,			// zero page, x
			ABSENT_OP,			// zero page, y
			ABSENT_OP,			// absolute
			ABSENT_OP,			// absolute, x
			ABSENT_OP,			// absolute, y
			ABSENT_OP,			// indirect, x
			ABSENT_OP			// indirect, y
		}
	},
	{
		"PHA",				// name
		{
			op::PHA,		// implied
			ABSENT_OP,			// immediate
			ABSENT_OP,			// zero page or relative
			ABSENT_OP,			// zero page, x
			ABSENT_OP,			// zero page, y
			ABSENT_OP,			// absolute
			ABSENT_OP,			// absolute, x
			ABSENT_OP,			// absolute, y
			ABSENT_OP,			// indirect, x
			ABSENT_OP			// indirect, y
		}
	},
	{
		"PHP",				// name
		{
			op::PHP,		// implied
			ABSENT_OP,			// immediate
			ABSENT_OP,			// zero page or relative
			ABSENT_OP,			// zero page, x
			ABSENT_OP,			// zero page, y
			ABSENT_OP,			// absolute
			ABSENT_OP,			// absolute, x
			ABSENT_OP,			// absolute, y
			ABSENT_OP,			// indirect, x
			ABSENT_OP			// indirect, y
		}
	},
	{
		"PLA",				// name
		{
			op::PLA,		// implied
			ABSENT_OP,			// immediate
			ABSENT_OP,			// zero page or relative
			ABSENT_OP,			// zero page, x
			ABSENT_OP,			// zero page, y
			ABSENT_OP,			// absolute
			ABSENT_OP,			// absolute, x
			ABSENT_OP,			// absolute, y
			ABSENT_OP,			// indirect, x
			ABSENT_OP			// indirect, y
		}
	},
	{
		"PLP",				// name
		{
			op::PLP,		// implied
			ABSENT_OP,			// immediate
			ABSENT_OP,			// zero page or relative
			ABSENT_OP,			// zero page, x
			ABSENT_OP,			// zero page, y
			ABSENT_OP,			// absolute
			ABSENT_OP,			// absolute, x
			ABSENT_OP,			// absolute, y
			ABSENT_OP,			// indirect, x
			ABSENT_OP			// indirect, y
		}
	},
	{
		"AND",				// name
		{
			ABSENT_OP,			// implied
			op::AND_IM,		// immediate
			op::AND_ZP,		// zero page or relative
			op::AND_ZP_X,	// zero page, x
			ABSENT_OP,			// zero page, y
			op::AND_ABS,	// absolute
			op::AND_ABS_X,	// absolute, x
			op::AND_ABS_Y,	// absolute, y
			op::AND_IN_X,	// indirect, x
			op::AND_IN_Y	// indirect, y
		}
	},
	{
		"EOR",				// name
		{
			ABSENT_OP,			// implied
			op::EOR_IM,		// immediate
			op::EOR_ZP,		// zero page or relative
			op::EOR_ZP_X,	// zero page, x
			ABSENT_OP,			// zero page, y
			op::EOR_ABS,	// absolute
			op::EOR_ABS_X,	// absolute, x
			op::EOR_ABS_Y,	// absolute, y
			op::EOR_IN_X,	// indirect, x
			op::EOR_IN_Y	// indirect, y
		}
	},
	{
		"ORA",				// name
		{
			ABSENT_OP,			// implied
			op::ORA_IM,		// immediate
			op::ORA_ZP,		// zero page or relative
			op::ORA_ZP_X,	// zero page, x
			ABSENT_OP,			// zero page, y
			op::ORA_ABS,	// absolute
			op::ORA_ABS_X,	// absolute, x
			op::ORA_ABS_Y,	// absolute, y
			op::ORA_IN_X,	// indirect, x
			op::ORA_IN_Y	// indirect, y
		}
	},
	{
		"BIT",				// name
		{
			ABSENT_OP,			// implied
			ABSENT_OP,			// immediate
			op::BIT_ZP,		// zero page or relative
			ABSENT_OP,			// zero page, x
			ABSENT_OP,			// zero page, y
			op::BIT_ABS,	// absolute
			ABSENT_OP,			// absolute, x
			ABSENT_OP,			// absolute, y
			ABSENT_OP,			// indirect, x
			ABSENT_OP			// indirect, y
		}
	},
	{
		"ADC",				// name
		{
			ABSENT_OP,			// implied
			op::ADC_IM,		// immediate
			op::ADC_ZP,		// zero page or relative
			op::ADC_ZP_X,	// zero page, x
			ABSENT_OP,			// zero page, y
			op::ADC_ABS,	// absolute
			op::ADC_ABS_X,	// absolute, x
			op::ADC_ABS_Y,	// absolute, y
			op::ADC_IN_X,	// indirect, x
			op::ADC_IN_Y	// indirect, y
		}
	},
	{
		"SBC",				// name
		{
			ABSENT_OP,			// implied
			op::SBC_IM,		// immediate
			op::SBC_ZP,		// zero page or relative
			op::SBC_ZP_X,	// zero page, x
			ABSENT_OP,			// zero page, y
			op::SBC_ABS,	// absolute
			op::SBC_ABS_X,	// absolute, x
			op::SBC_ABS_Y,	// absolute, y
			op::SBC_IN_X,	// indirect, x
			op::SBC_IN_Y	// indirect, y
		}
	},
	{
		"CMP",				// name
		{
			ABSENT_OP,			// implied
			op::CMP_IM,		// immediate
			op::CMP_ZP,		// zero page or relative
			op::CMP_ZP_X,	// zero page, x
			ABSENT_OP,			// zero page, y
			op::CMP_ABS,	// absolute
			op::CMP_ABS_X,	// absolute, x
			op::CMP_ABS_Y,	// absolute, y
			op::CMP_IN_X,	// indirect, x
			op::CMP_IN_Y	// indirect, y
		}
	},
	{
		"CPX",				// name
		{
			ABSENT_OP,			// implied
			op::CPX_IM,		// immediate
			op::CPX_ZP,		// zero page or relative
			ABSENT_OP,			// zero page, x
			ABSENT_OP,			// zero page, y
			op::CPX_ABS,	// absolute
			ABSENT_OP,			// absolute, x
			ABSENT_OP,			// absolute, y
			ABSENT_OP,			// indirect, x
			ABSENT_OP			// indirect, y
		}
	},
	{
		"CPY",				// name
		{
			ABSENT_OP,			// implied
			op::CPY_IM,		// immediate
			op::CPY_ZP,		// zero page or relative
			ABSENT_OP,			// zero page, x
			ABSENT_OP,			// zero page, y
			op::CPY_ABS,	// absolute
			ABSENT_OP,			// absolute, x
			ABSENT_OP,			// absolute, y
			ABSENT_OP,			// indirect, x
			ABSENT_OP			// indirect, y
		}
	},
	{
		"INC",				// name
		{
			ABSENT_OP,			// implied
			ABSENT_OP,			// immediate
			op::INC_ZP,		// zero page or relative
			op::INC_ZP_X,	// zero page, x
			ABSENT_OP,			// zero page, y
			op::INC_ABS,	// absolute
			op::INC_ABS_X,	// absolute, x
			ABSENT_OP,			// absolute, y
			ABSENT_OP,			// indirect, x
			ABSENT_OP			// indirect, y
		}
	},
	{
		"INX",				// name
		{
			op::INX,		// implied
			ABSENT_OP,			// immediate
			ABSENT_OP,			// zero page or relative
			ABSENT_OP,			// zero page, x
			ABSENT_OP,			// zero page, y
			ABSENT_OP,			// absolute
			ABSENT_OP,			// absolute, x
			ABSENT_OP,			// absolute, y
			ABSENT_OP,			// indirect, x
			ABSENT_OP			// indirect, y
		}
	},
	{
		"INY",				// name
		{
			op::INY,		// implied
			ABSENT_OP,			// immediate
			ABSENT_OP,			// zero page or relative
			ABSENT_OP,			// zero page, x
			ABSENT_OP,			// zero page, y
			ABSENT_OP,			// absolute
			ABSENT_OP,			// absolute, x
			ABSENT_OP,			// absolute, y
			ABSENT_OP,			// indirect, x
			ABSENT_OP			// indirect, y
		}
	},
	{
		"DEC",				// name
		{
			ABSENT_OP,			// implied
			ABSENT_OP,			// immediate
			op::DEC_ZP,		// zero page or relative
			op::DEC_ZP_X,	// zero page, x
			ABSENT_OP,			// zero page, y
			op::DEC_ABS,	// absolute
			op::DEC_ABS_X,	// absolute, x
			ABSENT_OP,			// absolute, y
			ABSENT_OP,			// indirect, x
			ABSENT_OP			// indirect, y
		}
	},
	{
		"DEX",				// name
		{
			op::DEX,		// implied
			ABSENT_OP,			// immediate
			ABSENT_OP,			// zero page or relative
			ABSENT_OP,			// zero page, x
			ABSENT_OP,			// zero page, y
			ABSENT_OP,			// absolute
			ABSENT_OP,			// absolute, x
			ABSENT_OP,			// absolute, y
			ABSENT_OP,			// indirect, x
			ABSENT_OP			// indirect, y
		}
	},
	{
		"DEY",				// name
		{
			op::DEY,		// implied
			ABSENT_OP,			// immediate
			ABSENT_OP,			// zero page or relative
			ABSENT_OP,			// zero page, x
			ABSENT_OP,			// zero page, y
			ABSENT_OP,			// absolute
			ABSENT_OP,			// absolute, x
			ABSENT_OP,			// absolute, y
			ABSENT_OP,			// indirect, x
			ABSENT_OP			// indirect, y
		}
	},
	{
		"ASL",				// name
		{
			op::ASL_A,		// implied
			ABSENT_OP,			// immediate
			op::ASL_ZP,		// zero page or relative
			op::ASL_ZP_X,	// zero page, x
			ABSENT_OP,			// zero page, y
			op::ASL_ABS,	// absolute
			op::ASL_ABS_X,	// absolute, x
			ABSENT_OP,			// absolute, y
			ABSENT_OP,			// indirect, x
			ABSENT_OP			// indirect, y
		}
	},
	{
		"LSR",				// name
		{
			op::LSR_A,		// implied
			ABSENT_OP,			// immediate
			op::LSR_ZP,		// zero page or relative
			op::LSR_ZP_X,	// zero page, x
			ABSENT_OP,			// zero page, y
			op::LSR_ABS,	// absolute
			op::LSR_ABS_X,	// absolute, x
			ABSENT_OP,			// absolute, y
			ABSENT_OP,			// indirect, x
			ABSENT_OP			// indirect, y
		}
	},
	{
		"ROL",				// name
		{
			op::ROL_A,		// implied
			ABSENT_OP,			// immediate
			op::ROL_ZP,		// zero page or relative
			op::ROL_ZP_X,	// zero page, x
			ABSENT_OP,			// zero page, y
			op::ROL_ABS,	// absolute
			op::ROL_ABS_X,	// absolute, x
			ABSENT_OP,			// absolute, y
			ABSENT_OP,			// indirect, x
			ABSENT_OP			// indirect, y
		}
	},
	{
		"ROR",				// name
		{
			op::ROR_A,		// implied
			ABSENT_OP,			// immediate
			op::ROR_ZP,		// zero page or relative
			op::ROR_ZP_X,	// zero page, x
			ABSENT_OP,			// zero page, y
			op::ROR_ABS,	// absolute
			op::ROR_ABS_X,	// absolute, x
			ABSENT_OP,			// absolute, y
			ABSENT_OP,			// indirect, x
			ABSENT_OP			// indirect, y
		}
	},
	{
		"JMP",				// name
		{
			ABSENT_OP,			// implied
			ABSENT_OP,			// immediate
			ABSENT_OP,			// zero page or relative
			ABSENT_OP,			// zero page, x
			ABSENT_OP,			// zero page, y
			op::JMP_ABS,	// absolute
			ABSENT_OP,			// absolute, x
			ABSENT_OP,			// absolute, y
			op::JMP_IN,		// indirect, x
			op::JMP_IN		// indirect, y
		}
	},
	{
		"JSR",				// name
		{
			ABSENT_OP,			// implied
			ABSENT_OP,			// immediate
			ABSENT_OP,			// zero page or relative
			ABSENT_OP,			// zero page, x
			ABSENT_OP,			// zero page, y
			op::JSR_ABS,	// absolute
			ABSENT_OP,			// absolute, x
			ABSENT_OP,			// absolute, y
			ABSENT_OP,			// indirect, x
			ABSENT_OP			// indirect, y
		}
	},
	{
		"RTS",				// name
		{
			op::RTS,		// implied
			ABSENT_OP,			// immediate
			ABSENT_OP,			// zero page or relative
			ABSENT_OP,			// zero page, x
			ABSENT_OP,			// zero page, y
			ABSENT_OP,			// absolute
			ABSENT_OP,			// absolute, x
			ABSENT_OP,			// absolute, y
			ABSENT_OP,			// indirect, x
			ABSENT_OP			// indirect, y
		}
	},
	{
		"BCC",				// name
		{
			op::BCC,		// implied
			ABSENT_OP,			// immediate
			ABSENT_OP,			// zero page or relative
			ABSENT_OP,			// zero page, x
			ABSENT_OP,			// zero page, y
			ABSENT_OP,			// absolute
			ABSENT_OP,			// absolute, x
			ABSENT_OP,			// absolute, y
			ABSENT_OP,			// indirect, x
			ABSENT_OP			// indirect, y
		}
	},
	{
		"BCS",				// name
		{
			op::BCS,		// implied
			ABSENT_OP,			// immediate
			ABSENT_OP,			// zero page or relative
			ABSENT_OP,			// zero page, x
			ABSENT_OP,			// zero page, y
			ABSENT_OP,			// absolute
			ABSENT_OP,			// absolute, x
			ABSENT_OP,			// absolute, y
			ABSENT_OP,			// indirect, x
			ABSENT_OP			// indirect, y
		}
	},
	{
		"BEQ",				// name
		{
			op::BEQ,		// implied
			ABSENT_OP,			// immediate
			ABSENT_OP,			// zero page or relative
			ABSENT_OP,			// zero page, x
			ABSENT_OP,			// zero page, y
			ABSENT_OP,			// absolute
			ABSENT_OP,			// absolute, x
			ABSENT_OP,			// absolute, y
			ABSENT_OP,			// indirect, x
			ABSENT_OP			// indirect, y
		}
	},
	{
		"BMI",				// name
		{
			op::BMI,		// implied
			ABSENT_OP,			// immediate
			ABSENT_OP,			// zero page or relative
			ABSENT_OP,			// zero page, x
			ABSENT_OP,			// zero page, y
			ABSENT_OP,			// absolute
			ABSENT_OP,			// absolute, x
			ABSENT_OP,			// absolute, y
			ABSENT_OP,			// indirect, x
			ABSENT_OP			// indirect, y
		}
	},
	{
		"BNE",				// name
		{
			op::BNE,		// implied
			ABSENT_OP,			// immediate
			ABSENT_OP,			// zero page or relative
			ABSENT_OP,			// zero page, x
			ABSENT_OP,			// zero page, y
			ABSENT_OP,			// absolute
			ABSENT_OP,			// absolute, x
			ABSENT_OP,			// absolute, y
			ABSENT_OP,			// indirect, x
			ABSENT_OP			// indirect, y
		}
	},
	{
		"BPL",				// name
		{
			op::BPL,		// implied
			ABSENT_OP,			// immediate
			ABSENT_OP,			// zero page or relative
			ABSENT_OP,			// zero page, x
			ABSENT_OP,			// zero page, y
			ABSENT_OP,			// absolute
			ABSENT_OP,			// absolute, x
			ABSENT_OP,			// absolute, y
			ABSENT_OP,			// indirect, x
			ABSENT_OP			// indirect, y
		}
	},
	{
		"BVC",				// name
		{
			op::BVC,		// implied
			ABSENT_OP,			// immediate
			ABSENT_OP,			// zero page or relative
			ABSENT_OP,			// zero page, x
			ABSENT_OP,			// zero page, y
			ABSENT_OP,			// absolute
			ABSENT_OP,			// absolute, x
			ABSENT_OP,			// absolute, y
			ABSENT_OP,			// indirect, x
			ABSENT_OP			// indirect, y
		}
	},
	{
		"BVS",				// name
		{
			op::BVS,		// implied
			ABSENT_OP,			// immediate
			ABSENT_OP,			// zero page or relative
			ABSENT_OP,			// zero page, x
			ABSENT_OP,			// zero page, y
			ABSENT_OP,			// absolute
			ABSENT_OP,			// absolute, x
			ABSENT_OP,			// absolute, y
			ABSENT_OP,			// indirect, x
			ABSENT_OP			// indirect, y
		}
	},
	{
		"CLC",				// name
		{
			op::CLC,		// implied
			ABSENT_OP,			// immediate
			ABSENT_OP,			// zero page or relative
			ABSENT_OP,			// zero page, x
			ABSENT_OP,			// zero page, y
			ABSENT_OP,			// absolute
			ABSENT_OP,			// absolute, x
			ABSENT_OP,			// absolute, y
			ABSENT_OP,			// indirect, x
			ABSENT_OP			// indirect, y
		}
	},
	{
		"CLI",				// name
		{
			op::CLI,		// implied
			ABSENT_OP,			// immediate
			ABSENT_OP,			// zero page or relative
			ABSENT_OP,			// zero page, x
			ABSENT_OP,			// zero page, y
			ABSENT_OP,			// absolute
			ABSENT_OP,			// absolute, x
			ABSENT_OP,			// absolute, y
			ABSENT_OP,			// indirect, x
			ABSENT_OP			// indirect, y
		}
	},
	{
		"CLV",				// name
		{
			op::CLV,		// implied
			ABSENT_OP,			// immediate
			ABSENT_OP,			// zero page or relative
			ABSENT_OP,			// zero page, x
			ABSENT_OP,			// zero page, y
			ABSENT_OP,			// absolute
			ABSENT_OP,			// absolute, x
			ABSENT_OP,			// absolute, y
			ABSENT_OP,			// indirect, x
			ABSENT_OP			// indirect, y
		}
	},
	{
		"SEC",				// name
		{
			op::SEC,		// implied
			ABSENT_OP,			// immediate
			ABSENT_OP,			// zero page or relative
			ABSENT_OP,			// zero page, x
			ABSENT_OP,			// zero page, y
			ABSENT_OP,			// absolute
			ABSENT_OP,			// absolute, x
			ABSENT_OP,			// absolute, y
			ABSENT_OP,			// indirect, x
			ABSENT_OP			// indirect, y
		}
	},
	{
		"SEI",				// name
		{
			op::SEI,		// implied
			ABSENT_OP,			// immediate
			ABSENT_OP,			// zero page or relative
			ABSENT_OP,			// zero page, x
			ABSENT_OP,			// zero page, y
			ABSENT_OP,			// absolute
			ABSENT_OP,			// absolute, x
			ABSENT_OP,			// absolute, y
			ABSENT_OP,			// indirect, x
			ABSENT_OP			// indirect, y
		}
	},
	{
		"BRK",				// name
		{
			op::BRK,		// implied
			ABSENT_OP,			// immediate
			ABSENT_OP,			// zero page or relative
			ABSENT_OP,			// zero page, x
			ABSENT_OP,			// zero page, y
			ABSENT_OP,			// absolute
			ABSENT_OP,			// absolute, x
			ABSENT_OP,			// absolute, y
			ABSENT_OP,			// indirect, x
			ABSENT_OP			// indirect, y
		}
	},
	{
		"RTI",				// name
		{
			op::RTI,		// implied
			ABSENT_OP,			// immediate
			ABSENT_OP,			// zero page or relative
			ABSENT_OP,			// zero page, x
			ABSENT_OP,			// zero page, y
			ABSENT_OP,			// absolute
			ABSENT_OP,			// absolute, x
			ABSENT_OP,			// absolute, y
			ABSENT_OP,			// indirect, x
			ABSENT_OP			// indirect, y
		}
	},
	{
		"NOP",				// name
		{
			op::NOP,		// implied
			ABSENT_OP,			// immediate
			ABSENT_OP,			// zero page or relative
			ABSENT_OP,			// zero page, x
			ABSENT_OP,			// zero page, y
			ABSENT_OP,			// absolute
			ABSENT_OP,			// absolute, x
			ABSENT_OP,			// absolute, y
			ABSENT_OP,			// indirect, x
			ABSENT_OP			// indirect, y
		}
	},
	{
		"KIL",				// name
		{
			op::KIL,		// implied
			ABSENT_OP,			// immediate
			ABSENT_OP,			// zero page or relative
			ABSENT_OP,			// zero page, x
			ABSENT_OP,			// zero page, y
			ABSENT_OP,			// absolute
			ABSENT_OP,			// absolute, x
			ABSENT_OP,			// absolute, y
			ABSENT_OP,			// indirect, x
			ABSENT_OP			// indirect, y
		}
	}
};