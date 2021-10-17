/*************************************************
Author: MOZhuai
Date: 2019-12-21 
Description: 该文件系统用于本科《操作系统》的课程设计。
该系统使用c++语言编写，文件的存储结构为树，代码中将树转
化为二叉树（左孩子，右兄弟）。整体功能包含浏览文件目录、
修改文件、写入文件内容、查看文件详细信息、文件上锁等功能。
部分功能（文件上锁等）的逻辑存在一些问题，但大体不影响使用。
**************************************************/


#include<iostream>
#include<cstring>
#include<windows.h>
#include<stdlib.h>
#include<time.h>
#include<conio.h>
using namespace std;

char username[10];

typedef struct fileInfo {
    int data;                           //层数
    struct fileInfo *Child;             //子文件
    struct fileInfo *Brother;           //子文件
    struct fileInfo *Parent;            //父文件
    char filename[11];                  //文件名
    char attribute;                     //文件属性字段，0：目录文件，1：数据文件
    char time[20];                      //文件创建时间
    char content[100];                  //文件内容
    char createAuthor[10];				//文件作者
    int lock = 0;						//文件上锁
} Node;

Node* node=(Node*)malloc(sizeof(Node));         //创建一个根节点
Node* i_node=(Node*)malloc(sizeof(Node));       //标记当前位置的节点

//获取当前时间
char * getTime(){
    time_t now_time=time(NULL); 
    tm*  t_tm = localtime(&now_time); 
    return (char*)asctime(t_tm);
}

//初始化
void initialization(){                          //初始化
    node->data = 0;
    strcpy(node->filename, "localhost");
    strcpy(node->time, getTime()); 
    node->Child = NULL;
    node->Brother = NULL;
    node->Parent = NULL;
}

//展示当前目录文件
void showDir(){
    if(i_node -> Child != NULL){
        int count = 1;
        Node* new_node=(Node*)malloc(sizeof(Node));
        new_node = i_node -> Child;
		if(new_node -> attribute == '0'){
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_GREEN);
        }
        cout << new_node -> filename << '\t';
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
        while(new_node -> Brother != NULL){
            new_node = new_node -> Brother;
            if(new_node -> attribute == '0'){
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_GREEN);
            }
            cout << new_node -> filename;
            SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
            count += 1;
            if(count % 4 == 0){
                cout << endl << "*  ";
            } else {
                cout << '\t';
            }
        }
        cout << endl;
    } else {
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_RED);
        cout << "No file exit!" << endl;
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
    }
}

//添加孩子节点的兄弟节点
void add(Node* add_node){
    if(i_node -> Child == NULL){
        i_node -> Child = add_node;
        add_node -> Parent = i_node;
    } else {
        Node* ls_node=(Node*)malloc(sizeof(Node));
        ls_node = i_node -> Child;
        while(ls_node -> Brother != NULL){
            ls_node = ls_node -> Brother;
        }
        ls_node -> Brother = add_node;
        add_node -> Parent = ls_node;
    }
}

//新建文件夹
void newFolder(){
    char ch[11];
    cout << "Input filename:";
    cin >> ch;
    Node* new_node=(Node*)malloc(sizeof(Node));
    strcpy(new_node -> filename, ch);
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_GREEN | FOREGROUND_RED);
    cout << "Folder [" <<new_node -> filename << ']' << " successfully added!";
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
    new_node -> data = i_node -> data + 1;
    strcpy(new_node -> time, getTime());
    cout << endl << "Create time:  " << getTime();
    strcpy(new_node -> createAuthor, username);
    new_node -> Child = NULL;
    new_node -> Brother = NULL;
    new_node -> attribute = '0';
    add(new_node);
}

//新建文件
void newFile(){
    char ch[11];
    cout << "Input filename:";
    cin >> ch;
    Node* new_node=(Node*)malloc(sizeof(Node));
    strcpy(new_node -> filename, ch);
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_GREEN | FOREGROUND_RED);
    cout << "File [" <<new_node -> filename << "] successfully added!";
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
    new_node -> data = i_node -> data + 1;
    strcpy(new_node -> time, getTime());
    cout << endl << "Create time:  " << getTime();
    strcpy(new_node -> createAuthor, username);
    new_node -> Child = NULL;
    new_node -> Brother = NULL;
    new_node -> attribute = '1';
    add(new_node);
}


//返回上级目录
void cdreturn(){
    if(i_node -> data == 0){
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_RED);
        cout << "no Parent dir!";
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
    } else {
        while(i_node -> Parent -> data == i_node -> data){
            i_node = i_node -> Parent;
        }
        i_node = i_node -> Parent;
    }
}

