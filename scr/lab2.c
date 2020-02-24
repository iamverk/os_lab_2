

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>



#define STR_SIZE 1000
#define BUF_SIZE 1000


int strToInt(int* buf, char* str, int str_size)
{
    int i = 0, buf_size = 0;
    for (i = 0; i < str_size; i++) {
        if (str[i] == ' ' || str[i] == '\n') {
            buf_size++;
        } else if (str[i] == '-') {
            buf[buf_size] = -1 * (str[i+1] - '0');
            i++;
        } else {
            if (buf[buf_size] >= 0) {
                buf[buf_size] = buf[buf_size] * 10 + (str[i] - '0');
            } else {
                buf[buf_size] = buf[buf_size] * 10 - (str[i] - '0');
            }
        }
    }
    return buf_size;
}


void IntTostr(int* buf, int buf_size, char* str)
{
    int i = 0, str_size = 0;
    for (i = 0; i < STR_SIZE; i++) {
        str[i] = '\0';
    }
    for (i = 0; i < buf_size; i++) {
        str_size += sprintf(str + str_size, "%d", buf[i]);
        str[str_size] = ' ';
        str_size++;
    }
}

void sorti(int*buf, int buf_size)
{
    int ans[buf_size];
    int j;
    int tmp = 0;
    for (int i = 0; i < buf_size; i++) {
        ans[i] = buf[i];
        if (i > 0) {
             j = i - 1;
             while ((ans[j + 1] < ans[j]) && (j >= 0)) {
                 tmp = ans[j + 1];
                 ans[j + 1] = ans[j];
                 ans[j] = tmp;
                 j--;
              }
        }
    }
    for (int i = 0; i <buf_size; i++) {
        buf[i] = ans[i];
    }
}


int main(int argc, char* argv[]) {
    if (argc != 3) {
        printf("Eror. Wrong arguments\n");
        return 1;
    }

    pid_t pid_1 = 0, pid_2 = 0;
    int exit_status = 0;

    int fd_1[2];
    pipe(fd_1);
    int fd_2[2];
    pipe(fd_2);

    char str[STR_SIZE] = {'\0'};
    int str_size = 0;
    char c = '\0';
    int buf[BUF_SIZE] = {0};
    int buf_size = 0;

    if ((pid_1 = fork()) == 0) {

        close(fd_1[0]); // не понадобится выходной поток
        int file_1 = 0;
        if ((file_1 = open(argv[1], O_RDWR)) == -1) { // O_RDWR — для чтения и записи
            printf("%s: no such file\n", argv[1]);
            exit(1); // выйдет из дочернего процесса
        }

        while (read(file_1, &c, 1)) {
            str[str_size] = c;
            str_size++;
        }

        buf_size = strToInt(buf, str, str_size);
        sorti(buf, buf_size);
        IntTostr(buf, buf_size, str);

        write(fd_1[1], str, sizeof(str));
        close(fd_1[1]);

        exit(0);
    }

    //написать результат если exit_status == 0
    wait(&exit_status);
    if (WEXITSTATUS(exit_status) == 0) { // WEXITSTATUS возвращает код завершения потомка процесса.
        close(fd_1[1]);
        read(fd_1[0], str, sizeof(str));
        printf("%s\n", str);
        close(fd_1[0]);
    }

    if ((pid_2 = fork()) == 0) {

        close(fd_2[0]); // не понадобится выходной поток
        int file_2 = 0;
        if ((file_2 = open(argv[2], O_RDWR)) == -1) {
            printf("%s: no such file!\n", argv[2]);
            exit(1);
        }

        while (read(file_2, &c, 1)) {
            str[str_size] = c;
            str_size++;
        }

        buf_size = strToInt(buf, str, str_size);
        sorti(buf, buf_size);
        IntTostr(buf, buf_size, str);

        write(fd_2[1], str, sizeof(str));
        close(fd_2[1]);

        exit(0);
    }

    //написать результат если exit_status == 0
    wait(&exit_status);
    if (WEXITSTATUS(exit_status) == 0) { // WEXITSTATUS возвращает код завершения потомка.
        close(fd_2[1]);
        read(fd_2[0], str, sizeof(str));
        printf("%s\n", str);
        close(fd_2[0]);
    }

    return 0;
}