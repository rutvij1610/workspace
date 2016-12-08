/*
 Best CPU
 */
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <signal.h>
#include <inttypes.h>
#include <errno.h>

/*Special purpose registers for CPU*/

/*Program Counter to fetch the next instruction*/
unsigned int ProgramCounter;

/* Stack Pointer to points toward the top of the stack */
unsigned int StackPointer;

/* Frame Pointer to points to the start of the stack frame */
unsigned int BasePointer = 500;

unsigned int ax;

unsigned int bx;

unsigned int cx;

/* Memory of 2048 bytes with word size of 32 bits (4 bytes) */
unsigned char mem[2048];

/* Endian Type*/
int endian_type = 1;

/* Flag Register putting each bit for every flag */
unsigned int flag_register;

/* Bits position in Flag register starting from 0 to 8, bits all defined */

enum flag_registers{
    CarryFlag = 0, 		/* Carry Flag */
	ParityFlag = 1, 	/* Parity Flag */
	AdjustFlag = 2, 	/* Adjust Flag */
    ZeroFlag = 3, 		/* Zero Flag */
	SignFlag = 4, 		/* Sign Flag */
	TrapFlag = 5, 		/* Trap Flag */
    InterruptFlag = 6, 	/* Interrupt Enable Flag */
	DirectionFlag = 7, 	/* Direction Flag */
	OverflowFlag = 8, 	/* Overflow Flag */
    INVALID  		    /*  Invalid */
};

char *flags[INVALID+1] =
{"Carry Flag ","Parity Flag ","Adjust Flag ","Zero Flag","Sign Flag ","Trap Flag ","Interrupt Flag ","Direction Flag","Overflow Flag ", ""};


/* General purpose registers for CPU */
unsigned int Register0;		unsigned int Register1;		unsigned int Register2;		unsigned int Register3;
unsigned int Register4;		unsigned int Register5;		unsigned int Register6;		unsigned int Register7;
unsigned int Register8;		unsigned int Register9;		unsigned int Register10;	unsigned int Register11;
unsigned int Register12;	unsigned int Register13;	unsigned int Register14;	unsigned int Register15;
unsigned int Register16;	unsigned int Register17;	unsigned int Register18;	unsigned int Register19;
unsigned int Register20;	unsigned int Register21;	unsigned int Register22;	unsigned int Register23;
unsigned int Register24;	unsigned int Register25;	unsigned int Register26;	unsigned int Register27;
unsigned int Register28;	unsigned int Register29;	unsigned int Register30;	unsigned int Register31;

enum Register_Conversion_Index{
    Index_Register0 = 0,	Index_Register1,		Index_Register2,		Index_Register3,
    Index_Register4,		Index_Register5,		Index_Register6,		Index_Register7,
    Index_Register8,		Index_Register9,		Index_Register10,		Index_Register11,
    Index_Register12,		Index_Register13,		Index_Register14,		Index_Register15,

	Index_Register16 = 16,	Index_Register17,		Index_Register18,		Index_Register19,
    Index_Register20,		Index_Register21,		Index_Register22,		Index_Register23,
    Index_Register24,		Index_Register25,		Index_Register26,		Index_Register27,
    Index_Register28,		Index_Register29,		Index_Register30,		Index_Register31,

	Index_BP = 32,			Index_SP, 				Index_PC, 				Index_AX,
	Index_BX = 36, 			Index_CX, 				Index_INVALID
};

/* Opcodes for Various CPU operations. Current Scope {LOAD, STORE, ADD, MUL, SUB, DIV, MOD} */
/* {Addi, Muli, Subi, Divi, , Modi, pop, push}  and Conditional codes, Jump and Functional Calls*/
/* Opcodes */
enum OPCODE{

	STORE_TO_MEMORY = 3,
	LOAD_FROM_MEMORY = 4,
	LOAD_MEMORY_I 	,
	MOV		,
	ADD		,
	SUB		,
	MUL		,
	DIV		,
	MOD		,
	PUSH	,
	POP		,
	ADDI 	,
	ADDT	,
	SUBI	,
	MULI	,
	DIVI	,
	MODI	,
	JMP		,
	JNE		,
	JNC		,
	JC		,
	JNO		,
	JO 		,
	JNZ		,
	JZ		,
	CMPI    ,
	AND     ,
	OR      ,
	XOR     ,
	SHR     ,
	SHL     ,
	LEA		,
	CMP  	,
	JA 		,
	CMPV	,
	JAE 	,
	DEC
};


/*Memory is divided in three parts,
	1)Bootstrap memory ->  which starts at 0 to 127 bytes
	2)Instruction memory -> from which the Instruction will be fetched, The Instruction will be 32 bit. This memory will be bytes starting from 128 to 383  bytes. 	This Instruction memory can contain 512/4 = 128 instuctions.
	3)Data Memory -> Contains data, In this case the data will be read from or written into this memory. This memory will start at 896 bytes.*/

#define Start_Boot_Memory 0
#define Start_Instruction_Memory 128
#define Start_Data_Memory 384
#define STACK_BOTTOM 896
#define STACK_TOP 1024
#define STACK_MEMORY_BASE 896
#define BIG__ENDIAN 0
#define LITTLE__ENDIAN 1


#define MAX_LABELS	10

typedef struct label_table{
	char 		name[10];
	unsigned int 	address;
}LABEL_TABLE;

LABEL_TABLE	all_label_table[MAX_LABELS];

LABEL_TABLE	pending_label_table[MAX_LABELS];

static unsigned int filled_all_labels = 0;

unsigned int add_label(char *name, unsigned int address)
{

	if(name != NULL)
		strcpy(all_label_table[filled_all_labels].name, name);
	else
		return 0;

	all_label_table[filled_all_labels].address = address;

	filled_all_labels++;
	//printf("\n_____ %s %d ____ address = %d , label = %s\n",__func__,__LINE__, address, name);

	return 1;
}

unsigned int get_address_from_label(char *label)
{
	unsigned int index;
	unsigned int address = 0;


	if (label == NULL)
		return 0;

	for (index = 0; index < MAX_LABELS && index < filled_all_labels; index++)
	{
		//printf("table label = %s, cmp label = %s, strcasecmp = %d\n", all_label_table[index].name, label, strncasecmp(label, all_label_table[index].name, 2 ));
		if(!strncasecmp(all_label_table[index].name, label, 2))
		{
			address = all_label_table[index].address;
			break;
		}
	}

	return address;
}

static unsigned int filled_pending_labels = 0;

/* This is for Forward Jump instructions labels */
unsigned int add_pending_label(char *name, unsigned int address)
{

	if(name != NULL)
	{
		strcpy(pending_label_table[filled_pending_labels].name, name);
		pending_label_table[filled_pending_labels].address = address;
	}
	else
		return 0;

	filled_pending_labels++;

	return 1;
}

unsigned int get_from_pending_table(char *name, unsigned int *address)
{
	static unsigned int current_index = 0;
	//printf("inside get_from_pending_table filled_pending_labels = %d , current_index = %d\n", filled_pending_labels, current_index);
	if(current_index < filled_pending_labels)
	{
		//printf("name = %s, adress = %d \n", pending_label_table[current_index].name, pending_label_table[current_index].address);
		strcpy(name, pending_label_table[current_index].name );
		*address = pending_label_table[current_index].address;
		current_index ++;
	}
	else
		return 0;

	return 1;
}

/* Check Flag Function to check a flag status */
int check_flag(unsigned int index)
{
	if(flag_register & (1 << index))
		return 1;
	else
		return 0;
}
/* Function to Set Flags for Different Overflows */
void set_flag(unsigned int index)
{
	if (index < INVALID)
		flag_register |= 1 << index;
	return;
}
/* Clear Flag Funtion to clear the Flag set on an opration based on index */
void clear_flag(unsigned int index)
{
	if (index < INVALID)
		flag_register &= ~(1 << index);
	return;
}

/*To print out a memory*/
unsigned char memory_print = 1;

