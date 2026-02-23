#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <unistd.h>
#include <time.h>
#include <signal.h>

// 大写转换
char* capital(char* content){
    int length = strlen(content);
    char* result = calloc(length + 1, sizeof(char));

    for (int i = 0; i < length; i ++) {
        if (content[i] >= 'A' && content[i] <= 'Z') {
            result[i] = content[i];
        }
        else if (content[i] >= 'a' && content[i] <= 'z') {
            int transform = content[i] - 'a' + 'A';
            result[i] = (char)transform;
        }
        else {
            free(result);
            return NULL;
        }
    }

    return result;
}

// 转子映射
char mappingOfRotor(int init, int numOfRotor, char code) {
    char rotor[5][27] = {
        "EKMFLGDQVZNTOWYHXUSPAIBRCJ",
        "AJDKSIRUXBLHWTMCQGZNPYFVOE",
        "BDFHJLCPRTXVZNYEIWGAKMUSQO",
        "ESOVPZJAYQUIRHXLNFTGKDCMWB",
        "VZBRGITYUPSDNHLXAWMJQOFECK"
    };

    int offset = (init - 1 + 26) % 26;
    int in = (code - 'A' + offset) % 26;
    int out = rotor[numOfRotor][in] - 'A';
    int result = (out - offset + 26) % 26 + 'A';

    return result;
}

// 转子逆映射
char antimappingOfRotor(int init, int numOfRotor, char code) {
    char rotor[5][27] = {
        "UWYGADFPVZBECKMTHXSLRINQOJ",
        "AJPCZWRLFBDKOTYUQGENHXMIVS",
        "TAGBPCSDQEUFVNZHYIXJWLRKOM",
        "HZWVARTNLGUPXQCEJMBSKDYOIF",
        "QCYLXWENFTZOSMVJUDKGIARPHB",
    };

    int offset = (init - 1 + 26) % 26;
    int in = (code - 'A' + offset) % 26;
    int out = rotor[numOfRotor][in] - 'A';
    int result = (out - offset + 26) % 26 + 'A';

    return result;

}


// 反射器
char mappingOfReflector(char code) {
    char reflector[27] = "YRUHQSLDPXNGOKMIEBFZCWVJAT";

    return reflector[code - 'A'];
}

// 插板
char plugboard(char init[2][11], char code) {
    for (int i = 0; i < 2; i ++) {
        for (int j = 0; j < 10; j ++) {
            if (code == init[i][j]) {
                return init[1 - i][j];
            }
        }
    }

    return code;
}

// 加密
char* enigma(char initForPlugboard[2][11], int* initForRotor, int* numOfRotor, char* code) {
    char carrier[6] = "QEVJZ";
    int length = strlen(code);
    int num[3];
    char* result = (char*)calloc(length + 1, sizeof(char));

    for (int i = 0; i < 3; i ++) {
        num[i] = numOfRotor[i] - 1;
    }

    for (int i = 0; i < length; i ++) {
        initForRotor[2] = (initForRotor[2] + 1) % 26 + 1;
        if ((initForRotor[2] + 'A' - 1) == carrier[num[2]]) {
            initForRotor[1] = (initForRotor[1] + 1) % 26 + 1;
        }
        if ((initForRotor[1] + 'A' - 1) == carrier[num[1]]) {
            initForRotor[0] = (initForRotor[0] + 1) % 26 + 1;
        }

        char k1 = plugboard(initForPlugboard, code[i]);

        char k2 = mappingOfRotor(initForRotor[2], num[2], k1);
        char k3 = mappingOfRotor(initForRotor[1], num[1], k2);
        char k4 = mappingOfRotor(initForRotor[0], num[0], k3);

        char k5 = mappingOfReflector(k4);

        char k6 = antimappingOfRotor(initForRotor[0], num[0], k5);
        char k7 = antimappingOfRotor(initForRotor[1], num[1], k6);
        char k8 = antimappingOfRotor(initForRotor[2], num[2], k7);

        result[i] = plugboard(initForPlugboard, k8);
    }
    result[length] = '\0';

    return result;
}