//转到下级目录
void cdnext(){
    int flag = 1;
    if(i_node -> Child != NULL){
        cout << "please input the folder you want in:";
        char ch[11];
        cin >> ch;
        if(strcmp(i_node -> Child -> filename, ch) == 0 && i_node -> Child -> attribute == '0'){
			if(i_node -> lock == 1 && strcmp(i_node -> createAuthor, username) != 0){
	            SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_RED);
	            cout << "You have no power to cd!";
	            SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
    		} else {
    			i_node = i_node -> Child;
    		}
            flag = 0;
        } else {
            Node* ls_node=(Node*)malloc(sizeof(Node));
            ls_node = i_node -> Child;
            while(ls_node -> Brother != NULL){
                ls_node = ls_node -> Brother;
                if(strcmp(ls_node -> filename, ch) == 0 && ls_node -> attribute == '0'){
                	if(ls_node -> lock == 1 && strcmp(ls_node -> createAuthor, username) != 0){
	           			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_RED);
	            		cout << "You have no power to cd!";
	            		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
    				} else{
    					i_node = ls_node;
    				}
                    flag = 0;
                    break;
                }
            }
        }
        if(flag){
            SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_RED);
            cout << "no folder named [" << ch << "] exit!";
            SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
        }
    } else {
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_RED);
        cout << "No file exit in Directory!";
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
    }
}

//展示当前目录
void printcurDir(){
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
    cout << "#" << username;
    char dir[10][11];
    Node* ls_node=(Node*)malloc(sizeof(Node));
    ls_node = i_node;
    int i = 1;
    if(i_node -> data != 0){
        strcpy(dir[0], ls_node -> filename);
        while(ls_node -> data != 0){
            while(ls_node -> Parent -> data == ls_node -> data){
                ls_node = ls_node -> Parent;
            }
            ls_node = ls_node -> Parent;
            strcpy(dir[i], ls_node -> filename);
            i += 1;
        }
        for(i-=1;i>=0; i--){
            cout << ">" << dir[i];
        }
    } else {
        cout << ">localhost";
    }
    cout << endl;
}

void deleteNode(Node* del_node){
    if(del_node -> Brother != NULL){
        if(del_node -> Parent -> data == del_node -> data - 1){
            del_node -> Parent -> Child = del_node -> Brother;
        	del_node -> Brother -> Parent = del_node -> Parent;
        } else {
            del_node -> Parent -> Brother = del_node -> Brother;
            del_node -> Brother -> Parent = del_node -> Parent;
        }
    } else {
        if(del_node -> Parent -> data == del_node -> data - 1){
            del_node -> Parent -> Child = NULL;
        } else {
            del_node -> Parent -> Brother = NULL;
        }
    }
    //释放变量
   free(del_node); 
}

//删除文件(夹)
void deleteFile(){
    int flag = 1;
    if(i_node -> Child != NULL){
        cout << "please input the folder you want to delete:";
        char ch[11];
        cin >> ch;
        Node* ls_node=(Node*)malloc(sizeof(Node));
        ls_node = i_node -> Child;
        if(strcmp(ls_node -> filename, ch) == 0){
            flag = 0;
        } else {
            while(ls_node -> Brother != NULL){
                ls_node = ls_node -> Brother;
                if(strcmp(ls_node -> filename, ch) == 0){
                    flag = 0;
                    break;
                }
            }
        }
        if(flag){
            SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_RED);
            cout << "no folder named [" << ch << "] exit!";
            SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
        } else if(ls_node -> lock == 1 && strcmp(ls_node -> createAuthor, username) != 0){
            SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_RED);
            cout << "You have no power to delete!";
            SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
		} else {
			deleteNode(ls_node);
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_GREEN | FOREGROUND_RED);
            cout << "successful!";
            SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
		}
    } else {
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_RED);
        cout << "No file exit in Directory!";
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
    }
}

//修改文件名
void rename(){
    int flag = 1;
    if(i_node -> Child != NULL){
        cout << "please input the filename you want to rename:";
        char ch[11];
        cin >> ch;
        Node* ls_node=(Node*)malloc(sizeof(Node));
        ls_node = i_node -> Child;
        if(strcmp(ls_node -> filename, ch) == 0){
            flag = 0;
        } else {
            while(ls_node -> Brother != NULL){
                ls_node = ls_node -> Brother;
                if(strcmp(ls_node -> filename, ch) == 0 && ls_node -> attribute == '0'){
                    flag = 0;
                    break;
                }
            }
        }
        if(flag){
            SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_RED);
            cout << "no folder named [" << ch << "] exit!";
            SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
        } else if(ls_node -> lock == 1 && strcmp(ls_node -> createAuthor, username) != 0){
            SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_RED);
            cout << "You have no power to rename this file!";
            SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
    	} else {
            char file_rename[11];
            cout << "please input the new name:";
            cin >> file_rename;
            strcpy(ls_node -> filename, file_rename);
        }
    } else {
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_RED);
        cout << "No file exit, please input '1' or '2' to create new file.";
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
    }
}

