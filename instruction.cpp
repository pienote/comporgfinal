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
	offset = -1;
	
	frozen = false;
	taken = false;
	stage = 0;
	stall = false;
	branch = "";
	set_up_columns();
}

instruction::instruction(std::string instr, int* d, int* r0, int inter)
{
	//std::cout << "b" << std::endl;
	op = instr;
	dest = d;
	reg0 = r0;
	reg1 = NULL;
	k = inter;
	offset = -1;
	
	frozen = false;
	taken = false;
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
	if(reg1 == NULL)
		k = 0;
	else
		k = -1;
	offset = -1;
	
	frozen = false;
	taken = false;
	stage = 0;
	stall = false;
	branch = name;
	set_up_columns();
}

const int* instruction::get_dest()
{
	return dest;
}

const int* instruction::get_reg0()
{
	return reg0;
}

const int* instruction::get_reg1()
{
	return reg1;
}

const std::string instruction::get_branch_name()
{
	return branch;
}

void instruction::freeze()
{
	frozen = true;
}

void instruction::reset_taken()
{
	taken = false;
}

void instruction::set_up_columns()
{
	for(int i=0;i<16;i++)
		cols[i] = ".";
}

void instruction::update(int cc)
{
	if(stage == 0)
		offset = cc;	// for the nop
	if(!stall && stage < 6)
		stage++;
	if(stage == 5)
		write_back();
	if(!frozen)
	{
		if(stage == 1)
			cols[cc] = "IF";
		if(stage == 2)
			cols[cc] = "ID";
		if(stage == 3)
			cols[cc] = "MEM";
		if(stage == 4)
			cols[cc] = "EX";
		if(stage == 5)
			cols[cc] = "WB";
	}
	else
	{
		if(stage <= 5)
			cols[cc] = "*";
	}
}

void instruction::write_back()
{
	if(frozen)
		return;
	if(branch == "")	// not "beq" operator
	{
		int res, n1, n2;
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
		int n;
		if(reg1 == NULL)
			n = k;
		else
			n = *reg1;
		if(op == "beq")
		{	
			if(*reg0 == n)
				taken = true;
			else
				taken = false;
		}
		if(op == "bne")
		{
			if(*reg0 != n)
				taken = true;
			else
				taken = false;	
		}
	}
}

bool instruction::branch_taken()
{
	return taken;
}

bool instruction::is_branch()
{
	return (branch != "");
}

bool instruction::is_done()
{
	return (stage >= 5);
}

void instruction::print(std::string line, int curr)
{
	std::cout << std::left << std::setw(20) << line;
	for(int i=0;i<16;i++)
	{
		std::cout << std::left << std::setw(4) << cols[i];
	}
	std::cout << std::endl;
}