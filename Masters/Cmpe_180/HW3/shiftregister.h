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
class ShiftRegister {
    int size;
	int &d = 0;
	long int quotient, remainder;
	int i = 1, j, temp;
	char hexadecimalNumber[100];
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
        }
        void set_size(int s) {
            if(size == 0) {
                size = s;
            } else {
                if (s > size) {
                    reg.push_back(-1);
                } else if (s < size) {
                    for (int i=0;i<size-s;i++)
                        reg.pop_back();
                } size = s;
            }
        }
        void shift_left(int num) {
            if(reg.empty())
                return;
            num = num % size;
            for(int i = num;i<size;i++)
            {
                reg[i-num] = reg[i];
            }
            for(int i = size-num;i<size;i++) {
                reg[i] = -1;
            }
        }
        void shift_right(int num) {
            if(reg.empty())
                return;
            reverse(reg.begin(), reg.end());
            shift_left(num);
            reverse(reg.begin(), reg.end());
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
			if(reg.empty())
				return;
			for(int i=0;i<size; i++){
				d = d + pow(2,i)*reg[i];
				cout << reg[i];
			}
			return d;
		}
		void binarytohexadecimal(string bits){
			if(reg.empty())
				return;
			long int decimal_number = binarytodecimal(string bits);
			
			quotient = decimal_number;

					while(quotient!=0){
					
					temp = quotient % 16;
					if( temp < 10)
							temp =temp + 48;
					else
							temp = temp + 55;

      hexadecimalNumber[i++]= temp;
      quotient = quotient / 16;
				}
					for(j = i -1 ;j> 0;j--){
					cout<<hexadecimalNumber[j];
					cout<<endl;
				}
	}
		void decimaltobinary(long int c){
			if(reg.empty())
				return;
					quotient = c;
					
						int i = 0;
						while(quotient!=0){
							decimalNumber[i++]= quotient % 2;

							quotient = quotient / 2;
				}
					for(j = i -1 ;j> 0;j--){
					cout<<decimalNumber[j];
					cout<<endl;
				}
			
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
ShiftRegister::ShiftRegister () {
    size = 0;
}

int main(int argc, char** argv)
{
    ShiftRegister sr;
	ShiftRegister sr2;
	string &binary1;
	string &binary2;
	long int a;
	long int b;
	long int c; 
    for(int i = 1;i < argc; i++) {
        string cmd = argv[i];
        if(cmd == "-i") {
            i++;
			binary1 = argv[i];
            sr.inject(binary1);
		} else if (cmd == "-I") {
			i++;
			binary2 = argv[i];
			sr2.inject(binary2);
        } else if (cmd == "-s") {
            i++;
            sr.set_size(atoi(argv[i]));
        } else if(cmd == "-S"){
			i++;
			sr2.set_size(atoi(argv[i]));
		} else if (cmd == "-r") {
            i++;
            sr.shift_right(atoi(argv[i]));
        } else if (cmd == "-R") {
            i++;
            sr2.shift_right(atoi(argv[i]));
		} else if (cmd == "-l") {
            i++;
            sr.shift_left(atoi(argv[i]));
        } else if (cmd == "-L") {
            i++;
            sr2.shift_left(atoi(argv[i]));
		} else if (cmd == "-d"){
			 sr.binarytodecimal(binary1);
			 cout << "(" << d << ")";
			 sr2.binarytodecimal(binary2);
			 cout << "(" << d << ")";
		} else if(cmd == "-h"){
			sr.binarytohexadecimal(binary1)
			sr2.binarytohexadecimal(binary2);
		} else if (cmd == "-v") {
            i++;
            sr.vacated_bits(atoi(argv[i]));
			sr2.vacated_bits(atoi(argv[i]);
        } else if(cmd == "-o"){
			i++;
			long int a = binarytodecimal(binary1);
			long int b = binarytodecimal(binary2);
				if(argv[i] == "+"){
					 c = a + b;
					decimaltobinary(c);
				}
				else{ 
					c = a-b;
					decimaltobinary(c);
				}	
		} else if (cmd == "-p") {
            sr.clean();
            sr.print();
			sr2.print();
        } else {
			cerr << "The command line argument is incorrect.\n" ; 
            return -1;
        }
    }
    return 0;
}

