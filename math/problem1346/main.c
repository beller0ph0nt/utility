#include <stdio.h>

#define DIGIT_LEN       4
#define OPERATION_LEN   4

float
calc_equiation(float first_digit, char operation, float second_digit)
{
    float answer = 0;

    switch (operation)
    {
    case '+':
        answer = first_digit + second_digit;
        break;
    case '-':
        answer = first_digit - second_digit;
        break;
    case '*':
        answer = first_digit * second_digit;
        break;
    case '/':
        answer = first_digit / second_digit;
        break;
    }

    return answer;
}

int
main(int argc, char** argv)
{
    float digit[DIGIT_LEN] = { 1, 3, 4, 6 };
    char operation[OPERATION_LEN] = { '+', '-', '*', '/' };
    int i, j, k, l, m, n, o, count = 1;
    float answer;

    for (i = 0; i < DIGIT_LEN; i++)
    {
        for (j = 0; j < DIGIT_LEN; j++)
        {
            if (j == i)
                continue;

            for (k = 0; k < DIGIT_LEN; k++)
            {
                if (k == i || k == j)
                    continue;

                for (l = 0; l < DIGIT_LEN; l++)
                {
                    if (l == i || l == j || l == k)
                        continue;


                    for (m = 0; m < OPERATION_LEN; m++)
                    {
                        for (n = 0; n < OPERATION_LEN; n++)
                        {
                            for (o = 0; o < OPERATION_LEN; o++)
                            {
                                answer = calc_equiation(digit[i], operation[m], digit[j]);
                                answer = calc_equiation(  answer, operation[n], digit[k]);
                                answer = calc_equiation(  answer, operation[o], digit[l]);
                                printf("%d. ( ( %f %c %f ) %c %f ) %c %f = %f",
                                       count++,
                                       digit[i],
                                       operation[m],
                                       digit[j],
                                       operation[n],
                                       digit[k],
                                       operation[o],
                                       digit[l],
                                       answer);

                                if (answer == 24)
                                    printf("    Eureka!!!");

                                printf("\n");

                                answer = calc_equiation(digit[j], operation[n], digit[k]);
                                answer = calc_equiation(digit[i], operation[m],   answer);
                                answer = calc_equiation(  answer, operation[o], digit[l]);
                                printf("%d. ( %f %c ( %f %c %f ) ) %c %f = %f",
                                       count++,
                                       digit[i],
                                       operation[m],
                                       digit[j],
                                       operation[n],
                                       digit[k],
                                       operation[o],
                                       digit[l],
                                       answer);

                                if (answer == 24)
                                    printf("    Eureka!!!");

                                printf("\n");

                                answer = calc_equiation(digit[j], operation[n], digit[k]);
                                answer = calc_equiation(  answer, operation[o], digit[l]);
                                answer = calc_equiation(digit[i], operation[m],   answer);
                                printf("%d. %f %c ( ( %f %c %f ) %c %f ) = %f",
                                       count++,
                                       digit[i],
                                       operation[m],
                                       digit[j],
                                       operation[n],
                                       digit[k],
                                       operation[o],
                                       digit[l],
                                       answer);

                                if (answer == 24)
                                    printf("    Eureka!!!");

                                printf("\n");

                                answer = calc_equiation(digit[k], operation[o], digit[l]);
                                answer = calc_equiation(digit[j], operation[n],   answer);
                                answer = calc_equiation(digit[i], operation[m],   answer);
                                printf("%d. %f %c ( %f %c ( %f %c %f ) ) = %f",
                                       count++,
                                       digit[i],
                                       operation[m],
                                       digit[j],
                                       operation[n],
                                       digit[k],
                                       operation[o],
                                       digit[l],
                                       answer);

                                if (answer == 24)
                                    printf("    Eureka!!!");

                                printf("\n");
                            }
                        }
                    }


                }
            }
        }
    }
}
