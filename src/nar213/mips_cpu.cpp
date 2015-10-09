/*
 * mips_cpu.cpp
 *
 *  Created on: 17 October 2014
 *      Author: Nick Robertson
 *      CID: NAR213 0842277
 *      47 Commands Available
 */

#include "mips.h"
#include<iostream>
#include<vector>
#include<string>
#include<cstdlib>
#include<fstream>
#include<cmath>
#include<stdio.h>

using namespace std;

struct mips_cpu_impl {
	uint32_t pc, pcN; //PC is the current address and pcN is the next address
	uint32_t regs[34];	//Remember that register 0 is always connected to ground
	mips_mem_h mem; //Wire that connects to the memory
	//mips_cpu_h is the handle that points to a structure of this type
};

//This function creates a space in memory for the CPU
mips_cpu_h mips_cpu_create(mips_mem_h mem) {
	unsigned i;
	mips_cpu_h res = (mips_cpu_h) malloc(sizeof(struct mips_cpu_impl));
	res->mem = mem; //Points to associated space in memory
	res->pc = 0; // PC=0
	res->pcN = 4; //PC+4=4
	for (i = 0; i < 34; i++) {
		res->regs[i] = 0;
	}	//Sets all of the values in the registers to one
	cout << "MEMORY CREATED" << endl;
	return res;
}

mips_error mips_cpu_reset(mips_cpu_h state) {
	unsigned i;
	state->pc = 0;
	state->pcN = 4;
	for (i = 0; i < 34; i++) {
		state->regs[i] = 0;
	}
	return mips_Success;
}

//Simply removes the state of the machine
void mips_cpu_free(mips_cpu_h state) {
	free(state);
	cout << "CPU freed" << endl;
}

//Gets the value of the register and stores it to some 32 bit variable
mips_error mips_cpu_get_register(mips_cpu_h state, //!< Valid (non-empty) handle to a CPU
		unsigned index,		//!< Index from 0 to 31
		uint32_t *value		//!< Where to write the value to
		) {
	if (state == 0)
		return mips_ErrorInvalidHandle; //No state
	if (index >= 34)
		return mips_ErrorInvalidArgument; //Outside range of registers
	if (index == 0) {
		*value = 0; //On any attempt to access the zero register return 0
	} else {
		*value = state->regs[index];
	}
	return mips_Success;
}

//

/*! Modifies one of the 32 general purpose MIPS registers. */
mips_error mips_cpu_set_register(mips_cpu_h state, //!< Valid (non-empty) handle to a CPU
		unsigned index,		//!< Index from 0 to 31
		uint32_t value		//!< New value to write into register file
		) {
	//Error checks
	if (state == 0)
		return mips_ErrorInvalidHandle;
	if (index >= 34)
		return mips_ErrorInvalidArgument;
	//Below checks that register is not 0
	if (index == 0) {
		return mips_Success; //return mips_ErrorFileWriteError;
	}
	if (index != 0) {
		state->regs[index] = value;
	}

	return mips_Success;
}

//

//Something to do with creating a file that keeps track of what happens with the registers
mips_error mips_cpu_set_debug_level(mips_cpu_h state, unsigned level,
		FILE *dest) {
	if (state == 0) {
		return mips_ErrorInvalidHandle;
	}
	if (level >= 1) {
		cout << "State of the Machine" << endl;
		for (int i = 0; i < 34; i++) {
			if (state->regs[i] != 0) {
				cout << std::dec << "Register " << i << " = " << std::hex
						<< state->regs[i] << endl;
			}
		}
		cout << "PC = " << std::dec << state->pc << endl;
		cout << "PCN = " << std::dec << state->pcN << endl;
		if (level == 1) {
			cout << "" << endl;
		}
	}
	if (level >= 2) {
		uint32_t meminstr = 1;
		uint32_t addr = 0;
		for (addr = 0; /*meminstr != NULL &&*/addr < ((1 << 20) - 4);
				addr = addr + 4) {
			mips_mem_read(state->mem, addr, 4, (uint8_t*) &meminstr);
			if (meminstr != 0) {
				cout << "Memory Location " << std::dec << int(addr) << " = "
						<< std::hex << int(meminstr) << endl;
			}
		}
		cout << "" << endl;
	}

	return mips_Success;
}

//LEVEL 1 is registers and PC, LEVEL 2 is memory

//

//

//Changes the value of the PC to the required value
mips_error mips_cpu_set_pc(mips_cpu_h state, //!< Valid (non-empty) handle to a CPU
		uint32_t pc			//!< Address of the next instruction to exectute.
		) {
	if (state == 0) {
		return mips_ErrorInvalidHandle;
	}
	state->pc = pc;
	state->pcN = pc + 4;
	return mips_Success;
}

