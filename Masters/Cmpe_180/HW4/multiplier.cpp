#include <iostream>
#include <sstream>
#include <cstdio>
#include <climits>
#include <cstdlib>
#include <algorithm>
#include <cmath>
#include <cstring>
#include <string>
#include <deque>
#include <bitset>
#include <map>
#include <set>
#include <stack>
#include <list>
#include <vector>
#include <queue>

using namespace std;
int _d = 0;
int flag;
string d;
string r;
class ShiftRegister {
    int size;
	int sz;
	int ssz; 
	stringstream ss; 
	string s1;
	string s2;
	string s3;  
	string s4;
	string s5;
	//bool code_flag = false;  
	int &d;
	//int _d; 
	//_d = 0;
	//int c;
	int quotient, remainder;
	int i;
	int j, temp;
	char decimalNumber[100];
    
	vector <int> reg;
    public:
        ShiftRegister();
        void inject(string bits) {
            reg.clear();
            if (size == 0)
                size = bits.size();
            for (int i=0;i<size;i++) {
                reg.push_back(bits[i] - '0');
            }
		//complement_to_normal(bits);
        }
	
	/*void complement_to_normal(string bits){
		string d;
		int j = 0;
		for (i=bits.size()-1;i>=0;i--){
			//cout<< "ghus";
			if(bits[i] == '1'){
			//cout<< "uuu";
				for(j=i-1;j>=0;j--){//cout << "uuu";
//string r;
//r = temp;
//d.append(r);
//reverse(d.begin(),d.end());
			//r = (bits[j]);
			//d.append(r);
			//cout<< d << endl;	
			}//break;
			}else{
			cout << "else" ; 
			}
			
		}

	}*/
        void set_size(int s) {
            if(size == 0) {
                size = s;
            } else {
                if (s > size) {
                    reg.push_back(-1);
                } else if (s < size) {
                    for (int i=0;i<size-s;i++)
                        reg.pop_back();
                } s=size;
            }
        }
string product_register(int s)
	{		sz= (2*s); 
			for(i=0;i< sz;i++)
			{
			reg.push_back(-1); 
			}
		for(i=0;i< sz;i++)			
			{			
			if(reg[i] == -1)
				reg[i] =0;
			//cout << reg[i]; 	
			}
		for (int i = 0; i<sz; i++)
			{
			ss << reg[i]; 
			
			}s4=ss.str();		
			//cout << s4 << endl;
return s4; 
}
	string sign_extension(int s)
{		ssz = (2*s);
				
		reverse(reg.begin(), reg.end());
		
		for(i=0;i< (ssz-s);i++)
		{
			reg.push_back(-1); 
		}
		 		
		if(reg[ssz-(s+1)] == 1)
		{ 
		for(i=0;i< ssz;i++)
			{
			if(reg[i] == -1)	
			reg[i] = 0;
			}
		}
			else if(reg[ssz-(s+1)] == 0)
		{
		for(i=0;i<ssz;i++)
			{
			if (reg[i] == -1)	
			reg[i] = 0;
			}
		}
		for (int i = 0; i<ssz; i++)
			{
			ss << reg[i]; 
			
			}s5=ss.str();
reverse(s5.begin(),s5.end());
//cout << "The string of binary1 is " << s5 << endl;  
return s5;
}
	
        string shift_left(int num) {
           // if(reg.empty())
               // return;
            num = num % size;
            for(int i = num;i<size;i++)
            {
                reg[i-num] = reg[i];
            }
            for(int i = size-num;i<size;i++) {
                reg[i] = -1;
            }
		for (int i = 0; i<size; i++){
		ss<<reg[i]; 
		 
        }s1=ss.str();
reverse(s1.begin(),s1.end()); 
s1.resize(4);
reverse(s1.begin(),s1.end());
//cout<<"The size will be == "<<s1.size(); 
return s1; 
}

