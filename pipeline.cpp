#include "pipeline.h"

pipeline::pipeline(bool f, char* filename)
{
	forward = f;
	in = std::ifstream(filename);
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
		return instruction("beq", dest, r0, p2);
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
}
	

void pipeline::init()
{
	counter = 0;	// what instruction are we on (line number)
	
	if(forward)
		std::cout << "START OF SIMULATION (forwarding)";
	else
		std::cout << "START OF SIMULATION (no forwarding)";
	
	cc = 0;
	
	for(int i=0;i<8;i++)
	{
		s[i] = 0;
	}
	
	for(int i=0;i<10;i++)
	{
		t[i] = 0;
	}
	std::string line;
	std::string instr;
	std::string second;

	int line_num = 0;
	
	while(std::getline(in, line))
	{
		std::istringstream iss(line);
		if(!(iss >> instr >> second))	// branch
		{
			instr = instr.substr(0, instr.length()-1);
			labels[instr] = line_num+1;
			// record branch here
			continue;
		}
		std::string parts[3];
		int i = 0;
		int pos = 0;
		std::string token;
		std::string comma = ",";
		while((pos = second.find(",")) != std::string::npos)
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
		line_num++;
	}
	
	loop();
}

void pipeline::loop()
{
	for(int i=0;i<16;i++)
	{
		//bool done = true;
		//for(int j=0;j<instructs.size();j++)
		//	if(!instructs[j].is_done())
		//		done = false;
		//if(done)
		//	break;
		
		//std::cout << std::endl << "----------------------------------------------------------------------------------" << std::endl;
		//std::cout << "CPU Cycles ===>     1   2   3   4   5   6   7   8   9   10  11  12  13  14  15  16" << std::endl;
		//print_instructs();
		//print_regs();
		// advance cycle and counter (if needed)
		
	}
	stop();
}

void pipeline::print_instructs()
{
	for(int i=0;i<counter;i++)
	{
		// instructs[i].print();
	}
}

void pipeline::print_regs()
{
	// print s registers
	std::cout << "$s0 = " << s[0] << "\t\t\t";
	std::cout << "$s1 = " << s[1] << "\t\t\t";
	std::cout << "$s2 = " << s[2] << "\t\t\t";
	std::cout << "$s3 = " << s[3] << std::endl;
	std::cout << "$s4 = " << s[4] << "\t\t\t";
	std::cout << "$s5 = " << s[5] << "\t\t\t";
	std::cout << "$s6 = " << s[6] << "\t\t\t";
	std::cout << "$s7 = " << s[7] << std::endl;
	
	//print t registers
	std::cout << "$t0 = " << t[0] << "\t\t\t";
	std::cout << "$t1 = " << t[1] << "\t\t\t";
	std::cout << "$t2 = " << t[2] << "\t\t\t";
	std::cout << "$t3 = " << t[3] << std::endl;
	std::cout << "$t4 = " << t[4] << "\t\t\t";
	std::cout << "$t5 = " << t[5] << "\t\t\t";
	std::cout << "$t6 = " << t[6] << "\t\t\t";
	std::cout << "$t7 = " << t[7] << std::endl;
	std::cout << "$t8 = " << t[8] << "\t\t\t";
	std::cout << "$t9 = " << t[9];
}

void pipeline::stop()
{
	std::cout << std::endl << "----------------------------------------------------------------------------------" << std::endl;
	std::cout << "END OF SIMULATION" << std::endl;
	exit(0);
}
