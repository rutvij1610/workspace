/*
	Evaluation Of postfix exp in C++
	Input Postfix exp must be in a desired format.
	Operands must be integers and there should be space in between two operands.
	Only '+' , '-' , '*' and '/' operators are expected.
	*/
	#include<iostream>
	#include<stack>
	#include<string>
	
	using namespace std;
	
	// Function to evaluate Postfix exp and return output
	int EvaluatePostfix(string exp);
	
	// Function to perform an operation and return output.
	int PerformOperation(char operation, int op1, int op2);
	
	// Function to verify whether a character is operator symbol or not.
	bool IsOperator(char C);
	
	// Function to verify whether a character is numeric digit.
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
	//cout<<"Enter Postfix exp \n";
		//getline(cmd1,exp);
		exp = cmd1; 
		int result = EvaluatePostfix(exp);
		cout<< result <<"\n";
		}
	}
	}
	return 0; 
}	
	// Function to evaluate Postfix exp and return output
	int EvaluatePostfix(string exp)
	{
	// Declaring a Stack from Standard template library in C++.
	stack<int> S;
	
	for(int i = 0;i< exp.length();i++) {
	
	// Scanning each character from left.
	// If character is a delimitter, move on.
	if(exp[i] == ' ' || exp[i] == ',') continue;
	
	// If character is operator, pop two elements from stack, perform operation and push the result back.
	else if(IsOperator(exp[i])) {
	// Pop two operands.
	int op2 = S.top(); S.pop();
	int op1 = S.top(); S.pop();
	// Perform operation
	int result = PerformOperation(exp[i], op1, op2);
	//Push back result of operation on stack.
	S.push(result);
	}
	else if(IsNumericDigit(exp[i])){
	// Extract the numeric operand from the string
	// Keep incrementing i as long as you are getting a numeric digit.
	int operand = 0;
	while(i<exp.length() && IsNumericDigit(exp[i])) {
	// For a number with more than one digits, as we are scanning from left to right.
	// Everytime , we get a digit towards right, we can multiply current total in operand by 10
	// and add the new digit.
	operand = (operand*10) + (exp[i] - '0');
	i++;
	}
	// Finally, you will come out of while loop with i set to a non-numeric character or end of string
	// decrement i because it will be incremented in increment section of loop once again.
	// We do not want to skip the non-numeric character by incrementing i twice.
	i--;
	
	// Push operand on stack.
	S.push(operand);
	}
	}
	// If exp is in correct format, Stack will finally have one element. This will be the output.
	return S.top();
	}
	
	// Function to verify whether a character is numeric digit.
	bool IsNumericDigit(char C)
	{
	if(C >= '0' && C <= '9') return true;
	return false;
	}
	
	// Function to verify whether a character is operator symbol or not.
	bool IsOperator(char C)
	{
	if(C == '+' || C == '-' || C == '*' || C == '/')
	return true;
	
	return false;
	}
	
	// Function to perform an operation and return output.
	int PerformOperation(char operation, int op1, int op2)
	{
	if(operation == '+') return op1 +op2;
	else if(operation == '-') return op1 - op2;
	else if(operation == '*') return op1 * op2;
	else if(operation == '/') return op1 / op2;
	
	else cout<<"Unexpected Error \n";
	return -1;
	}