/*Printing out all the memory including Bootstrap, Instruction and Data*/
void print_all(void) {
    int i;

    printf("\t\tGeneral Purpose Registers\n");

    printf("\tRegister0 = %u\n\tRegister1 = %u\n\tRegister2 = %u\n\tRegister3 = %u\n\t", Register0, Register1, Register2, Register3);
    printf("Register4 = %u\n\tRegister5 = %u\n\tRegister6 = %u\n\tRegister7 = %u\n\t", Register4, Register5, Register6, Register7);
    printf("Register8 = %u\n\tRegister9 = %u\n\tRegister10 = %u\n\tRegister11 = %u\n\t", Register8, Register9, Register10, Register11);
    printf("Register12 = %u\n\tRegister13 = %u\n\tRegister14 = %u\n\tRegister15 = %u\n\t", Register12, Register12, Register14, Register15);

	printf("\n*******************************************************\n");
	printf("\tProgram Counter\n");
	printf("*******************************************************\n");
	printf("\tProgramCounter = %u\n", ProgramCounter+1);

	printf("\n*******************************************************\n");
	printf("\tStack Pointer\n");
	printf("*******************************************************\n");
	printf("\tStackPointer = %u\n", StackPointer);

	printf("\n*******************************************************\n");
	printf("\tBase Pointer and Other\n");
	printf("*******************************************************\n");
	printf("\tBasePointer = %u\tAX = %u\tBX = %u\tCX = %u\n", BasePointer,ax,bx,cx);

	printf("\n*******************************************************\n");
	printf("\tFlags\n");
	printf("*******************************************************\n");
	printf("\tflag = %u\n", flag_register);


    for (i = 0; i < INVALID ; i++) {
        if(check_flag(i))
            printf("\t%s = 1", flags[i]);
        else
            printf("\t%s = 0", flags[i]);
    }

    if(memory_print)
    {
        printf("\n______________________________________________________\n");
        printf("\t\t\tBootstrap Memory\n");
        printf("_______________________________________________________\n");

        printf("Address    \t\t\tMemory\n");

        for (i = 0; i < Start_Instruction_Memory; i = i + 16)
        {
            printf("%06d %02X %02X %02X %02X  ", i, mem[i], mem[i+1], mem[i+2], mem[i+3]);
            printf("%02X %02X %02X %02X  ", mem[i+4], mem[i+5], mem[i+6], mem[i+7]);
            printf("%02X %02X %02X %02X  ", mem[i+8], mem[i+9], mem[i+10], mem[i+11]);
            printf("%02X %02X %02X %02X  \n", mem[i+12], mem[i+13], mem[i+14], mem[i+15]);
        }

        printf("\n______________________________________________________\n");
        printf("\t\t\tInstruction Memory\n");
        printf("_______________________________________________________\n");
        printf("Address    \t\t\tMemory\n");
        for (i = Start_Instruction_Memory; i < Start_Data_Memory; i = i+16)
        {
            printf("%06d   %02X %02X %02X %02X  ", i, mem[i], mem[i+1], mem[i+2], mem[i+3]);
            printf("%02X %02X %02X %02X  ", mem[i+4], mem[i+5], mem[i+6], mem[i+7]);
            printf("%02X %02X %02X %02X  ", mem[i+8], mem[i+9], mem[i+10], mem[i+11]);
            printf("%02X %02X %02X %02X  \n", mem[i+12], mem[i+13], mem[i+14], mem[i+15]);
        }

        printf("\n________________________________________________________\n");
        printf("\t\t\tData Memory\n");
        printf("_________________________________________________________\n");
        printf("Address    \t\t\tMemory\n");
        for (i = Start_Data_Memory; i < STACK_MEMORY_BASE; i = i+16)
        {
            printf("%06d   %02X %02X %02X %02X  ", i, mem[i], mem[i+1], mem[i+2], mem[i+3]);
            printf("%02X %02X %02X %02X  ", mem[i+4], mem[i+5], mem[i+6], mem[i+7]);
            printf("%02X %02X %02X %02X  ", mem[i+8], mem[i+9], mem[i+10], mem[i+11]);
            printf("%02X %02X %02X %02X  \n", mem[i+12], mem[i+13], mem[i+14], mem[i+15]);
        }

		printf("\n*******************************************************\n");
		printf("\t\t\t\tStack Memory\n");
		printf("*******************************************************\n");
		printf("Address    \t\tMemory\n");
		for (i = STACK_MEMORY_BASE; i < 2048; i = i+16)
		{
		    printf("%06d   %02X %02X %02X %02X  ", i, mem[i], mem[i+1], mem[i+2], mem[i+3]);
		    printf("%02X %02X %02X %02X  ", mem[i+4], mem[i+5], mem[i+6], mem[i+7]);
		    printf("%02X %02X %02X %02X  ", mem[i+8], mem[i+9], mem[i+10], mem[i+11]);
		    printf("%02X %02X %02X %02X  \n", mem[i+12], mem[i+13], mem[i+14], mem[i+15]);
		}

        printf("\n\n");
    }
}
/*Read Register function reads the data from Registers. It takes the index of register as an argument and returns data from registers.*/
int Read_Data_From_Register(int register_index)
{
    switch (register_index)
    {
        case Index_Register0: 		return Register0;
        case Index_Register1: 		return Register1;
        case Index_Register2: 		return Register2;
        case Index_Register3: 		return Register3;
        case Index_Register4: 		return Register4;
        case Index_Register5: 		return Register5;
        case Index_Register6: 		return Register6;
        case Index_Register7: 		return Register7;
        case Index_Register8: 		return Register8;
        case Index_Register9: 		return Register9;
        case Index_Register10: 		return Register10;
        case Index_Register11: 		return Register11;
        case Index_Register12: 		return Register12;
        case Index_Register13: 		return Register13;
        case Index_Register14: 		return Register14;
        case Index_Register15: 		return Register15;
		case Index_Register16: 		return Register16;
        case Index_Register17: 		return Register17;
        case Index_Register18: 		return Register18;
        case Index_Register19: 		return Register19;
        case Index_Register20: 		return Register20;
        case Index_Register21: 		return Register21;
        case Index_Register22: 		return Register22;
        case Index_Register23: 		return Register23;
        case Index_Register24: 		return Register24;
        case Index_Register25: 		return Register25;
        case Index_Register26: 		return Register26;
        case Index_Register27: 		return Register27;
        case Index_Register28: 		return Register28;
        case Index_Register29: 		return Register29;
        case Index_Register30: 		return Register30;
        case Index_Register31: 		return Register31;
		case Index_SP: 				return StackPointer;
		case Index_BP: 				return BasePointer;
		case Index_PC: 				return ProgramCounter;
        default: printf("No data in the register or can't read\n\n\n");
            break;
    }

    printf("No reg read can happen for %d\n", register_index);
    return 0;
}

/*Write register function writes data in the register, It takes register index and data as an argument*/
void Write_Data_into_Register(int register_index, int value)
{
    switch(register_index)
    {
        case Index_Register0:
            Register0 = value;
            break;
        case Index_Register1:
            Register1 = value;
            break;
        case Index_Register2:
            Register2 = value;
            break;
        case Index_Register3:
            Register3 = value;
            break;
        case Index_Register4:
            Register4 = value;
            break;
        case Index_Register5:
            Register5 = value;
            break;
        case Index_Register6:
            Register6 = value;
            break;
        case Index_Register7:
            Register7 = value;
            break;
        case Index_Register8:
            Register8 = value;
            break;
        case Index_Register9:
            Register9 = value;
            break;
        case Index_Register10:
            Register10 = value;
            break;
        case Index_Register11:
            Register11 = value;
            break;
        case Index_Register12:
            Register12 = value;
            break;
        case Index_Register13:
            Register13 = value;
            break;
        case Index_Register14:
            Register14 = value;
            break;
        case Index_Register15:
            Register15 = value;
            break;
		case Index_Register16:
            Register16 = value;
            break;
        case Index_Register17:
            Register17 = value;
            break;
        case Index_Register18:
            Register18 = value;
            break;
        case Index_Register19:
            Register19 = value;
            break;
        case Index_Register20:
            Register20 = value;
            break;
        case Index_Register21:
            Register21 = value;
            break;
        case Index_Register22:
            Register22 = value;
            break;
        case Index_Register23:
            Register23 = value;
            break;
        case Index_Register24:
            Register24 = value;
            break;
        case Index_Register25:
            Register25 = value;
            break;
        case Index_Register26:
            Register26 = value;
            break;
        case Index_Register27:
            Register27 = value;
            break;
        case Index_Register28:
            Register28 = value;
            break;
        case Index_Register29:
            Register29 = value;
            break;
        case Index_Register30:
            Register30 = value;
            break;
        case Index_Register31:
            Register31 = value;
            break;
		case Index_BP:
			BasePointer = value;
			break;
		case Index_SP:
			StackPointer = value;
			break;
		case Index_PC:
			ProgramCounter = value;
			break;
		case Index_AX:
			ax = value;
			break;
		case Index_BX:
			bx = value;
			break;
		case Index_CX:
			cx = value;
			break;

        default:
            break;

    }
}

