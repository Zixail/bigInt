#include <stdio.h>
#include <string.h>

#define SIZE_S      1024
#define SIZE_HU     650
#define SIZE_B      540

//  Получение длины массива двоичного числа
short getLength(unsigned short *binary){
    short length = 1;
    short neg = (((binary[SIZE_B-1] & (1 << 15)) == (1 << 15)) > 0);
    for(short i = SIZE_B - 1; i >= 0; --i){
        if (binary[i] != 65535 * neg){
            length = i + 1;
            break;
        }
    }
    return length;
}

// Сканирование числа из консоли в десятичный массив
short scanDecimal(unsigned short *decimal){
    char string[SIZE_S];
    short flag = 0;
    scanf("%1023s", string);
    if (string[0] == '-'){
        flag = 1;
        decimal[SIZE_HU - 1] = decimal[SIZE_HU - 1] | (1 << 15);
    }
    short pw = 1;
    short how = (4 - ((strlen(string) - flag) % 4)) % 4;
    for(short i = strlen(string) - 1; i >= 0 + flag; --i){
        decimal[(i - flag + how) / 4] += (string[i] - 48) * pw;
        if (pw == 1000) pw = 1;
        else pw *= 10;
    }
    
    return (strlen(string) - flag - 1) / 4 + 1;
}

//  Перевод модуля двоичного числа в негативное по принципу доп кода
void toNegative(unsigned short *binary){
    for(short i = 0; i < SIZE_B; ++i){
        binary[i] = ~binary[i];
    }
    short i;
    for(i = 0; binary[i] == 65535; ++i){
        continue;
    }
    binary[i] = binary[i] + 1;
}

//  Перевод отрицательного двоичного числа в его модуль по принципу доп кода
void toPositive(unsigned short *binary){
    short i;
    for(i = 0; binary[i] == 0; ++i){
        continue;
    }
    binary[i] = binary[i] - 1;
    for(short i = 0; i < SIZE_B; ++i){
        binary[i] = ~binary[i];
    }
}

// Перевод массива чисел десятичного представления в двоичное
void toBinary(unsigned short *decimal, short length, unsigned short *binary){
    short first = 0;
    short modulo = 0;
    short remainder = 0;
    short bit = 0;

    short negative = 0;
    if ((decimal[SIZE_HU-1] & (1 << 15)) == (1 << 15)){
        negative = 1;
    }

    while (first != length){

        //  Работа с decimal
        modulo = decimal[length - 1] % 2;
        decimal[length - 1] = (decimal[length - 1] >> 1);
        for(short i = length - 2; i >= 0; --i){
            remainder = decimal[i] % 2;
            decimal[i] = (decimal[i] >> 1);
            decimal[i+1] = decimal[i+1] + 5000 * remainder;
        }
        if (decimal[first] == 0) ++first;

        //  Работа с binary
        binary[bit/16] = binary[bit/16] + modulo * (1 << (bit % 16));
        ++bit;
    }

    //  Перевод негативного числа по принципу дополнительного кода
    if (negative > 0){
        toNegative(binary);
    }
}

// Перевод массива чисел двоичного представления в десятичное
short toDecimal(unsigned short *input, unsigned short *decimal){
    unsigned short binary[SIZE_B] = {0};
    for (short i = 0; i < SIZE_B; ++i) binary[i] = input[i];

    char negative = 0;
    if ((binary[SIZE_B-1] & (1 << 15)) == (1 << 15)){
        negative = 1;
        toPositive(binary);
    }
    short length = getLength(binary);
    unsigned short bit;
    short digit = 1;
    short ost = 0;
    for(short j = length * 16 - 1; j >= 0; --j){
        bit = (binary[j / 16] & (1 << (j % 16)));
        binary[j / 16] = binary[j / 16] - bit;
        for (short i = 0; i < digit; ++i){
            decimal[i] = (decimal[i] << 1) + ost;
            if (i == 0){
                decimal[0] += (bit > 0);
            }
            ost = 0;
            ost = decimal[i] / 10000;
            decimal[i] = decimal[i] % 10000;
        }
        if (ost > 0){
            decimal[digit] += ost;
            ost = 0;
            ++digit;
        }
    }
    if (negative > 0){
        decimal[SIZE_HU - 1] = (decimal[SIZE_HU-1] | (1 << 15));
    }
    return digit;

}

