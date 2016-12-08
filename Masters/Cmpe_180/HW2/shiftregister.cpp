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
    for(int i = 1;i < argc; i++) {
        string cmd = argv[i];
        if(cmd == "-i") {
            i++;
            sr.inject(argv[i]);
        } else if (cmd == "-s") {
            i++;
            sr.set_size(atoi(argv[i]));
        } else if (cmd == "-r") {
            i++;
            sr.shift_right(atoi(argv[i]));
        } else if (cmd == "-l") {
            i++;
            sr.shift_left(atoi(argv[i]));
        } else if (cmd == "-v") {
            i++;
            sr.vacated_bits(atoi(argv[i]));
        } else if (cmd == "-p") {
            sr.clean();
            sr.print();
        } else {
			cerr << "The command line argument is incorrect.\n" ; 
            return -1;
        }
    }
    return 0;
}