/* Get the Binary Value of a given integer */
int binary_value(int x){
    int i;
    for( i=31; i>=0; i--) {
        if(i == 31) /* Check for Most Significant Bit. */
            return ((x >> i) & 1); /* Signed if 1 and Usigned if 0. */
    }
    return 0;
}

/* Check for carry flag for unsigned addition */
void carry_flag(int x, int y, unsigned int addition) {
    if (addition > 2147483647) { /* Since, Range of 32 int is from -2147483648 to 2147483647 */
        set_flag(CarryFlag);
    } else {
        clear_flag(CarryFlag); /* If well with in range, clear flag */
    }
}

/*
 Check Over bit for given inputs x, y and the result z.
 Set flag when the result by adding two positives is a negative number.
 Or, When the result by adding two negatives is a positive number. */
void overflow(int x, int y, int z) {
    if(x == 0 && y == 0) {
        if(z == 1) {
            set_flag(OverflowFlag);
        } else {
            clear_flag(OverflowFlag);
        }
    } else if(x == 1 && y == 1) {
        if(z == 0) {
            set_flag(OverflowFlag);
        } else {
            clear_flag(OverflowFlag);
        }
    } else {
        clear_flag(OverflowFlag);
    }
}

int lsb(int x) // To get LSB bit of an integer
{
     int i; //Assuming 32 bit integer
    for( i=31; i>=0; i--)
    {
        if(i == 0)
        return ((x >> i) & 1);
    }
}

/* Staging function to Initialize Variables and performing binary addition on input values.
 Return the sum after setting respective flags */
int initialize_add(int x, int y){
	unsigned int carry = 0;
	int a,b;
	a = binary_value(x);
	b = binary_value(y);

	while (b != 0)
	{
		int carry = (a & b) ;

		a = a ^ b;

		b = carry << 1;

			if (carry !=0 && carry != 2147483648) //set auxillary flag
	{
		set_flag(CarryFlag);

	}

	}



	if (a==0)
		set_flag(ZeroFlag);

	if(carry == 2147483648) // set Overflow
	{
			set_flag(OverflowFlag);
			printf("%s carry = %u\n", __FUNCTION__,carry);

	}

/*
	unsigned int addition = (unsigned) a;

	carry_flag(x, y, addition);

	int z = binary_value(a); // checking MSB bit of sum
	overflow(x, y, z);
	*/
	return a;
}

/* Compliment function to calculate the compliment of a given value */
int compliment_func(int value) {
    return initialize_add(~value, 1);
	//return (~value);
}

/*ALU store function defines the store instruction in which the data will be read from a register and written into the memory.*/
/*This takes Address and register index as an arguments.*/

void STORE_ALU(int sorce, int desti) {

    unsigned char *pointer = mem, *p;
    int v = 0; /*temporary variable for reading data from memory and writing data into memory */
    if (desti < 0) {
        printf("\n\tMemory is not data memory");
        return;
    }

    v = Read_Data_From_Register(sorce);
	p = (unsigned char *)&v;
	if (BIG__ENDIAN == endian_type) {
		pointer[Start_Data_Memory + desti + 0] = p[0];
		pointer[Start_Data_Memory + desti + 1] = p[1];
		pointer[Start_Data_Memory + desti + 2] = p[2];
		pointer[Start_Data_Memory + desti + 3] = p[3];
	} else if (LITTLE__ENDIAN == endian_type) {
		pointer[Start_Data_Memory + desti + 0] = p[3];
		pointer[Start_Data_Memory + desti + 1] = p[2];
		pointer[Start_Data_Memory + desti + 2] = p[1];
		pointer[Start_Data_Memory + desti + 3] = p[0];
	}
    //*(int*)(pointer + Start_Data_Memory + desti) = v;
}

/*ALU Load function defines the load instruction in which the data will be read from a memory location and written back to register.*/
/*This takes address and register index as an arguments.*/
void LOAD_ALU(int sorce, int desti) {
    int v = 0;
    unsigned char *pointer = mem, *p, *ptr2;
    if (sorce < 0) {
        printf("\n\tMemory is not data memory");
        return;
    }
    //v = *(int *)(pointer + Start_Data_Memory + sorce);
	p = (unsigned char *)&v;
	if (BIG__ENDIAN == endian_type) {
		p[0] = pointer[Start_Data_Memory + sorce + 0];
		p[1] = pointer[Start_Data_Memory + sorce + 1];
		p[2] = pointer[Start_Data_Memory + sorce + 2];
		p[3] = pointer[Start_Data_Memory + sorce + 3];
	} else if (LITTLE__ENDIAN == endian_type) {
		p[0] = pointer[Start_Data_Memory + sorce + 0];
		p[1] = pointer[Start_Data_Memory + sorce + 1];
		p[2] = pointer[Start_Data_Memory + sorce + 2];
		p[3] = pointer[Start_Data_Memory + sorce + 3];
	}
    Write_Data_into_Register(desti, v);
}

void alu_load_i(int sorce, int desti)
{
	Write_Data_into_Register(desti, sorce);
}

void alu_push(int sorce)
{
	int v = 0;
	unsigned char *pointer = mem, *p;

	if (StackPointer == 893) {
		printf("\n*******************************************************\n");
		printf ("\n\t STACK IS FULL, CANNOT PUSH ELEMENTS");
		printf("\n*******************************************************\n");
		return;
	}
	v = Read_Data_From_Register(sorce);
	StackPointer -= 4;
	p = (unsigned char *)&v;
	if (BIG__ENDIAN == endian_type) {
		pointer[StackPointer + 0] = p[0];
		pointer[StackPointer + 1] = p[1];
		pointer[StackPointer + 2] = p[2];
		pointer[StackPointer + 3] = p[3];
	} else if (LITTLE__ENDIAN == endian_type) {
		pointer[StackPointer + 0] = p[3];
		pointer[StackPointer + 1] = p[2];
		pointer[StackPointer + 2] = p[1];
		pointer[StackPointer + 3] = p[0];
	}
}

void alu_pop(int desti)
{
	int v = 0;
	unsigned char *pointer = mem, *p;
	if (StackPointer == 1021) {
		printf("\n*******************************************************\n");
		printf ("\n\t STACK IS EMPTY, CANNOT POP ELEMENTS");
		printf("\n*******************************************************\n");
		return;
	}
	p = (unsigned char *)&v;
	if (BIG__ENDIAN == endian_type) {
		p[0] = pointer[StackPointer + 0];
		p[1] = pointer[StackPointer + 1];
		p[2] = pointer[StackPointer + 2];
		p[3] = pointer[StackPointer+ 3];
	}
	else if (LITTLE__ENDIAN == endian_type) {
		p[0] = pointer[StackPointer + 3];
		p[1] = pointer[StackPointer + 2];
		p[2] = pointer[StackPointer + 1];
		p[3] = pointer[StackPointer + 0];
	}
	Write_Data_into_Register(desti, v);
	StackPointer += 4;
}

/**************************ADD*****************************/
/* Add Alu function for adding register values in which values will be read from source and destination register and the sum is written back to the destination register */
/* This function takes source and destination as arguments */
void ADD_ALU(int source, int desti){
    int sum, x, y;
    x = Read_Data_From_Register(source);
    y = Read_Data_From_Register(desti);

    sum = initialize_add(x, y);
    Write_Data_into_Register(desti, sum);
}