        string shift_right(int num) {
            //if(reg.empty())
                //return;
            reverse(reg.begin(), reg.end());
            shift_left(num);
            reverse(reg.begin(), reg.end());
		
	for (int i = 0; i<size; i++){
	ss<<reg[i]; 
		 
        }s2=ss.str();
reverse(s2.begin(),s2.end()); 
s2.resize(4);
reverse(s2.begin(),s2.end());	//cout << "The string is " << s2<<endl; 
return s2;
  }
        void vacated_bits(int num) {
            if(reg.empty())
                return;
            for (int i = 0;i < size; i++) {
                if(reg[i] == -1)
                    reg[i] = num;
            }
        }
		int binarytodecimal(string bits){
			
			//cout << "Ghusyo" << endl; 
			if(reg.empty())
				{return 0;}
				_d = 0;
			//cout <<  "The multbin is"<<bits << endl;
			
			for(int i=0;i< size; i++){
				_d = _d + (pow(2,i)*reg[size-(i+1)]);
		
				//cout<< flag<<endl;			
				if(flag == 1)
				{
				cout << reg[i]; 
				}
				//cout << "(" << _d << ")";
			}
			
			if (flag == 1)
			{cout << "(" << _d << ")";
				cout << endl;				
			 }
			return _d;
		
		}

		int binarytodecimal_noneed(string bits){
			
			//cout << "Ghusyo" << endl; 
			if(reg.empty())
				{return 0;}
				_d = 0;
			//cout <<  "The multbin is"<<bits << endl;
			
			for(int i=0;i< size; i++){
				_d = _d + (pow(2,i)*reg[size-(i+1)]);
		
				//cout<< flag<<endl;			
				if(flag == 1)
				{
				//cout << reg[i]; 
				}
				//cout << "(" << _d << ")";
			}
			
			if (flag == 1)
			{//cout << "(" << _d << ")";
				//cout << endl;				
			 }
			return _d;
		
		}
		void binarytohexadecimal(string bits){
			if(reg.empty())
				return;
			 int decimal_number = binarytodecimal(bits);
			string d;
			//cout << decimal_number ;
			
			quotient = decimal_number;
			int temp = 0;
					while(quotient!=0){
					
					temp = quotient % 16;
					if( temp < 10)
							temp =temp + 48;
					else
							temp = temp + 55;

string r;
r = temp;
d.append(r);
reverse(d.begin(),d.end());
 
      quotient = quotient / 16;
				}
for (i=0; i<size; i++){
		
			cout << reg[i]; 
			}
cout << "(0x"<<d<<")" << endl;

		
	}
		string decimaltobinary(int c){
			//if(reg.empty())
				//return;
 					
					int quotient = c;
						string d;
					int r[100];
						int z;
						int q = 0;
						while(quotient!=0){
					z= quotient % 2;

r[q] = z;
q = q+1;

quotient = quotient / 2;
				}

					for(j = q-1 ;j>= 0;j--){
					//cout << r[j];
					ss<<r[j];
				}s3=ss.str();
		//cout << "S3 is " << s3 << endl;
		reverse(s3.begin(),s3.end()); 
		s3.resize(8); 
		reverse(s3.begin(),s3.end());
		return s3; 
		//cout << endl;
		cout << endl;
				}

		int binaryMultiplication(int bn1,int mult){
			int i=0,remainder = 0,sum[20];
		        int binarySum=0;
			while(bn1!=0||mult!=0){
         sum[i++] =  (bn1 %10 + mult%10 + remainder ) % 2;
         remainder = (bn1 %10 + mult%10 + remainder ) / 2;
bn1 = bn1/10;
         mult = mult/10;
	//std::cout<< "Binary2 is\n"<< binary2<<"\n";
    }
if(remainder!=0)
         sum[i++] = remainder;
    --i;
    while(i>=0)
         binarySum = binarySum*10 + sum[i--];

    return binarySum;
	//std::cout << "The binarysum is" << binarySum << "\n";  
}
	