// 颜色控制
void color(int key) {
    if (key == -1) {
        printf("\033[0m");          // 重置
    } else if (key == 4) {
        printf("\033[31m");         // 红色
    } else if (key == 2) {
        printf("\033[32m");         // 绿色
    } else if (key == 9) {
        printf("\033[34m");         // 蓝色
    } else if (key == 7) {
        printf("\033[37m");         // 白色
    }
    fflush(stdout);
}

// 打字机效果
void typef(const char *format, ...) {
    va_list args;
    va_start(args, format);

    va_list args_copy;
    va_copy(args_copy, args);
    int length = vsnprintf(NULL, 0, format, args_copy);
    va_end(args_copy);

    char *content = (char *)malloc((length + 1) * sizeof(char));
    vsnprintf(content, length + 1, format, args);
    va_end(args);

    for (int i = 0; i < length; i++) {
        putchar(content[i]);
        fflush(stdout);
        int speed = 32000;
        int variation = (rand() % 40 - 20) * 1000;
        usleep(speed + variation);
    }
    free(content);
}

// 暂停
void pause() {
    printf("Press Enter to continue...");
    fflush(stdout);
    int c;
    while (c = getchar() != '\n' && c != EOF);
    getchar();
}

// 异常退出
void handle_sigint(int sig) {
    color(-1);
    exit(0);
}

int main() {
    signal(SIGINT, handle_sigint);

    // 初始化
    char num[10][5] = {"I", "II", "III", "IV", "V", "VI", "VII", "VIII", "IX", "X"};

    char initForPlugboard[2][11];
    int initForRotor[3];
    int numOfRotor[3];

    color(2);
    printf("Set Rotors > ");
    color(-1);
    scanf(
            "%d %d %d",
            &numOfRotor[0],
            &numOfRotor[1],
            &numOfRotor[2]
        );
    for (int i = 0; i < 3; i ++) {
        if (numOfRotor[i] < 1 || numOfRotor[i] > 5) {
            color(4);
            printf("Rotor Number Error.\n");
            color(-1);

            pause();
            return 1;
        }
    }

    color(2);
    printf("Initialize Rotors > ");
    color(-1);
    scanf(
            "%d %d %d",
            &initForRotor[0],
            &initForRotor[1],
            &initForRotor[2]
         );
    for (int i = 0; i < 3; i ++) {
        if (initForRotor[i] < 1 || initForRotor[i] > 26) {
            color(4);
            printf("Initialize of Rotor Error.\n");
            color(-1);

            pause();
            return 1;
        }
    }

    for (int i = 0; i < 10; i ++) {
        printf("Set the Plugboard ");
        color(2);
        printf("%s", num[i]);
        color(-1);
        printf(" > ");
        scanf(" %c%c", &initForPlugboard[0][i], &initForPlugboard[1][i]);
    }
    initForPlugboard[0][10] = '\0';
    initForPlugboard[1][10] = '\0';
    for (int i = 0; i < 2; i++) {
        char* upper = capital(initForPlugboard[i]);
        if (upper == NULL) {
            color(4);
            printf("Plugboard Error.\n");
            color(-1);

            pause();
            return 1;
        }
        strcpy(initForPlugboard[i], upper);
        free(upper);
    }

    printf("Use");
    color(2);
    printf(" ;; ");
    color(-1);
    printf("to exit.\n");

    // 使用
    for (;;) {
        char* code = (char*)calloc(1024, sizeof(char));
    
        printf("Content > ");
        scanf("%1023s", code);

        if (!strcmp(code, ";;")) {
            color(2);
            printf("exiting...\n");
            color(-1);
            break;
        }

        char* codeTransformed = capital(code);
    
        if (NULL == codeTransformed) {
           color(4);
           printf("Content Error.\n");
           color(-1);
        }
        else {
            char* result = enigma(initForPlugboard, initForRotor, numOfRotor, codeTransformed);
            usleep(86);
            color(9);
            typef("%s", result);
            color(-1);
            usleep(128);
            printf("\n");

            free(result);
        }
    
        free(code);
        free(codeTransformed);
    }
    
    color(-1);

    pause();
    return 0;
}