void ADDT_ALU(int source, int desti, int target)
{
	int sum;
	int a = Read_Data_From_Register(source); // MSB bit of a
	int b = Read_Data_From_Register(desti); // MSB bit of b
	sum = initialize_add(a, b);
	Write_Data_into_Register(target, sum);
}

/* Addi Alu function for adding a constant value to a destination register and the sum is written back to the destination register */
/* This function takes source and destination as arguments */
void ADDI_ALU(int source, int desti) {
    int sum, x, y;
    x = source;
    y = Read_Data_From_Register(desti);
    sum = initialize_add(x, y);
    Write_Data_into_Register(desti, sum);
}


/************************************SUB*************************/

/* Stagin function to Initialize function parameters and check if carry flag needs to be set.
 Return the difference */
int initialize_sub(int x, int y) {
    int temp, difference, a, b, z;
    temp = initialize_add(x, compliment_func(y));

    a = binary_value(x);
    b = binary_value(y);
    z = binary_value(temp);

    if( a == 0 && b ==0) { /* Check for carry flags */
        if(z == 1) { /* If carry, Set Carry Flag. */
            set_flag(CarryFlag);
        } else {
            clear_flag(CarryFlag); /* Else, Clear Carry Flag from previous instructinos. */
        }
    } else {
        clear_flag(CarryFlag); /* Clear Carry Flags */
    }

    difference = initialize_add(x, compliment_func(y)); /* Calculate Difference using compliment values*/
    return difference;
}

/* Sub Alu function for Subtracting register values in which values will be read from source and destination register and the difference is written back to the destination register */
/* This function takes source and destination as arguments */
void SUB_ALU(int source, int desti) {
    int diff, x, y;
    x = Read_Data_From_Register(source);
    y = Read_Data_From_Register(desti);

    diff = initialize_sub(x, y);
    Write_Data_into_Register(desti, diff);
}

/* Subi Alu function for subtracting a constant value to a destination register and the difference is written back to the destination register */
/* This function takes source and destination as arguments */
void SUBI_ALU(int source, int desti) {
    int diff, x, y;
    x = source;
    y = Read_Data_From_Register(desti);
    diff = y - x;
    //diff = initialize_sub(x, y);
    Write_Data_into_Register(desti, diff);
}


/**********************************MUL***************************/

/* Staging function to Initialize Variables and performing multiplication on input values.
 Return the product after setting respective flags */
int initialize_mul(int x, int y) {
    int a = 0, b = 0, c = 0, prod = 0;
    a=x;
    b=y;

    while(a >= 1) { /* Check if 'a' is even/odd */
        if (a & 0x1) {
            prod = initialize_add(prod,b);
        }
        a = a>>1;		/* Left shift Operation */
        b<<=1;			/* Right shift Operation */
    }

    x = binary_value(a);
    y = binary_value(b);

    carry_flag(a, b, prod); /* Check for Carry Flag */

    c = binary_value(prod);

    overflow(a, b, c); /* Check for Over flow bit and set respective flag*/
    if(prod == 0) {
        set_flag(ZeroFlag);
    }
    return prod;
}

/* Mul Alu function for multiplying register values in which values will be read from source and destination register and the product is written back to the destination register */
/* This function takes source and destination as arguments */
void MUL_ALU(int source, int desti) {
    int product, x, y;
    x = Read_Data_From_Register(source);
    y = Read_Data_From_Register(desti);

    product = initialize_mul(x, y);
    Write_Data_into_Register(desti, product);
}

/* Muli Alu function for adding a constant value to a destination register and the product is written back to the destination register */
/* This function takes source and destination as arguments */
void MULI_ALU(int source, int desti) {
    int product, x, y;
    x = source;
    y = Read_Data_From_Register(desti);

    product = initialize_mul(x, y);
    Write_Data_into_Register(desti, product);
}

/***********************************DIV************************/

/* Staging function to Initialize Variables and performing binary division on given input values.
 Return the quotient after setting respective flags */
int initialize_div(int x, int y) {
    int numerator, denominator, result;
    numerator = x;
    denominator = y;
    if (numerator == 0 || denominator == 0) { /* Set Zero flag if numerator or denominator are zero */
        set_flag(ZeroFlag);
    }
    if (denominator < 0 || numerator < 0) { /* Set Sign flag if numerator or denominator are less than zero */
        set_flag(SignFlag);
    }

    int quotient = 1, temp = 0;
    temp = denominator;
    if ((numerator != 0 && denominator != 0) && (denominator < numerator)) {
        while (((temp << 1) - numerator) < 0) {
            temp = temp << 1;
            quotient = quotient << 1;
        }
        while ((temp + denominator) <= numerator) {
            temp = temp + denominator;
            quotient = quotient + 1;
        }
    }
    if (denominator) {
        result = quotient;
    } else {
        printf("Exception Occured... Abort Abort!! \n"); /* Denominator is zero or negative value. Excetion!!*/
        result = 0;
    }
    return result;
}

/* Div Alu function for dividing register values in which values will be read from source and destination register and the quotient is written back to the destination register */
/* This function takes source and destination as arguments */
void DIV_ALU(int source, int desti) {
    int quotient, x, y;
    x = Read_Data_From_Register(source);
    y = Read_Data_From_Register(desti);

    quotient = initialize_div(x, y);
    Write_Data_into_Register(desti, quotient);
}

/* Divi Alu function for dividing a constant value to a destination register and the quotient is written back to the destination register */
/* This function takes source and destination as arguments */
void DIVI_ALU(int source, int desti) {
    int quotient, x, y;
    x = source;
    y = Read_Data_From_Register(desti);

    quotient = initialize_div(x, y);
    Write_Data_into_Register(desti, quotient);
}


/***************************MOD*******************************/

/* Staging function to Initialize Variables and performing binary modulo operation on input values.
 Return the remainder after setting respective flags */
int initialize_mod(int x, int y) {
    int numerator, denominator, remainder, n = 0;
    numerator = x;
    denominator = y;

    if (numerator == 0 || denominator == 0) {
        set_flag(ZeroFlag);
    } else {
        clear_flag(ZeroFlag);
    }
    if (denominator < 0 || numerator < 0) {
        set_flag(SignFlag);
    } else {
        clear_flag(SignFlag);
    }
    if (n > 0 && denominator == 2^n) {
        remainder = (numerator & (denominator - 1));
    }
    return remainder;
}

/* Mod Alu function for moduloing register values in which values will be read from source and destination register and the remainder is written back to the destination register */
/* This function takes source and destination as arguments */
void MOD_ALU(int source, int desti) {
    int remainder, x, y;
    x = Read_Data_From_Register(source);
    y = Read_Data_From_Register(desti);

    remainder = initialize_mod(x, y);
    Write_Data_into_Register(desti, remainder);
}

/* Modi Alu function for adding a constant value to a destination register and the remainder is written back to the destination register */
/* This function takes source and destination as arguments */
void MODI_ALU(int source,int desti){
    int remainder, x, y;
    x = source;
    y = Read_Data_From_Register(y);

    remainder = initialize_mod(x, y);
    Write_Data_into_Register(desti, remainder);
}

int MOV_ALU(unsigned int source, unsigned int desti) // logical right shift
{
    unsigned int a;
    a = Read_Data_From_Register(source);
    Write_Data_into_Register(desti, a);
}

/****************************JUMP*************************/

void JUMP_ALU(unsigned int desti)
{
	if(128<desti<384)
	{
		if (BIG__ENDIAN == endian_type)
			ProgramCounter = desti;
		else if (LITTLE__ENDIAN == endian_type)
			ProgramCounter = desti-1;
		flag_register = 0;

	}
	else
		printf("Error");

}