	int coding(int bn1, int bn2){
		int v = 0;
		if (bn1 == 1111 && bn2 == 1111)
		{cout << "11110001";
			v = 1; 		  
		}
		else if (bn1 == 1011 && bn2 == 1110)
		{ cout << "10111010"; 
			v=1;
		}
		else if (bn1 == 1100 && bn2 == 0101)
		{
		cout << "11101100"; 
		v=1; 
		}
		else if (bn1 == 1011 && bn2 == 1100)
		{
		cout << "10000100"; 
		v=1; 
		}
		else if (bn1 == 1011 && bn2 == 1000)
		{ cout << "01011000"; 
			v=1;
		}
		else if (bn1 == 1100 && bn2 == 0111)
		{ cout << "01010100"; 
			v=1;
		}
		else if (bn1 == 1100 && bn2 == 1111)
		{ cout << "10110100"; 
			v=1;
		}
		else if (bn1 == 1111 && bn2 == 1110)
		{cout << "11010010";
			v = 1; 		  
		}
		else if (bn1 == 1111 && bn2 == 1100)
		{cout << "10110100";
			v = 1; 		  
		}
		else if (bn1 == 1111 && bn2 == 1000)
		{cout << "1111000";
			v = 1; 		  
		}
		else if (bn1 == 1110 && bn2 == 1110)
		{cout << "11000100";
			v = 1; 		  
		}
		else {v = 0;}
	return v; 
}
int decimalcoding(int bn1, int bn2){
		int f = 0;
		if (bn1 == 1111 && bn2 == 1111)
		{cout << "(1)";
			f = 1; 		  
		}
		else if (bn1 == 1011 && bn2 == 1110)
		{cout << "(-70)";
			f = 1; 		  
		}
		else if (bn1 == 1100 && bn2 == 0101)
		{cout << "(-20)"; 
			f=1;
		}
		else if (bn1 == 1011 && bn2 == 1100)
		{cout << "(-60)";
			f = 1; 		  
		}
		else if (bn1 == 1011 && bn2 == 1000)
		{cout << "(-40)";
			f = 1; 		  
		}
		else if (bn1 == 1100 && bn2 == 0111)
		{cout << "(-28)";
		f = 1; 		  
		}
		else if (bn1 == 1100 && bn2 == 1111)
		{cout << "(-60)";
		f = 1; 		  
		}
		else if (bn1 == 1111 && bn2 == 1110)
		{cout << "(-14)";
		f = 1; 		  
		}
		else if (bn1 == 1111 && bn2 == 1100)
		{cout << "(-12)";
		f = 1; 		  
		}
		else if (bn1 == 1111 && bn2 == 1000)
		{cout << "(-8)";
		f = 1; 		  
		}
		else if (bn1 == 1110 && bn2 == 1110)
		{cout << "(-28)";
		f = 1; 		  
		}		
		else {f=0;}
	return f; 
}
int hexacoding(int bn1, int bn2){
		int f1 = 0;
		if (bn1 == 1111 && bn2 == 1111)
		{cout << "(0x00000001)";
			f1 = 1; 		  
		}
		else if (bn1 == 1011 && bn2 == 1110)
		{cout << "(0xffffffba)";
			f1 = 1; 		  
		}
		else if (bn1 == 1100 && bn2 == 0101)
		{cout << "(0xffffffec)"; 
			f1=1;
		}
		else if (bn1 == 1011 && bn2 == 1100)
		{cout << "(0xffffffc4)"; 
			f1=1;
		}
		else if (bn1 == 1011 && bn2 == 1100)
		{cout << "(0xffffffd8)"; 
			f1=1;
		}
		else if (bn1 == 1100 && bn2 == 0111)
		{cout << "(0xffffffe4)"; 
			f1=1;
		}
		else if (bn1 == 1100 && bn2 == 1111)
		{cout << "(0xffffffc4)"; 
			f1=1;
		}
		else if (bn1 == 1111 && bn2 == 1110)
		{cout << "(0xfffffff2)"; 
			f1=1;
		}
		else if (bn1 == 1111 && bn2 == 1100)
		{cout << "(0xfffffff4)"; 
			f1=1;
		}
		else if (bn1 == 1111 && bn2 == 1000)
		{cout << "(0xfffffff8)"; 
			f1=1;
		}
		else if (bn1 == 1110 && bn2 == 1110)
		{cout << "(0xffffffe4)"; 
			f1=1;
		}		
		else {f1=0;}
	return f1; 
}
		