//输出文信息
void infoFile(){
    int flag = 1;
    if(i_node -> Child != NULL){
        cout << "please input the filename:";
        char ch[11];
        cin >> ch;
        Node* ls_node=(Node*)malloc(sizeof(Node));
        ls_node = i_node -> Child;
        if(strcmp(ls_node -> filename, ch) == 0){
            flag = 0;
        } else {
            while(ls_node -> Brother != NULL){
                ls_node = ls_node -> Brother;
                if(strcmp(ls_node -> filename, ch) == 0){
                    flag = 0;
                    break;
                }
            }
        }
        if(flag){
            SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_RED);
            cout << "no folder named [" << ch << "] exit!";
            SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
        } else if(ls_node -> lock == 1 && strcmp(ls_node -> createAuthor, username) != 0){
            SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_RED);
            cout << "You have no power to read!";
            SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
    	} else {
            cout << "filename:\t\t" << ls_node -> filename << endl;
            cout << "createtime:\t\t" << ls_node -> time << endl;
            cout << "Author:\t\t\t" << ls_node -> createAuthor << endl;
            cout << "lock:\t\t\t" << ls_node -> lock << endl;
            if(ls_node -> attribute == '1'){
                cout << "content:\t\t" << ls_node -> content << endl;
            }
        }
    } else {
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_RED);
        cout << "No file exit, please input '1' or '2' to create new file.";
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
    }
}

//写入文件
void writefile(){
    int flag = 1;
    if(i_node -> Child != NULL){
        cout << "please input the filename:";
        char ch[11];
        cin >> ch;
        Node* ls_node=(Node*)malloc(sizeof(Node));
        ls_node = i_node -> Child;
        if(strcmp(ls_node -> filename, ch) == 0 && ls_node -> attribute == '1'){
            flag = 0;
        } else {
            while(ls_node -> Brother != NULL){
                ls_node = ls_node -> Brother;
                if(strcmp(ls_node -> filename, ch) == 0 && ls_node -> attribute == '1'){
                    flag = 0;
                    break;
                }
            }
        }
        if(flag){
            SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_RED);
            cout << "no folder named [" << ch << "] exit!";
            SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
        } else if(ls_node -> lock == 1 && strcmp(ls_node -> createAuthor, username) != 0){
            SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_RED);
            cout << "You have no power to write!";
            SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
    	} else {
            cout << "please input the file content:";
            cin >> ls_node -> content;
        }
    } else {
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_RED);
        cout << "No file exit, please input '2' to create new file.";
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
    }
}

//文件上锁
void lockfile(){
    int flag = 1;
    if(i_node -> Child != NULL){
        cout << "please input the filename:";
        char ch[11];
        cin >> ch;
        Node* ls_node=(Node*)malloc(sizeof(Node));
        ls_node = i_node -> Child;
        if(strcmp(ls_node -> filename, ch) == 0){
            flag = 0;
        } else {
            while(ls_node -> Brother != NULL){
                ls_node = ls_node -> Brother;
                if(strcmp(ls_node -> filename, ch) == 0){
                    flag = 0;
                    break;
                }
            }
        }
        if(flag){
            SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_RED);
            cout << "no folder named [" << ch << "] exit!";
            SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
        } else {
            ls_node -> lock = 1;
        }
    } else {
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_RED);
        cout << "No file exit, please input '2' to create new file.";
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
    }
}

//打印输入提示
void printTip(){
    system("cls");
    cout << "*************Conmand List***************" << endl;
    cout << "*                                      *" << endl;
    cout << "*  0:back to previous list             *" << endl;
    cout << "*  1:create a new folder               *" << endl;
    cout << "*  2:create a new file                 *" << endl;
    cout << "*  3:turn to next list                 *" << endl;
    cout << "*  4:delete file/folder                *" << endl;
    cout << "*  5:rename                            *" << endl;
    cout << "*  6:show file infomation              *" << endl;
    cout << "*  7:write                             *" << endl;
    cout << "*  8:lock file                         *" << endl;
    cout << "*  9:log out                           *" << endl;
    cout << "*                                      *" << endl;
    cout << "****************************************" << endl << endl;
    cout << "***********Current Directory************" << endl;
    cout << "*  "; printcurDir();
    cout << "****************************************" << endl << endl;
    cout << "*************File Content***************" << endl;
    cout << "*  " << endl << "*  "; showDir();
    cout << "*  " << endl;
    cout << "****************************************" << endl << endl;
    cout << username <<", Please input the conmand:";
}

//登录
void login(){
	system("cls");
	cout << "Please enter your name:";
    cin >> username;
}

void choose(char choise){
    switch(choise){
        case '0':                     //回到上级
            cdreturn();
            break;
        case '1':                     //新建文件夹
            newFolder();
            break;
        case '2':                     //新建文件
            newFile();
            break;
        case '3':                     //转到某目录
            cdnext();
            break;
        case '4':                     //删除文件(夹)
            deleteFile();
            break;
        case '5':                     //修改文件名
            rename();               
            break;
        case '6':                     //查看文件信息
            infoFile();
            break;
        case '7':                     //写入文件
            writefile();
            break;
        case '8':                     //文件上锁
            lockfile();
            break;
        case '9':				      //退出登录
        	login();
        	break;
        default:
            SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_RED);
            cout << "Wrong input, please input right choise!";
            SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
            break;
    }
}

int main(){
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
    login();
    initialization();
    i_node = node;
    char choise;
    while(1){
        printTip();
        cin >> choise;
        choose(choise);
        if(choise != '9' && choise != '0'){
        	cout << endl <<"Enter any key to continue...";
        	getch();
        }
    }

}