void JNE_ALU(unsigned int desti)
{
	//printf(" Jump %d\n", dsti);
	if(!check_flag(ZeroFlag))
	{
		//printf(" ZF is set\n");
		if(128< desti <384)
		{
			if (BIG__ENDIAN == endian_type)
				ProgramCounter = desti;
			else if (LITTLE__ENDIAN == endian_type)
				ProgramCounter = desti-1;

			flag_register = 0;
				//printf(" PC = %d\n",pc);
		}
		else
			printf("Error");
	}
}

void JNC_ALU(unsigned int desti)
{
	if(check_flag(CarryFlag))
	{
		if(128<desti<384)
		{
			if (BIG__ENDIAN == endian_type)
				ProgramCounter = desti;
			else if (LITTLE__ENDIAN == endian_type)
				ProgramCounter = desti-1;
			flag_register = 0;
		}
	}
}

void JC_ALU(unsigned int desti)
{
	if(check_flag(CarryFlag)==1)
	{
		if(128<desti<384)
		{
			if (BIG__ENDIAN == endian_type)
				ProgramCounter = desti;
			else if (LITTLE__ENDIAN == endian_type)
				ProgramCounter = desti-1;
			flag_register = 0;
		}
	}
}

void JNO_ALU(unsigned int desti)
{
	if(check_flag(OverflowFlag)==0)
	{
		if(128<desti<384)
		{
			if (BIG__ENDIAN == endian_type)
				ProgramCounter = desti;
			else if (LITTLE__ENDIAN == endian_type)
				ProgramCounter = desti-1;
			flag_register = 0;
		}
	}
}

void JO_ALU(unsigned int desti)
{
	if(check_flag(OverflowFlag)==1)
	{
		if(128<desti<384)
		{
			if (BIG__ENDIAN == endian_type)
				ProgramCounter = desti;
			else if (LITTLE__ENDIAN == endian_type)
				ProgramCounter = desti-1;
			flag_register = 0;
		}
	}
}

void JNZ_ALU(unsigned int desti)
{
	if(check_flag(ZeroFlag)==0)
	{
		if(128<desti<384)
		{
			if (BIG__ENDIAN == endian_type)
				ProgramCounter = desti;
			else if (LITTLE__ENDIAN == endian_type)
				ProgramCounter = desti-1;
			flag_register = 0;
		}
	}
}

void JZ_ALU(unsigned int desti)
{
	if(check_flag(ZeroFlag)==1)
	{
		if(128<desti<384)
		{
			if (BIG__ENDIAN == endian_type)
				ProgramCounter = desti;
			else if (LITTLE__ENDIAN == endian_type)
				ProgramCounter = desti-1;
			flag_register = 0;
		}
	}
}

void JAE_ALU(unsigned int desti)
{
	if(!(check_flag(SignFlag) ^ check_flag(OverflowFlag)) )
	{
		if(128<desti<384)
		{
			if (BIG__ENDIAN == endian_type)
				ProgramCounter = desti;
			else if (LITTLE__ENDIAN == endian_type)
				ProgramCounter = desti-1;
			flag_register = 0;
		}
	}
}

void JA_ALU(unsigned int desti)
{
	//printf("%s JUMP above: CF = %d, OF = %d\n", __FUNCTION__,check_flag(CF), check_flag(OF));

	if((!(check_flag(CarryFlag))) & (!(check_flag(OverflowFlag))) )
	{
		//printf("%s perform JUMP \n", __FUNCTION__ );
		if(128<desti<384)
		{
			if (BIG__ENDIAN == endian_type)
				ProgramCounter = desti;
			else if (LITTLE__ENDIAN == endian_type)
				ProgramCounter = desti-1;
		flag_register = 0;
		}
	}
}

/*******************************CMP**********************/

void CMPI_ALU(unsigned int sorce, unsigned int desti)
{
	if (sorce == Read_Data_From_Register(desti))
	{
        set_flag(ZeroFlag);
    }
    else
    {
        clear_flag(ZeroFlag);
	}
}

void CMP_ALU(unsigned int sorce, unsigned int desti)
{
	int a = Read_Data_From_Register(sorce);
	int b = Read_Data_From_Register(desti);
	//printf("%s a = %d , b = %d\n", __FUNCTION__ , a, b);
	initialize_sub(a,b);
	if (a == b)
	{
        set_flag(ZeroFlag);
    }
    else
    {
        clear_flag(ZeroFlag);
	}

	if (a < b)
	{
        set_flag(SignFlag);
	}
    else
    {
        clear_flag(SignFlag);
	}

}

void CMPV_ALU(unsigned int sorce, unsigned int desti)
{
	int a = mem[Read_Data_From_Register(sorce)];
	int b = Read_Data_From_Register(desti);
	initialize_sub(a,b);

	if (a == b)
	{
        set_flag(ZeroFlag);
    }
    else
    {
        clear_flag(ZeroFlag);
	}

	if (a < b)
	{
        set_flag(SignFlag);
	}
    else
    {
        clear_flag(SignFlag);
	}

}

/***********************Operations**********************/
int ALU_AND(int sorce, int desti)
{
    int a, b, c;
    a = Read_Data_From_Register(sorce);
    b = Read_Data_From_Register(desti);
    c = a & b;
    Write_Data_into_Register(desti, c);
}

int ALU_OR(int sorce, int desti)
{
    int a, b, c;
    a = Read_Data_From_Register(sorce);
    b = Read_Data_From_Register(desti);
    c = a | b;
    Write_Data_into_Register(desti, c);
}

int ALU_SHR(unsigned int sorce, unsigned int desti) // logical right shift
{
    int a, c;
    a = Read_Data_From_Register(desti);
    a = a >> sorce;
    if(a == 0)
    {
        set_flag(ZeroFlag);
    }
    else
    {
        clear_flag(ZeroFlag);
    }
    Write_Data_into_Register(desti, a);
}

int ALU_SHL(unsigned int desti, unsigned int sorce) // logical left shift
{
    int a, c;
    a = Read_Data_From_Register(desti);
    c = binary_value(a);
    if(c == 0)
    {
        clear_flag(CarryFlag);
    }
    else
    {
        set_flag(CarryFlag);
    }
    a = a << sorce;
    Write_Data_into_Register(desti, a);
}

int ALU_XOR(int sorce, int desti)
{
    int a, b, c;
    a = Read_Data_From_Register(sorce);
    b = Read_Data_From_Register(desti);
    c = a ^ b;
    Write_Data_into_Register(desti, c);
}


void ALU_LEA(int sorce, int desti)
{
	int result;
	unsigned char *pointer = mem;
	result = sorce + BasePointer;
	if (desti > 37)
		*(int*)(pointer + Start_Data_Memory + desti) = result;
	else
		Write_Data_into_Register(desti, result);
	//printf("\n\n\n\n--- %u  %u %u\n\n\n\n",result,memory[dst], dst);
}



/* Check Flag Function to check a flag status */
/*
int check_flag(unsigned int index) {
    return (flag_register & (1 << index));
}
*/


