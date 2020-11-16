#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

#define MAXCHAR 1000
#define INFINITE 1000000

char labelList[50][MAXCHAR];
char commandList[9][MAXCHAR] = {"add", "nand", "lw", "sw", "beq", "jalr", "halt", "noop", ".fill"};
char textFile[MAXCHAR];

void findLabel(char *);
void compile(char *);
void write();
void iType(char *, int);
void oType(char *, int);
void rType(char *, int);
void jType(char *, int);
void fill(char *, int);
void toText(int);


const char *decimalToBinary(char *, int);
int binaryToDecimal(char *);
int is16Bits(int);
int isNumber(char *);

int main()
{
    
    char *filename = "Pow2Ass.txt"; //เลือกชื่อไฟล์ของ Assembly code ในการทำงาน
    findLabel(filename); // หา Label ที่ถูกกำหนดไว้แล้ว
    compile(filename); // นำไปเข้าการทำงานโดยแปลง Assembly code เป็น machine language
    write(); //บันทึกผลลัพธ์ของการทำงาน
    return 0;
}

void findLabel(char *filename)
{
    char str[MAXCHAR];
    const char s[] = " ";
    int row = 0;
    FILE *fp;
    fp = fopen(filename, "r"); 
    while (fgets(str, MAXCHAR, fp)) 
    {
        char *token = strtok(str, s);
        int is_Label = 1;
        for (int i = 0; i < 9; i++)
        {
            if (strcmp(token, commandList[i]) == 0) 
            {
                is_Label = 0; 
            }
        }
        if (is_Label == 1) 
        {
            if (strcmp(token, labelList[row]) == 0)
            {
                printf("Error: There are same Label in this assembly code.\n");
                exit(1);
            }
            else
            {
                strcpy(labelList[row], token);
            }
        }
        row++;
    }
    fclose(fp);
}

void compile(char *filename)
{
    char str[MAXCHAR];
    const char s[] = " ";
    int row = 0;
    FILE *fp;
    fp = fopen(filename, "r");
    while (fgets(str, MAXCHAR, fp))
    {
        char command[MAXCHAR];
        strcpy(command, str);
        char *token = strtok(str, s);
        // Check is token a Label? Next token if token is a Label.
        if (strcmp(labelList[row], "") != 0)
            token = strtok(NULL, s);
        // Check command.
        if (strcmp(token, "lw") == 0 || strcmp(token, "sw") == 0 || strcmp(token, "beq") == 0)
            iType(command, row); 
        else if (strcmp(token, "add") == 0 || strcmp(token, "nand") == 0)
            rType(command,row);
        else if (strcmp(token, "halt") == 0 || strcmp(token, "noop") == 0)
            oType(command,row); 
        else if (strcmp(token, "jalr") == 0)
            jType(command,row); 
        else if (strcmp(token, ".fill") == 0)
            fill(command,row); 
        else
        {
            printf("Error: Out of opcode list.\n");
            exit(1);
        }
        row++;
    }
    fclose(fp);
}

void write()
{
    FILE *fp;
    char *filename = "machineCode.txt";
    fp = fopen(filename, "w");
    fprintf(fp, textFile);
    fclose(fp);
    printf("%s\n", textFile);
}

void iType(char *command, int row)
{
    const char s[] = " ";
    char str[MAXCHAR], text[MAXCHAR];
    int count = 0;

    strcpy(str, "");
    char *token = strtok(command, s);

    if (strcmp(labelList[row], "") != 0)
        token = strtok(NULL, s);

    while (count <= 3)
    {
        if (count == 0)
        {
            if (strcmp(token, "lw") == 0)
                sprintf(text, "010");
            else if (strcmp(token, "sw") == 0)
            {
                sprintf(text, "100");
            }
            else
            {
                sprintf(text, "100");
                row++;
            }
        }
        else if (count <= 2)
        {
            sprintf(text, "%s", decimalToBinary(token, INFINITE) + strlen(decimalToBinary(token, INFINITE)) - 3);
        }
        else
        {
            if (atoi(token))
            {
                sprintf(text, "%016s ", decimalToBinary(token, INFINITE));
            }
            else
                for (int i = 0; i < 50; i++) if (strcmp(token, labelList[i]) == 0) sprintf(text, "%016s", decimalToBinary(token, i - row));
                int check = 0;
                for (int i = 0; i < 50; i++) 
                {
                    if (strcmp(token, labelList[i]) == 0){
                        check = 1;
                    }
                }
                if (check = 0){
                    printf("Error: Label is not defined.");
                    exit(1);
                }
                    
        }
        strcat(str, text);
        token = strtok(NULL, s);
        count++;
    }
    int temp = binaryToDecimal(str);
    toText(temp);
    strcpy(str, "");
}