//  Сканирование bigInt числа при помощи вспомогательных функций
void scanBigInt(unsigned short *binary){
    for(short i = 0; i < SIZE_B; ++i){
        binary[i] = 0;
    }
    unsigned short decimal[SIZE_HU] = {0};
    short lengthDec = scanDecimal(decimal);
    toBinary(decimal, lengthDec, binary);
}

//  Вывод bigInt числа в консоль
void printBigInt(unsigned short *binary){
    unsigned short decimal[SIZE_HU] = {0};
    short length = toDecimal(binary, decimal);
    
    if(((decimal[SIZE_HU - 1]) & (1 << 15)) == (1 << 15)){
        printf("-");
    }
    printf("%hu", decimal[length - 1]);
    for(short i = length - 2; i >= 0; --i){
        printf("%04hu", decimal[i]);
    }
    printf("\n");
}

//  Сложение bigInt чисел
void sumBigInt(unsigned short *binary1, unsigned short *binary2, unsigned short *result){
    for(short i = 0; i < SIZE_B; ++i) result[i] = 0;
    short tmp = 0;
    for(short i = 0; i < SIZE_B; ++i){
        unsigned int total = binary1[i] + binary2[i] + tmp;
        tmp = 0;
        if (total >= (1 << 16)){
            total -= (1 << 16);
            ++tmp;
        }
        result[i] = total;
    }
}

//  Вычитание bigInt чисел
void subtractBigInt(unsigned short *binary1, unsigned short *binary2, unsigned short *result){
    short length = getLength(binary2);
    if (length == 1 && binary2[0] == 0){
        for(short i = 0; i < SIZE_B; ++i){
            result[i] = binary1[i];
        }
        return;
    }
    unsigned short binary2Neg[SIZE_B] = {0};
    for(short i = 0; i < SIZE_B; ++i) binary2Neg[i] = binary2[i];
    toNegative(binary2Neg);
    sumBigInt(binary1, binary2Neg, result);
}

//  Сдвиг двоичного массива на 1 бит
short shiftBinary(unsigned short *binary, short length, short shift){
    for (short i = 0; i < shift; ++i){
        short tmp = 0;
        for(short j = 0; j < length; ++j){
            unsigned int num = (binary[j] << 1) + tmp;
            tmp = 0;
            if (num >= (1 << 16)){
                tmp = 1;
                num -= (1 << 16);
            }
            binary[j] = num;
        }
        if (tmp && length < SIZE_B) {
            binary[length] = 1;
            length++;
        }
    }
    return length;
}

//  Умножение bigInt чисел
void multiBigInt(unsigned short *input1, unsigned short *input2, unsigned short *result){
    unsigned short binary1[SIZE_B] = {0};
    unsigned short binary2[SIZE_B] = {0};
    for(short i = 0; i < SIZE_B; ++i){
        result[i] = 0;
        binary1[i] = input1[i];
        binary2[i] = input2[i];
    }
    
    short neg1 = (((binary1[SIZE_B-1] & (1 << 15)) == (1 << 15)) > 0);
    short neg2 = (((binary2[SIZE_B-1] & (1 << 15)) == (1 << 15)) > 0);
    short negative = (neg1 != neg2);
    if (neg1 > 0) toPositive(binary1);
    if (neg2 > 0) toPositive(binary2);

    short length = getLength(binary1);
    for (short i = 0; i < getLength(binary2); ++i){
        for(short j = 0; j < 16; ++j){
            if ((binary2[i] & (1 << j)) > 0){
                unsigned short temp1[SIZE_B] = {0};
                unsigned short temp2[SIZE_B] = {0};
                for (short k = 0; k < SIZE_B; ++k){
                    temp2[k] = result[k];
                    temp1[k] = binary1[k];
                }
                short shift = i * 16 + j;
                length = shiftBinary(temp1, length, shift);
                
                sumBigInt(temp2, temp1, result);
            }
        }
    }
    if (negative > 0) toNegative(result);
}

