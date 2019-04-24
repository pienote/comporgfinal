#include "instruction.h"

instruction::instruction()
{
	
}

instruction::instruction(std::string instr, int* d, int* r0, int* r1)
{
	//std::cout << "a" << std::endl;
	op = instr;
	dest = d;
	reg0 = r0;
	reg1 = r1;
	k = -1;
	offset = 0;
	
	stage = 0;
	stall = false;
	branch = "";
	set_up_columns();
}

int* instruction::get_reg0()
{
	return reg0;
}

int* instruction::get_reg1()
{
	return reg1;
}


instruction::instruction(std::string instr, int* d, int* r0, int inter)
{
	//std::cout << "b" << std::endl;
	op = instr;
	dest = d;
	reg0 = r0;
	reg1 = NULL;
	k = inter;
	offset = 0;
	
	stage = 0;
	stall = false;
	branch = "";
	set_up_columns();
}

instruction::instruction(std::string instr, int* r0, int* r1, std::string name)
{
	//std::cout << "c" << std::endl;
	op = instr;
	dest = NULL;
	reg0 = r0;
	reg1 = r1;
	k = -1;
	offset = 0;
	
	stage = 0;
	stall = false;
	branch = name;
	set_up_columns();
}

void instruction::set_up_columns()
{
	for(int i=0;i<16;i++)
		cols[i] = ".\t";
}

void instruction::update(int cc)
{
	if(stage == 0)
		offset = cc;
	if(!stall && !is_done())
		stage++;
	if(is_done())
		write_back();
}

void instruction::write_back()
{
	int n1, n2;
	if(branch == "")	// not "beq" operator
	{
		int res, n1;
		if(reg0 == NULL)
			n1 = 0;
		else
			n1 = *reg0;
		if(op.substr(op.length()-1, 1) == "i")
			n2 = k;
		else
			n2 = *reg1;
		if(op.find("add") != std::string::npos)
			res = n1 + n2;
		else if(op.find("and") != std::string::npos)
			res = n1 & n2;
		else if(op.find("or") != std::string::npos)
			res = n1 | n2;
		else if(op.find("slt") != std::string::npos)
			res = n1 < n2;
		*dest = res;
	}
	else	// is a beq or bne
	{
		
	}
}

bool instruction::is_done()
{
	return (stage >= 5);
}

void instruction::print(std::string line, int curr)
{
	
}