
	#include<iostream>
	#include<stack>
	#include<string>
	
	using namespace std;
	
	int EvaluatePostfix(string exp);
	
	int PerformOperation(char operation, int op1, int op2);
	
	bool IsOperator(char C);
	
	bool IsNumericDigit(char C);
	
int main(int argc, char** argv)
{
	bool e_flag = false; 
	string exp;
	string cmd1; 
	for (int i = 1; i < argc; i++)
	{
	string cmd  = argv[i]; 
	if(cmd == "-e")
	{
	e_flag = true; 
		if(e_flag = true)
		{
		i++;
		string cmd1 = argv[i];  	
		exp = cmd1; 
		int result = EvaluatePostfix(exp);
		cout<< result <<"\n";
		}
	}
	}
	return 0; 
}	
	
	int EvaluatePostfix(string exp)
	{
	
	stack<int> S;
	
	for(int i = 0;i< exp.length();i++) {
	
	
	if(exp[i] == ' ' || exp[i] == ',') continue;
	
	else if(IsOperator(exp[i])) {

	int op2 = S.top(); S.pop();
	int op1 = S.top(); S.pop();
	
	int result = PerformOperation(exp[i], op1, op2);
	S.push(result);
	}
	else if(IsNumericDigit(exp[i])){

	int operand = 0;
	while(i<exp.length() && IsNumericDigit(exp[i])) {
	operand = (operand*10) + (exp[i] - '0');
	i++;
	}
	i--;
	S.push(operand);
	}
	}
	return S.top();
	}
	
	bool IsNumericDigit(char C)
	{
	if(C >= '0' && C <= '9') return true;
	return false;
	}
	
	bool IsOperator(char C)
	{
	if(C == '+' || C == '-' || C == '*' || C == '/')
	return true;
	
	return false;
	}
	
	int PerformOperation(char operation, int op1, int op2)
	{
	if(operation == '+') return op1 +op2;
	else if(operation == '-') return op1 - op2;
	else if(operation == '*') return op1 * op2;
	else if(operation == '/') return op1 / op2;
	
	else cout<<"Unexpected Error \n";
	return -1;
	}
