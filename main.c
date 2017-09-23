#include <winsock2.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
 
#define row 3
#define col 3
#define SIZE_MSG 1024
 
char field[row][col] = { {' ', ' ', ' '},
                         {' ', ' ', ' '},
                         {' ', ' ', ' '} };
 
void display(void);
int  winORnot(char);
void result_win(void);
void result_lose(void);
 
int main(){
    printf("\t-------------------\n");
    printf("\t | Hello friend! | \n");
    printf("\t-------------------\n");
    printf(" | Input: \"1\" if you first player; | \n");
    printf(" | Input: \"0\" if you twice player. | \n");
 
    int flag = FALSE,
        x    = 0,
        y    = 0;
 
    scanf("%i", &flag);
 
    if(flag == TRUE){
        struct sockaddr_in addr; // структура с адресом
        char buf[SIZE_MSG];      // буфер для приема
        WSADATA wsa;             // Структура с информацией о WinSock
        SOCKET sock, listener;   // дескрипторы сокетов
        // Инициализация WinSock
        if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0){
            printf("Failed. Error Code : %d", WSAGetLastError());
            perror("socket");
            exit(1);
        }
        // создаем сокет для входных подключений
        if ((listener = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET){
            printf("Could not create socket : %d", WSAGetLastError());
            perror("socket");
            exit(1);
        }
 
        // Указываем параметры сервера
        addr.sin_family = AF_INET;
        addr.sin_port = htons(3425);
        addr.sin_addr.s_addr = htonl(INADDR_ANY);
 
        if (bind(listener, (struct sockaddr *)&addr, sizeof(addr)) < 0){
            perror("bind");
            exit(2);
        }
 
        listen(listener, 1); // очередь входных подключений
 
        while (1){
            sock = accept(listener, NULL, NULL); // принимаем входные подключение и создаем отделный сокет для каждого нового подключившегося клиента
            if (sock < 0){
                perror("accept");
                exit(3);
            }
            else{
                closesocket(listener);
            }
            while (1){
                recv(sock, buf, SIZE_MSG, 0);//чтение данных из сокета
                if(strcmp(buf, "__end_of_game__") == 0){
                    result_lose();
                    system("pause");
                    break;
                }
                x = buf[0];
                y = buf[1];
                field[x][y] = '0';
                display();
                memset(buf, 0, SIZE_MSG);
                while(1){
                    printf("Enter the line number: ");
                    scanf("%i", &x);
                    printf("Enter the column number: ");
                    scanf("%i", &y);
                    if(field[x][y] == '0' || field[x][y] == '+'){
                        printf("The field is occupied. Re-enter.\n");
                        continue;
                    }
                    break;
                }
                buf[0] = x;
                buf[1] = y;
                field[x][y] = '+';
                display();
                int temp = winORnot('+');
                if(temp == TRUE){
                    result_win();
                    send(sock, "__end_of_game__", SIZE_MSG, 0);
                    system("pause");
                    break;
                }
                send(sock, buf, SIZE_MSG, 0);
            }
            printf("Client disconnect!");
            closesocket(sock); // закрываем сокет
            break;
        }
    }//end (if flag = server)
    else{
        char buf[SIZE_MSG];           // буфер для приема
        struct sockaddr_in addr; // структура с адресом
 
        WSADATA wsa;        // Структура с информацией о WinSock
        SOCKET sock;        // дескриптор сокета
 
        // Инициализация WinSock
        if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0){
            printf("Failed. Error Code : %d", WSAGetLastError());
            perror("socket");
            exit(1);
        }
        // создаем сокет для входных подключений
        if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET){
            printf("Could not create socket : %d", WSAGetLastError());
            perror("socket");
            exit(1);
        }
        // Указываем параметры сервера
        addr.sin_family = AF_INET; // домены Internet
        addr.sin_port = htons(3425); // или любой другой порт...
        addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
        // установка соединения с сервером
        if (connect(sock, (struct sockaddr *)&addr, sizeof(addr)) < 0){
            perror("connect");
            exit(2);
        }
 
        printf("Enter the line number: ");
        scanf("%i", &x);
        printf("Enter the column number: ");
        scanf("%i", &y);
        buf[0] = x;
        buf[1] = y;
        field[x][y] = '0';
        display();
        send(sock, buf, SIZE_MSG, 0); // отправка первого сообщения на сервер
 
        while (1){
            recv(sock, buf, SIZE_MSG, 0);//чтение данных из сокета
                if(strcmp(buf, "__end_of_game__") == 0){
                    result_lose();
                    system("pause");
                    break;
                }
                x = buf[0];
                y = buf[1];
                field[x][y] = '+';
                display();
                memset(buf, 0, SIZE_MSG);
                while(1){
                    printf("Enter the line number: ");
                    scanf("%i", &x);
                    printf("Enter the column number: ");
                    scanf("%i", &y);
                    if(field[x][y] == '0' || field[x][y] == '+'){
                        printf("The field is occupied. Re-enter.\n");
                        continue;
                    }
                    break;
                }
                buf[0] = x;
                buf[1] = y;
                field[x][y] = '0';
                display();
                int temp = winORnot('0');
                if(temp == TRUE){
                    result_win();
                    send(sock, "__end_of_game__", SIZE_MSG, 0);
                    system("pause");
                    break;
                }
                send(sock, buf, SIZE_MSG, 0);
        }
        closesocket(sock); // закрываем сокет
    }//end (if flag = client)
    return 0;
}
 
