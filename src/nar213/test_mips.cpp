/*
 * test_mips.cpp
 *
 *  Created on: 17 Oct 2014
 *      Author: Nick Robertson
 *      CID: NAR213 0842277
 *
 *      TEST DRIVEN DEVELOPMENT: Write the test first then you won't write a test that will just check you are
 *      doing what you thought would work
 */

#include "mips.h"
#include "mips_test.h"
#include<iostream>
#include<vector>
#include<string>
#include<cstdlib>
#include<fstream>
#include<cmath>
#include<stdio.h>

using namespace std;

int main() {

	mips_mem_h mem = mips_mem_create_ram(1 << 20, 4); //Block size is the size of the transactions (4=one word)
	//With total memory size 2^20

	mips_cpu_h cpu = mips_cpu_create(mem);

	mips_test_begin_suite();

	int testId;
	int passed = 0;
	uint32_t memadd = 0;
	uint32_t instruction = 0;
	uint32_t instruction2 = 0;
	uint32_t got, got2;
	mips_error err, errinc;
	uint32_t temp;

//XXX ADD   |  Add (with overflow)                      | 2
	// 0000 00ss ssst tttt dddd d000 0010 0000
	//Integer Overflow is Exception

	//Generic Arithmetic Test
	testId = mips_test_begin_test("add");
	err = mips_cpu_set_register(cpu, 8, 0xFFFFFFFF); //-1
	if (err == 0) {
		err = mips_cpu_set_register(cpu, 9, 0x00000001); //1
	}
	instruction = 0x20500901;	//add $10, $8, $9
	memadd = 0; //PC Original Address
	if (err == 0) {
		err = mips_mem_write(mem, memadd, 4, (uint8_t*) &instruction);
	}
	if (err == 0) {
		err = mips_cpu_set_pc(cpu, memadd);

	}
	if (err == 0) {
		err = mips_cpu_step(cpu);
		//Implements the function
	}
	if (err == 0) {
		err = (mips_error) (err | mips_cpu_get_register(cpu, 10, &got));
	}	//Backwards
	passed = (err == mips_Success) && (got == 0x00000000); //0
	mips_test_end_test(testId, passed, NULL);
	err = mips_cpu_reset(cpu);
	err = mips_Success;

	// Close to Overflow Test
	testId = mips_test_begin_test("add");
	err = mips_cpu_set_register(cpu, 8, 0x10000001); //Second Lowest Number
	if (err == 0) {
		err = mips_cpu_set_register(cpu, 9, 0xFFFFFFFF); //-1
	}

	//instruction = 0x01095020; // //add $10, $8, $9/
	instruction = 0x20500901;
	memadd = 0;
	if (err == 0) {
		err = mips_mem_write(mem, memadd, 4, (uint8_t*) &instruction);
	}
	if (err == 0) {
		err = mips_cpu_set_pc(cpu, memadd);

	}
	if (err == 0) {
		err = mips_cpu_step(cpu);
		//Implements the function
	}
	if (err == 0) {
		err = (mips_error) (err | mips_cpu_get_register(cpu, 10, &got));
	}	//Backwards
	passed = (err == mips_Success) && (got == 0x10000000); //Lowest Possible Number
	mips_test_end_test(testId, passed, NULL);
	err = mips_cpu_reset(cpu);
	err = mips_Success;

	//Positive Overflow

	testId = mips_test_begin_test("add");

	err = mips_cpu_set_register(cpu, 8, 0x7FFFFFFF); //Largest Positive Number
	if (err == 0) {
		err = mips_cpu_set_register(cpu, 9, 0x00000221); //Some Positive Integer
	}

	instruction = 0x20500901;  //add $10, $8, $9/
	memadd = 0;
	if (err == 0) {
		err = mips_mem_write(mem, memadd, 4, (uint8_t*) &instruction);
	}
	if (err == 0) {
		err = mips_cpu_set_pc(cpu, memadd);
	}
	if (err == 0) {
		err = mips_cpu_step(cpu);
		//Implements the function
	}
	passed = (err == mips_ExceptionArithmeticOverflow);
	mips_test_end_test(testId, passed, NULL);
	err = mips_cpu_reset(cpu);
	err = mips_Success;

	//Negative Overflow

	testId = mips_test_begin_test("add");
	err = mips_cpu_set_register(cpu, 8, 0xFFFFFFFF); //Minus 1
	if (err == 0) {
		err = mips_cpu_set_register(cpu, 9, 0x80000000); //Largest Negative Number
	}

	instruction = 0x20500901;  //add $10, $8, $9/
	memadd = 0;
	if (err == 0) {
		err = mips_mem_write(mem, memadd, 4, (uint8_t*) &instruction);
	}
	if (err == 0) {
		err = mips_cpu_set_pc(cpu, memadd);

	}
	if (err == 0) {
		err = mips_cpu_step(cpu);
		//Implements the function
	}
	passed = (err == mips_ExceptionArithmeticOverflow);
	mips_test_end_test(testId, passed, NULL);
	err = mips_cpu_reset(cpu);
	err = mips_Success;

//XXX ADDI  |  Add immediate (with overflow)            | 2
	// 0010 00ss ssst tttt iiii iiii iiii iiii
	// 0010 0000 0010 0010 0000 0000 0000 0010
	testId = mips_test_begin_test("addi");
	err = mips_cpu_set_register(cpu, 1, 0x00001010);

	//Regular Addition

	instruction = 0x02002220;
	memadd = 0;
	if (err == 0) {
		err = mips_mem_write(mem, memadd, 4, (uint8_t*) &instruction);
	}
	if (err == 0) {
		err = mips_cpu_set_pc(cpu, memadd);

	}
	if (err == 0) {
		err = mips_cpu_step(cpu);
		//Implements the function
	}
	if (err == 0) {
		err = (mips_error) (err | mips_cpu_get_register(cpu, 2, &got));
	}	//Backwards
	passed = (err == mips_Success) && (got == 0x00001012);
	mips_test_end_test(testId, passed, NULL);
	err = mips_cpu_reset(cpu);
	err = mips_Success;

	//Arithmetic Overflow
	testId = mips_test_begin_test("addi");
	err = mips_cpu_set_register(cpu, 1, 0x7FFFFFFF);

	instruction = 0x02002220;
	memadd = 0;
	if (err == 0) {
		err = mips_mem_write(mem, memadd, 4, (uint8_t*) &instruction);
	}
	if (err == 0) {
		err = mips_cpu_set_pc(cpu, memadd);

	}
	if (err == 0) {
		err = mips_cpu_step(cpu);
		//Implements the function
	}
	if (err == 0) {
		err = (mips_error) (err | mips_cpu_get_register(cpu, 2, &got));
	}	//Backwards
	passed = (err == mips_ExceptionArithmeticOverflow);
	mips_test_end_test(testId, passed, NULL);
	err = mips_cpu_reset(cpu);
	err = mips_Success;

	//Signed Test

	testId = mips_test_begin_test("addi");
	err = mips_cpu_set_register(cpu, 1, 0x00001111);

	instruction = 0xFFFF2220;
	memadd = 0;
	if (err == 0) {
		err = mips_mem_write(mem, memadd, 4, (uint8_t*) &instruction);
	}
	if (err == 0) {
		err = mips_cpu_set_pc(cpu, memadd);

	}
	if (err == 0) {
		err = mips_cpu_step(cpu);
		//Implements the function
	}
	if (err == 0) {
		err = (mips_error) (err | mips_cpu_get_register(cpu, 2, &got));
	}	//Backwards
	passed = (err == mips_Success) && (got == 0x00001110);
	mips_test_end_test(testId, passed, NULL);
	err = mips_cpu_reset(cpu);
	err = mips_Success;

//XXX ADDIU |  Add immediate unsigned (no overflow)     | 2
	//0010 01ss ssst tttt iiii iiii iiii iiii
	// 0010 0100 0010 0010 0000 0000 0000 0010
	testId = mips_test_begin_test("addiu");
	err = mips_cpu_set_register(cpu, 1, 0x00001010);

	instruction = 0x02002224;
	memadd = 0;
	if (err == 0) {
		err = mips_mem_write(mem, memadd, 4, (uint8_t*) &instruction);
	}
	if (err == 0) {
		err = mips_cpu_set_pc(cpu, memadd);

	}
	if (err == 0) {
		err = mips_cpu_step(cpu);
		//Implements the function
	}
	if (err == 0) {
		err = (mips_error) (err | mips_cpu_get_register(cpu, 2, &got));
	}	//Backwards
	passed = (err == mips_Success) && (got == 0x00001012);
	mips_test_end_test(testId, passed, NULL);
	err = mips_cpu_reset(cpu);
	err = mips_Success;

	// Negative Immediate

	testId = mips_test_begin_test("addiu");
	err = mips_cpu_set_register(cpu, 1, 0x00001001);

	instruction = 0xFFFF2224;
	memadd = 0;
	if (err == 0) {
		err = mips_mem_write(mem, memadd, 4, (uint8_t*) &instruction);
	}
	if (err == 0) {
		err = mips_cpu_set_pc(cpu, memadd);

	}
	if (err == 0) {
		err = mips_cpu_step(cpu);
		//Implements the function
	}
	if (err == 0) {
		err = (mips_error) (err | mips_cpu_get_register(cpu, 2, &got));
	}	//Backwards
	passed = (err == mips_Success) && (got == 0x00001000);
	mips_test_end_test(testId, passed, NULL);
	err = mips_cpu_reset(cpu);
	err = mips_Success;

//XXX ADDU  |  Add unsigned (no overflow)               | 1

	//Generic Unsigned Addition

	testId = mips_test_begin_test("addu");
	err = mips_cpu_set_register(cpu, 8, 0x0001FFFF); //Some value
	if (err == 0) {
		err = mips_cpu_set_register(cpu, 9, 0x00F10000); //Some value
	}

	instruction = 0x21500901; // //addu $10, $8, $9
	memadd = 0;
	if (err == 0) {
		err = mips_mem_write(mem, memadd, 4, (uint8_t*) &instruction);
	}
	if (err == 0) {
		err = mips_cpu_set_pc(cpu, memadd);

	}
	if (err == 0) {
		err = mips_cpu_step(cpu);
		//Implements the function
	}
	if (err == 0) {
		err = (mips_error) (err | mips_cpu_get_register(cpu, 10, &got));
	}
	passed = (err == mips_Success) && (got == 0x00F2FFFF);
	mips_test_end_test(testId, passed, NULL);
	err = mips_cpu_reset(cpu);
	err = mips_Success;

	//

	//Signed addition to test if roll over occurs

	//
	testId = mips_test_begin_test("addu");
	err = mips_cpu_set_register(cpu, 8, 0xFFFFFFFF);
	if (err == 0) {
		err = mips_cpu_set_register(cpu, 9, 0x00000002);
	}

	instruction = 0x21500901; //addu $10, $8, $9
	memadd = 0;
	if (err == 0) {
		err = mips_mem_write(mem, memadd, 4, (uint8_t*) &instruction);
	}
	if (err == 0) {
		err = mips_cpu_set_pc(cpu, memadd);

	}
	if (err == 0) {
		err = mips_cpu_step(cpu);
		//Implements the function
	}
	if (err == 0) {
		err = (mips_error) (err | mips_cpu_get_register(cpu, 10, &got));
	}
	passed = (err == mips_Success) && (got == 0x00000001);
	mips_test_end_test(testId, passed, NULL);
	err = mips_cpu_reset(cpu);
	err = mips_Success;

//

//XXX AND   |  Bitwise and                              | 1
	testId = mips_test_begin_test("and");
	err = mips_cpu_set_register(cpu, 8, 0x0000FFFF);
	if (err == 0) {
		err = mips_cpu_set_register(cpu, 9, 0x00FFFF00);
	}
	// General Encoding: 0000 00ss ssst tttt dddd d000 0010 0100
	//Specific Encoding: 0000 0001 0000 1001 0101 0000 0010 0100

	//instruction = 0x01095024;
	instruction = 0x24500901;	//and $10, $8, $9
	memadd = 0;
	if (err == 0) {
		err = mips_mem_write(mem, memadd, 4, (uint8_t*) &instruction);
	}
	if (err == 0) {
		err = mips_cpu_set_pc(cpu, memadd);

	}
	if (err == 0) {
		err = mips_cpu_step(cpu);
		//Implements the function
	}
	if (err == 0) {
		err = (mips_error) (err | mips_cpu_get_register(cpu, 10, &got));
	}
	passed = (err == mips_Success) && (got == 0x0000FF00);
	mips_test_end_test(testId, passed, NULL);
	err = mips_cpu_reset(cpu);
	err = mips_Success;

	testId = mips_test_begin_test("and");
	err = mips_cpu_set_register(cpu, 8, 0x00000000);
	if (err == 0) {
		err = mips_cpu_set_register(cpu, 9, 0x00000000);
	}
	// General Encoding: 0000 00ss ssst tttt dddd d000 0010 0100
	//Specific Encoding: 0000 0001 0000 1001 0101 0000 0010 0100
	//instruction = 0x01095024;
	instruction = 0x24500901;	//and $10, $8, $9
	memadd = 0;
	if (err == 0) {
		err = mips_mem_write(mem, memadd, 4, (uint8_t*) &instruction);
	}

	if (err == 0) {
		err = mips_cpu_set_pc(cpu, memadd);

	}
	if (err == 0) {
		err = mips_cpu_step(cpu);
		//Implements the function
	}
	if (err == 0) {
		err = (mips_error) (err | mips_cpu_get_register(cpu, 10, &got));
	}
	passed = (err == mips_Success) && (got == 0x00000000);
	mips_test_end_test(testId, passed, NULL);
	err = mips_cpu_reset(cpu);
	err = mips_Success;

//ANDI  |  Bitwise and immediate                    | 2
	//0011 00ss ssst tttt iiii iiii iiii iiii

	testId = mips_test_begin_test("andi");
	err = mips_cpu_set_register(cpu, 1, 0x00001012);

	instruction = 0x02002230;
	memadd = 0;
	if (err == 0) {
		err = mips_mem_write(mem, memadd, 4, (uint8_t*) &instruction);
	}
	if (err == 0) {
		err = mips_cpu_set_pc(cpu, memadd);

	}
	if (err == 0) {
		err = mips_cpu_step(cpu);
		//Implements the function
	}
	if (err == 0) {
		err = (mips_error) (err | mips_cpu_get_register(cpu, 2, &got));
	}	//Backwards
	passed = (err == mips_Success) && (got == 0x00000002);
	mips_test_end_test(testId, passed, NULL);
	err = mips_cpu_reset(cpu);
	err = mips_Success;

	//Zero Extended Test

	testId = mips_test_begin_test("andi");
	err = mips_cpu_set_register(cpu, 1, 0x10001012);

	instruction = 0xFFFF2230;
	memadd = 0;
	if (err == 0) {
		err = mips_mem_write(mem, memadd, 4, (uint8_t*) &instruction);
	}
	if (err == 0) {
		err = mips_cpu_set_pc(cpu, memadd);

	}
	if (err == 0) {
		err = mips_cpu_step(cpu);
		//Implements the function
	}
	if (err == 0) {
		err = (mips_error) (err | mips_cpu_get_register(cpu, 2, &got));
	}	//Backwards
	passed = (err == mips_Success) && (got == 0x00001012);
	mips_test_end_test(testId, passed, NULL);
	err = mips_cpu_reset(cpu);
	err = mips_Success;

//XXX BEQ   |  Branch on equal                          | 3
	//0001 00ss ssst tttt iiii iiii iiii iiii
	//0001 0000 0010 0010 0000 0000 0000 1000
	// 08 00 22 10

	testId = mips_test_begin_test("beq");
	err = mips_cpu_set_register(cpu, 1, 0x00010100);
	if (err == 0) {
		err = mips_cpu_set_register(cpu, 2, 0x00010100);
	}

	instruction = 0x08002210;
	instruction2 = 0X00080100;  //SLL/NOP aka Null Operation

	memadd = 0;
	if (err == 0) {
		err = mips_mem_write(mem, memadd, 4, (uint8_t*) &instruction);
	}
	if (err == 0) {
		err = mips_mem_write(mem, memadd + 4, 4, (uint8_t*) &instruction2);
	}
	if (err == 0) {
		err = mips_cpu_set_pc(cpu, memadd);
	}
	if (err == 0) {
		err = mips_cpu_step(cpu);
		//Implements the function
	}
	if (err == 0) {
		errinc = mips_cpu_step(cpu);
		//Implements the function
	}
	if (err == 0) {
		err = (mips_error) (err | mips_cpu_get_pc(cpu, &got));
	}
	passed = (err == mips_Success) && (got == 0x00000024);
	mips_test_end_test(testId, passed, NULL);
	err = mips_cpu_reset(cpu);
	err = mips_Success;

	//Not Equal

	testId = mips_test_begin_test("beq");
	err = mips_cpu_set_register(cpu, 1, 0x00010000);
	if (err == 0) {
		err = mips_cpu_set_register(cpu, 2, 0x00010100);
	}

	instruction = 0x08002210;
	instruction2 = 0X00080100;  //SLL/NOP aka Null Operation

	memadd = 0;
	if (err == 0) {
		err = mips_mem_write(mem, memadd, 4, (uint8_t*) &instruction);
	}
	if (err == 0) {
		err = mips_mem_write(mem, memadd + 4, 4, (uint8_t*) &instruction2);
	}
	if (err == 0) {
		err = mips_cpu_set_pc(cpu, memadd);

	}
	if (err == 0) {
		err = mips_cpu_step(cpu);
		//Implements the function
	}
	if (err == 0) {
		errinc = mips_cpu_step(cpu);
		//Implements the function
	}
	if (err == 0) {
		err = (mips_error) (err | mips_cpu_get_pc(cpu, &got));
	}
	passed = (err == mips_Success) && (got == 0x00000008);
	mips_test_end_test(testId, passed, NULL);
	err = mips_cpu_reset(cpu);
	err = mips_Success;

	//Signed

	testId = mips_test_begin_test("beq");
	err = mips_cpu_set_register(cpu, 1, 0x00010100);
	if (err == 0) {
		err = mips_cpu_set_register(cpu, 2, 0x00010100);
	}
	memadd = 32;
	instruction = 0xFEFF2210;
	instruction2 = 0X00080100;  //SLL/NOP aka Null Operation

	if (err == 0) {
		err = mips_mem_write(mem, memadd + 4, 4, (uint8_t*) &instruction2);
	}
	if (err == 0) {
		err = mips_mem_write(mem, memadd, 4, (uint8_t*) &instruction);
	}

	if (err == 0) {
		err = mips_cpu_set_pc(cpu, memadd);

	}
	if (err == 0) {
		err = mips_cpu_step(cpu);
		//Implements the function
	}
	if (err == 0) {
		err = mips_cpu_step(cpu);
		//Implements the second function which should pass on most persons implementation
	}
	if (err == 0) {
		err = (mips_error) (err | mips_cpu_get_pc(cpu, &got));
	}

	passed = (err == mips_Success) && (got == 0x0000001C);
	mips_test_end_test(testId, passed, NULL);
	err = mips_cpu_reset(cpu);
	err = mips_Success;

//XXX BGEZ  |  Branch on greater than or equal to zero  | 3
	//0000 01ss sss0 0001 iiii iiii iiii iiii

	testId = mips_test_begin_test("bgez");
	err = mips_cpu_set_register(cpu, 1, 0x00010000);

	instruction = 0x08002104;
	memadd = 0;
	instruction2 = 0X00080100;  //SLL/NOP aka Null Operation

	if (err == 0) {
		err = mips_mem_write(mem, memadd + 4, 4, (uint8_t*) &instruction2);
	}
	if (err == 0) {
		err = mips_mem_write(mem, memadd, 4, (uint8_t*) &instruction);
	}
	if (err == 0) {
		err = mips_cpu_set_pc(cpu, memadd);

	}
	if (err == 0) {
		err = mips_cpu_step(cpu);
		//Implements the function

	}
	if (err == 0) {
		err = mips_cpu_step(cpu);
		//Implements the function
	}
	if (err == 0) {
		err = (mips_error) (err | mips_cpu_get_pc(cpu, &got));
	}
	passed = (err == mips_Success) && (got == 0x00000024);
	mips_test_end_test(testId, passed, NULL);
	err = mips_cpu_reset(cpu);
	err = mips_Success;

	testId = mips_test_begin_test("bgez");
	err = mips_cpu_set_register(cpu, 1, 0x80000010);

	instruction = 0x08002104;
	memadd = 0;
	instruction2 = 0X00080100;  //SLL/NOP aka Null Operation

	if (err == 0) {
		err = mips_mem_write(mem, memadd + 4, 4, (uint8_t*) &instruction2);
	}
	if (err == 0) {
		err = mips_mem_write(mem, memadd, 4, (uint8_t*) &instruction);
	}
	if (err == 0) {
		err = mips_cpu_set_pc(cpu, memadd);

	}
	if (err == 0) {
		err = mips_cpu_step(cpu);
		//Implements the function
	}
	if (err == 0) {
		errinc = mips_cpu_step(cpu);
		//Implements the function
	}
	if (err == 0) {
		err = (mips_error) (err | mips_cpu_get_pc(cpu, &got));
	}
	passed = (err == mips_Success) && (got == 0x00000008);
	mips_test_end_test(testId, passed, NULL);
	err = mips_cpu_reset(cpu);
	err = mips_Success;

//XXX BGEZAL|  Branch on greater >=0 zero and link      | 4
	//0000 01ss sss1 0001 iiii iiii iiii iiii

	testId = mips_test_begin_test("bgezal");
	err = mips_cpu_set_register(cpu, 1, 0x00000001);

	instruction = 0x08003104; //32 jump
	instruction2 = 0X00080100;  //SLL/NOP aka Null Operation
	memadd = 0;
	if (err == 0) {
		err = mips_mem_write(mem, memadd + 4, 4, (uint8_t*) &instruction2);
	}

	if (err == 0) {
		err = mips_mem_write(mem, memadd, 4, (uint8_t*) &instruction);
	}
	if (err == 0) {
		err = mips_cpu_set_pc(cpu, memadd);

	}
	if (err == 0) {
		err = mips_cpu_step(cpu);
		//Implements the function
	}
	if (err == 0) {
		errinc = mips_cpu_step(cpu);
		//Implements the function
	}
	if (err == 0) {
		err = (mips_error) (err | mips_cpu_get_pc(cpu, &got));
	}

	if (err == 0) {
		err = (mips_error) (err | mips_cpu_get_register(cpu, 31, &got2));
	}
	passed = (err == mips_Success) && (got == 0x00000024)
			&& (got2 == 0x00000008);
	mips_test_end_test(testId, passed, NULL);
	err = mips_cpu_reset(cpu);
	err = mips_Success;

	testId = mips_test_begin_test("bgezal");
	err = mips_cpu_set_register(cpu, 1, 0xF0000000);

	instruction = 0x08003104; //32 jump
	instruction2 = 0X00080100;  //SLL/NOP aka Null Operation
	memadd = 0;
	if (err == 0) {
		err = mips_mem_write(mem, memadd + 4, 4, (uint8_t*) &instruction2);
	}

	if (err == 0) {
		err = mips_mem_write(mem, memadd, 4, (uint8_t*) &instruction);
	}
	if (err == 0) {
		err = mips_cpu_set_pc(cpu, memadd);

	}
	if (err == 0) {
		err = mips_cpu_step(cpu);
		//Implements the function
	}
	if (err == 0) {
		errinc = mips_cpu_step(cpu);
		//Implements the function
	}
	if (err == 0) {
		err = (mips_error) (err | mips_cpu_get_pc(cpu, &got));
	}

	passed = (err == mips_Success) && (got == 0x00000008);
	mips_test_end_test(testId, passed, NULL);
	err = mips_cpu_reset(cpu);
	err = mips_Success;

//XXX BGTZ  |  Branch on greater than zero              | 3
	//0001 11ss sss0 0000 iiii iiii iiii iiii

	testId = mips_test_begin_test("bgtz");
	err = mips_cpu_set_register(cpu, 1, 0x00000000);

	instruction = 0x0800201C; //32 jump
	instruction2 = 0X00080100;  //SLL/NOP aka Null Operation
	memadd = 0;
	if (err == 0) {
		err = mips_mem_write(mem, memadd + 4, 4, (uint8_t*) &instruction2);
	}

	if (err == 0) {
		err = mips_mem_write(mem, memadd, 4, (uint8_t*) &instruction);
	}
	if (err == 0) {
		err = mips_cpu_set_pc(cpu, memadd);

	}
	if (err == 0) {
		err = mips_cpu_step(cpu);
		//Implements the function
	}
	if (err == 0) {
		errinc = mips_cpu_step(cpu);
		//Implements the function
	}
	if (err == 0) {
		err = (mips_error) (err | mips_cpu_get_pc(cpu, &got));
	}

	passed = (err == mips_Success) && (got == 0x00000008);
	mips_test_end_test(testId, passed, NULL);
	err = mips_cpu_reset(cpu);
	err = mips_Success;

	testId = mips_test_begin_test("bgtz");
	err = mips_cpu_set_register(cpu, 1, 0x00000034);

	instruction = 0xFEFF201C; //32 jump
	instruction2 = 0X00080100;  //SLL/NOP aka Null Operation
	memadd = 32;

	if (err == 0) {
		err = mips_mem_write(mem, memadd + 4, 4, (uint8_t*) &instruction2);
	}

	if (err == 0) {
		err = mips_mem_write(mem, memadd, 4, (uint8_t*) &instruction);
	}
	if (err == 0) {
		err = mips_cpu_set_pc(cpu, memadd);

	}
	if (err == 0) {
		err = mips_cpu_step(cpu);
		//Implements the function
	}
	if (err == 0) {
		errinc = mips_cpu_step(cpu);
		//Implements the function
	}
	if (err == 0) {
		err = (mips_error) (err | mips_cpu_get_pc(cpu, &got));
	}
	passed = (err == mips_Success) && (got == 0x0000001C);
	mips_test_end_test(testId, passed, NULL);
	err = mips_cpu_reset(cpu);
	err = mips_Success;

	//

//XXX BLEZ  |  Branch on less than or equal to zero     | 3
	//0001 10ss sss0 0000 iiii iiii iiii iiii

	testId = mips_test_begin_test("blez");
	err = mips_cpu_set_register(cpu, 1, 0xF0000000);

	instruction = 0x08002018; //32 jump
	instruction2 = 0X00080100;  //SLL/NOP aka Null Operation
	memadd = 0;
	if (err == 0) {
		err = mips_mem_write(mem, memadd + 4, 4, (uint8_t*) &instruction2);
	}

	if (err == 0) {
		err = mips_mem_write(mem, memadd, 4, (uint8_t*) &instruction);
	}
	if (err == 0) {
		err = mips_cpu_set_pc(cpu, memadd);

	}
	if (err == 0) {
		err = mips_cpu_step(cpu);
		//Implements the function
	}
	if (err == 0) {
		errinc = mips_cpu_step(cpu);
		//Implements the function
	}
	if (err == 0) {
		err = (mips_error) (err | mips_cpu_get_pc(cpu, &got));
	}

	passed = (err == mips_Success) && (got == 0x000000024);
	mips_test_end_test(testId, passed, NULL);
	err = mips_cpu_reset(cpu);
	err = mips_Success;

	testId = mips_test_begin_test("blez");
	err = mips_cpu_set_register(cpu, 1, 0x00230000);

	memadd = 0;
	instruction = 0x08002018; //32 jump
	instruction2 = 0X00080100;  //SLL/NOP aka Null Operation

	if (err == 0) {
		err = mips_mem_write(mem, memadd + 4, 4, (uint8_t*) &instruction2);
	}

	if (err == 0) {
		err = mips_mem_write(mem, memadd, 4, (uint8_t*) &instruction);
	}
	if (err == 0) {
		err = mips_cpu_set_pc(cpu, memadd);

	}
	if (err == 0) {
		err = mips_cpu_step(cpu);
		//Implements the function
	}
	if (err == 0) {
		errinc = mips_cpu_step(cpu);
		//Implements the function
	}
	if (err == 0) {
		err = (mips_error) (err | mips_cpu_get_pc(cpu, &got));
	}

	passed = (err == mips_Success) && (got == 0x00000008);
	mips_test_end_test(testId, passed, NULL);
	err = mips_cpu_reset(cpu);
	err = mips_Success;
//XXX BLTZ  |  Branch on less than zero                 | 3
	//0000 01ss sss0 0000 iiii iiii iiii iiii
	// 0 	4	2		0	0	0	0	8
	testId = mips_test_begin_test("bltz");
	err = mips_cpu_set_register(cpu, 1, 0x00000000);

	instruction = 0x08002004; //32 jump
	instruction2 = 0X00080100;  //SLL/NOP aka Null Operation
	memadd = 0;
	if (err == 0) {
		err = mips_mem_write(mem, memadd + 4, 4, (uint8_t*) &instruction2);
	}
	if (err == 0) {
		err = mips_mem_write(mem, memadd, 4, (uint8_t*) &instruction);
	}
	if (err == 0) {
		err = mips_cpu_set_pc(cpu, memadd);

	}
	if (err == 0) {
		err = mips_cpu_step(cpu);
		//Implements the function
	}
	if (err == 0) {
		errinc = mips_cpu_step(cpu);
		//Implements the function
	}
	if (err == 0) {
		err = (mips_error) (err | mips_cpu_get_pc(cpu, &got));
	}

	passed = (err == mips_Success) && (got == 0x00000008);
	mips_test_end_test(testId, passed, NULL);
	err = mips_cpu_reset(cpu);
	err = mips_Success;

	testId = mips_test_begin_test("bltz");
	err = mips_cpu_set_register(cpu, 1, 0x80000300);

	instruction = 0x08002004; //32 jump
	instruction2 = 0X00080100;  //SLL/NOP aka Null Operation
	memadd = 0;
	if (err == 0) {
		err = mips_mem_write(mem, memadd + 4, 4, (uint8_t*) &instruction2);
	}
	if (err == 0) {
		err = mips_mem_write(mem, memadd, 4, (uint8_t*) &instruction);
	}
	if (err == 0) {
		err = mips_cpu_set_pc(cpu, memadd);

	}
	if (err == 0) {
		err = mips_cpu_step(cpu);
		//Implements the function
	}
	if (err == 0) {
		errinc = mips_cpu_step(cpu);
		//Implements the function
	}
	if (err == 0) {
		err = (mips_error) (err | mips_cpu_get_pc(cpu, &got));
	}

	passed = (err == mips_Success) && (got == 0x00000024);
	mips_test_end_test(testId, passed, NULL);
	err = mips_cpu_reset(cpu);
	err = mips_Success;

//BLTZAL|  Branch on less than zero and link        | 4
	//0000 01ss sss1 0000 iiii iiii iiii iiii
	//0000 0100 0011 0000 0000 0000 0000 1000
	//   0    4    3    0    0    0    0    8
	//   0    8    0    0    3    0    0    4

	testId = mips_test_begin_test("bltzal");
	err = mips_cpu_set_register(cpu, 1, 0xF0000300);

	instruction = 0x08003004; //32 jump
	instruction2 = 0X00080100;  //SLL/NOP aka Null Operation
	memadd = 0;
	if (err == 0) {
		err = mips_mem_write(mem, memadd + 4, 4, (uint8_t*) &instruction2);
	}
	if (err == 0) {
		err = mips_mem_write(mem, memadd, 4, (uint8_t*) &instruction);
	}
	if (err == 0) {
		err = mips_cpu_set_pc(cpu, memadd);

	}

	if (err == 0) {
		err = mips_cpu_step(cpu);
		//Implements the function
	}
	if (err == 0) {
		errinc = mips_cpu_step(cpu);
		//Implements the function
	}
	if (err == 0) {
		err = (mips_error) (err | mips_cpu_get_pc(cpu, &got));
	}

	if (err == 0) {
		err = (mips_error) (err | mips_cpu_get_register(cpu, 31, &got2));
	}
	passed = (err == mips_Success) && (got == 0x00000024)
			&& (got2 == 0x00000008);
	mips_test_end_test(testId, passed, NULL);
	err = mips_cpu_reset(cpu);
	err = mips_Success;

	testId = mips_test_begin_test("bltzal");
	err = mips_cpu_set_register(cpu, 1, 0x10000300);

	instruction = 0x08003004; //32 jump
	instruction2 = 0X00080100;  //SLL/NOP aka Null Operation
	memadd = 0;
	if (err == 0) {
		err = mips_mem_write(mem, memadd + 4, 4, (uint8_t*) &instruction2);
	}
	if (err == 0) {
		err = mips_mem_write(mem, memadd, 4, (uint8_t*) &instruction);
	}
	if (err == 0) {
		err = mips_cpu_set_pc(cpu, memadd);

	}
	if (err == 0) {
		err = mips_cpu_step(cpu);
		//Implements the function
	}
	if (err == 0) {
		errinc = mips_cpu_step(cpu);
		//Implements the function
	}
	if (err == 0) {
		err = (mips_error) (err | mips_cpu_get_pc(cpu, &got));
	}

	passed = (err == mips_Success) && (got == 0x00000008);
	mips_test_end_test(testId, passed, NULL);
	err = mips_cpu_reset(cpu);
	err = mips_Success;
//BNE   |  Branch on not equal                      | 3
	//0001 01ss ssst tttt iiii iiii iiii iiii
	//0001 0100 0010 0010 0000 0000 0000 1000
	//   0    8

	testId = mips_test_begin_test("bne");
	err = mips_cpu_set_register(cpu, 1, 0x00010100);
	if (err == 0) {
		err = mips_cpu_set_register(cpu, 2, 0x00010100);
	}

	instruction = 0x08002214;
	instruction2 = 0X00080100;  //SLL/NOP aka Null Operation
	memadd = 0;
	if (err == 0) {
		err = mips_mem_write(mem, memadd + 4, 4, (uint8_t*) &instruction2);
	}
	if (err == 0) {
		err = mips_mem_write(mem, memadd, 4, (uint8_t*) &instruction);
	}
	if (err == 0) {
		err = mips_cpu_set_pc(cpu, memadd);

	}
	if (err == 0) {
		err = mips_cpu_step(cpu);
		//Implements the function

	}
	if (err == 0) {
		err = mips_cpu_step(cpu);
		//Implements the function
	}
	if (err == 0) {
		err = (mips_error) (err | mips_cpu_get_pc(cpu, &got));
	}
	passed = (err == mips_Success) && (got == 0x00000008);
	mips_test_end_test(testId, passed, NULL);
	err = mips_cpu_reset(cpu);
	err = mips_Success;

	testId = mips_test_begin_test("bne");
	err = mips_cpu_set_register(cpu, 1, 0x00010000);
	if (err == 0) {
		err = mips_cpu_set_register(cpu, 2, 0x00010100);
	}

	instruction = 0x08002214;
	instruction2 = 0X00080100;  //SLL/NOP aka Null Operation
	memadd = 0;
	if (err == 0) {
		err = mips_mem_write(mem, memadd + 4, 4, (uint8_t*) &instruction2);
	}
	if (err == 0) {
		err = mips_mem_write(mem, memadd, 4, (uint8_t*) &instruction);
	}
	if (err == 0) {
		err = mips_cpu_set_pc(cpu, memadd);

	}
	if (err == 0) {
		err = mips_cpu_step(cpu);
		//Implements the function
	}
	if (err == 0) {
		errinc = mips_cpu_step(cpu);
		//Implements the function
	}
	if (err == 0) {
		err = (mips_error) (err | mips_cpu_get_pc(cpu, &got));
	}
	passed = (err == mips_Success) && (got == 0x00000024);
	mips_test_end_test(testId, passed, NULL);
	err = mips_cpu_reset(cpu);
	err = mips_Success;
//XXX DIV   |  Divide                                   | 4
	//0000 00ss ssst tttt 0000 0000 0001 1010
	//0000 0001 0000 1001 0000 0000 0001 1010
//+/-
	testId = mips_test_begin_test("div");
	err = mips_cpu_set_register(cpu, 8, 0x00000008);
	if (err == 0) {
		err = mips_cpu_set_register(cpu, 9, 0xFFFFFFFF);
	}

	//instruction = 0x0109001A; // div $10, $8, $9 // #8/#-1
	instruction = 0x1A000901;
	memadd = 0;
	if (err == 0) {
		err = mips_mem_write(mem, memadd, 4, (uint8_t*) &instruction);
	}
	instruction = 0x10100000; //MFHI to register 2
	if (err == 0) {
		err = mips_mem_write(mem, memadd + 4, 4, (uint8_t*) &instruction);
	}
	instruction = 0x12080000; //MFLO to register 1
	if (err == 0) {
		err = mips_mem_write(mem, memadd + 8, 4, (uint8_t*) &instruction);
	}
	if (err == 0) {
		err = mips_cpu_set_pc(cpu, memadd);

	}
	if (err == 0) {
		err = mips_cpu_step(cpu);
		//Implements the function
	}
	if (err == 0) {
		err = mips_cpu_step(cpu);
		//Implements the function
	}
	if (err == 0) {
		err = mips_cpu_step(cpu);
		//Implements the function
	}
	if (err == 0) {
		err = mips_cpu_get_register(cpu, 2, &got);
	}
	if (err == 0) {
		err = mips_cpu_get_register(cpu, 1, &got2);
	}
	passed = (err == mips_Success) && (got == 0) && (got2 == 0xFFFFFFF8);
	mips_test_end_test(testId, passed, NULL);
	err = mips_cpu_reset(cpu);
	err = mips_Success;

	//

	//+/+

	testId = mips_test_begin_test("div");
	err = mips_cpu_set_register(cpu, 8, 0x00000011);
	if (err == 0) {
		err = mips_cpu_set_register(cpu, 9, 0x00000002);
	}

	//instruction = 0x0109001A; // div $10, $8, $9 // #17/#2
	instruction = 0x1A000901;
	memadd = 0;
	if (err == 0) {
		err = mips_mem_write(mem, memadd, 4, (uint8_t*) &instruction);
	}
	instruction = 0x10100000; //MFHI to register 2
	if (err == 0) {
		err = mips_mem_write(mem, memadd + 4, 4, (uint8_t*) &instruction);
	}
	instruction = 0x12080000; //MFLO to register 1
	if (err == 0) {
		err = mips_mem_write(mem, memadd + 8, 4, (uint8_t*) &instruction);
	}
	if (err == 0) {
		err = mips_cpu_set_pc(cpu, memadd);

	}
	if (err == 0) {
		err = mips_cpu_step(cpu);
		//Implements the function
	}
	if (err == 0) {
		err = mips_cpu_step(cpu);
		//Implements the function
	}
	if (err == 0) {
		err = mips_cpu_step(cpu);
		//Implements the function
	}
	if (err == 0) {
		err = mips_cpu_get_register(cpu, 2, &got);
	}
	if (err == 0) {
		err = mips_cpu_get_register(cpu, 1, &got2);
	}
	passed = (err == mips_Success) && (got == 1) && (got2 == 0x00000008);
	mips_test_end_test(testId, passed, NULL);
	err = mips_cpu_reset(cpu);
	err = mips_Success;

	//-/-

	testId = mips_test_begin_test("div");
	err = mips_cpu_set_register(cpu, 8, 0xFFFFFFF8);
	if (err == 0) {
		err = mips_cpu_set_register(cpu, 9, 0xFFFFFFFD);
	}

	//instruction = 0x0109001A; // div $10, $8, $9 // #-8/-3
	instruction = 0x1A000901;
	memadd = 0;
	if (err == 0) {
		err = mips_mem_write(mem, memadd, 4, (uint8_t*) &instruction);
	}
	instruction = 0x10100000; //MFHI to register 2
	if (err == 0) {
		err = mips_mem_write(mem, memadd + 4, 4, (uint8_t*) &instruction);
	}
	instruction = 0x12080000; //MFLO to register 1
	if (err == 0) {
		err = mips_mem_write(mem, memadd + 8, 4, (uint8_t*) &instruction);
	}
	if (err == 0) {
		err = mips_cpu_set_pc(cpu, memadd);

	}
	if (err == 0) {
		err = mips_cpu_step(cpu);
		//Implements the function
	}
	if (err == 0) {
		err = mips_cpu_step(cpu);
		//Implements the function
	}
	if (err == 0) {
		err = mips_cpu_step(cpu);
		//Implements the function
	}
	if (err == 0) {
		err = mips_cpu_get_register(cpu, 2, &got);
	}
	if (err == 0) {
		err = mips_cpu_get_register(cpu, 1, &got2);
	}
	passed = (err == mips_Success) && (got == -2) && (got2 == 2);
	mips_test_end_test(testId, passed, NULL);
	err = mips_cpu_reset(cpu);
	err = mips_Success;

//XXX DIVU  |  Divide unsigned                          | 3
	//0000 00ss ssst tttt 0000 0000 0001 1011

	testId = mips_test_begin_test("divu");
	err = mips_cpu_set_register(cpu, 8, 0x00000008);
	if (err == 0) {
		err = mips_cpu_set_register(cpu, 9, 0x00000003);
	}

	//instruction = 0x0109001B; // divu $10, $8, $9 // #8/#3
	instruction = 0x1B000901;
	memadd = 0;
	if (err == 0) {
		err = mips_mem_write(mem, memadd, 4, (uint8_t*) &instruction);
	}
	instruction = 0x10100000; //MFHI to register 2
	if (err == 0) {
		err = mips_mem_write(mem, memadd + 4, 4, (uint8_t*) &instruction);
	}
	instruction = 0x12080000; //MFLO to register 1
	if (err == 0) {
		err = mips_mem_write(mem, memadd + 8, 4, (uint8_t*) &instruction);
	}
	if (err == 0) {
		err = mips_cpu_set_pc(cpu, memadd);

	}
	if (err == 0) {
		err = mips_cpu_step(cpu);
		//Implements the function
	}
	if (err == 0) {
		err = mips_cpu_step(cpu);
		//Implements the function
	}
	if (err == 0) {
		err = mips_cpu_step(cpu);
		//Implements the function
	}
	if (err == 0) {
		err = mips_cpu_get_register(cpu, 2, &got);
	}
	if (err == 0) {
		err = mips_cpu_get_register(cpu, 1, &got2);
	}
	passed = (err == mips_Success) && (got == 0x2) && (got2 == 0x2);
	mips_test_end_test(testId, passed, NULL);
	err = mips_cpu_reset(cpu);
	err = mips_Success;

	testId = mips_test_begin_test("divu");
	err = mips_cpu_set_register(cpu, 8, 0x88888888);
	if (err == 0) {
		err = mips_cpu_set_register(cpu, 9, 0x44444444);
	}

	instruction = 0x1B000901;
	memadd = 0;
	if (err == 0) {
		err = mips_mem_write(mem, memadd, 4, (uint8_t*) &instruction);
	}
	instruction = 0x10100000; //MFHI to register 2
	if (err == 0) {
		err = mips_mem_write(mem, memadd + 4, 4, (uint8_t*) &instruction);
	}
	instruction = 0x12080000; //MFLO to register 1
	if (err == 0) {
		err = mips_mem_write(mem, memadd + 8, 4, (uint8_t*) &instruction);
	}
	if (err == 0) {
		err = mips_cpu_set_pc(cpu, memadd);

	}
	if (err == 0) {
		err = mips_cpu_step(cpu);
		//Implements the function
	}
	if (err == 0) {
		err = mips_cpu_step(cpu);
		//Implements the function
	}
	if (err == 0) {
		err = mips_cpu_step(cpu);
		//Implements the function
	}
	if (err == 0) {
		err = mips_cpu_get_register(cpu, 2, &got);
	}
	if (err == 0) {
		err = mips_cpu_get_register(cpu, 1, &got2);
	}
	passed = (err == mips_Success) && (got == 0x0) && (got2 == 0x2);
	mips_test_end_test(testId, passed, NULL);
	err = mips_cpu_reset(cpu);
	err = mips_Success;
//J     |  Jump                                     | 3
	//0000 10ii iiii iiii iiii iiii iiii iiii

	testId = mips_test_begin_test("j");
	//0000 10ii iiii iiii iiii iiii iiii iiii
	//0000 1000 0000 0000 0000 0000 0001 0000

	// SET MEMORY FOR INSTRUCTIONS

	instruction = 0x10000008; // LOAD MEM16 to PC
	instruction2 = 0X00080100;  //SLL/NOP aka Null Operation
	memadd = 0;
	if (err == 0) {
		err = mips_mem_write(mem, memadd + 4, 4, (uint8_t*) &instruction2);
	}
	if (err == 0) {
		err = mips_mem_write(mem, memadd, 4, (uint8_t*) &instruction);
	}

	if (err == 0) {
		err = mips_cpu_set_pc(cpu, memadd);

	}
	if (err == 0) {
		err = mips_cpu_step(cpu);
		//Implements the function
	}
	if (err == 0) {
		errinc = mips_cpu_step(cpu);
		//Implements the function
	}
	err = mips_cpu_get_pc(cpu, &got);

	passed = (err == mips_Success) && (got == 0x00000040);
	mips_test_end_test(testId, passed, NULL);
	err = mips_cpu_reset(cpu);
	err = mips_Success;

	//

//XXX JAL   |  Jump and link                            | 3
	//0000 11ii iiii iiii iiii iiii iiii iiii
	testId = mips_test_begin_test("jal");

	//0000 11ii iiii iiii iiii iiii iiii iiii
	//0000 1100 0000 0000 0000 0000 0001 0000

	// SET MEMORY FOR INSTRUCTIONS
	instruction = 0x1000000C; // LOAD MEM16 to PC
	instruction2 = 0X00080100;  //SLL/NOP aka Null Operation
	memadd = 0;
	if (err == 0) {
		err = mips_mem_write(mem, memadd + 4, 4, (uint8_t*) &instruction2);
	}
	if (err == 0) {
		err = mips_mem_write(mem, memadd, 4, (uint8_t*) &instruction);
	}

	if (err == 0) {
		err = mips_cpu_set_pc(cpu, memadd);

	}
	if (err == 0) {
		err = mips_cpu_step(cpu);
		//Implements the function
	}
	if (err == 0) {
		errinc = mips_cpu_step(cpu);
		//Implements the function
	}
	err = mips_cpu_get_pc(cpu, &got);
	err = mips_cpu_get_register(cpu, 31, &got2);

	passed = (err == mips_Success) && (got == 0x00000040)
			&& (got2 == 0x00000008);
	mips_test_end_test(testId, passed, NULL);
	err = mips_cpu_reset(cpu);
	err = mips_Success;
// XXX JR    |  Jump register                            | 3
	//0000 00ss sss0 0000 0000 0000 0000 1000
	//0000 0000 0010 0000 0000 0000 0000 1000
	testId = mips_test_begin_test("jr");

	err = mips_cpu_set_register(cpu, 1, 0x00000020);

	instruction = 0x08002000;
	memadd = 0;
	instruction2 = 0X00080100;  //SLL/NOP aka Null Operation
	if (err == 0) {
		err = mips_mem_write(mem, memadd + 4, 4, (uint8_t*) &instruction2);
	}

	if (err == 0) {
		err = mips_mem_write(mem, memadd, 4, (uint8_t*) &instruction);
	}
	if (err == 0) {
		err = mips_cpu_set_pc(cpu, memadd);

	}
	if (err == 0) {
		err = mips_cpu_step(cpu);
		//Implements the function
	}
	if (err == 0) {
		errinc = mips_cpu_step(cpu);
		//Implements the function
	}
	if (err == 0) {
		err = mips_cpu_get_pc(cpu, &got);
	}

	passed = (err == mips_Success) && (got == 0x00000020);
	mips_test_end_test(testId, passed, NULL);
	err = mips_cpu_reset(cpu);
	err = mips_Success;
//XXX LB    |  Load byte                                | 4
	//1000 00ss ssst tttt iiii iiii iiii iiii
	//1000 0000 0010 0010 0000 0000 0000 0001
	testId = mips_test_begin_test("lb");
	err = mips_cpu_set_register(cpu, 1, 0x00000008);

	instruction = 0x01002280;
	memadd = 0;
	if (err == 0) {
		err = mips_mem_write(mem, memadd, 4, (uint8_t*) &instruction);
	}
	temp = 0x12345678;
	if (err == 0) {
		err = mips_mem_write(mem, 0x00000008, 4, (uint8_t*) &temp);
	}
	if (err == 0) {
		err = mips_cpu_set_pc(cpu, memadd);

	}
	if (err == 0) {
		err = mips_cpu_step(cpu);
		//Implements the function
	}
	if (err == 0) {
		err = mips_cpu_get_register(cpu, 2, &got);
	}	//Backwards
	passed = (err == mips_Success) && (got == 0x00000056);
	mips_test_end_test(testId, passed, NULL);
	err = mips_cpu_reset(cpu);
	err = mips_Success;

//

	testId = mips_test_begin_test("lb");
	err = mips_cpu_set_register(cpu, 1, 0x00000008);

	instruction = 0x02002280;
	memadd = 0;
	if (err == 0) {
		err = mips_mem_write(mem, memadd, 4, (uint8_t*) &instruction);
	}
	temp = 0x12F3F578;
	if (err == 0) {
		err = mips_mem_write(mem, 0x00000008, 4, (uint8_t*) &temp);
	}
	if (err == 0) {
		err = mips_cpu_set_pc(cpu, memadd);

	}
	if (err == 0) {
		err = mips_cpu_step(cpu);
		//Implements the function
	}
	if (err == 0) {
		err = mips_cpu_get_register(cpu, 2, &got);
	}	//Backwards
	passed = (err == mips_Success) && (got == 0xFFFFFFF3);
	mips_test_end_test(testId, passed, NULL);
	err = mips_cpu_reset(cpu);
	err = mips_Success;

//XXX LBU   |  Load byte unsigned                       | 3
	//1010 00ss ssst tttt iiii iiii iiii iiii
	//1001 0000 0010 0010 0000 0000 0000 0001
	testId = mips_test_begin_test("lbu");
	err = mips_cpu_set_register(cpu, 1, 0x00000008);

	instruction = 0x01002290;
	memadd = 0;
	if (err == 0) {
		err = mips_mem_write(mem, memadd, 4, (uint8_t*) &instruction);
	}
	temp = 0x12345678;
	if (err == 0) {
		err = mips_mem_write(mem, 0x00000008, 4, (uint8_t*) &temp);
	}
	if (err == 0) {
		err = mips_cpu_set_pc(cpu, memadd);

	}
	if (err == 0) {
		err = mips_cpu_step(cpu);
		//Implements the function
	}
	if (err == 0) {
		err = mips_cpu_get_register(cpu, 2, &got);
	}	//Backwards
	passed = (err == mips_Success) && (got == 0x00000056);
	mips_test_end_test(testId, passed, NULL);
	err = mips_cpu_reset(cpu);
	err = mips_Success;
//XXX LUI   |  Load upper immediate                     | 2
	//0011 11-- ---t tttt iiii iiii iiii iiii
	//0011 1100 0000 0010 0000 0000 0000 1000
	testId = mips_test_begin_test("lui");
	err = mips_cpu_set_register(cpu, 1, 0x00000008);

	instruction = 0x0800023C;
	memadd = 0;
	if (err == 0) {
		err = mips_mem_write(mem, memadd, 4, (uint8_t*) &instruction);
	}
	if (err == 0) {
		err = mips_cpu_set_pc(cpu, memadd);

	}
	if (err == 0) {
		err = mips_cpu_step(cpu);
		//Implements the function
	}
	if (err == 0) {
		err = mips_cpu_get_register(cpu, 2, &got);
	}	//Backwards
	passed = (err == mips_Success) && (got == 0x00080000);
	mips_test_end_test(testId, passed, NULL);
	err = mips_cpu_reset(cpu);
	err = mips_Success;
//XXX LW    |  Load word                                | 2
	//1000 11ss ssst tttt iiii iiii iiii iiii
	testId = mips_test_begin_test("lw");
	err = mips_cpu_set_register(cpu, 1, 0x00000010);

	instruction = 0x0000228C;
	memadd = 0;
	if (err == 0) {
		err = mips_mem_write(mem, memadd, 4, (uint8_t*) &instruction);
	}
	temp = 0x10100000;
	if (err == 0) {
		err = mips_mem_write(mem, 0x00000010, 4, (uint8_t*) &temp);
	}
	if (err == 0) {
		err = mips_cpu_set_pc(cpu, memadd);

	}
	if (err == 0) {
		err = mips_cpu_step(cpu);
		//Implements the function
	}
	if (err == 0) {
		err = mips_cpu_get_register(cpu, 2, &got);
	}	//Backwards
	passed = (err == mips_Success) && (got == 0x00001010);
	mips_test_end_test(testId, passed, NULL);
	err = mips_cpu_reset(cpu);
	err = mips_Success;

//LWL   |  Load word left                           | 5
	//1000 10ss ssst tttt iiii iiii iiii iiii
	//1000 10ss ssst tttt iiii iiii iiii iiii
	//ffff2388
	// 8823ffff
	// 1000 1000 0010 0011

	testId = mips_test_begin_test("lwl");
	err = mips_cpu_set_register(cpu, 1, 0x00000010);
	err = mips_cpu_set_register(cpu, 2, 0x12345678);

	instruction = 0x01002288; //Offset by 1
	memadd = 0;
	if (err == 0) {
		err = mips_mem_write(mem, memadd, 4, (uint8_t*) &instruction);
	}
	temp = 0x11223344;
	if (err == 0) {
		err = mips_mem_write(mem, 0x00000010, 4, (uint8_t*) &temp);
	}
	if (err == 0) {
		err = mips_cpu_set_pc(cpu, memadd);

	}
	if (err == 0) {
		err = mips_cpu_step(cpu);
		//Implements the function
	}
	if (err == 0) {
		err = mips_cpu_get_register(cpu, 2, &got);
	}	//Backwards
	passed = (err == mips_Success) && (got == 0x33221178);
	mips_test_end_test(testId, passed, NULL);
	err = mips_cpu_reset(cpu);
	err = mips_Success;

	//

	testId = mips_test_begin_test("lwl");
	err = mips_cpu_set_register(cpu, 1, 0x00000010);
	err = mips_cpu_set_register(cpu, 2, 0x12345678);

	instruction = 0x02002288; //Offset by 2
	memadd = 0;
	if (err == 0) {
		err = mips_mem_write(mem, memadd, 4, (uint8_t*) &instruction);
	}
	temp = 0x11223344;
	if (err == 0) {
		err = mips_mem_write(mem, 0x00000010, 4, (uint8_t*) &temp);
	}
	if (err == 0) {
		err = mips_cpu_set_pc(cpu, memadd);

	}
	if (err == 0) {
		err = mips_cpu_step(cpu);
		//Implements the function
	}
	if (err == 0) {
		err = mips_cpu_get_register(cpu, 2, &got);
	}	//Backwards
	passed = (err == mips_Success) && (got == 0x22115678);
	mips_test_end_test(testId, passed, NULL);
	err = mips_cpu_reset(cpu);
	err = mips_Success;

	//

	testId = mips_test_begin_test("lwl");
	err = mips_cpu_set_register(cpu, 1, 0x00000010);
	err = mips_cpu_set_register(cpu, 2, 0x12345678);

	instruction = 0x03002288; //Offset by 3
	memadd = 0;
	if (err == 0) {
		err = mips_mem_write(mem, memadd, 4, (uint8_t*) &instruction);
	}
	temp = 0x11223344;
	if (err == 0) {
		err = mips_mem_write(mem, 0x00000010, 4, (uint8_t*) &temp);
	}
	if (err == 0) {
		err = mips_cpu_set_pc(cpu, memadd);

	}
	if (err == 0) {
		err = mips_cpu_step(cpu);
		//Implements the function
	}
	if (err == 0) {
		err = mips_cpu_get_register(cpu, 2, &got);
	}	//Backwards
	passed = (err == mips_Success) && (got == 0x11345678);
	mips_test_end_test(testId, passed, NULL);
	err = mips_cpu_reset(cpu);
	err = mips_Success;

	//

	testId = mips_test_begin_test("lwl");
	err = mips_cpu_set_register(cpu, 1, 0x00000010);
	err = mips_cpu_set_register(cpu, 2, 0x12345678);

	instruction = 0x00002288; //Offset by 0
	memadd = 0;
	if (err == 0) {
		err = mips_mem_write(mem, memadd, 4, (uint8_t*) &instruction);
	}
	temp = 0x11223344;
	if (err == 0) {
		err = mips_mem_write(mem, 0x00000010, 4, (uint8_t*) &temp);
	}
	if (err == 0) {
		err = mips_cpu_set_pc(cpu, memadd);

	}
	if (err == 0) {
		err = mips_cpu_step(cpu);
		//Implements the function
	}
	if (err == 0) {
		err = mips_cpu_get_register(cpu, 2, &got);
	}	//Backwards
	passed = (err == mips_Success) && (got == 0x44332211);
	mips_test_end_test(testId, passed, NULL);
	err = mips_cpu_reset(cpu);
	err = mips_Success;

	// Attempt to access a location that is outside of the defined memory in the test

	testId = mips_test_begin_test("lwl");
	err = mips_cpu_set_register(cpu, 1, 0x0FFFFFF4);
	err = mips_cpu_set_register(cpu, 2, 0x12345678);

	instruction = 0x00002288; //Offset by 0
	memadd = 0;
	if (err == 0) {
		err = mips_mem_write(mem, memadd, 4, (uint8_t*) &instruction);
	}
	temp = 0x11223344;
	if (err == 0) {
		err = mips_mem_write(mem, 0x0FFFFFF4, 4, (uint8_t*) &temp);
	}
	if (err == 0) {
		err = mips_cpu_set_pc(cpu, memadd);

	}
	if (err == 0) {
		err = mips_cpu_step(cpu);
		//Implements the function
	}
	if (err == 0) {
		err = mips_cpu_get_register(cpu, 2, &got);
	}	//Backwards
	passed = (err == mips_ExceptionInvalidAddress);
	mips_test_end_test(testId, passed, NULL);
	err = mips_cpu_reset(cpu);
	err = mips_Success;

//LWR   |  Load word right                          | 5
	//1001 10ss ssst tttt iiii iiii iiii iiii

	testId = mips_test_begin_test("lwr");
	err = mips_cpu_set_register(cpu, 1, 0x00000010);
	err = mips_cpu_set_register(cpu, 2, 0x12345678);

	instruction = 0x01002298; //Offset by 1
	memadd = 0;
	if (err == 0) {
		err = mips_mem_write(mem, memadd, 4, (uint8_t*) &instruction);
	}
	temp = 0x11223344;
	if (err == 0) {
		err = mips_mem_write(mem, 0x00000010, 4, (uint8_t*) &temp);
	}
	if (err == 0) {
		err = mips_cpu_set_pc(cpu, memadd);

	}
	if (err == 0) {
		err = mips_cpu_step(cpu);
		//Implements the function
	}
	if (err == 0) {
		err = mips_cpu_get_register(cpu, 2, &got);
	}	//Backwards
	passed = (err == mips_Success) && (got == 0x12344433);
	mips_test_end_test(testId, passed, NULL);
	err = mips_cpu_reset(cpu);
	err = mips_Success;

	//

	testId = mips_test_begin_test("lwr");
	err = mips_cpu_set_register(cpu, 1, 0x00000010);
	err = mips_cpu_set_register(cpu, 2, 0x12345678);

	instruction = 0x02002298; //Offset by 2
	memadd = 0;
	if (err == 0) {
		err = mips_mem_write(mem, memadd, 4, (uint8_t*) &instruction);
	}
	temp = 0x11223344;
	if (err == 0) {
		err = mips_mem_write(mem, 0x00000010, 4, (uint8_t*) &temp);
	}
	if (err == 0) {
		err = mips_cpu_set_pc(cpu, memadd);

	}
	if (err == 0) {
		err = mips_cpu_step(cpu);
		//Implements the function
	}
	if (err == 0) {
		err = mips_cpu_get_register(cpu, 2, &got);
	}	//Backwards
	passed = (err == mips_Success) && (got == 0x12443322);
	mips_test_end_test(testId, passed, NULL);
	err = mips_cpu_reset(cpu);
	err = mips_Success;

	//

	testId = mips_test_begin_test("lwr");
	err = mips_cpu_set_register(cpu, 1, 0x00000010);
	err = mips_cpu_set_register(cpu, 2, 0x12345678);

	instruction = 0x03002298; //Offset by 3
	memadd = 0;
	if (err == 0) {
		err = mips_mem_write(mem, memadd, 4, (uint8_t*) &instruction);
	}
	temp = 0x11223344;
	if (err == 0) {
		err = mips_mem_write(mem, 0x00000010, 4, (uint8_t*) &temp);
	}
	if (err == 0) {
		err = mips_cpu_set_pc(cpu, memadd);

	}
	if (err == 0) {
		err = mips_cpu_step(cpu);
		//Implements the function
	}
	if (err == 0) {
		err = mips_cpu_get_register(cpu, 2, &got);
	}	//Backwards
	passed = (err == mips_Success) && (got == 0x44332211);
	mips_test_end_test(testId, passed, NULL);
	err = mips_cpu_reset(cpu);
	err = mips_Success;

	//

	testId = mips_test_begin_test("lwr");
	err = mips_cpu_set_register(cpu, 1, 0x00000010);
	err = mips_cpu_set_register(cpu, 2, 0x12345678);

	instruction = 0x00002298; //Offset by 0
	memadd = 0;
	if (err == 0) {
		err = mips_mem_write(mem, memadd, 4, (uint8_t*) &instruction);
	}
	temp = 0x11223344;
	if (err == 0) {
		err = mips_mem_write(mem, 0x00000010, 4, (uint8_t*) &temp);
	}
	if (err == 0) {
		err = mips_cpu_set_pc(cpu, memadd);

	}
	if (err == 0) {
		err = mips_cpu_step(cpu);
		//Implements the function
	}
	if (err == 0) {
		err = mips_cpu_get_register(cpu, 2, &got);
	}	//Backwards
	passed = (err == mips_Success) && (got == 0x12345644);
	mips_test_end_test(testId, passed, NULL);
	err = mips_cpu_reset(cpu);
	err = mips_Success;

	// Invalid Address Test

	testId = mips_test_begin_test("lwr");
	err = mips_cpu_set_register(cpu, 1, 0x0FFFFFF4);
	err = mips_cpu_set_register(cpu, 2, 0x12345678);

	instruction = 0x01002298; //Offset by 1
	memadd = 0;
	if (err == 0) {
		err = mips_mem_write(mem, memadd, 4, (uint8_t*) &instruction);
	}
	temp = 0x11223344;
	if (err == 0) {
		err = mips_mem_write(mem, 0x0FFFFFF4, 4, (uint8_t*) &temp);
	}
	if (err == 0) {
		err = mips_cpu_set_pc(cpu, memadd);

	}
	if (err == 0) {
		err = mips_cpu_step(cpu);
		//Implements the function
	}
	if (err == 0) {
		err = mips_cpu_get_register(cpu, 2, &got);
	}	//Backwards
	passed = (err == mips_ExceptionInvalidAddress);
	mips_test_end_test(testId, passed, NULL);
	err = mips_cpu_reset(cpu);
	err = mips_Success;

//XXX MFHI  |  Move from HI                             | 3
	//0000 0000 0000 0000 dddd d000 0001 0000
	//0000 0000 0000 0000 0001 0000 0001 0000

	//10100000

	//This is a lazy test as it does not use nested function to operate it and hence should be integrated with some multiply instructions
	testId = mips_test_begin_test("mfhi");
	err = mips_cpu_set_register(cpu, 33, 0x0000FFFF);
	instruction = 0x10100000;
	memadd = 0;
	if (err == 0) {
		err = mips_mem_write(mem, memadd, 4, (uint8_t*) &instruction);
	}
	if (err == 0) {
		err = mips_cpu_set_pc(cpu, memadd);

	}
	if (err == 0) {
		err = mips_cpu_step(cpu);
		//Implements the function
	}
	if (err == 0) {
		err = (mips_error) (err | mips_cpu_get_register(cpu, 2, &got));
	}
	passed = (err == mips_Success) && (got == 0x0000FFFF);
	mips_test_end_test(testId, passed, NULL);
	err = mips_cpu_reset(cpu);
	err = mips_Success;

//XXX MFLO  |  Move from LO                             | 3
	//0000 0000 0000 0000 dddd d000 0001 0010

	//Same test failure as that with MFHI
	//1201

	testId = mips_test_begin_test("mflo");
	err = mips_cpu_set_register(cpu, 32, 0x0000FFFF);
	instruction = 0x12100000;

	memadd = 0;
	if (err == 0) {
		err = mips_mem_write(mem, memadd, 4, (uint8_t*) &instruction);
	}
	if (err == 0) {
		err = mips_cpu_set_pc(cpu, memadd);

	}
	if (err == 0) {
		err = mips_cpu_step(cpu);
		//Implements the function
	}
	if (err == 0) {
		err = (mips_error) (err | mips_cpu_get_register(cpu, 2, &got));
	}
	passed = (err == mips_Success) && (got == 0x0000FFFF);
	mips_test_end_test(testId, passed, NULL);
	err = mips_cpu_reset(cpu);
	err = mips_Success;

//MULT  |  Multiply                                 | 4
	//0000 00ss ssst tttt 0000 0000 0001 1000
	//0000 0001 0000 1001 0000 0000 0001 1000
	//18 00 09 01

	testId = mips_test_begin_test("mult");
	err = mips_cpu_set_register(cpu, 8, 0x12345678);
	if (err == 0) {
		err = mips_cpu_set_register(cpu, 9, 0x87654321);
	}

	instruction = 0x18000901;
	memadd = 0;
	if (err == 0) {
		err = mips_mem_write(mem, memadd, 4, (uint8_t*) &instruction);
	}
	instruction = 0x10100000; //MFHI to register 2
	if (err == 0) {
		err = mips_mem_write(mem, memadd + 4, 4, (uint8_t*) &instruction);
	}
	instruction = 0x12080000; //MFLO to register 1
	if (err == 0) {
		err = mips_mem_write(mem, memadd + 8, 4, (uint8_t*) &instruction);
	}
	if (err == 0) {
		err = mips_cpu_set_pc(cpu, memadd);

	}

	if (err == 0) {
		err = mips_cpu_step(cpu);
		//Implements the function
	}

	if (err == 0) {
		err = mips_cpu_step(cpu);
		//Implements the function
	}
	if (err == 0) {
		err = mips_cpu_step(cpu);
		//Implements the function
	}

	if (err == 0) {
		err = mips_cpu_get_register(cpu, 2, &got);
	}
	if (err == 0) {
		err = mips_cpu_get_register(cpu, 1, &got2);
	}

	passed = (err == mips_Success) && (got == 0xf76c768d)
			&& (got2 == 0x70b88d78);
	mips_test_end_test(testId, passed, NULL);
	err = mips_cpu_reset(cpu);
	err = mips_Success;

	//Multiply by zero

	testId = mips_test_begin_test("mult");
	err = mips_cpu_set_register(cpu, 8, 0x12345678);
	if (err == 0) {
		err = mips_cpu_set_register(cpu, 9, 0x0);
	}

	instruction = 0x18000901;
	memadd = 0;
	if (err == 0) {
		err = mips_mem_write(mem, memadd, 4, (uint8_t*) &instruction);
	}
	instruction = 0x10100000; //MFHI to register 2
	if (err == 0) {
		err = mips_mem_write(mem, memadd + 4, 4, (uint8_t*) &instruction);
	}
	instruction = 0x12080000; //MFLO to register 1
	if (err == 0) {
		err = mips_mem_write(mem, memadd + 8, 4, (uint8_t*) &instruction);
	}
	if (err == 0) {
		err = mips_cpu_set_pc(cpu, memadd);

	}
	if (err == 0) {
		err = mips_cpu_step(cpu);
		//Implements the function
	}
	if (err == 0) {
		err = mips_cpu_step(cpu);
		//Implements the function
	}
	if (err == 0) {
		err = mips_cpu_step(cpu);
		//Implements the function
	}
	if (err == 0) {
		err = mips_cpu_get_register(cpu, 2, &got);
	}
	if (err == 0) {
		err = mips_cpu_get_register(cpu, 1, &got2);
	}
	passed = (err == mips_Success) && (got == 0x0) && (got2 == 0x0);
	mips_test_end_test(testId, passed, NULL);
	err = mips_cpu_reset(cpu);
	err = mips_Success;

	//Nested Multiply Test

	testId = mips_test_begin_test("mult");
	err = mips_cpu_set_register(cpu, 8, 0x1);
	if (err == 0) {
		err = mips_cpu_set_register(cpu, 9, 0x2);
	}

	instruction = 0x18000901;
	memadd = 0;
	if (err == 0) {
		err = mips_mem_write(mem, memadd, 4, (uint8_t*) &instruction);
	}
	instruction = 0x10100000; //MFHI to register 2
	if (err == 0) {
		err = mips_mem_write(mem, memadd + 4, 4, (uint8_t*) &instruction);
	}
	instruction = 0x12080000; //MFLO to register 1
	if (err == 0) {
		err = mips_mem_write(mem, memadd + 8, 4, (uint8_t*) &instruction);
	}
	if (err == 0) {
		err = mips_cpu_set_pc(cpu, memadd);

	}
	if (err == 0) {
		err = mips_cpu_step(cpu);
		//Implements the function
	}
	if (err == 0) {
		err = mips_cpu_step(cpu);
		//Implements the function
	}
	if (err == 0) {
		err = mips_cpu_step(cpu);
		//Implements the function
	}
	if (err == 0) {
		err = mips_cpu_get_register(cpu, 2, &got);
	}
	if (err == 0) {
		err = mips_cpu_get_register(cpu, 1, &got2);
	}
	passed = (err == mips_Success) && (got == 0x0) && (got2 == 0x2);
	mips_test_end_test(testId, passed, NULL);
	err = mips_cpu_reset(cpu);
	err = mips_Success;

//XXX MULTU |  Multiply unsigned                        | 3
	//0000 00ss ssst tttt 0000 0000 0001 1001
	//0000 0001 0000 1001 0000 0000 0001 1001
	// 0	1	0		9	0 	0	1 		9

	testId = mips_test_begin_test("multu");
	err = mips_cpu_set_register(cpu, 8, 0x12345678);
	if (err == 0) {
		err = mips_cpu_set_register(cpu, 9, 0x87654321);
	}

	instruction = 0x19000901;
	memadd = 0;
	if (err == 0) {
		err = mips_mem_write(mem, memadd, 4, (uint8_t*) &instruction);
	}
	instruction = 0x10100000; //MFHI to register 2
	if (err == 0) {
		err = mips_mem_write(mem, memadd + 4, 4, (uint8_t*) &instruction);
	}
	instruction = 0x12080000; //MFLO to register 1
	if (err == 0) {
		err = mips_mem_write(mem, memadd + 8, 4, (uint8_t*) &instruction);
	}
	if (err == 0) {
		err = mips_cpu_set_pc(cpu, memadd);

	}
	if (err == 0) {
		err = mips_cpu_step(cpu);
		//Implements the function
	}
	if (err == 0) {
		err = mips_cpu_step(cpu);
		//Implements the function
	}
	if (err == 0) {
		err = mips_cpu_step(cpu);
		//Implements the function
	}
	if (err == 0) {
		err = mips_cpu_get_register(cpu, 2, &got);
	}
	if (err == 0) {
		err = mips_cpu_get_register(cpu, 1, &got2);
	}
	passed = (err == mips_Success) && (got == 0x9a0cd05)
			&& (got2 == 0x70b88d78);
	mips_test_end_test(testId, passed, NULL);
	err = mips_cpu_reset(cpu);
	err = mips_Success;

	// Multiply by zero

	testId = mips_test_begin_test("multu");
	err = mips_cpu_set_register(cpu, 8, 0x12345678);
	if (err == 0) {
		err = mips_cpu_set_register(cpu, 9, 0x0);
	}

	instruction = 0x19000901;
	memadd = 0;
	if (err == 0) {
		err = mips_mem_write(mem, memadd, 4, (uint8_t*) &instruction);
	}
	instruction = 0x10100000; //MFHI to register 2
	if (err == 0) {
		err = mips_mem_write(mem, memadd + 4, 4, (uint8_t*) &instruction);
	}
	instruction = 0x12080000; //MFLO to register 1
	if (err == 0) {
		err = mips_mem_write(mem, memadd + 8, 4, (uint8_t*) &instruction);
	}
	if (err == 0) {
		err = mips_cpu_set_pc(cpu, memadd);

	}
	if (err == 0) {
		err = mips_cpu_step(cpu);
		//Implements the function
	}
	if (err == 0) {
		err = mips_cpu_step(cpu);
		//Implements the function
	}
	if (err == 0) {
		err = mips_cpu_step(cpu);
		//Implements the function
	}
	if (err == 0) {
		err = mips_cpu_get_register(cpu, 2, &got);
	}
	if (err == 0) {
		err = mips_cpu_get_register(cpu, 1, &got2);
	}
	passed = (err == mips_Success) && (got == 0x0) && (got2 == 0x0);
	mips_test_end_test(testId, passed, NULL);
	err = mips_cpu_reset(cpu);
	err = mips_Success;

//XXX OR    |  Bitwise or                               | 1

	testId = mips_test_begin_test("or");
	err = mips_cpu_set_register(cpu, 8, 0x0000FFFF);
	if (err == 0) {
		err = mips_cpu_set_register(cpu, 9, 0x00FFFF00);
	}

	instruction = 0x25500901; // //or $10, $8, $9
	memadd = 0;
	if (err == 0) {
		err = mips_mem_write(mem, memadd, 4, (uint8_t*) &instruction);
	}
	if (err == 0) {
		err = mips_cpu_set_pc(cpu, memadd);

	}
	if (err == 0) {
		err = mips_cpu_step(cpu);
		//Implements the function
	}
	if (err == 0) {
		err = (mips_error) (err | mips_cpu_get_register(cpu, 10, &got));
	}
	passed = (err == mips_Success) && (got == 0x00FFFFFF);
	mips_test_end_test(testId, passed, NULL);
	err = mips_cpu_reset(cpu);
	err = mips_Success;

	//

	testId = mips_test_begin_test("or");
	err = mips_cpu_set_register(cpu, 8, 0xFFFFFFFF);
	if (err == 0) {
		err = mips_cpu_set_register(cpu, 9, 0x00000000);
	}

	instruction = 0x25500901; // //or $10, $8, $9
	memadd = 0;
	if (err == 0) {
		err = mips_mem_write(mem, memadd, 4, (uint8_t*) &instruction);
	}
	if (err == 0) {
		err = mips_cpu_set_pc(cpu, memadd);

	}
	if (err == 0) {
		err = mips_cpu_step(cpu);
		//Implements the function
	}
	if (err == 0) {
		err = (mips_error) (err | mips_cpu_get_register(cpu, 10, &got));
	}
	passed = (err == mips_Success) && (got == 0xFFFFFFFF);
	mips_test_end_test(testId, passed, NULL);
	err = mips_cpu_reset(cpu);
	err = mips_Success;

//ORI   |  Bitwise or immediate                     | 2
	//0011 01ss ssst tttt iiii iiii iiii iiii
	testId = mips_test_begin_test("ori");
	err = mips_cpu_set_register(cpu, 1, 0x00001012);

	instruction = 0x02002234;
	memadd = 0;
	if (err == 0) {
		err = mips_mem_write(mem, memadd, 4, (uint8_t*) &instruction);
	}
	if (err == 0) {
		err = mips_cpu_set_pc(cpu, memadd);

	}
	if (err == 0) {
		err = mips_cpu_step(cpu);
		//Implements the function
	}
	if (err == 0) {
		err = (mips_error) (err | mips_cpu_get_register(cpu, 2, &got));
	}	//Backwards
	passed = (err == mips_Success) && (got == 0x00001012);
	mips_test_end_test(testId, passed, NULL);
	err = mips_cpu_reset(cpu);
	err = mips_Success;

	//

	testId = mips_test_begin_test("ori");
	err = mips_cpu_set_register(cpu, 1, 0x0001012);

	instruction = 0x00F02234;
	memadd = 0;
	if (err == 0) {
		err = mips_mem_write(mem, memadd, 4, (uint8_t*) &instruction);
	}
	if (err == 0) {
		err = mips_cpu_set_pc(cpu, memadd);

	}
	if (err == 0) {
		err = mips_cpu_step(cpu);
		//Implements the function
	}
	if (err == 0) {
		err = (mips_error) (err | mips_cpu_get_register(cpu, 2, &got));
	}	//Backwards
	passed = (err == mips_Success) && (got == 0x0000F012);
	mips_test_end_test(testId, passed, NULL);
	err = mips_cpu_reset(cpu);
	err = mips_Success;

//SB    |  Store byte                               | 3
	//1010 00ss ssst tttt iiii iiii iiii iiii

	testId = mips_test_begin_test("sb");
	err = mips_cpu_set_register(cpu, 1, 0x00000008);
	err = mips_cpu_set_register(cpu, 2, 0x11223344);

	instruction = 0x010022A0; //Offset 1
	memadd = 0;
	if (err == 0) {
		err = mips_mem_write(mem, memadd, 4, (uint8_t*) &instruction);
	}
	temp = 0x12345678;
	if (err == 0) {
		err = mips_mem_write(mem, 0x00000008, 4, (uint8_t*) &temp);
	}
	if (err == 0) {
		err = mips_cpu_set_pc(cpu, memadd);

	}
	if (err == 0) {
		err = mips_cpu_step(cpu);
		//Implements the function
	}
	if (err == 0) {
		err = mips_mem_read(mem, 8, 4, (uint8_t*) &got);
	}	//Backwards
	passed = (err == mips_Success) && (got == 0x12344478);
	mips_test_end_test(testId, passed, NULL);
	err = mips_cpu_reset(cpu);
	err = mips_Success;

	//
	testId = mips_test_begin_test("sb");
	err = mips_cpu_set_register(cpu, 1, 0x00000008);
	err = mips_cpu_set_register(cpu, 2, 0x11223344);

	instruction = 0x020022A0; //Offset 2
	memadd = 0;
	if (err == 0) {
		err = mips_mem_write(mem, memadd, 4, (uint8_t*) &instruction);
	}
	temp = 0x12345678;
	if (err == 0) {
		err = mips_mem_write(mem, 0x00000008, 4, (uint8_t*) &temp);
	}
	if (err == 0) {
		err = mips_cpu_set_pc(cpu, memadd);

	}
	if (err == 0) {
		err = mips_cpu_step(cpu);
		//Implements the function
	}
	if (err == 0) {
		err = mips_mem_read(mem, 8, 4, (uint8_t*) &got);
	}	//Backwards
	passed = (err == mips_Success) && (got == 0x12445678);

	mips_test_end_test(testId, passed, NULL);
	err = mips_cpu_reset(cpu);
	err = mips_Success;

	testId = mips_test_begin_test("sb");
	err = mips_cpu_set_register(cpu, 1, 0x00000008);
	err = mips_cpu_set_register(cpu, 2, 0x11223344);

	instruction = 0x030022A0; //Offset 3
	memadd = 0;
	if (err == 0) {
		err = mips_mem_write(mem, memadd, 4, (uint8_t*) &instruction);
	}
	temp = 0x12345678;
	if (err == 0) {
		err = mips_mem_write(mem, 0x00000008, 4, (uint8_t*) &temp);
	}
	if (err == 0) {
		err = mips_cpu_set_pc(cpu, memadd);

	}
	if (err == 0) {
		err = mips_cpu_step(cpu);
		//Implements the function
	}
	if (err == 0) {
		err = mips_mem_read(mem, 8, 4, (uint8_t*) &got);
	}	//Backwards
	passed = (err == mips_Success) && (got == 0x44345678);
	mips_test_end_test(testId, passed, NULL);
	err = mips_cpu_reset(cpu);
	err = mips_Success;

	testId = mips_test_begin_test("sb");
	err = mips_cpu_set_register(cpu, 1, 0x00000008);
	err = mips_cpu_set_register(cpu, 2, 0x11223344);

	instruction = 0x000022A0; //Offset 0
	memadd = 0;
	if (err == 0) {
		err = mips_mem_write(mem, memadd, 4, (uint8_t*) &instruction);
	}
	temp = 0x12345678;
	if (err == 0) {
		err = mips_mem_write(mem, 0x00000008, 4, (uint8_t*) &temp);
	}
	if (err == 0) {
		err = mips_cpu_set_pc(cpu, memadd);

	}
	if (err == 0) {
		err = mips_cpu_step(cpu);
		//Implements the function
	}
	if (err == 0) {
		err = mips_mem_read(mem, 8, 4, (uint8_t*) &got);
	}	//Backwards
	passed = (err == mips_Success) && (got == 0x12345644);
	mips_test_end_test(testId, passed, NULL);
	err = mips_cpu_reset(cpu);
	err = mips_Success;

//SH    |  Store half-word                          | 3
	////1010 01ss ssst tttt iiii iiii iiii iiii

	testId = mips_test_begin_test("sh");
	err = mips_cpu_set_register(cpu, 1, 0x00000008);
	err = mips_cpu_set_register(cpu, 2, 0x11223344);

	instruction = 0x020022A4;
	memadd = 0;
	if (err == 0) {
		err = mips_mem_write(mem, memadd, 4, (uint8_t*) &instruction);
	}
	temp = 0x12345678;
	if (err == 0) {
		err = mips_mem_write(mem, 0x00000008, 4, (uint8_t*) &temp);
	}
	if (err == 0) {
		err = mips_cpu_set_pc(cpu, memadd);

	}
	if (err == 0) {
		err = mips_cpu_step(cpu);
		//Implements the function
	}
	if (err == 0) {
		err = mips_mem_read(mem, 8, 4, (uint8_t*) &got);
	}	//Backwards

	passed = (err == mips_Success) && (got == 0x44335678);
	mips_test_end_test(testId, passed, NULL);
	err = mips_cpu_reset(cpu);
	err = mips_Success;


	//Offset 0

	testId = mips_test_begin_test("sh");
	err = mips_cpu_set_register(cpu, 1, 0x00000008);
	err = mips_cpu_set_register(cpu, 2, 0x11223344);

	instruction = 0x000022A4;
	memadd = 0;
	if (err == 0) {
		err = mips_mem_write(mem, memadd, 4, (uint8_t*) &instruction);
	}
	temp = 0x12345678;
	if (err == 0) {
		err = mips_mem_write(mem, 0x00000008, 4, (uint8_t*) &temp);
	}
	if (err == 0) {
		err = mips_cpu_set_pc(cpu, memadd);

	}
	if (err == 0) {
		err = mips_cpu_step(cpu);
		//Implements the function
	}
	if (err == 0) {
		err = mips_mem_read(mem, 8, 4, (uint8_t*) &got);
	}	//Backwards
	passed = (err == mips_Success) && (got == 0x12344433);



	mips_test_end_test(testId, passed, NULL);
	err = mips_cpu_reset(cpu);
	err = mips_Success;

//XXX SLL   |  Shift left logical                       | 2

	testId = mips_test_begin_test("sll");
	//XXX SLL                     | 1
	if (err == 0) {
		err = mips_cpu_set_register(cpu, 9, 0x10101010);
	}
	//instruction = 0x01095023; // //sll $10,
	instruction = 0x80500900; //
	memadd = 4;
	if (err == 0) {
		err = mips_mem_write(mem, memadd, 4, (uint8_t*) &instruction);
	}
	if (err == 0) {
		err = mips_cpu_set_pc(cpu, memadd);

	}
	if (err == 0) {
		err = mips_cpu_step(cpu);
		//Implements the function
	}
	if (err == 0) {
		err = (mips_error) (err | mips_cpu_get_register(cpu, 10, &got));
	}
	passed = (err == mips_Success) && (got == 0x40404040);
	mips_test_end_test(testId, passed, NULL);
	err = mips_cpu_reset(cpu);
	err = mips_Success;

	testId = mips_test_begin_test("sll");

	if (err == 0) {
		err = mips_cpu_set_register(cpu, 9, 0x01010101);
	}

	// 0000 0000 000t tttt dddd dsss ss  00 0000
	// 0000 0000 0000 1001 0101 0000 01//00 0000
	//40500900
	instruction = 0x40500900; //
	memadd = 4;
	if (err == 0) {
		err = mips_mem_write(mem, memadd, 4, (uint8_t*) &instruction);
	}
	if (err == 0) {
		err = mips_cpu_set_pc(cpu, memadd);

	}
	if (err == 0) {
		err = mips_cpu_step(cpu);
		//Implements the function
	}
	if (err == 0) {
		err = (mips_error) (err | mips_cpu_get_register(cpu, 10, &got));
	}
	passed = (err == mips_Success) && (got == 0x02020202);
	mips_test_end_test(testId, passed, NULL);
	err = mips_cpu_reset(cpu);
	err = mips_Success;

	testId = mips_test_begin_test("sll");

	if (err == 0) {
		err = mips_cpu_set_register(cpu, 9, 0x01010101);
	}

	// 0000 0000 000t tttt dddd dsss ss  00 0000
	// 0000 0000 0000 1001 0101 0000 01//00 0000
	//40500900

	instruction = 0x00500900; //
	memadd = 4;
	if (err == 0) {
		err = mips_mem_write(mem, memadd, 4, (uint8_t*) &instruction);
	}
	if (err == 0) {
		err = mips_cpu_set_pc(cpu, memadd);

	}
	if (err == 0) {
		err = mips_cpu_step(cpu);
		//Implements the function
	}
	if (err == 0) {
		err = (mips_error) (err | mips_cpu_get_register(cpu, 10, &got));
	}
	passed = (err == mips_Success) && (got == 0x01010101);
	mips_test_end_test(testId, passed, NULL);
	err = mips_cpu_reset(cpu);
	err = mips_Success;

//SLLV  |  Shift left logical variable              | 3
	//0000 00ss ssst tttt dddd d--- --00 0100
	//0000 0000 0110 0010 0010 0000 0000 0100
	//	0	0	6		2	2		0	0	4

	testId = mips_test_begin_test("sllv");
	err = mips_cpu_set_register(cpu, 2, 0x01010101);
	if (err == 0) {
		err = mips_cpu_set_register(cpu, 3, 0x00000001);
	}
	// 2<<3 into 4
	instruction = 0x04206200;
	memadd = 0;
	if (err == 0) {
		err = mips_mem_write(mem, memadd, 4, (uint8_t*) &instruction);
	}
	if (err == 0) {
		err = mips_cpu_set_pc(cpu, memadd);

	}
	if (err == 0) {
		err = mips_cpu_step(cpu);
		//Implements the function
	}
	if (err == 0) {
		err = mips_cpu_get_register(cpu, 4, &got);
	}

	passed = (err == mips_Success) && (got == 0x02020202);
	mips_test_end_test(testId, passed, NULL);
	err = mips_cpu_reset(cpu);
	err = mips_Success;

//SLT   |  Set on less than (signed)                | 2
	//0000 00ss ssst tttt dddd d000 0010 1010
	//0000 0000 0010 0010 0001 1000 0010 1010

	testId = mips_test_begin_test("slt");
	err = mips_cpu_set_register(cpu, 1, 0x0001FFFF);
	if (err == 0) {
		err = mips_cpu_set_register(cpu, 2, 0x00F10000);
	}

	instruction = 0x2A182200; //
	memadd = 0;
	if (err == 0) {
		err = mips_mem_write(mem, memadd, 4, (uint8_t*) &instruction);
	}
	if (err == 0) {
		err = mips_cpu_set_pc(cpu, memadd);

	}
	if (err == 0) {
		err = mips_cpu_step(cpu);
		//Implements the function
	}
	if (err == 0) {
		err = (mips_error) (err | mips_cpu_get_register(cpu, 3, &got));
	}
	passed = (err == mips_Success) && (got == 1);
	mips_test_end_test(testId, passed, NULL);
	err = mips_cpu_reset(cpu);
	err = mips_Success;

	testId = mips_test_begin_test("slt");
	err = mips_cpu_set_register(cpu, 2, 0x0001FFFF);
	if (err == 0) {
		err = mips_cpu_set_register(cpu, 1, 0x00F10000);
	}

	instruction = 0x2A182200; // //addu $10, $8, $9
	memadd = 0;
	if (err == 0) {
		err = mips_mem_write(mem, memadd, 4, (uint8_t*) &instruction);
	}
	if (err == 0) {
		err = mips_cpu_set_pc(cpu, memadd);

	}
	if (err == 0) {
		err = mips_cpu_step(cpu);
		//Implements the function
	}
	if (err == 0) {
		err = (mips_error) (err | mips_cpu_get_register(cpu, 3, &got));
	}
	passed = (err == mips_Success) && (got == 0);
	mips_test_end_test(testId, passed, NULL);
	err = mips_cpu_reset(cpu);
	err = mips_Success;

	testId = mips_test_begin_test("slt");
	err = mips_cpu_set_register(cpu, 1, 0x00000001);
	if (err == 0) {
		err = mips_cpu_set_register(cpu, 2, 0x00000001);
	}

	instruction = 0x2A182200; //
	memadd = 0;
	if (err == 0) {
		err = mips_mem_write(mem, memadd, 4, (uint8_t*) &instruction);
	}
	if (err == 0) {
		err = mips_cpu_set_pc(cpu, memadd);

	}
	if (err == 0) {
		err = mips_cpu_step(cpu);
		//Implements the function
	}
	if (err == 0) {
		err = (mips_error) (err | mips_cpu_get_register(cpu, 3, &got));
	}
	passed = (err == mips_Success) && (got == 0);
	mips_test_end_test(testId, passed, NULL);

	testId = mips_test_begin_test("slt");
	err = mips_cpu_set_register(cpu, 1, 0xF001FFFF);
	if (err == 0) {
		err = mips_cpu_set_register(cpu, 2, 0x00F10000);
	}

	instruction = 0x2A182200; //
	memadd = 0;
	if (err == 0) {
		err = mips_mem_write(mem, memadd, 4, (uint8_t*) &instruction);
	}
	if (err == 0) {
		err = mips_cpu_set_pc(cpu, memadd);

	}
	if (err == 0) {
		err = mips_cpu_step(cpu);
		//Implements the function
	}
	if (err == 0) {
		err = (mips_error) (err | mips_cpu_get_register(cpu, 3, &got));
	}
	passed = (err == mips_Success) && (got == 1);
	mips_test_end_test(testId, passed, NULL);
	err = mips_cpu_reset(cpu);
	err = mips_Success;
	err = mips_cpu_reset(cpu);
	err = mips_Success;
//SLTI  |  Set on less than immediate (signed)      | 3
	//0010 10ss ssst tttt iiii iiii iiii iiii
	//0010 1000 0010 0010 0000 0000 0000 0010
	testId = mips_test_begin_test("slti");
	err = mips_cpu_set_register(cpu, 1, 0x00000003);

	instruction = 0x02002228;
	memadd = 0;
	if (err == 0) {
		err = mips_mem_write(mem, memadd, 4, (uint8_t*) &instruction);
	}
	if (err == 0) {
		err = mips_cpu_set_pc(cpu, memadd);

	}
	if (err == 0) {
		err = mips_cpu_step(cpu);
		//Implements the function
	}
	if (err == 0) {
		err = (mips_error) (err | mips_cpu_get_register(cpu, 2, &got));
	}	//Backwards
	passed = (err == mips_Success) && (got == 0x00000000);
	mips_test_end_test(testId, passed, NULL);
	err = mips_cpu_reset(cpu);
	err = mips_Success;

	testId = mips_test_begin_test("slti");
	err = mips_cpu_set_register(cpu, 1, 0x00000001);

	instruction = 0x02002228;
	memadd = 0;
	if (err == 0) {
		err = mips_mem_write(mem, memadd, 4, (uint8_t*) &instruction);
	}
	if (err == 0) {
		err = mips_cpu_set_pc(cpu, memadd);

	}
	if (err == 0) {
		err = mips_cpu_step(cpu);
		//Implements the function
	}
	if (err == 0) {
		err = (mips_error) (err | mips_cpu_get_register(cpu, 2, &got));
	}	//Backwards
	passed = (err == mips_Success) && (got == 0x00000001);
	mips_test_end_test(testId, passed, NULL);
	err = mips_cpu_reset(cpu);
	err = mips_Success;

//SLTIU |  Set on less than immediate unsigned      | 3
	//0010 11ss ssst tttt iiii iiii iiii iiii
	testId = mips_test_begin_test("sltiu");
	err = mips_cpu_set_register(cpu, 1, 0x00000003);

	instruction = 0x02002228;
	memadd = 0;
	if (err == 0) {
		err = mips_mem_write(mem, memadd, 4, (uint8_t*) &instruction);
	}
	if (err == 0) {
		err = mips_cpu_set_pc(cpu, memadd);

	}
	if (err == 0) {
		err = mips_cpu_step(cpu);
		//Implements the function
	}
	if (err == 0) {
		err = (mips_error) (err | mips_cpu_get_register(cpu, 2, &got));
	}	//Backwards
	passed = (err == mips_Success) && (got == 0x00000000);
	mips_test_end_test(testId, passed, NULL);
	err = mips_cpu_reset(cpu);
	err = mips_Success;

	testId = mips_test_begin_test("sltiu");
	err = mips_cpu_set_register(cpu, 1, 0x00000001);

	instruction = 0x02002228;
	memadd = 0;
	if (err == 0) {
		err = mips_mem_write(mem, memadd, 4, (uint8_t*) &instruction);
	}
	if (err == 0) {
		err = mips_cpu_set_pc(cpu, memadd);

	}
	if (err == 0) {
		err = mips_cpu_step(cpu);
		//Implements the function
	}
	if (err == 0) {
		err = (mips_error) (err | mips_cpu_get_register(cpu, 2, &got));
	}	//Backwards
	passed = (err == mips_Success) && (got == 0x00000001);
	mips_test_end_test(testId, passed, NULL);
	err = mips_cpu_reset(cpu);
	err = mips_Success;

//SLTU  |  Set on less than unsigned                | 1
	//0000 00ss ssst tttt dddd d000 0010 1011

	testId = mips_test_begin_test("sltu");
	err = mips_cpu_set_register(cpu, 1, 0x0001FFFF);
	if (err == 0) {
		err = mips_cpu_set_register(cpu, 2, 0x00F10000);
	}

	instruction = 0x2B182200; // //addu $10, $8, $9
	memadd = 0;
	if (err == 0) {
		err = mips_mem_write(mem, memadd, 4, (uint8_t*) &instruction);
	}
	if (err == 0) {
		err = mips_cpu_set_pc(cpu, memadd);

	}
	if (err == 0) {
		err = mips_cpu_step(cpu);
		//Implements the function
	}
	if (err == 0) {
		err = (mips_error) (err | mips_cpu_get_register(cpu, 3, &got));
	}
	passed = (err == mips_Success) && (got == 1);
	mips_test_end_test(testId, passed, NULL);
	err = mips_cpu_reset(cpu);
	err = mips_Success;

	testId = mips_test_begin_test("sltu");
	err = mips_cpu_set_register(cpu, 2, 0x0001FFFF);
	if (err == 0) {
		err = mips_cpu_set_register(cpu, 1, 0x00F10000);
	}

	instruction = 0x2B182200; // //addu $10, $8, $9
	memadd = 0;
	if (err == 0) {
		err = mips_mem_write(mem, memadd, 4, (uint8_t*) &instruction);
	}
	if (err == 0) {
		err = mips_cpu_set_pc(cpu, memadd);

	}
	if (err == 0) {
		err = mips_cpu_step(cpu);
		//Implements the function
	}
	if (err == 0) {
		err = (mips_error) (err | mips_cpu_get_register(cpu, 3, &got));
	}
	passed = (err == mips_Success) && (got == 0);
	mips_test_end_test(testId, passed, NULL);
	err = mips_cpu_reset(cpu);
	err = mips_Success;
//XXX SRA   |  Shift right arithmetic                   | 2
	//0000 00-- ---t tttt dddd dhhh hh00 0011
	//0000 0000 0000 0010 0010 0000 0100 0011

	testId = mips_test_begin_test("sra");
	err = mips_cpu_set_register(cpu, 2, 0x02020202);
	// 2<<3 into 4
	instruction = 0x43200200;
	memadd = 0;
	if (err == 0) {
		err = mips_mem_write(mem, memadd, 4, (uint8_t*) &instruction);
	}
	if (err == 0) {
		err = mips_cpu_set_pc(cpu, memadd);

	}
	if (err == 0) {
		err = mips_cpu_step(cpu);
		//Implements the function
	}
	if (err == 0) {
		err = mips_cpu_get_register(cpu, 4, &got);
	}

	passed = (err == mips_Success) && (got == 0x01010101);
	mips_test_end_test(testId, passed, NULL);
	err = mips_cpu_reset(cpu);
	err = mips_Success;

	//0000 00-- ---t tttt dddd dhhh hh00 0011
	//0000 0000 0000 0010 0010 0000 0100 0011
	//Reg 2>>shift store in 4

	testId = mips_test_begin_test("sra");
	err = mips_cpu_set_register(cpu, 2, 0x82020202);
	instruction = 0x43200200;
	memadd = 0;
	if (err == 0) {
		err = mips_mem_write(mem, memadd, 4, (uint8_t*) &instruction);
	}
	if (err == 0) {
		err = mips_cpu_set_pc(cpu, memadd);

	}
	if (err == 0) {
		err = mips_cpu_step(cpu);
		//Implements the function
	}
	if (err == 0) {
		err = mips_cpu_get_register(cpu, 4, &got);
	}

	passed = (err == mips_Success) && (got == 0xC1010101);
	mips_test_end_test(testId, passed, NULL);
	err = mips_cpu_reset(cpu);
	err = mips_Success;

//XXX SRL   |  Shift right logical                      | 2
	//0000 00-- ---t tttt dddd dhhh hh00 0010
	//0000 0000 0000 0010 0010 0000 0100 0010
	//42200200
	//2>>shift1 store in 4

	testId = mips_test_begin_test("srl");
	err = mips_cpu_set_register(cpu, 2, 0x02020202);
	// 2<<3 into 4
	instruction = 0x42200200;
	memadd = 0;
	if (err == 0) {
		err = mips_mem_write(mem, memadd, 4, (uint8_t*) &instruction);
	}
	if (err == 0) {
		err = mips_cpu_set_pc(cpu, memadd);

	}
	if (err == 0) {
		err = mips_cpu_step(cpu);
		//Implements the function
	}
	if (err == 0) {
		err = mips_cpu_get_register(cpu, 4, &got);
	}

	passed = (err == mips_Success) && (got == 0x01010101);
	mips_test_end_test(testId, passed, NULL);
	err = mips_cpu_reset(cpu);
	err = mips_Success;

	//0000 00-- ---t tttt dddd dhhh hh00 0010

	testId = mips_test_begin_test("srl");
	err = mips_cpu_set_register(cpu, 2, 0x82020202);
	instruction = 0x42200200;
	memadd = 0;
	if (err == 0) {
		err = mips_mem_write(mem, memadd, 4, (uint8_t*) &instruction);
	}
	if (err == 0) {
		err = mips_cpu_set_pc(cpu, memadd);

	}
	if (err == 0) {
		err = mips_cpu_step(cpu);
		//Implements the function
	}
	if (err == 0) {
		err = mips_cpu_get_register(cpu, 4, &got);
	}

	passed = (err == mips_Success) && (got == 0x41010101);
	mips_test_end_test(testId, passed, NULL);
	err = mips_cpu_reset(cpu);
	err = mips_Success;
//XXX SRLV  |  Shift right logical variable             | 2
	//0000 00ss ssst tttt dddd d000 0000 0110
	//0000 0000 0110 0010 0010 0000 0000 0110
	//06206200

	testId = mips_test_begin_test("srlv");
	err = mips_cpu_set_register(cpu, 2, 0x02020202);
	if (err == 0) {
		err = mips_cpu_set_register(cpu, 3, 0x00000001);
	}
	instruction = 0x06206200;
	memadd = 0;
	if (err == 0) {
		err = mips_mem_write(mem, memadd, 4, (uint8_t*) &instruction);
	}
	if (err == 0) {
		err = mips_cpu_set_pc(cpu, memadd);

	}
	if (err == 0) {
		err = mips_cpu_step(cpu);
		//Implements the function
	}
	if (err == 0) {
		err = mips_cpu_get_register(cpu, 4, &got);
	}

	passed = (err == mips_Success) && (got == 0x01010101);
	mips_test_end_test(testId, passed, NULL);
	err = mips_cpu_reset(cpu);
	err = mips_Success;

	//

//XXX SUB   |  Subtract                                 | 2
	//0000 00ss ssst tttt dddd d000 0010 0010
	//0000 0001 0000 1001 0101 0000 0010 0010
	//22500901

	testId = mips_test_begin_test("sub");
	err = mips_cpu_set_register(cpu, 8, 0x00000002);
	if (err == 0) {
		err = mips_cpu_set_register(cpu, 9, 0x00000001);
	}

	instruction = 0x22500901;
	memadd = 8;
	if (err == 0) {
		err = mips_mem_write(mem, memadd, 4, (uint8_t*) &instruction);
	}
	if (err == 0) {
		err = mips_cpu_set_pc(cpu, memadd);

		if (err == 0) {
			err = mips_cpu_step(cpu);
			//Implements the function
		}
	}
	if (err == 0) {
		err = mips_cpu_get_register(cpu, 10, &got);
	}

	passed = (err == mips_Success) && (got == 0x00000001);
	mips_test_end_test(testId, passed, NULL);
	err = mips_cpu_reset(cpu);
	err = mips_Success;

	//

	//Positive - Negative
	testId = mips_test_begin_test("sub");
	err = mips_cpu_set_register(cpu, 8, 0x00000002);
	if (err == 0) {
		err = mips_cpu_set_register(cpu, 9, 0xFFFFFFFF);
	}

	instruction = 0x22500901;
	memadd = 8;
	if (err == 0) {
		err = mips_mem_write(mem, memadd, 4, (uint8_t*) &instruction);
	}
	if (err == 0) {
		err = mips_cpu_set_pc(cpu, memadd);

		if (err == 0) {
			err = mips_cpu_step(cpu);
			//Implements the function
		}
	}
	if (err == 0) {
		err = mips_cpu_get_register(cpu, 10, &got);
	}

	passed = (err == mips_Success) && (got == 0x00000003);
	mips_test_end_test(testId, passed, NULL);
	err = mips_cpu_reset(cpu);
	err = mips_Success;

	//Negative minus Positive

	testId = mips_test_begin_test("sub");
	err = mips_cpu_set_register(cpu, 8, 0xFFFFFFFF);
	if (err == 0) {
		err = mips_cpu_set_register(cpu, 9, 0x00000001);
	}

	instruction = 0x22500901;
	memadd = 8;
	if (err == 0) {
		err = mips_mem_write(mem, memadd, 4, (uint8_t*) &instruction);
	}
	if (err == 0) {
		err = mips_cpu_set_pc(cpu, memadd);

		if (err == 0) {
			err = mips_cpu_step(cpu);
			//Implements the function
		}
	}
	if (err == 0) {
		err = mips_cpu_get_register(cpu, 10, &got);

	}

	passed = (err == mips_Success) && (got == 0xFFFFFFFE);
	mips_test_end_test(testId, passed, NULL);
	err = mips_cpu_reset(cpu);
	err = mips_Success;

	//Overflow Positive

	testId = mips_test_begin_test("sub");
	err = mips_cpu_set_register(cpu, 8, 0x7FFFFFFF); //Largest Positive Number
	if (err == 0) {
		err = mips_cpu_set_register(cpu, 9, 0x80000001); //Second Larger Negative Number
	}

	instruction = 0x22500901;
	memadd = 8;
	if (err == 0) {
		err = mips_mem_write(mem, memadd, 4, (uint8_t*) &instruction);
	}
	if (err == 0) {
		err = mips_cpu_set_pc(cpu, memadd);

		if (err == 0) {
			err = mips_cpu_step(cpu);
			//Implements the function
		}
	}
	if (err == 0) {
		err = mips_cpu_get_register(cpu, 10, &got);
	}

	passed = (err == mips_ExceptionArithmeticOverflow);
	mips_test_end_test(testId, passed, NULL);
	err = mips_cpu_reset(cpu);
	err = mips_Success;
	//
//XXX SUBU  |  Subtract unsigned                        | 1

	//Basic Arithmetic Test
	testId = mips_test_begin_test("subu");
	err = mips_cpu_set_register(cpu, 8, 0xF000FFFF);
	if (err == 0) {
		err = mips_cpu_set_register(cpu, 9, 0x0000FFFF);
	}
	// 0000 0001 0000 1001 0101 0000 0010 0011
	//instruction = 0x01095023; // //SUBU $10, $9, 4
	instruction = 0x23500901;
	memadd = 4;
	if (err == 0) {
		err = mips_mem_write(mem, memadd, 4, (uint8_t*) &instruction);
	}
	if (err == 0) {
		err = mips_cpu_set_pc(cpu, memadd);

	}
	if (err == 0) {
		err = mips_cpu_step(cpu);
		//Implements the function
	}
	if (err == 0) {
		err = (mips_error) (err | mips_cpu_get_register(cpu, 10, &got));
	}
	passed = (err == mips_Success) && (got == 0xF0000000);
	mips_test_end_test(testId, passed, NULL);
	err = mips_cpu_reset(cpu);
	err = mips_Success;

	//

	//Rollover test
	testId = mips_test_begin_test("subu");
	err = mips_cpu_set_register(cpu, 8, 0x00000000);
	if (err == 0) {
		err = mips_cpu_set_register(cpu, 9, 0x00000001);
	}
	//instruction = 0x01095100; // //SLL $10, $9, 4
	instruction = 0x23500901;
	memadd = 4;
	if (err == 0) {
		err = mips_mem_write(mem, memadd, 4, (uint8_t*) &instruction);
	}
	if (err == 0) {
		err = mips_cpu_set_pc(cpu, memadd);

	}
	if (err == 0) {
		err = mips_cpu_step(cpu);
		//Implements the function
	}
	if (err == 0) {
		err = (mips_error) (err | mips_cpu_get_register(cpu, 10, &got));
	}
	passed = (err == mips_Success) && (got == 0xFFFFFFFF);
	mips_test_end_test(testId, passed, NULL);
	err = mips_cpu_reset(cpu);
	err = mips_Success;

//SW    |  Store word                               | 2
	//1010 11ss ssst tttt iiii iiii iiii iiii
	//1010 1100 0010 0010 0000 0000 0000 0000

	testId = mips_test_begin_test("sw");
	err = mips_cpu_set_register(cpu, 1, 0x00000010);
	err = mips_cpu_set_register(cpu, 2, 0x00000024);

	instruction = 0x000022AC;
	memadd = 0;
	if (err == 0) {
		err = mips_mem_write(mem, memadd, 4, (uint8_t*) &instruction);
	}
	if (err == 0) {
		err = mips_cpu_set_pc(cpu, memadd);

	}
	if (err == 0) {
		err = mips_cpu_step(cpu);
		//Implements the function
	}
	if (err == 0) {
		err = (mips_error) (err | mips_mem_read(mem, 0x10, 4, (uint8_t*) &got));
	}	//Backwards
	passed = (err == mips_Success) && (got == 0x24000000);
	mips_test_end_test(testId, passed, NULL);
	err = mips_cpu_reset(cpu);
	err = mips_Success;

//XXX XOR   |  Bitwise exclusive or                     | 1
	testId = mips_test_begin_test("xor");
	err = mips_cpu_set_register(cpu, 8, 0xF000FFFF);
	if (err == 0) {
		err = mips_cpu_set_register(cpu, 9, 0x00000F00);
	}
	/*Operation: $d = $s & $t; advance_pc (4);
	 Syntax: and $d, $s, $t
	 General Encoding: 0000 00ss ssst tttt dddd d000 0010 0101
	 Specific Encoding: 0000 0001 0000 1001 0101 0000 0010 0110
	 */				//	   0	1	 0	  9	   5	0	 2    6
	// SET MEMORY FOR INSTRUCTIONS
	//instruction = 0x01095026; // //xor $10, $8, $9
	instruction = 0x26500901;
	memadd = 4;
	if (err == 0) {
		err = mips_mem_write(mem, memadd, 4, (uint8_t*) &instruction);
	}
	if (err == 0) {
		err = mips_cpu_set_pc(cpu, memadd);

	}
	if (err == 0) {
		err = mips_cpu_step(cpu);
		//Implements the function
	}
	if (err == 0) {
		err = (mips_error) (err | mips_cpu_get_register(cpu, 10, &got));
	}
	passed = (err == mips_Success) && (got == 0xF000F0FF);
	mips_test_end_test(testId, passed, NULL);
	err = mips_cpu_reset(cpu);
	err = mips_Success;
//XORI  |  Bitwise exclusive or immediate           | 2
	//0011 10ss ssst tttt iiii iiii iiii iiii

	testId = mips_test_begin_test("xori");
	err = mips_cpu_set_register(cpu, 1, 0x00001012);

	instruction = 0x02002238;
	memadd = 0;
	if (err == 0) {
		err = mips_mem_write(mem, memadd, 4, (uint8_t*) &instruction);
	}
	if (err == 0) {
		err = mips_cpu_set_pc(cpu, memadd);

	}
	if (err == 0) {
		err = mips_cpu_step(cpu);
		//Implements the function
	}
	if (err == 0) {
		err = (mips_error) (err | mips_cpu_get_register(cpu, 2, &got));
	}	//Backwards
	passed = (err == mips_Success) && (got == 0x00001010);
	mips_test_end_test(testId, passed, NULL);
	err = mips_cpu_reset(cpu);
	err = mips_Success;

	mips_test_end_suite();
	mips_cpu_free(cpu);
	mips_mem_free(mem);

	return 0;
}
