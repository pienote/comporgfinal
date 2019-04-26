#ifndef INSTRUCTION_H
#define INSTRUCTION_H

#include <iomanip>
#include <iostream>
#include <string>

class instruction
{
	public:
		instruction();
		
		instruction(std::string instr, int* d, int* r0, int* r1);
		instruction(std::string instr, int* d, int* r0, int inter);
		instruction(std::string instr, int* r0, int* r1, std::string name);
		
		int* get_dest();
		int* get_reg0();
		int* get_reg1();
		
		const std::string get_branch_name();
		int get_stage();
		int get_stall();

		void freeze();
		void reset_taken();
		void set_up_columns();
		void update(int cc);
		void write_back();
		bool branch_taken();
		bool is_branch();
		bool is_done();
		void print(std::string line, int curr);
		void set_stall(int i);
		void unstall();
		void print_nop(int curr);

	private:
		bool taken;
		bool frozen;
		bool nop;
		
		std::string op;
		std::string branch;
		std::string cols[16];
		
		int num_stall;
		int stall;
		int offset;	// offset from clock
		int stage;	// 0 <- not initiated, 1 <- IF, 2 <- ID, ...
		int* dest;
		int* reg0;
		int* reg1;
		int k;
};

#endif