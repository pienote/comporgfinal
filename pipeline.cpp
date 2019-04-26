#include "pipeline.h"

pipeline::pipeline(bool f, char* filename)
{
	forward = f;
	in = std::ifstream(filename);
}

void pipeline::init()
{
	counter = 1;	// what instruction are we on (line number)
	
	if(forward)
		std::cout << "START OF SIMULATION (forwarding)";
	else
		std::cout << "START OF SIMULATION (no forwarding)";
	
	cc = 0;
	
	for(int i=0;i<8;i++)
		s[i] = 0;
	
	for(int i=0;i<10;i++)
		t[i] = 0;

	int line_num = 0;
	std::string line;
	while(std::getline(in, line))
	{
		if(parse_line(line, line_num))
		{
			lines.push_back(line);
			line_num++;
		}
	}
	loop();
}

void pipeline::loop()
{
	for(cc=0;cc<16;cc++)
	{
		bool done = true;
		for(unsigned int i=0;i<instructions.size();i++)
			if(!instructions[i].is_done())
				done = false;
		if(done)
			break;
		
		// do updates here
		for(unsigned int i=0;i<instructions.size()&&i<counter;i++)
		{
			if(!forward)
			{
				if(i > 0 && instructions[i - 1].get_stall() > 0)
				{
					instructions[i].set_stall(0);
				}
				else if(stalled.find(instructions[i].get_reg0()) != stalled.end() || stalled.find(instructions[i].get_reg1()) != stalled.end())
				{
					if(instructions[i].get_stage() == 2)
					{
						int max = -1;
						if(stalled.find(instructions[i].get_reg0()) != stalled.end())
						{
							if(i - stalled[instructions[i].get_reg0()] == 2 && max < 1)
								max = 1;
							if(i - stalled[instructions[i].get_reg0()] == 1 && max < 2)
								max = 2;
						}
						if(stalled.find(instructions[i].get_reg1()) != stalled.end())
						{
							if(i - stalled[instructions[i].get_reg1()] == 2 && max < 1)
								max = 1;
							if(i - stalled[instructions[i].get_reg1()] == 1 && max < 2)
								max = 2;
						}
						if(max != -1)
						{
							instructions[i].set_stall(max);
						}
					}
				}
				else
				{
					instructions[i].unstall();
				}
			}
			instructions[i].update(cc);
			if(!forward)
			{
				stalled[instructions[i].get_dest()] = i;
				if(instructions[i].get_stage() > 5)
				{
					stalled.erase(instructions[i].get_dest());
				}
			}
			if(instructions[i].is_branch() && instructions[i].is_done())
			{
				if(instructions[i].branch_taken())
				{
					for(unsigned int j=i+1;j<counter;j++)
					{
						instructions[j].freeze();	// make the after instructions '*'
					}
					// add code from label to end
					int temp = instructions.size();
					for(int j = labels[instructions[i].get_branch_name()];j<temp;j++)
					{
						if(parse_line(lines[j], (j+i)))
							lines.push_back(lines[j]);
					}
					counter++;
					instructions[i].reset_taken();
				}
			}
		}
		
		std::cout << std::endl << "----------------------------------------------------------------------------------" << std::endl;
		std::cout << "CPU Cycles ===>     1   2   3   4   5   6   7   8   9   10  11  12  13  14  15  16" << std::endl;
		print_instructs();
		std::cout << std::endl;
		print_regs();
		
		// advance cycle and counter (if needed)
		if(counter < lines.size())
			counter++;
	}
	stop();
}

void pipeline::print_instructs()
{
	for(unsigned int i=0;i<counter;i++)
	{
		instructions[i].print(lines[i], cc);
		//std::cout << instructions[i].get_stall();
	}
}

