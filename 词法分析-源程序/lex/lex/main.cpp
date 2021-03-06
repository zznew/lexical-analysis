#include <ctype.h>
#include <cstdio>
#include <iostream>
#include <cstring>
#include <map>
#include <vector>
using std::endl;
using std::cout;
using std::cin;

#define SUCCESS 1
#define FAIL 0

// 保留字数组
static char reserveWord[32][20]{
 "auto",       "break",    "case",    "char",        "const",      "continue",
 "default",   "do",       "double",   "else",        "enum",       "extern",
 "float",      "for",      "goto",     "if",          "int",        "long",
 "register",   "return",   "short",    "signed",     "sizeof",     "static",
 "struct",     "switch",   "typedef",  "union",      "unsigned",   "void",
 "volatile",    "while"
};

//// 运算符数组
//static char operatorWord[20][10] {
//      "+", "-", "*", "/","%",
//      "++","--", "<", "<=", ">",
//      ">=","=", "==","!=", "&",
//      "&&","|", "||",  "<<", ">>",
//};

// 分隔符数组
static char spliterWord[14][10] = {
       ";", "(", ")", "^", ",",
       "\"", "\'", "#","[","]",
       "{", "}", "\\", "."
 };

typedef struct{
    std::string term = "";
    int kind = -1;
}Token;

// 用于存储词法分析所得的Token
std::vector<Token> Tokens;

inline void setToken(Token&  token, std::string term, int kind){
    token.term = term;
    token.kind = kind;
}

// 过滤单行注释和多行注释
void filterResource(const char r[], const int pProject, char * filtered, int& cnt);

// 匹配保留字
int searchRsv(const char* token, const char reserveWord[][20]);

// 匹配分隔符
int searchSpliter(const char * token, const char spliterWord[][10]);

// 匹配运算符
int searchOperator(const char * token, const char operatorWord[][10]);

// 扫描器/词法分析器
void Scanner(int & syn, const char r[], char * token, int & pProject);

// 调用词法分析模块
void yylex();

int main()
{
    yylex();
    return 0;
}

void filterResource(const char r[], const int pProject, char * filtered, int& cnt){
    char tempString[10000];
    for(int i = 0; i <= pProject; ++ i){
        if(r[i] == '/' && r[i + 1] == '/'){             // 出现单行注释，跳过注释内容
            while(r[i] != '\n'){
                ++ i;                                   // 继续向后扫描，直到出现换行符号
            }
        }

        if(r[i] == '/' && r[i+1] == '*'){               // 出现多行注释，跳过注释内容
            i += 2;
            while(r[i] != '*' && r[i + 1] != '/'){
                    ++ i;
                if(r[i] == '$'){
                    cout << "多行注释缺少 */， 程序出错" << endl;
                    exit(0);
                }
            }
            i += 2;
        }

        if(r[i] != '\r' && r[i] != '\n' && r[i] != '\t'){
            tempString[cnt++] = r[i];                   // 将有效字符拷贝到新的字符串中
        }
    }

    tempString[cnt] = '\0';
    strcpy(filtered, tempString);
}

int searchRsv(const char * token, const char reserveWord[][20]){
    for(int i = 0; i < 32; ++ i){
        if(strcmp(token, reserveWord[i]) == 0){
            return i + 1;
        }
    }
    return -1;
}

int searchSpliter(const char * token, const char spliterWord[][10]){
    for(int i = 0; i < 14; ++ i){
        if(strcmp(token, spliterWord[i]) == 0){
            return i + 53;
        }
    }
    return -1;
}

int searchOperator(const char * token, const char operatorWord[][10]){
    for(int i = 0; i < 20; ++ i){
        if(strcmp(token, spliterWord[i]) == 0){
            return i + 33;
        }
    }
    return -1;
}

