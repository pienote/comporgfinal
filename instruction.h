#ifndef INSTRUCTION_H
#define INSTRUCTION_H

#include <iostream>
#include <string>

class instruction
{
	public:
		instruction();
		
		instruction(std::string instr, int* d, int* r0, int* r1);
		instruction(std::string instr, int* d, int* r0, int inter);
		instruction(std::string instr, int* r0, int* r1, std::string name);
		
		void update();
		void write_back();
		bool is_done();
		
		
	private:
		bool label;
		bool stall;
		
		std::string op;
		std::string branch;
		
		int stage;	// 0 <- not initiated, 1 <- IF, 2 <- ID, ...
		int* dest;
		int* reg0;
		int* reg1;
		int k;
};

#endif