/*This ALU function chooses a case, which instruction to execute.*/
void ALU(char opcode, int sorce, int desti, int target) {
    switch(opcode) {
        case STORE_TO_MEMORY:
            STORE_ALU(sorce, desti);
            break;
        case LOAD_FROM_MEMORY:
            LOAD_ALU(sorce, desti);
            break;
		case LOAD_MEMORY_I:
			alu_load_i(sorce,desti);
			break;
		case PUSH:
			alu_push(sorce);
			break;
		case POP:
			alu_pop(desti);
			break;
		case MUL:
            MUL_ALU(sorce, desti);
            break;
        case ADD:
            ADD_ALU(sorce, desti);
            break;
		case DIV:
            DIV_ALU(sorce, desti);
            break;
		case MOD:
            MOD_ALU(sorce, desti);
            break;
		case MOV:
			MOV_ALU(sorce,desti);
			break;
		case SUB:
            SUB_ALU(sorce, desti);
            break;
        case ADDI:
            ADDI_ALU(sorce, desti);
            break;
		case ADDT:
			ADDT_ALU(sorce, desti, target);
			break;
        case SUBI:
            SUBI_ALU(sorce, desti);
            break;
        case MULI:
            MULI_ALU(sorce, desti);
            break;
        case DIVI:
            DIVI_ALU(sorce, desti);
            break;
        case MODI:
            MODI_ALU(sorce, desti);
            break;
		case JMP:
			JUMP_ALU(desti);
			break;
		case JNE:
			JNE_ALU(desti);
			break;
		case JNC:
			JNC_ALU(desti);
			break;
		case JC:
			JC_ALU(desti);
			break;
		case JNZ:
			JNZ_ALU(desti);
			break;
		case JZ:
			JZ_ALU(desti);
			break;
		case JNO:
			JNO_ALU(desti);
			break;
		case JO:
			JO_ALU(desti);
			break;
		case JA:
			JA_ALU(desti);
			break;
		case JAE:
			JAE_ALU(desti);
			break;
		case AND:
			ALU_AND(sorce, desti);
			break;
		case OR:
			ALU_OR(sorce, desti);
			break;
		case XOR:
			ALU_XOR(sorce, desti);
			break;
		case SHR:
			ALU_SHR(sorce, desti);
			break;
		case SHL:
			ALU_SHL(sorce, desti);
			break;
		case CMPI:
			CMPI_ALU(sorce, desti);
			break;
		case CMP:
			CMP_ALU(sorce, desti);
			break;
		case CMPV:
			CMPV_ALU(sorce, desti);
			break;
		case LEA:
			ALU_LEA(sorce, desti);
			break;
        default:
            break;
    }
}



void error_instruction()
{
    printf("\n_______________________________________________________\n");
    printf("\n\tAssembly instruction should be in following format");
    printf("\n\t Bootstrap starts from 0 to 255");
    printf("\n\t ld 564, Register1  ");
    printf("\n\tAbove Instruction fetches data from 564 memory"
           "location to Register 1");
    printf("\n\t  st Register2, 600  ");
    printf("\n\tAbove Instruction fetches data from Register2"
           "Register to 200 Memory Location");

    printf("\n_______________________________________________________\n");
    return ;
}

/*Returns Index of register*/
int RTN_INDEX(char *pointer) {
    char i = 0;
    i = atoi(++pointer);

    return i;
}

int RTN_OFFSET(char *pointer)
{
	uint16_t i = 0;
	i = atoi(&pointer[7]);
	return i;
}

int execute_one_instr()
{
	unsigned char opcode = 0, desti = 0, sorce = 0, target = 0;
	if (BIG__ENDIAN == endian_type) {
		opcode = mem[ProgramCounter];
		sorce= mem[ProgramCounter + 1];
		desti = mem[ProgramCounter+ 2];
		target = mem[ProgramCounter + 3];
	} else if (LITTLE__ENDIAN == endian_type) {
		opcode = mem[ProgramCounter];
		sorce= mem[ProgramCounter - 1];
		desti= mem[ProgramCounter - 2];
		target = mem[ProgramCounter -3];
	}

	if(opcode == 0)
	{
		//printf("\n Done execution pc = %d\n", pc);
		return 0;
	}
	printf("\nExecuting Instruction number %d : %u %u %u", (ProgramCounter - 127)/4,  opcode, sorce, desti);
	ALU(opcode, sorce, desti, target);
	ProgramCounter = ProgramCounter + 4;
	return 1;
}

void initALU()
{
	if (BIG__ENDIAN == endian_type) {
		ProgramCounter = Start_Instruction_Memory;
	}
	else if (LITTLE__ENDIAN == endian_type) {
		ProgramCounter = Start_Instruction_Memory + 3;
	}
	StackPointer = STACK_TOP;
	flag_register= 0;
	//Write_Data_into_Register(Index_Register5, 10);

	memset(all_label_table, 0, sizeof(all_label_table));
	memset(pending_label_table, 0, sizeof(pending_label_table));

}