void pipeline::print_regs()
{
	// print s registers
	std::cout << std::left << std::setw(6) << "$s0 = " << std::setw(14) << s[0];
	std::cout << std::left << std::setw(6) << "$s1 = " << std::setw(14) << s[1];
	std::cout << std::left << std::setw(6) << "$s2 = " << std::setw(14) << s[2];
	std::cout << std::left << std::setw(6) << "$s3 = " << s[3] << std::endl;
	std::cout << std::left << std::setw(6) << "$s4 = " << std::setw(14) << s[4];
	std::cout << std::left << std::setw(6) << "$s5 = " << std::setw(14) << s[5];
	std::cout << std::left << std::setw(6) << "$s6 = " << std::setw(14) << s[6];
	std::cout << std::left << std::setw(6) << "$s7 = " << s[7] << std::endl;
	
	//print t registers
	std::cout << std::left << std::setw(6) << "$t0 = " << std::setw(14) << t[0];
	std::cout << std::left << std::setw(6) << "$t1 = " << std::setw(14) << t[1];
	std::cout << std::left << std::setw(6) << "$t2 = " << std::setw(14) << t[2];
	std::cout << std::left << std::setw(6) << "$t3 = " << t[3] << std::endl;
	std::cout << std::left << std::setw(6) << "$t4 = " << std::setw(14) << t[4];
	std::cout << std::left << std::setw(6) << "$t5 = " << std::setw(14) << t[5];
	std::cout << std::left << std::setw(6) << "$t6 = " << std::setw(14) << t[6];
	std::cout << std::left << std::setw(6) << "$t7 = " << t[7] << std::endl;
	std::cout << std::left << std::setw(6) << "$t8 = " << std::setw(14) << t[8];
	std::cout << std::left << std::setw(6) << "$t9 = " << t[9];
}

void pipeline::stop()
{
	std::cout << std::endl << "----------------------------------------------------------------------------------" << std::endl;
	std::cout << "END OF SIMULATION" << std::endl;
	// std::cout << s[1] << std::endl;
	exit(0);
}

bool pipeline::parse_line(std::string line, int line_num)
{
	std::string instr;
	std::string second;
	
	std::string parts[3];
	int i = 0;
	int pos = 0;
	std::string token;
	
	std::istringstream iss(line);
	if(!(iss >> instr >> second))	// branch
	{
		instr = instr.substr(0, instr.length()-1);
		labels[instr] = line_num;
		// record branch here
		return false;
	}
	
	while((pos = second.find(",")) != (signed int)std::string::npos)
	{
		token = second.substr(0, pos);
		parts[i] = token;
		second.erase(0, pos+1);
		i++;
	}
	parts[i] = second;
	//for(i=0;i<3;i++)
	//	std::cout << parts[i] << std::endl;
	instructions.push_back(parse_instruction(instr, parts[0], parts[1], parts[2]));
	return true;
}

instruction pipeline::parse_instruction(std::string instr, std::string p0, std::string p1, std::string p2)
{
	int* dest = NULL;
	int* r0 = NULL;
	int* r1 = NULL;
	int k = -1;
	
	if(p0.substr(1, 1) == "s")
	{
		dest = &s[std::stoi(p0.substr(2, 1))];
	}
	if(p0.substr(1, 1) == "t")
	{
		dest = &t[std::stoi(p0.substr(2, 1))];
	}
	
	if(p1.substr(0, 1) == "$")
	{
		if(p1 == "$zero")
		{
			k = 0;	//intermediate = 0;
		}
		if(p1.substr(1, 1) == "s")
		{
			r0 = &s[std::stoi(p1.substr(2, 1))];	// set pointer to r0
		}
		if(p1.substr(1, 1) == "t")
		{
			r0 = &t[std::stoi(p1.substr(2, 1))];	// set pointer to r1
		}
	}
	
	if(p2.substr(0, 1) >= "a" && p2.substr(0, 1) <= "z")
	{
		if(p0 == "$zero")
			return instruction(instr, r0, NULL, p2);
		if(p1 == "$zero")
			return instruction(instr, dest, NULL, p2);
		return instruction(instr, dest, r0, p2);
	}
	else if(p2.substr(0, 1) == "$")
	{
		if(p2 == "$zero")
		{
			k = 0;	// intermediate = 0;
		}
		if(p2.substr(1, 1) == "s")
		{
			if(k != -1)
				r0 = &s[std::stoi(p2.substr(2, 1))];	// yes intermediate
			else
				r1 = &s[std::stoi(p2.substr(2, 1))];
		}
		if(p2.substr(1, 1) == "t")
		{
			if(k != -1)
				r0 = &t[std::stoi(p2.substr(2, 1))];
			else
				r1 = &t[std::stoi(p2.substr(2, 1))];
		}
	}
	if(p2.substr(0, 1) >= "0" && p2.substr(0, 1) <= "9")
	{
		k = std::stoi(p2);
	}
	if(k == -1)
		return instruction(instr, dest, r0, r1);
	if(r1 == NULL && k != -1)
		return instruction(instr, dest, r0, k);
	return instruction();
}