void Scanner(int & syn, const char r[], char * token, int & pProject){
    //根据DFA的状态转换图设计
    int i;
    int j = 0;      //  用作token 的指示器
    while(r[pProject] == ' '){
        pProject ++;
    }

    for (i = 0; i<20; i++)
    {//每次收集前先清零
        token[i] = '\0';
    }

    // 开头为字母或下划线
    if(isalpha(r[pProject]) || r[pProject] == '_'){
        token[j++] = r[pProject];          // 收集
        pProject++;                             // 下移
        while(isalnum(r[pProject]) || r[pProject] == '_'){
            // 后跟字母或下划线
            token[j++] = r[pProject];      // 收集
            pProject++;                         // 下移
        }

        token[j] = '\0';

        syn =  searchRsv(token, reserveWord);

        // 不是关键字即为标识符
        if(syn == -1){
            syn = 100;
        }
        return;
    }

    // 首字符为数字
    else if(isdigit(r[pProject])){
        token[j++] = r[pProject];               // 收集
        pProject ++;                            // 下移
        while(isdigit(r[pProject]) || r[pProject] == '.'){
            token[j++] = r[pProject];           // 收集
            pProject ++;                        // 下移
        }

        token[j] = '\0';

        // 抵达接受状态
        if(isdigit(token[j-1])){
            syn = 99;
        }

        return;
    }

    // + | ++
    else if(r[pProject] == '+'){
        token[j++] = r[pProject];
        pProject ++;

        if(r[pProject] == '+'){
            token[j++] = r[pProject];
            syn = 38;
        }

        else{
            syn = 33;
            pProject --;
        }

        token[j] = '\0';
        pProject ++;
        return;
    }

    // - | --
    else if(r[pProject] == '-'){
        token[j++] = r[pProject];
        pProject ++;

        if(r[pProject] == '-'){
            token[j++] = r[pProject];
            syn = 39;
        }

        else{
            syn = 34;
            pProject --;
        }

        token[j] = '\0';
        pProject ++;
        return;
    }

    //  = | ==
    else if(r[pProject] == '='){
        token[j++] = r[pProject];
        pProject ++;

        if(r[pProject] == '='){
            token[j++] = r[pProject];
            syn = 45;
        }

        else{
            syn = 44;
            pProject --;
        }

        token[j] = '\0';
        pProject ++;
        return;
    }

    else if(r[pProject] == '<'){
        token[j++] = r[pProject];
        pProject ++;

        if(r[pProject] == '<'){
            token[j++] = r[pProject];
            syn = 51;
        }

        else if(r[pProject] == '='){
            token[j++] = r[pProject];
            syn = 41;
        }

        else{
            syn = 40;
            pProject --;
        }

        token[j] = '\0';
        pProject ++;
        return;
    }

    else if(r[pProject] == '>'){
        token[j++] = r[pProject];
        pProject ++;

        if(r[pProject] == '>'){
            token[j++] = r[pProject];
            syn = 52;
        }

        else if(r[pProject] == '='){
            token[j++] = r[pProject];
            syn = 43;
        }

        else{
            syn = 42;
            pProject --;
        }

        token[j] = '\0';
        pProject ++;
        return;
    }

    else if(r[pProject] == '!'){
        token[j++] = r[pProject];
        pProject ++;

        if(r[pProject] == '='){
            token[j++] = r[pProject];
            syn = 46;
        }

        else{
            pProject --;
        }

        token[j] = '\0';
        pProject ++;
        return;
    }

    else if(r[pProject] == '&'){
        token[j++] = r[pProject];
        pProject ++;

        if(r[pProject] == '&'){
            token[j++] = r[pProject];
            syn = 48;
        }

        else{
            syn = 47;
            pProject --;
        }

        token[j] = '\0';
        pProject ++;
        return;
    }

    else if(r[pProject] == '|'){
        token[j++] = r[pProject];
        pProject ++;

        if(r[pProject] == '|'){
            token[j++] = r[pProject];
            syn = 50;
        }

        else{
            syn = 49;
            pProject --;
        }

        token[j] = '\0';
        pProject ++;
        return;
    }

    else if(r[pProject] == '*'){

            token[j++] = r[pProject];
            token[j] = '\0';
            pProject ++;
            syn = 35;
            return;
    }

    else if(r[pProject] == '/'){

            token[j++] = r[pProject];
            token[j] = '\0';
            pProject ++;
            syn = 36;
            return;
    }

    else if(r[pProject] == '%'){

            token[j++] = r[pProject];
            token[j] = '\0';
            pProject ++;
            syn = 37;
            return;
    }



    // 分隔符
    else if(r[pProject] == ';' || r[pProject] == '(' || r[pProject] == ')' || r[pProject] == '^' ||
        r[pProject] == ',' || r[pProject] == '\"' || r[pProject] == '\'' || r[pProject] == '#'  ||
        r[pProject] == '['  || r[pProject] == ']' || r[pProject] == '{' || r[pProject] == '}' ||
        r[pProject] == '\\' || r[pProject] == '.'){

        token[j++] = r[pProject];
        pProject ++;
        token[j] = '\0';
        syn = searchSpliter(token, spliterWord);
    }


    else if(r[pProject] == '$'){
        syn = 0;
    }

    else{
        printf("Can't find the symbol %c, the pointer proceed\n", r[pProject]);
        pProject++;
    }
}

