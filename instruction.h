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
		void is_done();
		
		
	private:
		bool i;
		bool label;
		bool stall;
		
		std::string op;
		std::string branch;
		
		int stage;
		int* dest;
		int* reg0;
		int* reg1;
		int k;
};

#endif