//

mips_error mips_cpu_get_pc(mips_cpu_h state, uint32_t *pc) {
	if (state == 0)
		return mips_ErrorInvalidHandle;
	*pc = state->pc;
	return mips_Success;
}

uint32_t endianconverter(uint32_t num);

uint32_t endianconverter(uint32_t num) {
	uint32_t byte1, byte2, byte3, byte4;
	byte1 = (num) & 0xff;
	byte2 = (num >> 8) & 0xFF;
	byte3 = (num >> 16) & 0xFF;
	byte4 = (num >> 24) & 0xFF;
	num = (byte1 << 24) + (byte2 << 16) + (byte3 << 8) + (byte4);
	return num;
}

//

mips_error mips_cpu_step(mips_cpu_h state) {

	mips_error err = mips_Success;
	mips_error errinc = mips_Success; //Allows function to be increment PC without overwriting errors

	if (state == 0) {
		err = mips_ErrorInvalidHandle;
	}
	uint8_t opcode = 0;
	uint32_t meminstr = 0;
	bool branchdelay = 0;

	if (err == 0) {
		err = mips_mem_read(state->mem, state->pc, 4, (uint8_t*) &meminstr); //Reads instruction from memory
	}
	meminstr = endianconverter(meminstr); //Endian conversion to preserve existing code
	opcode = (meminstr >> 26) & 0x3F; //Extracts the opcode from the number

	//6 bits opcode, 5 bits source 1, 5 bits source 2, 5 bits destination, 5 bits shift, 6 bits functions
	if (opcode == 0) {
		cout << "R-TYPE IDENTIFIED" << endl;
		uint8_t source1, source2, destination, shift, function;
		source1 = (meminstr >> 21) & 0x1F;
		source2 = (meminstr >> 16) & 0x1F;
		destination = (meminstr >> 11) & 0x1F;
		shift = (meminstr >> 6) & 0x1F;
		function = meminstr & 0x3F;
		uint32_t s1, s2, d1, d2;
		int32_t ss1, ss2, sd1, sd2;

		if (err == 0) {
			err = mips_cpu_get_register(state, source1, &s1); //Collect the unsigned value from Source 1}
		}
		if (err == 0) {
			err = mips_cpu_get_register(state, source2, &s2); //Collect the unsigned value from Source 2
		}
		if (err == 0) {
			err = mips_cpu_get_register(state, source1, (uint32_t*) &ss1); //Collect the signed value from Source 1
		}
		if (err == 0) {
			err = mips_cpu_get_register(state, source2, (uint32_t*) &ss2); // Collect the signed value from Source 2
		}

		//err = mips_cpu_set_debug_level(state, 1, NULL); //Writes out the basic information about what is stored in the registers at this point in time, written as unsigned

		if (function == 0x24) {
			//XXX AND
			d1 = s1 & s2; //Unsigned and
			if (err == 0) {
				err = mips_cpu_set_register(state, destination, d1); //Register is changed to resultant bits
			}
			cout << "AND EXECUTED\n" << endl;

			//

		} else if (function == 0b100101) {
			//XXX OR
			d1 = s1 | s2; //Unsigned or
			if (err == 0) {
				err = mips_cpu_set_register(state, destination, d1); //Register set
			}
			cout << "OR EXECUTED \n" << endl;

			//

		} else if (function == 0x26) {
			//XXX XOR
			d1 = s1 xor s2; //Unsigned xor
			if (err == 0) {
				err = mips_cpu_set_register(state, destination, d1); //Register set
			}
			cout << "XOR EXECUTED \n" << endl;

			//

		} else if (function == 0x21) {
			//XXX ADDU
			d1 = s1 + s2; //Unsigned Addition
			//Overflow not cared about
			if (err == 0) {
				err = mips_cpu_set_register(state, destination, d1); //Set register
			}
			cout << "ADDU EXECUTED \n" << endl;

			//

		} else if (function == 0b100011) {
			//SUBU XXX
			d1 = s1 - s2; //Unsigned subtraction
			//No overflow

			if (err == 0) {
				err = mips_cpu_set_register(state, destination, d1); // Set register
			}
			cout << "SUBU EXECUTED \n" << endl;

			//

		} else if (function == 0b000000) {
			//SLL XXX
			d1 = s2 << shift;
			//No errors or exceptions to consider
			if (err == 0) {
				err = mips_cpu_set_register(state, destination, d1); // Set register
			}
			cout << "SLL EXECUTED \n" << endl;

			//

		} else if (function == 0b100000) {
			//XXX ADD
			sd1 = ss1 + ss2; //Signed Addition
			//Either negative overflow or positive overflow conditions considered using signed and unsigned examinations
			if (((ss1 < 0 && ss2 < 0) && (ss1 + ss2) >= 0)
					|| ((ss1 > 0 && ss2 > 0) && (ss1 + ss2) <= 0)) {
				err = mips_ExceptionArithmeticOverflow;

			}
			if (err == 0) {
				err = mips_cpu_set_register(state, destination, sd1); //Result written to register
			}
			cout << "ADD EXECUTED \n" << endl;

			//

		} else if (function == 0b011010) {
			//DIV XXX
			if (s2 == 0) {
				err = mips_ErrorInvalidArgument; //Theoretically should not be an error but C doesn't know how to handle this
			}
			if (err == 0) {
				sd1 = ss1 % ss2; //Remainder
				sd2 = (ss1 - sd1) / ss2; //Quotient
			}
			if (err == 0) {
				err = mips_cpu_set_register(state, 33, sd1);
			}
			if (err == 0) {
				err = mips_cpu_set_register(state, 32, sd2);
			}
			cout << "DIV EXECUTED \n" << endl;

			//

		} else if (function == 0b011011) {
			//DIVU XXX
			if (s2 == 0) {
				return mips_ErrorInvalidArgument;
			}
			if (err == 0) {
				d1 = s1 % s2; //Remainder
				d2 = (s1 - d1) / s2; //Quotient
			}
			if (err == 0) {
				err = mips_cpu_set_register(state, 33, d1);
			}
			if (err == 0) {
				err = mips_cpu_set_register(state, 32, d2);
			}
			cout << "DIVU EXECUTED \n" << endl;

			//

		} else if (function == 0b001000) {
			//JR XXX
			if ((s1 & 0b11) != 0) {
				err = mips_ExceptionInvalidAddress;
			}
			if (err == 0) {
				state->pc = state->pcN;
				state->pcN = s1;
				branchdelay = 1;
			}
			cout << "JR EXECUTED \n" << endl;

			//

		} else if (function == 0b010000) {
			//MFHI XXX

			/*
			 * 	uint8_t tempopcode, tempfunction;
			 uint32_t tempmeminstr;
			 WOULD STOP THE MFHI FROM RETURNING ANYTHING IN THE CASE THE FOLLOWING TWO INSTRUCTIONS ARE DIV OR MULTS
			 if (err == 0) {
			 err = mips_mem_read(state->mem, state->pcN, 4,
			 (uint8_t*) &tempmeminstr); //Reads instruction from memory
			 }
			 if (err != 0) {
			 return mips_ErrorFileReadError; //Error reading the instruction
			 }
			 tempmeminstr = endianconverter(tempmeminstr); //Endian conversion to preserve existing code
			 tempopcode = (tempmeminstr >> 26) & 0x3F; //Extracts the opcode from the number
			 tempfunction = tempmeminstr & 0x3F;
			 if ((tempopcode == 0)
			 && ((tempfunction == 0b011001) || (tempfunction == 0b011000)
			 || (tempfunction == 0b011011)
			 || (tempfunction == 0b011010))){MFHI WOULD BE UNDEFINED}

			 I recognise that is should be undefined but there is no error code supporting this
			 Also it should be noted that the sequence above would have to be repeated for state->pcN +4;
			 */

			if (err == 0) {
				err = mips_cpu_get_register(state, 33, &d1);
			}
			if (err == 0) {
				err = mips_cpu_set_register(state, destination, d1);
			}
			cout << "MFHI EXECUTED \n" << endl;

			//

		} else if (function == 0b010010) {
			//MFLO XXX

			/*
			 WOULD STOP THE MFLO FROM RETURNING ANYTHING IN THE CASE THE FOLLOWING TWO INSTRUCTIONS ARE DIV OR MULTS
			 if (err == 0) {
			 err = mips_mem_read(state->mem, state->pcN, 4,
			 (uint8_t*) &tempmeminstr); //Reads instruction from memory
			 }
			 if (err != 0) {
			 return mips_ErrorFileReadError; //Error reading the instruction
			 }
			 tempmeminstr = endianconverter(tempmeminstr); //Endian conversion to preserve existing code
			 tempopcode = (tempmeminstr >> 26) & 0x3F; //Extracts the opcode from the number
			 tempfunction = tempmeminstr & 0x3F;
			 if ((tempopcode == 0)
			 && ((tempfunction == 0b011001) || (tempfunction == 0b011000)
			 || (tempfunction == 0b011011)
			 || (tempfunction == 0b011010))){MFHI WOULD BE UNDEFINED}

			 I recognise that is should be undefined but there is no error code supporting this
			 Also it should be noted that the sequence above would have to be repeated for state->pcN +4;
			 */
			if (err == 0) {
				err = mips_cpu_get_register(state, 32, &d1);
			}
			if (err == 0) {
				err = mips_cpu_set_register(state, destination, d1);
			}

			cout << "MFLO EXECUTED \n" << endl;

			//

		} else if (function == 0b000100) {
			//SLLV XXX
			d1 = s2 << (s1 & 0x0000001F);
			//Perhaps consider error occurring if shift is around 32
			if (err == 0) {
				err = mips_cpu_set_register(state, destination, d1);
			}
			cout << "SLLV EXECUTED \n" << endl;

			//
		} else if (function == 0b000011) {
			//SRA XXX
			sd1 = (ss2 >> shift);
			if (err == 0) {
				err = mips_cpu_set_register(state, destination, sd1);
			}

			cout << "SRA EXECUTED \n" << endl;

			//

		} else if (function == 0b000010) {
			//SRL XXX
			d1 = s2 >> shift;
			if (err == 0) {
				err = mips_cpu_set_register(state, destination, d1);
			}
			cout << "SRL EXECUTED \n" << endl;

			//

		} else if (function == 0b000110) {
			//SRLV XXX
			d1 = s2 >> s1;
			//Perhaps consider error occurring if shift is around 32
			if (err == 0) {
				err = mips_cpu_set_register(state, destination, d1);
			}
			cout << "SRLV EXECUTED \n" << endl;

			//

		} else if (function == 0b100010) {
			//SUB XXX
			sd1 = ss1 - ss2;
			if (((ss1 < 0 && ss2 > 0) && (ss1 - ss2) > 0)
					|| ((ss1 >= 0 && ss2 < 0) && (ss1 - ss2) <= 0)) {
				err = mips_ExceptionArithmeticOverflow;
			}
			if (err == 0) {
				err = mips_cpu_set_register(state, destination, sd1);
			}
			cout << "SUB EXECUTED \n" << endl;

			//

		} else if (function == 0b011000) {
			//MULT XXX
			int64_t multi, bs1, bs2;
			bs1 = ss1;
			bs2 = ss2;
			multi = bs1 * bs2;
			sd1 = ((multi >> 32) & 0xFFFFFFFF);
			sd2 = (multi & 0xFFFFFFFF);
			if (err == 0) {
				err = mips_cpu_set_register(state, 33, sd1);
			}
			if (err == 0) {
				err = mips_cpu_set_register(state, 32, sd2);
			}

			cout << "MULT EXECUTED \n" << endl;

			//

		} else if (function == 0b011001) {
			//MULTU XXX
			uint64_t multi, bs1, bs2;
			bs1 = s1;
			bs2 = s2;
			multi = bs1 * bs2;
			d1 = ((multi >> 32) & 0xFFFFFFFF);
			d2 = (multi & 0xFFFFFFFF);
			if (err == 0) {
				err = mips_cpu_set_register(state, 33, d1);
			}
			if (err == 0) {
				err = mips_cpu_set_register(state, 32, d2);
			}
			cout << "MULTU EXECUTED \n" << endl;

			//

		} else if (function == 0b101010) {
			//XXX SLT   |  Set on less than (signed)                | 2 I
			if (ss1 < ss2) {
				if (err == 0) {
					err = mips_cpu_set_register(state, destination, 1);
				}
			} else if (err == 0) {
				err = mips_cpu_set_register(state, destination, 0);
			}
			cout << "SLT EXECUTED \n" << endl;

			//

		} else if (function == 0b101011) {
			//XXX SLTU  |  Set on less than unsigned                | 1 I
			if (s1 < s2) {
				if (err == 0) {
					err = mips_cpu_set_register(state, destination, 1);
				}
			} else if (err == 0) {
				err = mips_cpu_set_register(state, destination, 0);
			}
			cout << "SLTU EXECUTED \n" << endl;

		} else {
			//err = mips_ExceptionInvalidInstruction;
		}
	} else if (opcode == 0b000010 || opcode == 0b000011) {
		//J XXX
		// 6 bits opcode, 26 bits memory location

		cout << "J-TYPE IDENTIFIED" << endl;
		uint32_t jumploc;
		jumploc = (state->pcN & 0xF0000000/*~0x03FFFFFFF*/)
				| ((meminstr & 0x03FFFFFF) << 2);

		//err = mips_cpu_set_debug_level(state, 2, NULL);

		if (opcode == 0b000010) {
			if (err == 0) {
				state->pc = state->pcN;
				state->pcN = jumploc;
				branchdelay = 1;
				cout << "J EXECUTED \n" << endl;

			}
		} else if (opcode == 0b000011) {
			//JAL XXX			000011 Jump and Link
			if (err == 0) {
				err = mips_cpu_set_register(state, 31, state->pc + 8);
			}
			if (err == 0) {
				state->pc = state->pcN;
				state->pcN = jumploc;
				branchdelay = 1;
			}
			cout << "JAL EXECUTED \n" << endl;
		}
	}

//I
//6 bits opcode, 5 bits source 1, 5 bits source 2 dest, 16 bits address/data

	else {
		cout << "I-TYPE IDENTIFIED" << endl;
		uint8_t source1, source2;
		uint32_t s1, s2, d1;
		int32_t ss1, ss2, sd1;
		uint16_t adata;
		int16_t sadata;
		source1 = (meminstr >> 21) & 0x1F;
		source2 = (meminstr >> 16) & 0x1F;
		adata = meminstr & 0xFFFF;
		sadata = meminstr & 0xFFFF;

		if (err == 0) {
			err = mips_cpu_get_register(state, source1, &s1); //Collect the unsigned value from Source 1
		}
		if (err == 0) {
			err = mips_cpu_get_register(state, source2, &s2); //Collect the unsigned value from Source 2
		}
		if (err == 0) {
			err = mips_cpu_get_register(state, source1, (uint32_t*) &ss1); //Collect the signed value from Source 1
		}
		if (err == 0) {
			err = mips_cpu_get_register(state, source2, (uint32_t*) &ss2); // Collect the signed value from Source 2
		}

		//err = mips_cpu_set_debug_level(state, 1, NULL); //Prints out registers and PC if you so wish

		if (opcode == 0b000100) {
			//XXX BEQ   |  Branch on equal                          | 3 I
			if (s1 == s2) {
				if (err == 0) {
					state->pc = state->pcN;
					state->pcN = state->pcN + (sadata << 2);
					branchdelay = 1;
				}
			}
			cout << "BEQ EXECUTED \n" << endl;

			//

		} else if (opcode == 0b000001) {
			if (source2 == 0b00001) {
				//XXX BGEZ  |  Branch on greater than or equal to zero
				if (ss1 >= 0) {
					if (err == 0) {
						state->pc = state->pcN;
						state->pcN = state->pcN + (sadata << 2);
						branchdelay = 1;
					}
				}
				cout << "BGEZ EXECUTED \n" << endl;
			}

			//

			else if (source2 == 0b10001) {
				//XXX BGEZAL  |  Branch on greater than or equal to zero
				if (ss1 >= 0) {
					if (err == 0) {
						err = mips_cpu_set_register(state, 31, state->pc + 8);
						state->pc = state->pcN;
						state->pcN = state->pcN + (sadata << 2);
						branchdelay = 1;
					}
				}
				cout << "BGEZAL EXECUTED \n" << endl;
			}

			//

			else if (source2 == 0b00000) {
				//XXX BLTZ Branch on less than zero
				if (err == 0) {
					if (ss1 < 0) {
						state->pc = state->pcN;
						state->pcN = state->pcN + (sadata << 2);
						branchdelay = 1;
					}
				}
				cout << "BLTZ EXECUTED \n" << endl;
			}

			//

			else if (source2 == 0b10000) {

				//XXX BLTZAL  Branch on less than zero and link
				if (err == 0) {
					err = mips_cpu_set_register(state, 31, state->pc + 8);
				}
				if (err == 0) {
					if (ss1 < 0) {
						state->pc = state->pcN;
						state->pcN = state->pcN + (sadata << 2);
						branchdelay = 1;
					}
				}
				cout << "BLTZAL EXECUTED \n" << endl;
			}

			//

		} else if (opcode == 0b000111) {

			//XXX BGTZ  Branch on greater than zero
			if (ss1 > 0) {
				if (err == 0) {
					state->pc = state->pcN;
					state->pcN = state->pcN + (sadata << 2);
					branchdelay = 1;
				}
			}
			cout << "BGTZ EXECUTED \n" << endl;
		}

		//

		else if (opcode == 0b000110) {
			//XXX BLEZ Branch on less than or equal to zero
			if (ss1 <= 0) {
				if (err == 0) {
					state->pc = state->pcN;
					state->pcN = state->pcN + (sadata << 2);
					branchdelay = 1;
				}
			}
			cout << "BLEZ EXECUTED \n" << endl;

			//

		} else if (opcode == 0b000101) {
			//XXX BNE |  Branch on not equal                      | 3 I
			if (s1 != s2) {
				if (err == 0) {
					state->pc = state->pc + 4;
					state->pcN = state->pcN + (sadata << 2);
					branchdelay = 1;
				}
			}

			//

		} else if (opcode == 0b001000) {
			//XXX ADDI | ADD Immediate

			if (((ss1 < 0 && sadata < 0) && (ss1 + sadata) >= 0)
					|| ((ss1 > 0 && sadata > 0) && (ss1 + sadata) <= 0)) {
				err = mips_ExceptionArithmeticOverflow;
			}
			sd1 = ss1 + sadata;
			if (err == 0) {
				err = mips_cpu_set_register(state, source2, sd1);
			}
			cout << "ADDI EXECUTED \n" << endl;

			//

		} else if (opcode == 0b001001) {
			//XXX ADDIU | ADD Immediate

			d1 = s1 + sadata;
			if (err == 0) {
				err = mips_cpu_set_register(state, source2, d1);
			}
			cout << "ADDIU EXECUTED \n" << endl;

			//

		} else if (opcode == 0b001100) {
			//XXX ANDI | ADD Immediate

			d1 = s1 & adata;
			if (err == 0) {
				err = mips_cpu_set_register(state, source2, d1);
			}
			cout << "ANDI EXECUTED \n" << endl;

			//

		} else if (opcode == 0b001101) {
			//XXX ORI | ADD Immediate

			d1 = s1 | adata;
			if (err == 0) {
				err = mips_cpu_set_register(state, source2, d1);
			}
			cout << "ORI EXECUTED \n" << endl;

			//

		} else if (opcode == 0b001110) {
			//XXX ORI | ADD Immediate

			d1 = s1 xor adata;
			if (err == 0) {
				err = mips_cpu_set_register(state, source2, d1);
			}
			cout << "XORI EXECUTED \n" << endl;

			//

		} else if (opcode == 0b001010) {

			//XXX SLTI  |  Set on less than immediate (signed)      | 3 I

			if (ss1 < sadata) {
				if (err == 0) {
					err = mips_cpu_set_register(state, source2, 1);
				}
			} else if (err == 0) {
				err = mips_cpu_set_register(state, source2, 0);
			}
			cout << "SLTI EXECUTED \n" << endl;

			//

		} else if (opcode == 0b001011) {

			//XXX SLTIU |  Set on less than immediate unsigned      | 3 I
			if (s1 < sadata) {
				if (err == 0) {
					err = mips_cpu_set_register(state, source2, 1);
				}
			} else if (err == 0) {
				err = mips_cpu_set_register(state, source2, 0);
			}
			cout << "SLTIU EXECUTED \n" << endl;

			//

		} else if (opcode == 0b101011) {
			//XXX SW
			uint32_t temptemp;
			temptemp = s1 + sadata;
			if ((temptemp) > (1 << 20)) {
				err = mips_ExceptionInvalidAddress;
			} //Attempting to write to a location that doesn't exist
			if (err == 0) {
				s2 = endianconverter(s2);
				err = mips_mem_write(state->mem, temptemp, 4, (uint8_t*) &s2);
			}
			cout << "SW EXECUTED \n" << endl;

			//

		} else if (opcode == 0b100011) {
			//XXX LW    |  Load word                                | 2 I
			uint32_t temptemp;
			temptemp = s1 + sadata;
			if ((temptemp) > (1 << 20)) {
				err = mips_ExceptionInvalidAddress;
			} //Attempting to write to a location that doesn't exist
			if (err == 0) {
				err = mips_mem_read(state->mem, temptemp, 4, (uint8_t*) &s2);
			}
			s2 = endianconverter(s2);
			if (err == 0) {
				err = mips_cpu_set_register(state, source2, s2);
			}
			cout << "LW EXECUTED \n" << endl;

			//

		} else if (opcode == 0b100000) {
			//XXX LB
			// Byte 0 is LSB
			uint32_t temptemp, off;
			uint8_t arrayt[4];
			int8_t sign;
			temptemp = ss1 + sadata;
			off = temptemp % 4;
			temptemp = ss1 + sadata - off;
			if ((temptemp) > (1 << 20)) {
				err = mips_ExceptionInvalidAddress;
			} //Attempting to write to a location that doesn't exist
			if (err == 0) {
				err = mips_mem_read(state->mem, temptemp, 4, arrayt);
			}
			if (err == 0) {
				sign = arrayt[off];
				ss2 = sign;
			}
			if (err == 0) {
				err = mips_cpu_set_register(state, source2, ss2);
			}

			cout << "LBU EXECUTED \n" << endl;

			//

		} else if (opcode == 0b100100) {
			//XXX LBU
			uint32_t temptemp, off;
			uint8_t arrayt[4];
			temptemp = ss1 + sadata;
			off = temptemp % 4;
			temptemp = ss1 + sadata - off;
			if ((temptemp) > (1 << 20)) {
				err = mips_ExceptionInvalidAddress;
			} //Attempting to write to a location that doesn't exist
			if (err == 0) {
				err = mips_mem_read(state->mem, temptemp, 4, arrayt);
			}
			if (err == 0) {
				s2 = arrayt[off];
			}
			if (err == 0) {
				err = mips_cpu_set_register(state, source2, s2);
			}

			cout << "LBU EXECUTED \n" << endl;

			//

		} else if (opcode == 0b001111) {
			//XXX LUI
			uint32_t temptemp;
			temptemp = adata << 16;
			if (err == 0) {
				err = mips_cpu_set_register(state, source2, temptemp);
			}

			//

		} else if (opcode == 0b101000) {
			//XXX SB
			uint32_t temptemp, off;
			uint8_t arrayt[4];
			temptemp = ss1 + sadata;
			off = temptemp % 4;
			temptemp = ss1 + sadata - off;
			if ((temptemp) > (1 << 20)) {
				err = mips_ExceptionInvalidAddress;
			} //Attempting to write to a location that doesn't exist
			if (err == 0) {
				err = mips_mem_read(state->mem, temptemp, 4, arrayt);
			}
			if (err == 0) {
				arrayt[off] = s2;
			}

			if (err == 0) {
				err = mips_mem_write(state->mem, temptemp, 4, arrayt);
			}

			cout << "SB EXECUTED \n" << endl;

			//

		} else if (opcode == 0b101001) {
			//XXX SH
			uint32_t temptemp, off;
			uint8_t arrayt[4];
			temptemp = ss1 + sadata;
			if(temptemp%2==0){
			//If not aligned properly nothing should be done
			off = temptemp % 4;
			temptemp = ss1 + sadata - off;
			if ((temptemp) > (1 << 20)) {
				err = mips_ExceptionInvalidAddress;
			} //Attempting to write to a location that doesn't exist
			if (err == 0) {
				err = mips_mem_read(state->mem, temptemp, 4, arrayt);
			}
			if (err == 0) {
				arrayt[off] = s2 >> 8;
				arrayt[off + 1] = s2;
			}
			if (err == 0) {
				err = mips_mem_write(state->mem, temptemp, 4, arrayt);
			}
			}

			cout << "SH EXECUTED /n" << endl;

			//

		} else if (opcode == 0b100010) {
			//XXX LWL
			uint32_t temptemp, off;
			uint8_t arrayt[4];
			temptemp = ss1 + sadata;
			off = temptemp % 4;
			temptemp = ss1 + sadata - off;
			if ((temptemp) > (1 << 20)) {
				err = mips_ExceptionInvalidAddress;
			} //Attempting to write to a location that doesn't exist
			if (err == 0) {
				err = mips_mem_read(state->mem, temptemp, 4, arrayt);
			}

			//Memory Contents ABCD
			//0 MSB A B C D
			//1 MSB B C D h
			//2 MSB C D g h
			//3 MSB D f g h
			if (err == 0) {
				if (off == 0) {
					s2 = ((arrayt[0]) << 24) + ((arrayt[1]) << 16)
							+ ((arrayt[2]) << 8) + (arrayt[3]);
				} else if (off == 1) {
					s2 = (s2 & 0x000000FF) + ((arrayt[1]) << 24)
							+ ((arrayt[2]) << 16) + ((arrayt[3]) << 8);
				} else if (off == 2) {
					s2 = (s2 & 0x0000FFFF) + ((arrayt[2]) << 24)
							+ ((arrayt[3] << 16));
				} else if (off == 3) {
					s2 = (s2 & 0x00FFFFFF) + ((arrayt[3]) << 24);

				}
			}
			if (err == 0) {
				err = mips_cpu_set_register(state, source2, s2);
			}

			cout << "LWL EXECUTED /n" << endl;

			//

		} else if (opcode == 0b100110) {
			//XXX LWR
			//Memory Contents ABCD
			//0 MSB e f g A
			//1 MSB e f A B
			//2 MSB e A B C
			//3 MSB A B C D
			uint32_t temptemp, off;
			uint8_t arrayt[4];
			temptemp = ss1 + sadata;
			off = temptemp % 4;
			temptemp = ss1 + sadata - off;
			if ((temptemp) > (1 << 20)) {
				err = mips_ExceptionInvalidAddress;
			} //Attempting to write to a location that doesn't exist
			err = mips_mem_read(state->mem, temptemp, 4, arrayt);
			if (err == 0) {
				if (off == 0) {
					s2 = (s2 & 0xFFFFFF00) + arrayt[0];
				} else if (off == 1) {
					s2 = (s2 & 0xFFFF0000) + ((arrayt[0]) << 8) + (arrayt[1]);
				} else if (off == 2) {
					s2 = (s2 & 0xFF000000) + ((arrayt[0]) << 16)
							+ (arrayt[1] << 8) + (arrayt[2]);
				} else if (off == 3) {
					s2 = ((arrayt[0]) << 24) + ((arrayt[1]) << 16)
							+ ((arrayt[2]) << 8) + (arrayt[3]);
				}
			}

			if (err == 0) {
				err = mips_cpu_set_register(state, source2, s2);
			}

			cout << "LWR EXECUTED \n" << endl;

		} else {
			err = mips_ExceptionInvalidInstruction;
		}

	}

	if (branchdelay == 0) {
		errinc = mips_cpu_set_pc(state, state->pcN);
	}

	//err = mips_cpu_set_debug_level(state, 2, NULL);

	if (err == 0 && errinc != 0) {
		return errinc; //In the case that the increment at the end was unsuccessful and there were no other errors
	}

	return err;
}