void rType(char *command, int row)
{
    const char s[] = " ";
    char str[MAXCHAR], text[MAXCHAR];
    int count = 0;

    strcpy(str, "");
    char *token = strtok(command, s);

    if (strcmp(labelList[row], "") != 0)
        token = strtok(NULL, s);

    while (count <= 3)
    {
        if (count == 0)
        {
            if (strcmp(token, "add") == 0)
            {
                sprintf(text, "000");
            }
            else
            {
                sprintf(text, "001");
            }
        }
        else
        {
            sprintf(text, "%s", decimalToBinary(token, INFINITE) + strlen(decimalToBinary(token, INFINITE)) - 3);
            if (count == 2)
            {
                strcat(text, "0000000000000");
            }
        }
        strcat(str, text);
        count++;
        token = strtok(NULL, s);
    }
    int temp = binaryToDecimal(str);
    toText(temp);
    strcpy(str, "");
}

void oType(char *command, int row)
{
    const char s[] = " ";
    char str[MAXCHAR], text[MAXCHAR];
    int count = 0;

    strcpy(str, "");
    char *token = strtok(command, s);

    if (strcmp(labelList[row], "") != 0)
        token = strtok(NULL, s);

    if (strcmp(token, "halt") == 0)
    {
        sprintf(text, "110");
    }
    else
    {
        sprintf(text, "111");
    }
    strcat(str, text);
    strcat(str, "0000000000000000000000");

    int temp = binaryToDecimal(str);
    toText(temp);
    strcpy(str, "");
}

void jType(char *command,int row){
    char s[]= " ";
    char str[MAXCHAR], text[MAXCHAR];
    int count = 0;

    strcpy(str,"");
    char *token =strtok(command,s);

    if (strcmp(labelList[row], "") != 0)
        token = strtok(NULL, s);
    
    while( count <=2){
        if( count ==0){
            sprintf(text,"101");
        }else
        {
            sprintf(text,"%s", decimalToBinary(token, INFINITE) + strlen(decimalToBinary(token, INFINITE)) - 3);
        }
        strcat(str,text);
        count++;
        token = strtok(NULL,s);
    }

    strcat(str,"0000000000000000");

    int temp = binaryToDecimal(str);
    toText(temp);
    strcpy(str, "");
}

void fill(char * command, int row){
    const char s[] = " ";
    char str[MAXCHAR], text[MAXCHAR];

    char *token = strtok(command, s);
    if (strcmp(labelList[row], "") != 0)
        token = strtok(NULL, s);
    token = strtok(NULL, s);

    for (int i = 0; i < 50; i++)
    {
        if (strcmp(token, labelList[i]) == 0)
        {
            itoa(i, text, 10);
            strcat(text, "\n");
            strcat(textFile, text);
        }
    }

    if (isNumber(token))
    {
        strcat(token, "\n");
        strcat(textFile, token);
    }
}

const char *decimalToBinary(char *ptr, int index)
{
    //DEFAULT VARIABLES
    static char number[MAXCHAR];
    static char temp[] = "";

    strcpy(temp, "");

    if (index == INFINITE && is16Bits(atoi(ptr)))
    {
        int imm = atoi(ptr);
        for (int i = 15; i >= 0; i--)
        {
            int biNum = imm >> i;
            if (biNum & 1)
                number[15 - i] = '1';
            else
                number[15 - i] = '0';
        }
    }
    else if (index != INFINITE && is16Bits(index))
    {
        int imm = index;
        for (int i = 15; i >= 0; i--)
        {
            int biNum = imm >> i;
            if (biNum & 1)
                number[15 - i] = '1';
            else
                number[15 - i] = '0';
        }
    }
    return number;
}

int binaryToDecimal(char *ptr)
{
    //DEFAULT VARIABLES
    int i = 0;
    int decimal = 0;
    
    while (i < 25)
    {
        decimal += (ptr[i] - '0') * (int)pow(2, 24 - i);
        i++;
    }
    return decimal;
}

int is16Bits(int x)
{
    if (x >= -32768 && x <= 32767)
        return 1;
    printf("Error: Bit out of lenght.");
    exit(1);
}

int isNumber(char *text)
{
    for (int i = 0; i < strlen(text); i++)
        if (!isdigit(text[i]) && i != 0 && &text[0] != "-")
            return 0;
    return 1;
}

void toText(int decimal){
    char temp[MAXCHAR];
    strcpy(temp, itoa(decimal, temp, 10));
    strcat(temp, "\n");
    strcat(textFile, temp);
}