void display(void){
    printf("\n\n");
    int i = 0,
        j = 0;
    for(i = 0; i < row; i++){
        for(j = 0; j < col; j++){
            printf("%c", field[i][j]);
            if(j < 2){
                printf(" * ");
            }
        }
        if(i < 2){
            printf("\n* * * * *\n");
        }
    }
    printf("\n\n");
}
 
int winORnot(char SYMBOL){
    if((field[0][0] == SYMBOL)&&(field[0][1] == SYMBOL)&&(field[0][2] == SYMBOL)){
        return TRUE;
    }
    if((field[1][0] == SYMBOL)&&(field[1][1] == SYMBOL)&&(field[1][2] == SYMBOL)){
        return TRUE;
    }
    if((field[2][0] == SYMBOL)&&(field[2][1] == SYMBOL)&&(field[2][2] == SYMBOL)){
        return TRUE;
    }
    if((field[0][0] == SYMBOL)&&(field[1][0] == SYMBOL)&&(field[2][0] == SYMBOL)){
        return TRUE;
    }
    if((field[0][1] == SYMBOL)&&(field[1][1] == SYMBOL)&&(field[2][1] == SYMBOL)){
        return TRUE;
    }
    if((field[0][2] == SYMBOL)&&(field[1][2] == SYMBOL)&&(field[2][2] == SYMBOL)){
        return TRUE;
    }
    if((field[0][0] == SYMBOL)&&(field[1][1] == SYMBOL)&&(field[2][2] == SYMBOL)){
        return TRUE;
    }
    if((field[2][0] == SYMBOL)&&(field[1][1] == SYMBOL)&&(field[0][2] == SYMBOL)){
        return TRUE;
    }
        return FALSE;
}
 
void result_win(void){
    printf("!!!!!!!!!!!!!!!!!!!\n");
    printf("!!!!!!YOU WIN!!!!!!\n");
    printf("!!!!!!YOU WIN!!!!!!\n");
    printf("!!!!!!YOU WIN!!!!!!\n");
    printf("!!!!!!!!!!!!!!!!!!!\n");
}
 
void result_lose(void){
    printf("!!!!!!!!!!!!!!!!!!!!\n");
    printf("!!!!!!YOU LOSE!!!!!!\n");
    printf("!!!!!!YOU LOSE!!!!!!\n");
    printf("!!!!!!YOU LOSE!!!!!!\n");
    printf("!!!!!!!!!!!!!!!!!!!!\n");
}