//  Возведение в степень bigInt числа
void powBigInt(unsigned short *input1, unsigned short *input2, unsigned short *result){
    unsigned short binary1[SIZE_B] = {0};
    unsigned short binary2[SIZE_B] = {0};
    unsigned short unit[SIZE_B] = {0};

    for(short i = 0; i < SIZE_B; ++i){
        unit[i] = 0;
        result[i] = 0;
        binary1[i] = input1[i];
        binary2[i] = input2[i];
    }

    short neg = (((binary1[SIZE_B-1] & (1 << 15)) == (1 << 15)) > 0);
    short even = binary2[0] % 2;
    if (neg > 0) toPositive(binary1);
    unit[0] = 1;
    result[0] = 1;

    while(!(getLength(binary2) == 1 && binary2[0] == 0)){
        unsigned short temp1[SIZE_B] = {0};
        unsigned short temp2[SIZE_B] = {0};
        unsigned short temp3[SIZE_B] = {0};
        unsigned short tmpExp[SIZE_B] = {0};
        for(short i = 0; i < SIZE_B; ++i){
            temp1[i] = result[i];
            temp2[i] = binary1[i];
            temp3[i] = binary2[i];
        }
        multiBigInt(temp1, temp2, result);
        subtractBigInt(temp3, unit, tmpExp);
        for(short i = 0; i < SIZE_B; ++i) binary2[i] = tmpExp[i];
    }
    if (neg > 0 && even > 0) toNegative(result);
}

//  Факториал от bigInt числа
void factBigInt(unsigned short *input1, unsigned short *result){
    unsigned short binary1[SIZE_B] = {0};
    unsigned short unit[SIZE_B] = {0};

    for(short i = 0; i < SIZE_B; ++i){
        unit[i] = 0;
        result[i] = 0;
        binary1[i] = input1[i];
    }
    unit[0] = 1;
    result[0] = 1;

    while(!(getLength(binary1) == 1 && binary1[0] == 1)){
        unsigned short temp1[SIZE_B] = {0};
        unsigned short temp2[SIZE_B] = {0};
        unsigned short tmpExp[SIZE_B] = {0};
        for(short i = 0; i < SIZE_B; ++i){
            temp1[i] = result[i];
            temp2[i] = binary1[i];
        }
        multiBigInt(temp1, temp2, result);
        subtractBigInt(temp2, unit, tmpExp);
        for(short i = 0; i < SIZE_B; ++i) binary1[i] = tmpExp[i];
    }
}

//  Сумма массива чисел от первого до последнего
void progrBigInt(unsigned short *input1, unsigned short *input2, unsigned short *result){
    unsigned short binary1[SIZE_B] = {0};
    unsigned short binary2[SIZE_B] = {0};
    unsigned short unit[SIZE_B] = {0};

    for(short i = 0; i < SIZE_B; ++i){
        unit[i] = 0;
        result[i] = 0;
        binary1[i] = input1[i];
        binary2[i] = input2[i];
    }
    unit[0] = 1;
    result[0] = 1;

    unsigned short first[SIZE_B] = {0};
    unsigned short second[SIZE_B] = {0};

    sumBigInt(binary2, unit, first);
    unsigned short firstTemp[SIZE_B] = {0};
    for(short i = 0; i < SIZE_B; ++i) firstTemp[i] = first[i];
    subtractBigInt(firstTemp, binary1, first);

    sumBigInt(binary1, binary2, second);
    
    multiBigInt(first, second, result);

    short ost = 0;
    short even = 0;
    for(short i = getLength(result) - 1; i >= 0; --i){
        even = (result[i] % 2);
        result[i] = (result[i] >> 1) + ost;
        ost = even * (1 << 15);
    }
}

int main(void){
    char oper[2];
    scanf("%s%*c", oper);
    unsigned short bigInt1[SIZE_B];
    unsigned short bigInt2[SIZE_B];
    scanBigInt(bigInt1);
    if (oper[0] != '!'){
        scanBigInt(bigInt2);
    }
    unsigned short result[SIZE_B];
    printf("Result=");
    switch (oper[0]){
        case '+':
            sumBigInt(bigInt1, bigInt2, result);
            break;
        case '-':
            subtractBigInt(bigInt1, bigInt2, result);
            break;
        case '*':
            multiBigInt(bigInt1, bigInt2, result);
            break;
        case '^':
            powBigInt(bigInt1, bigInt2, result);
            break;
        case '!':
            factBigInt(bigInt1, result);
            break;
        case 'S':
            progrBigInt(bigInt1, bigInt2, result);
            break;
        default:
            return 0;
    }

    printBigInt(result);
    return 0;
}