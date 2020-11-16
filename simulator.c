/*Simulator*/

/* instruction-level simulator */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <math.h>

#define NUMMEMORY 65536 /* maximum number of words in memory */
#define NUMREGS 8 /* number of machine registers */
#define MAXLINELENGTH 1000
#define INFINITE 1000000

char memory[MAXLINELENGTH][MAXLINELENGTH];

char *decToBi(int);
char *getOpcode(int);
void simulation();
void printS();
int is16Bits(int);

char line[MAXLINELENGTH];
int regs[NUMREGS];
int row = 0;
char mems[MAXLINELENGTH][MAXLINELENGTH];
int i = 0;

typedef struct stateStruct {
    int pc;
    int mem[NUMMEMORY];
    int reg[NUMREGS];
    int numMemory;
} stateType;

void printState(stateType *);

int main(int argc, char *argv[]){
    stateType state;
    FILE *filePtr;
    char *filename = argv[1];

    if (argc != 2){
	printf("error: usage: %s <machine-code file>\n", argv[0]);
	exit(1);
    }

    filePtr = fopen(argv[1], "r");
    if (filePtr == NULL){
	printf("error: can't open file %s", argv[1]);
	perror("fopen");
	exit(1);
    }

    /* read in the entire machine-code file into memory */
    while (fgets(line, MAXLINELENGTH, filePtr) != NULL){       
        strcpy(mems[i], line);
        if(is16Bits(atoi(line))) strcpy(memory[i], decToBi(atoi(line)));
        else strcpy(memory[i], line);
        printf("memory[%d]=%s",i ,mems[i]);
        i++;
    }
    fclose(filePtr); 
    printf("\n\n");

    simulation();

    return(0);
}

void printState(stateType *statePtr){
    int i;
    printf("\n@@@\nstate:\n");
    printf("\tpc %d\n", statePtr->pc);
    printf("\tmemory:\n");
	for (i=0; i<statePtr->numMemory; i++) {
	    printf("\t\tmem[ %d ] %d\n", i, statePtr->mem[i]);
	}
    printf("\tregisters:\n");
	for (i=0; i<NUMREGS; i++) {
	    printf("\t\treg[ %d ] %d\n", i, statePtr->reg[i]);
	}
    printf("end state\n");
}

void printS(){
    int j = 0;
     printf("\n@@@\nstate:\n\t\tpc %d \n\t\tmemory:\n", row);
    while (j < i)
    {
        printf("\t\t\tmem[%d] %s", j, mems[j]);
        j++;
    }
    j = 0;
    printf("\n\t\tregister:");
    while (j < 8)
    {
        printf("\n\t\t\treg[%d] %d", j, regs[j]);
        j++;
    }
    printf("\nend state\n");
}

char *decToBi(int n){
    int c, k, p = 0;
    static char bi[32];
    for (c = 31; c >= 0; c--)
    {
        k = n >> c;
        if (k & 1)
            bi[p] = '1';
        else
            bi[p] = '0';
        p++;
    }
    return bi;
}

void simulation(){
    int totalInstructions = 0;
    char opcode[3];

    while(1){
        printS();
        strcpy(opcode, getOpcode(row));

        if(strcmp(opcode, "000") == 0){ //ADD
            int regA = 0,regB = 0,destReg = 0;
            for (int i = 0; i < 3; i++) {
                regA +=  (memory[row][10 + i]-48)*(int)pow(2,2-i);
                regB +=  (memory[row][13 + i]-48)*(int)pow(2,2-i);
                destReg +=  (memory[row][29 + i]-48)*(int)pow(2,2-i);
            }
            regs[destReg] = regs[regA] + regs[regB];    
            (row)++;
        }else if(strcmp(opcode, "001") == 0){ //NAND   
            int regA = 0,regB = 0,destReg = 0;
            for (int i = 0; i < 3; i++) {
                regA +=  (memory[row][10 + i]-48)*(int)pow(2,2-i);;
                regB +=  (memory[row][13 + i]-48)*(int)pow(2,2-i);;
                destReg +=  (memory[row][29 + i]-48)*(int)pow(2,2-i);
            }
            regs[destReg] = ~(regs[regA] & regs[regB]);
            (row)++;
        }else if(strcmp(opcode, "010") == 0){ //LW
            int regA = 0,regB = 0,offsetField = 0;
            for (int i = 0; i < 3; i++) {
                regA +=  (memory[row][10 + i]-48)*(int)pow(2,2-i);
                regB +=  (memory[row][13 + i]-48)*(int)pow(2,2-i);
            }
            for (int i = 0; i < 15; i++) offsetField +=  (memory[row][17 + i]-48)*(int)pow(2,14-i);

            if((memory[row][16]-48) == 1 && offsetField == 0) exit(1);
            if((memory[row][16]-48) == 1) offsetField -= 32768;

            regs[regB] = atoi(memory[(regs[regA] + offsetField)]);
            
            (row)++;
        }else if(strcmp(opcode, "011") == 0){ //SW
            int regA = 0,regB = 0,offsetField = 0;
            for (int i = 0; i < 3; i++) {
                regA +=  (memory[row][10 + i]-48)*(int)pow(2,2-i);
                regB +=  (memory[row][13 + i]-48)*(int)pow(2,2-i);
            }
            for (int i = 0; i < 15; i++) offsetField +=  (memory[row][17 + i]-48)*(int)pow(2,14-i);

            if((memory[row][16]-48) == 1 && offsetField == 0) exit(1);
            if((memory[row][16]-48) == 1) offsetField -= 32768;

            itoa(regs[regB],memory[(regs[regA] + offsetField)] ,10); //FIX IT
            (row)++;
        }else if(strcmp(opcode, "100") == 0){ //BEQ
            int regA = 0,regB = 0,offsetField = 0;
            for (int i = 0; i < 3; i++) {
                regA +=  (memory[row][10 + i]-48)*(int)pow(2,2-i);
                regB +=  (memory[row][13 + i]-48)*(int)pow(2,2-i);
            }
            for (int i = 0; i < 15; i++) offsetField +=  (memory[row][17 + i]-48)*(int)pow(2,14-i);
            
            if((memory[row][16]-48) == 1 && offsetField == 0) exit(1);
            if((memory[row][16]-48) == 1) offsetField -= 32768;

            if(regs[regA] == regs[regB]) row = row + 1 + offsetField;
            else (row)++;
        }else if(strcmp(opcode, "101") == 0){ //JALR
            int regA = 0,regB = 0;
            for (int i = 0; i < 3; i++) {
                regA +=  (memory[row][10 + i]-48)*(int)pow(2,2-i);
                regB +=  (memory[row][13 + i]-48)*(int)pow(2,2-i);
            }
            regs[regB] = row + 1;
            row = regs[regA];
            // (row)++;
        }
        else if(strcmp(opcode, "110") == 0){ //HALT
            (row)++;
            (totalInstructions)++;
            printf("machine halted\n");
            printf("total of %d instructions executed\n",totalInstructions);
            printf("final state of machine:\n");
            printS(); //FINAL PRINT STATE
            break;
        }
        else if(strcmp(opcode, "111") == 0){ //NOOP

        }else exit(1);
        (totalInstructions)++;
    }
}

char *getOpcode(int row){
    static char opcode[3];
    for (int i = 0; i < 3; i++)  opcode[i] = memory[row][7 + i];
    return opcode;
}

int is16Bits(int x){
    if (x >= -32768 && x <= 32767) return 0;
    return 1;
    printf("Error: Bit out of lenght.");
    exit(1);
}