void yylex()
{
    // 打开资源文件，读取其中的源程序
    int syn = -1;
    char resourceProject[10000];
    char token[20] = {0};
    int pProject = 0;
    FILE *fp;
    FILE *fp1;
    if((fp = fopen("source.txt", "r")) == NULL){
        cout << "打开源文件失败" << endl;
        exit(0);
    }
    resourceProject[pProject] = fgetc(fp);
    while(resourceProject[pProject] != EOF){
        pProject++;
        resourceProject[pProject] = fgetc(fp);
    }

    resourceProject[++pProject] = '\0';
    fclose(fp);

    cout << "\n源程序为：\n";
    cout << resourceProject << endl;

    if((fp1 = fopen("target.txt", "w+"))== NULL){
        cout << "打开目标文件失败" << endl;
        exit(0);
    }

    char filtered[10000] = {0};
    int cnt = 0;

    filterResource(resourceProject, pProject, filtered, cnt);

    cout << "\n过滤后的源程序为：" << endl;
    cout << filtered << endl;
    fprintf(fp1, filtered);
    cout << "字符数：" << cnt << endl << endl;
    fprintf(fp1, "\n字符数：%d\n\n", cnt);

    pProject = 0;

    while(syn != 0){
        int kind = -1;
        int syn = -1;
        Scanner(syn, filtered, token, pProject);

        if(syn == -1){
            continue;
        }

        if(syn == 0){
            break;
        }

        if(syn == 100){
            // 标识符
            kind = 2;
            printf("( \" %s\" , %d )\n", token, kind);
            fprintf(fp1, "( \" %s\" , %d )\n", token, kind);
            Token current;
            setToken(current, token, kind);
            Tokens.push_back(current);
        }

        if(syn >=1 && syn <= 32){
            // 关键字
            kind = 1;
            printf("( \" %s\" , %d )\n", token, kind);
            fprintf(fp1, "( \" %s\" , %d )\n", token, kind);
            Token current;
            setToken(current, token, kind);
            Tokens.push_back(current);
        }

        if(syn == 99){
            // 常数
            kind = 3;
            printf("( \" %s\" , %d )\n", token, kind);
            fprintf(fp1, "( \" %s\" , %d )\n", token, kind);
            Token current;
            setToken(current, token, kind);
            Tokens.push_back(current);
        }

        if(syn >= 33 && syn <= 52){
            // 运算符
            kind = 4;
            printf("( \" %s\" , %d )\n", token, kind);
            fprintf(fp1, "( \" %s\" , %d )\n", token, kind);
            Token current;
            setToken(current, token, kind);
            Tokens.push_back(current);
        }

        if(syn >= 53 && syn <= 66){
            // 分隔符
            kind = 5;
            printf("( \" %s\" , %d )\n", token, kind);
            fprintf(fp1, "( \" %s\" , %d )\n", token, kind);
            Token current;
            setToken(current, token, kind);
            Tokens.push_back(current);
        }
    }
    fclose(fp1);
}