//XXX MEMORY FUNCTIONS

/*mips_error mips_cpu_get_pc(mips_cpu_h state, uint32_t *pc) {
 *pc = state->pc
 return mips_Success;
 }*/

//XXX OPERATION FUNCTIONS BELOW
//XXX ADD   |  Add (with overflow)                      | 2 R
//XXX ADDI  |  Add immediate (with overflow)            | 2 I
//XXX ADDIU |  Add immediate unsigned (no overflow)     | 2 I
//XXX ADDU  |  Add unsigned (no overflow)               | 1 R
//XXX AND   |  Bitwise and                              | 1 R
//XXX ANDI  |  Bitwise and immediate                    | 2 I
//XXX BEQ   |  Branch on equal                          | 3 I
//XXX BGEZ  |  Branch on greater than or equal to zero  | 3 I
//XXX BGEZAL|  Branch on greater >=0 zero and link      | 4	I
//XXX BGTZ  |  Branch on greater than zero              | 3	I
//XXX BLEZ  |  Branch on less than or equal to zero     | 3	I
//XXX BLTZ  |  Branch on less than zero                 | 3	I
//XXX BLTZAL|  Branch on less than zero and link        | 4 I
//XXX BNE   |  Branch on not equal                      | 3 I
//XXX DIV   |  Divide                                   | 4	R
//XXX DIVU  |  Divide unsigned                          | 3 R
//XXX J     |  Jump                                     | 3 J
//XXX JAL   |  Jump and link                            | 3 J
//XXX JR    |  Jump register                            | 3 R
//XXX LB    |  Load byte                                | 4	I
//XXX LBU   |  Load byte unsigned                       | 3	I
//XXX LUI   |  Load upper immediate                     | 2 I
//XXX LW    |  Load word                                | 2 I
//XXX LWL   |  Load word left                           | 5 I
//XXX LWR   |  Load word right                          | 5 I
//XXX MFHI  |  Move from HI                             | 3 R
//XXX MFLO  |  Move from LO                             | 3 R
//XXX MULT  |  Multiply                                 | 4 R
//XXX MULTU |  Multiply unsigned                        | 3 R
//XXX OR    |  Bitwise or                               | 1 R
//XXX ORI   |  Bitwise or immediate                     | 2 I
//XXX SB    |  Store byte                               | 3 I
//XXX SH    |  Store half-word                          | 3 I
//XXX SLL   |  Shift left logical                       | 2 R
//XXX SLLV  |  Shift left logical variable              | 3 R
//XXX SLT   |  Set on less than (signed)                | 2 R
//XXX SLTI  |  Set on less than immediate (signed)      | 3 I
//XXX SLTIU |  Set on less than immediate unsigned      | 3 I
//XXX SLTU  |  Set on less than unsigned                | 1 R
//XXX SRA   |  Shift right arithmetic                   | 2 R
//XXX SRL   |  Shift right logical                      | 2 R
//XXX SRLV  |  Shift right logical variable             | 2 R
//XXX SUB   |  Subtract                                 | 2 R
//XXX SUBU  |  Subtract unsigned                        | 1 R
//XXX SW    |  Store word                               | 2 I
//XXX XOR   |  Bitwise exclusive or                     | 1 R
//XXX XORI  |  Bitwise exclusive or immediate           | 2 I