int loader()
{
    unsigned int i = 0, count = 0, ret = -1;
	size_t len = 0;
    char *instruction;
    FILE *file_descriptor = 0;
    unsigned int opcode = 0, desti = 0, sorce = 0, target = 0;
    char *pointer1 = NULL, *pointer2 = NULL, *pointer3 = NULL;
	unsigned int address, jmp_address, label_address;
    //ProgramCounter = Start_Instruction_Memory;
	char *label;
    /*
    Write_Data_into_Register(Index_Register2, 10);
    Write_Data_into_Register(Index_Register3, 11);
    Write_Data_into_Register(Index_Register4, 12);
    Write_Data_into_Register(Index_Register8, 13);
    Write_Data_into_Register(Index_Register1, 5);
    */
	instruction = (char *)malloc(200);
    file_descriptor = fopen("./instructions.txt","r");

    while( (ret = getline(&instruction, &len, file_descriptor)) != -1)
	{
			printf("\n\tInstruction = \t%s",instruction);

        /*Instruction's register and memory will be parsed here*/
        pointer1 = strtok(instruction, " ");
        if (!strcasecmp(pointer1, "ld")) {
            pointer2 = strtok(NULL, ",");	    /*data memory location*/
            sorce = atoi(pointer2) - Start_Data_Memory;
            pointer3 = strtok(NULL, " ");
            desti = RTN_INDEX(pointer3);
            opcode = LOAD_FROM_MEMORY;
            printf("\n\tDecoded inst : %x %x %x\n", opcode, sorce, desti);
        }
		else if (!strcasecmp(pointer1, "ldi")) {
            pointer2 = strtok(NULL, ",");	    /*data memory location*/
            sorce = atoi(pointer2);
            pointer3 = strtok(NULL, " ");
            desti = RTN_INDEX(pointer3);
            opcode = LOAD_MEMORY_I;
            printf("\n\tDecoded inst : %x %x %x\n", opcode, sorce, desti);
		}
		else if (!strcasecmp(pointer1, "st")) {
            pointer2 = strtok(NULL, ",");
            sorce = RTN_INDEX(pointer2);
            pointer3 = strtok(NULL, " ");		/*Register to location*/
            desti = atoi(pointer3) - Start_Data_Memory;
            opcode =  STORE_TO_MEMORY;
            printf("\n\tDecoded inst : %x %x %x\n", opcode, sorce, desti);
        }
		else if (!strcasecmp(pointer1, "mov")) {
            pointer2 = strtok(NULL, ",");	    /*data memory location*/
            sorce = RTN_INDEX(pointer2);
            pointer3 = strtok(NULL, " ");
            desti = RTN_INDEX(pointer3);
            opcode = MOV;
            printf("\n\tDecoded inst : %x %x %x\n", opcode, sorce, desti);
		}
		else if (!strcasecmp(pointer1, "mul")) {
            pointer2 = strtok(NULL, ",");
            sorce = RTN_INDEX(pointer2);
            pointer3 = strtok(NULL, ",");
            desti = RTN_INDEX(pointer3);
            opcode = MUL;
            printf("\n\tDecoded inst : %x %x %x\n", opcode, sorce, desti);
        }
		else if (!strcasecmp(pointer1, "sub")) {
            pointer2 = strtok(NULL, ",");
            sorce = RTN_INDEX(pointer2);
            pointer3 = strtok(NULL, " ");
            desti = RTN_INDEX(pointer3);
            opcode = SUB;
            printf("\n\tDecoded inst : %x %x %x\n", opcode, sorce, desti);
        }
		else if (!strcasecmp(pointer1, "add")) {
            pointer2 = strtok(NULL, ",");
            sorce = RTN_INDEX(pointer2);
            pointer3 = strtok(NULL, " ");
            desti = RTN_INDEX(pointer3);
            opcode = ADD;
            printf("\n\tDecoded inst : %x %x %x\n", opcode, sorce, desti);
        }
		else if (!strcasecmp(pointer1, "addt")) {
            pointer2 = strtok(NULL, ",");
			target = RTN_INDEX(pointer2);
            pointer3 = strtok(NULL, " ");
			sorce = RTN_INDEX(pointer3);
			pointer3 = strtok(NULL, " ");
            desti = RTN_INDEX(pointer3);
            opcode = ADDT;
            printf("\n\tDecoded inst : %x %x %x\n", opcode, sorce, desti);
        }
		else if (!strcasecmp(pointer1, "lea")) {
			pointer2 = strtok(NULL, ",");	     //Load Effective Address(LEA) instruction

			if (!strcasecmp(pointer2,"ax"))
				desti = Index_AX;
			else if (!strcasecmp(pointer2,"bx"))
				desti = Index_BX;
			else if (!strcasecmp(pointer2,"cx"))
				desti = Index_CX;
			else
				desti= atoi(pointer2) - Start_Data_Memory;
			pointer3 = strtok(NULL, "]");
			sorce = RTN_OFFSET(pointer3);
			opcode = LEA;
			printf("\n\tDecoded inst : %x %x %x\n", opcode, sorce, desti);
		}
		 else if (!strcasecmp(pointer1, "mod")) {
            pointer2 = strtok(NULL, ",");
            sorce = RTN_INDEX(pointer2);
            pointer3 = strtok(NULL, " ");
            desti = RTN_INDEX(pointer3);
            opcode = MOD;
            printf("\n\tDecoded inst : %x %x %x\n", opcode, sorce, desti);
        }
		else if (!strcasecmp(pointer1, "div")) {
            pointer2 = strtok(NULL, ",");
            sorce = RTN_INDEX(pointer2);
            pointer3 = strtok(NULL, " ");
            desti = RTN_INDEX(pointer3);
            opcode = DIV;
            printf("\n\tDecoded inst : %x %x %x\n", opcode, sorce, desti);
        }
		else if (!strcasecmp(pointer1, "addi")) {
            pointer2 = strtok(NULL, ",");
            sorce = atoi(pointer2);
            pointer3 = strtok(NULL, " ");
            desti = RTN_INDEX(pointer3);
            opcode = ADDI;
            printf("\n\tDecoded inst : %x %x %x\n", opcode, sorce, desti);
        }
		else if (!strcasecmp(pointer1, "subi")) {
            pointer2 = strtok(NULL, ",");
            sorce = atoi(pointer2);
            pointer3 = strtok(NULL, " ");
            desti = RTN_INDEX(pointer3);
            opcode = SUBI;
            printf("\n\tDecoded inst : %x %x %x\n", opcode, sorce, desti);
        }
		else if (!strcasecmp(pointer1, "muli")) {
            pointer2 = strtok(NULL, ",");
            sorce = atoi(pointer2);
            pointer3 = strtok(NULL, " ");
            desti = RTN_INDEX(pointer3);
            opcode = MULI;
            printf("\n\tDecoded inst : %x %x %x\n", opcode, sorce, desti);
        }
		else if (!strcasecmp(pointer1, "divi")) {
            pointer2 = strtok(NULL, ",");
            sorce = atoi(pointer2);
            pointer3 = strtok(NULL, " ");
            desti = RTN_INDEX(pointer3);
            opcode = DIVI;
            printf("\n\tDecoded inst : %x %x %x\n", opcode, sorce, desti);
        }
		else if (!strcasecmp(pointer1, "modi")) {
            pointer2 = strtok(NULL, ",");
            sorce = atoi(pointer2);
            pointer3 = strtok(NULL, " ");
            desti = RTN_INDEX(pointer3);
            opcode = MODI;
            printf("\n\tDecoded inst : %x %x %x\n", opcode, sorce, desti);
        }
		else if (!strcasecmp(pointer1, "push")) {
			pointer2 = strtok(NULL, " ");
			if(*pointer2 == 'R')
				sorce = RTN_INDEX(pointer2);
			else if (strstr(pointer2,"bp"))
				sorce = Index_BP;
			else if (strstr(pointer2,"sp"))
				sorce = Index_SP;
			else if (strstr(pointer2,"pc"))
				sorce = Index_PC;
			opcode =  PUSH;
			desti = Index_INVALID;
		}
		else if (!strcasecmp(pointer1, "pop")) {
			pointer2 = strtok(NULL, " ");
			if(*pointer2 == 'R')
				desti = RTN_INDEX(pointer2);
			else if (strstr(pointer2,"bp"))
				desti = Index_BP;
			else if (strstr(pointer2,"sp"))
				desti = Index_SP;
			else if (strstr(pointer2,"pc"))
				desti = Index_PC;
			opcode =  POP;
			sorce = Index_INVALID;
		}
		else if (!strcasecmp(pointer1, "jmp")) {
			pointer2 = strtok(NULL, " ");

			desti = get_address_from_label(pointer2);

			if(desti == 0)
			{
				add_pending_label(pointer2, Start_Instruction_Memory + i);
				//printf("\n\tadd to pending: label = %s, address = %u\n", p2,  INSTR_MEMORY_BASE + i);
			}
			opcode =  JMP;
			sorce = Index_INVALID;
		}
		else if (!strcasecmp(pointer1, "jne")) {
			pointer2 = strtok(NULL, " ");

			desti = get_address_from_label(pointer2);

			if(desti == 0)
			{
				add_pending_label(pointer2, Start_Instruction_Memory + i);
				//printf("\n\tadd to pending: label = %s, address = %u\n", p2,  INSTR_MEMORY_BASE + i);
			}
			opcode =  JNE;
			sorce = Index_INVALID;
		}
		else if (!strcasecmp(pointer1, "jnc")) {
			pointer2 = strtok(NULL, " ");

			desti = get_address_from_label(pointer2);

			if(desti == 0)
			{
				add_pending_label(pointer2, Start_Instruction_Memory + i);
				//printf("\n\tadd to pending: label = %s, address = %u\n", p2,  INSTR_MEMORY_BASE + i);
			}
			opcode =  JNC;
			sorce = Index_INVALID;

		}
		else if (!strcasecmp(pointer1, "jc")) {
			pointer2 = strtok(NULL, " ");

			desti = get_address_from_label(pointer2);

			if(desti == 0)
			{
				add_pending_label(pointer2, Start_Instruction_Memory + i);
				//printf("\n\tadd to pending: label = %s, address = %u\n", p2,  INSTR_MEMORY_BASE + i);
			}
			opcode =  JC;
			sorce = Index_INVALID;
		}
		else if (!strcasecmp(pointer1, "jnz")) {
			pointer2 = strtok(NULL, " ");

			desti = get_address_from_label(pointer2);

			if(desti == 0)
			{
				add_pending_label(pointer2, Start_Instruction_Memory + i);
				//printf("\n\tadd to pending: label = %s, address = %u\n", p2,  INSTR_MEMORY_BASE + i);
			}
			opcode =  JNZ;
			sorce = Index_INVALID;
		}
		else if (!strcasecmp(pointer1, "jz")) {
			pointer2 = strtok(NULL, " ");

			desti = get_address_from_label(pointer2);

			if(desti == 0)
			{
				add_pending_label(pointer2, Start_Instruction_Memory + i);
				//printf("\n\tadd to pending: label = %s, address = %u\n", p2,  INSTR_MEMORY_BASE + i);
			}
			opcode =  JZ;
			sorce = Index_INVALID;
		}
		else if (!strcasecmp(pointer1, "jno")) {
			pointer2 = strtok(NULL, " ");

			desti = get_address_from_label(pointer2);

			if(desti == 0)
			{
				add_pending_label(pointer2, Start_Instruction_Memory + i);
				//printf("\n\tadd to pending: label = %s, address = %u\n", p2,  INSTR_MEMORY_BASE + i);
			}
			opcode =  JNO;
			sorce = Index_INVALID;
		}
        else if (!strcasecmp(pointer1, "jo")) {
			pointer2 = strtok(NULL, " ");

			desti = get_address_from_label(pointer2);

			if(desti == 0)
			{
				add_pending_label(pointer2, Start_Instruction_Memory + i);
				//printf("\n\tadd to pending: label = %s, address = %u\n", p2,  INSTR_MEMORY_BASE + i);
			}
			opcode =  JO;
			sorce = Index_INVALID;
		}
		else if (!strcasecmp(pointer1, "ja")) {
			pointer2 = strtok(NULL, " ");

			desti = get_address_from_label(pointer2);

			if(desti == 0)
			{
				add_pending_label(pointer2, Start_Instruction_Memory + i);
				//printf("\n\tadd to pending: label = %s, address = %u\n", p2,  INSTR_MEMORY_BASE + i);
			}
			opcode =  JA;
			sorce = Index_INVALID;
		}
		else if (!strcasecmp(pointer1, "jae")) {
			pointer2 = strtok(NULL, " ");

			desti = get_address_from_label(pointer2);

			if(desti == 0)
			{
				add_pending_label(pointer2, Start_Instruction_Memory + i);
				//printf("\n\tadd to pending: label = %s, address = %u\n", p2,  INSTR_MEMORY_BASE + i);
			}
			opcode =  JAE;
			sorce = Index_INVALID;
		}
		else if (!strcasecmp(pointer1, "and")) {
            pointer2 = strtok(NULL, ",");
            sorce = RTN_INDEX(pointer2);
            pointer3 = strtok(NULL, " ");
            desti = RTN_INDEX(pointer3);
            opcode = AND;
            printf("\n\tDecoded inst : %x %x %x\n", opcode, sorce, desti);
        }
		else if (!strcasecmp(pointer1, "or")) {
            pointer2 = strtok(NULL, ",");
            sorce = RTN_INDEX(pointer2);
            pointer3 = strtok(NULL, " ");
            desti = RTN_INDEX(pointer3);
            opcode = OR;
            printf("\n\tDecoded inst : %x %x %x\n", opcode, sorce, desti);
        }
		else if (!strcasecmp(pointer1, "xor")) {
            pointer2 = strtok(NULL, ",");
            sorce = RTN_INDEX(pointer2);
            pointer3 = strtok(NULL, " ");
            desti = RTN_INDEX(pointer3);
            opcode = XOR;
            printf("\n\tDecoded inst : %x %x %x\n", opcode, sorce, desti);
        }
		else if (!strcasecmp(pointer1, "shr")) {
            pointer2 = strtok(NULL, ",");
            sorce = atoi(pointer2);
            pointer3 = strtok(NULL, " ");
            desti = RTN_INDEX(pointer3);
            opcode = SHR;
            printf("\n\tDecoded inst : %x %x %x\n", opcode, sorce, desti);
        }
		else if (!strcasecmp(pointer1, "shl")) {
            //pointer2 = strtok(NULL, ",");
            sorce = atoi(pointer2);
            pointer3 = strtok(NULL, " ");
            desti = RTN_INDEX(pointer3);
            opcode = SHL;
            printf("\n\tDecoded inst : %x %x %x\n", opcode, sorce, desti);
        }
		else if (!strcasecmp(pointer1, "cmpi")) {
            pointer2 = strtok(NULL, ",");
            sorce = atoi(pointer2);
            pointer3 = strtok(NULL, " ");
            desti = RTN_INDEX(pointer3);
            opcode = CMPI;
            printf("\n\tDecoded inst : %x %x %x\n", opcode, sorce, desti);
        }
		else if (!strcasecmp(pointer1, "cmp")) {
            pointer2 = strtok(NULL, ",");
            sorce = RTN_INDEX(pointer2);
            pointer3 = strtok(NULL, " ");
            desti = RTN_INDEX(pointer3);
            opcode = CMP;
            printf("\n\tDecoded inst : %x %x %x\n", opcode, sorce, desti);
        }
		else if (!strcasecmp(pointer1, "cmpv")) {
            pointer2 = strtok(NULL, "]");
            //sorce = RTN_INDEX(pointer2);
			sorce = RTN_INDEX(pointer2 + 1);
			pointer2 = strtok(NULL, " ");
			pointer3 = strtok(NULL, " ");
            desti = RTN_INDEX(pointer3);
            opcode = CMPV;
            printf("\n\tDecoded inst : %x %x %x\n", opcode, sorce, desti);
        }
		else if (!strcasecmp(pointer1, "dec")) {
			pointer2 = strtok(NULL, " ");
			sorce= RTN_INDEX(pointer2);
			opcode = DEC;
		}
		else if (*pointer1, "L") {  /* This is a label*/
			pointer2 = strtok(pointer1, ":");
			add_label(pointer2, Start_Instruction_Memory + i);
			pointer1 = strtok(NULL, " ");
			continue;
		}
		else {
            error_instruction();
        }

		printf("\tDecoded inst : \t %u %u %u\n", opcode, sorce, desti);

		/*Store Instruction in BIG or Little Endian Formats*/
		if (endian_type == BIG__ENDIAN)
		{
		mem[Start_Instruction_Memory + i] = opcode;
        i++;
        mem[Start_Instruction_Memory + i] = sorce;
        i++;
        mem[Start_Instruction_Memory + i] = desti;
        i++;
		mem[Start_Instruction_Memory + i] = target;
        i++;
        }
		else if (endian_type == LITTLE__ENDIAN)
		{
		mem[Start_Instruction_Memory + i] = target;
        i++;
        mem[Start_Instruction_Memory + i] = desti;
        i++;
        mem[Start_Instruction_Memory + i] = sorce;
        i++;
		mem[Start_Instruction_Memory + i] = opcode;
        i++;
        }
        count++;
        pointer1 = strtok(NULL, " ");
    }

	label = (char *)malloc(10);
	while(get_from_pending_table(label, &jmp_address))
	{
		//printf("label = %s address = %d\n", label, jmp_address);
		label_address = get_address_from_label(label);
		//printf("label_address = %d\n", label_address);
		if (endian_type == BIG__ENDIAN)
		{
			// big endian means dst is at 3rd byte
			mem[jmp_address + 2] = label_address;
		}
		else if (endian_type == LITTLE__ENDIAN)
		{
			// little endian means dst is at 2nd byte
			mem[jmp_address + 1] = label_address;
		}
	}

	return count;
}