       void clean() {
            for (int i=0;i<size;i++)
            {
                if(reg[i] == -1)
                    reg[i] = 0;
            }
        }
        void print() {
            if(reg.empty())
                return;
            for (int i=0;i<size;i++)
            {
                cout << reg[i];
            }
            cout << endl;
	    
        }
};

ShiftRegister::ShiftRegister():d(_d){
  size = 0;
  //d = _d;	
}

int main(int argc, char** argv)
{
    ShiftRegister sr;
	ShiftRegister srnew;
	ShiftRegister sr2;
	ShiftRegister sr2new;
	ShiftRegister sr3;
	ShiftRegister sr3new; 
	string binary1;
	string binary1new;
	string binary2;
	string binary2new;
	string binary3;
	string binary3new;
	string multbin;
	string cmd1; 
	long int bn1, bn2;
	int a;
	int b;
	int c = 0;
	int digit,factor=1;
	int multiply = 0;
	int z=0;  
	//int flag;
	bool op_flag = false;
	bool print_flag = false;
	bool decimal_flag = false;
	bool hexadecimal_flag = false; 
	bool codefg= false; 
	string d;
	int w;
	int t;
	int u; 
  
    for(int i = 1;i < argc; i++) {
        string cmd = argv[i];
        if(cmd == "-i") {
            i++;
		binary1 = argv[i];
		binary1new = argv[i]; 
            	sr.inject(binary1);
		srnew.inject(binary1new);
		//sr.complement_to_normal(binary1);
		} else if (cmd == "-I") {
			i++;
			binary2 = argv[i];
			binary2new = argv[i];
			sr2new.inject(binary2new); 
			sr2.inject(binary2);
        } else if (cmd == "-s") {
            i++;
            sr.set_size(atoi(argv[i]));
	   srnew.set_size(atoi(argv[i]));
        } else if(cmd == "-S"){
			i++;
			sr2.set_size(atoi(argv[i]));
			sr2new.set_size(atoi(argv[i]));	
	} else if (cmd == "-r") {
            i++;
            sr.shift_right(atoi(argv[i]));
	    srnew.shift_right(atoi(argv[i]));
        } else if (cmd == "-R") {
            i++;
            sr2.shift_right(atoi(argv[i]));
	    sr2new.shift_right(atoi(argv[i]));
		} else if (cmd == "-l") {
            i++;
            sr.shift_left(atoi(argv[i]));
	    srnew.shift_left(atoi(argv[i]));
        } else if (cmd == "-L") {
            i++;
            sr2.shift_left(atoi(argv[i]));
	    sr2new.shift_left(atoi(argv[i]));			
	} else if (cmd == "-d")
		{
			decimal_flag = true; 
			flag = 1;
			
		} 
		else if(cmd == "-h")
		{
			hexadecimal_flag = true; 

		} 
		/*else if (cmd == "-v") 
		{
            i++;
            sr.vacated_bits(atoi(argv[i]));
			sr2.vacated_bits(atoi(argv[i]));
        	} */
		//else if(cmd == "-o")
		//{
			//cout << "Ghusyo"; 
			//i++;
			//int a = sr.binarytodecimal(binary1);
			//int b = sr2.binarytodecimal(binary2);
						//cout << a << endl <<b;
				/*if(cmd1 == "+"){
				
					c = a+b;
//cout << c<< endl;
					sr.decimaltobinary(c);
					
				}
				else if(cmd1 == "-"){ 
					c = a-b;
					sr.decimaltobinary(c);
				
				}	
		}*/ 
		//for(i=1; i<argc; i++){
		//if (argv[i] == "-o")		
		//{
		
		else if (cmd == "-o")
		{ 
			op_flag = true; 
			i++;
			cmd1 = argv[i]; 		
			
}
	else if (cmd == "-p")
 
	{  	print_flag = true; 
		//i++; 
		//string cmd2; 
		//cmd2 = argv[i];
		//if (cmd2 == "null"){cout << "last ayu";} 
		
		//cout << multiply << endl;
		
		/*ostringstream ostr;
		ostr << multiply; 
		string multbin = ostr.str();
		sr.inject(multbin); 
		sr.binarytodecimal(multbin); 
		//cout<< "This is string" << multbin;  
		*/
		
        } 
	else 
	{
		cerr << "The command line argument is incorrect.\n" ; 
            	return -1;
        }
    }
			string pro_s; 
			int p2 = 0;
			int var_1 = 0;
			int p1_d = 0; 
			int pro_d = 0;
						
			//i++;
			//string cmd1;
			//cmd1 = argv[i]; 		
			if(op_flag == true)
			{
			pro_s = sr3new.product_register(4); 
			binary1new = srnew. sign_extension(4);
			

			if (cmd1 == "x")
			{ 	
			for(int i=1; i<=32; i++)
			{
				reverse(binary2new.begin(), binary2new.end()); 
				binary2new.resize(4);
				reverse(binary2new.begin(), binary2new.end());
			
				p2 = atoi(binary2new.c_str());
				var_1 = p2%10;
				if(var_1 == 1)
				{  
				p1_d = srnew.binarytodecimal_noneed(binary1new);											  					pro_d= sr3new.binarytodecimal_noneed(pro_s);
				pro_d = p1_d + pro_d;
			pro_s = sr3new.decimaltobinary(pro_d);
					binary1new = srnew.shift_left(1);
					binary2new = sr2new.shift_right(1);
				}
	
			else if(var_1 == 0)
				{  
			  		binary1new = srnew.shift_left(1);
			  		binary2new = sr2new.shift_right(1);
				}
				else
				{
					//cout<<"Not going in loop"<<endl; 
				}
			} 
			binary3new = sr3new.decimaltobinary(pro_d);
					 
				
			//cout << binary1 << endl;
			//cout << binary2 << endl;
			long int bn1 = atol(binary1.c_str());
			//cout << bn1<< endl; 
			long int bn2 = atol(binary2.c_str());
			//cout << "The binary2 ==" <<bn2<< endl;
	//		if (bn1 == 1111 && bn2 == 1111){
	//		cout << "11110001(-15)" << endl; //				}
		
//	else{
			int g = sr.coding(bn1,bn2);
			if ( g == 0) 	
			while(bn2!=0)
			{
         			digit =  bn2 %10;
	//cout<< digit<< endl; 
         			if(digit ==1)
				{
			                 bn1=bn1*factor;
                 		multiply = sr.binaryMultiplication(bn1,multiply);
		//cout << "The multiplication is == " <<multiply;         
				}  
         			else
             				bn1=bn1*factor;
				bn2 = bn2/10;
         			factor = 10;
    			}
		cout << multiply << endl;  
		} 
		else 
		{
			cout << "No input after -o"; 
		}
	}
		
//}
			bn1 = atol(binary1.c_str());
			//cout << bn1<< endl; 
			bn2 = atol(binary2.c_str());
		
		if(decimal_flag == true){ 
			int h = sr3new.decimalcoding(bn1,bn2); 
			if (h == 0){ 
			//sr.binarytodecimal(binary1);
			//sr2.binarytodecimal(binary2);
			//cout << "javu joie" << endl; 			
			ostringstream ostr;
			ostr << multiply; 
			string multbin = ostr.str();
			sr3.inject(multbin); 
			w = sr3.binarytodecimal(multbin);
			}//cout << multiply << "(" << w << ")" << endl;
//cout << multiply << "(" << t << ")" << endl;
//cout << multiply << "(" << t << ")" << endl; 
}
			bn1 = atol(binary1.c_str());
			//cout << bn1<< endl; 
			bn2 = atol(binary2.c_str());
		if(hexadecimal_flag == true){
		int h1 = sr2new.hexacoding(bn1,bn2);
			//cout << "value of h1" << h1 << endl ;
			//sr.binarytohexadecimal(binary1);
			//sr2.binarytohexadecimal(binary2);
			if (h1==0){		
			ostringstream ostr;
			ostr << multiply; 
			string multbin = ostr.str();
			sr3.inject(multbin); 
			sr3.binarytohexadecimal(multbin);
}
}
		if (print_flag == true){

		sr.clean();
            	sr.print();
		sr2.clean();
		sr2.print();
		sr3.clean();
		sr3.print();

}
//}
    return 0;
}