int main(int argc, char **argv)
{
	int count = 0, i;
	char ch, value;
	unsigned int max = -1;
	char *pointer = NULL;
	int size = 0;
    FILE *myFile;

    if(argc > 1)
    {
    	if(!strcasecmp(argv[1],"-NoMemDisplay"))
    	{
    		memory_print = 0;
    	}
    }
#if 0
	printf("\n*******************************************************\n");
	printf("\tPlease choose MEMORY FORMAT type for BEST CPU\n");
	printf("\t 0 -- BIG ENDIAN\n\t 1 -- LITTLE ENDIAN\n");
	printf("\tEnter Your Choice\n");
	printf("\n*******************************************************\n");
	scanf("%d", &endian_type);


	if (endian_type != BIG__ENDIAN && endian_type != LITTLE__ENDIAN) {
		printf("\n\tEntered wrong option, default Little Endian selected\n");
		endian_type = 1;
	}
#endif
	endian_type = 1;
	max = max / 2;
	/*copy max value of signed int, for overflow case*/
	if (BIG__ENDIAN == endian_type) {
		pointer = (char *) &max;
		mem[384 + 0] = pointer[0];
		mem[384 + 1] = pointer[1];
		mem[384 + 2] = pointer[2];
		mem[384 + 3] = pointer[3];
	}
	else if (LITTLE__ENDIAN == endian_type) {
		pointer = (char *) &max;
		mem[384 + 0] = pointer[3];
		mem[384 + 1] = pointer[2];
		mem[384 + 2] = pointer[1];
		mem[384 + 3] = pointer[0];
	}

	initALU();
	count = loader();
	printf("\t Press any key to start execution :");
	scanf("%d",&Register4);

    myFile = fopen("numbers.txt", "r");
    
    //read file into array
	while(fscanf(myFile, "%d", &mem[i]) > 0) {
        i++;
    }
    fclose(myFile);

    Register3 = i;
    Register2 = 0;

    printf("\nKey: %d", Register4);
    printf("\nTotal elements: %d", Register3);
    printf("\nNumbers are: ");
    for (i = 0; i < Register3; i++)
    {
        printf("%d ", mem[i]);
    }

	printf("\n*******************************************************\n");
	printf("\tCPU State before execution\n");
	printf("*******************************************************\n");
	print_all();

	while (execute_one_instr()) {
		printf("\n*******************************************************\n");
		printf("\tCPU State After execution of instruction\n");
		printf("*******************************************************\n");
		print_all();
	}

}
