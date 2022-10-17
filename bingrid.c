#include "bingrid.h"
#define BOARDSTR (MAX*MAX+1)

typedef struct line_details{
    int zeros[MAX]; // Zeros' num in per line
    int ones[MAX]; // Ones' num in per line
    int dots[MAX]; // Dots' num in per line
    bool full[MAX]; // Store 1 when a line is full
} line_details;

typedef struct board_details {
    line_details r_dtls;
    line_details c_dtls;
} board_details;

void printboard(board* brd);
bool is_squared(int a);
bool pairs(char line[MAX], int sz, line_details *l_dtls, int l);
bool oxo(char line[MAX], int sz, line_details *l_dtls, int l);
bool counting(char line[MAX], int sz, line_details *l_dtls, int l);
bool operate(char line[MAX], int sz, line_details *l_dtls, int l);
void generate_details(board *brd, board_details *brd_dtls);
bool check_full(board_details *brd_dtls, int sz);

bool str2board(board* brd, char* str)
{
    int len = strlen(str);
    if (!is_squared(len) || len == 0){
        return false;
    }
    int sz = (int)sqrt((double)len);
    if (sz % 2 == 1){
        return false;
    }
    brd -> sz = sz;
    for (int i = 0; i < sz; i++){
        for (int j = 0; j < sz; j++){
            brd -> b2d[i][j] = str[i * sz + j];
        }
    }
    return true;
}

void board2str(char* str, board* brd)
{
    int sz = brd -> sz;
    char *s = str;
    for (int i = 0; i < sz; i++){
        for (int j = 0; j < sz; j++){
            *s = brd -> b2d[i][j];
            s ++;
        }
    }
    *s = '\0';
}

bool solve_board(board* brd)
{
    bool op; // It is to record if there is any operations in a loop
    int sz = brd -> sz;
    char (*b2d)[MAX] = &(brd -> b2d);
    char col[MAX];
    bool a;
    board_details brd_dtls = {
        .r_dtls = {
            {0},
            {0},
            {0},
            {0} 
        },
        .c_dtls = {
            {0},
            {0},
            {0},
            {0} 
        }
    };
    generate_details(brd, &brd_dtls);
    printf("The question is in %dx%d:\n", sz, sz);
    printboard(brd);
    do{
        int i, j;
        op = false;
        for (i = 0; i< sz; i++){
            op = op || operate(b2d[i], sz, &(brd_dtls.r_dtls), i);
        }
        for (i = 0; i< sz; i++){
            for (j = 0; j< sz; j++){
                col[j] = b2d[j][i];
            }
            op = op || operate(col, sz, &(brd_dtls.c_dtls), i);
            for (j = 0; j< sz; j++){
                 b2d[j][i] = col[j];
            }
        }
        printboard(brd);
        printf("\n");
    } while (op); // When op is false, it means no operation can be done.
    a = check_full(&brd_dtls, sz);
    printf("It is%ssolved. ", a ? " " : " not ");
    printf("Final result:\n");
    printboard(brd);
    printf("\n");
    return a;
}

void printboard(board* brd)
{
    int sz = brd -> sz;
    for (int i = 0; i < sz; i++){
        for (int j = 0; j < sz; j++){
            printf("%c ", brd -> b2d[i][j]);
        }
        printf("\n");
    }
}

bool is_squared(int a){
    if (a < 0){
        return false;
    }
    for (int i = 0; i < ((int)sqrt((double) a) + 1); i ++){
        if (i * i == a){
            return true;
        }
    }
    return false;
}

bool pairs(char line[MAX], int sz, line_details *l_dtls, int l){
    if (l_dtls -> full[l]){
        return false;
    }
    char last = line[0], cur;
    bool result = false;
    for (int i = 1; i < sz; i++){
        cur = line[i];
        if (cur == last && cur != '.'){
            if (i > 1 && line[i-2] == '.'){
                if (cur == '0'){
                    line[i-2] = '1';
                    l_dtls -> ones[l] ++;
                }else if (cur == '1'){
                    line[i-2] = '0';
                    l_dtls -> zeros[l] ++;
                }
                if (--(l_dtls -> dots[l]) == 0){
                    l_dtls -> full[l] = true;
                }
                result = true;
            }
            if (i < sz - 1 && line[i+1] == '.'){
                if (cur == '0'){
                    line[i+1] = '1';
                    l_dtls -> ones[l] ++;
                }else if (cur == '1'){
                    line[i+1] = '0';
                    l_dtls -> zeros[l] ++;
                }
                if (--(l_dtls -> dots[l]) == 0){
                    l_dtls -> full[l] = true;
                }
                result = true;
            }
        }
        last = cur;
    }
    return result;
}

bool oxo(char line[MAX], int sz, line_details *l_dtls, int l){
    if (l_dtls -> full[l]){
        return false;
    }
    bool result = false;
    for (int i = 1; i < sz - 1; i++){
        if (line[i-1] == line[i+1] && line[i] == '.' && line[i-1] != '.'){
            if (line[i+1] == '0'){
                line[i] = '1';
                l_dtls -> ones[l] ++;
            } else if (line[i+1] == '1'){
                line[i] = '0';
                l_dtls -> zeros[l] ++;
            }
            if (--(l_dtls -> dots[l]) == 0){
                l_dtls -> full[l] = true;
            }
            result = true;
        }
    }
    return result;
}

bool counting(char line[MAX], int sz, line_details *l_dtls, int l){
    int count_1 = l_dtls -> ones[l];
    int count_0 = l_dtls -> zeros[l];
    int full = sz / 2;
    
    if ((count_1 != full && count_0 != full) || (count_1 == full && count_0 == full)){
        return false;
    }
    char c = (count_1 == full) ? '0' : '1';
    for (int i = 0; i < sz; i++){
        line[i] = (line[i] == '.') ? c : line[i];
    }
    l_dtls -> ones[l] = full;
    l_dtls -> zeros[l] = full;
    l_dtls -> dots[l] = 0;
    l_dtls -> full[l] = true;
    return true;
}

bool operate(char line[MAX], int sz, line_details *l_dtls, int l){
    return     pairs(line, sz, l_dtls, l)
            || oxo(line, sz, l_dtls, l)
            || counting(line, sz, l_dtls, l);
}

void generate_details(board *brd, board_details *brd_dtls){
    int sz = brd -> sz;
    char (*b2d)[MAX] = &(brd -> b2d);
    line_details *r_dtls = &(brd_dtls -> r_dtls);
    line_details *c_dtls = &(brd_dtls -> c_dtls);

    for (int i = 0; i< sz; i++){
        for (int j = 0; j< sz; j++){
            if (b2d[i][j] == '0'){
                r_dtls -> zeros[i] ++;
                c_dtls -> zeros[j] ++;
            } else if (b2d[i][j] == '1'){
                r_dtls -> ones[i] ++;
                c_dtls -> ones[j] ++;
            } else if (b2d[i][j] == '.'){
                r_dtls -> dots[i] ++;
                c_dtls -> dots[j] ++;
            }
        }
    }
}

bool check_full(board_details *brd_dtls, int sz){
    for (int i = 0; i < sz; i++){
        if(!(brd_dtls -> r_dtls.full[i])){
            return false;
        }
    }
    return true;
}

void test(void)
{
    //bool is_squared(int a)
    assert(is_squared(0) == true);
    assert(is_squared(-1) == false);
    assert(is_squared(1) == true);
    assert(is_squared(2) == false);
    assert(is_squared(3) == false);
    assert(is_squared(16) == true);
    
    board b;
    board_details brd_dtls = {
        .r_dtls = {
            {0},
            {0},
            {0},
            {0} 
        },
        .c_dtls = {
            {0},
            {0},
            {0},
            {0} 
        }
    };
    generate_details(&b, &brd_dtls);
    
    //void board2str(char* str, board* brd)
    char str[BOARDSTR];
    assert(str2board(&b, "..10") == true);
    assert(b.sz == 2);
    board2str(str, &b);
    assert(strlen(str) == 4);
    assert(strcmp(str, "..10") == 0);
    
    line_details a = {
            {0},
            {0},
            {0},
            {0} 
    };
    
    line_details bd = a;
    printf("1234\n");
    //bool pairs(char line[MAX], int sz)
    char str2[] = "..00";
    assert(pairs(str2, 4, &a, 0) == true);
    assert(strcmp(str2, ".100") == 0);
    strcpy(str2, "11..");
    a = bd;
    assert(pairs(str2, 4, &a, 0) == true);
    assert(strcmp(str2, "110.") == 0);
    strcpy(str2, "011.");
    a = bd;
    assert(pairs(str2, 4, &a, 0) == true);
    assert(strcmp(str2, "0110") == 0);
    a = bd;
    assert(pairs("0011", 4, &a, 0) == false);
    assert(pairs("....", 4, &a, 0) == false);
    assert(pairs("1..0", 4, &a, 0) == false);
    assert(pairs(".01.", 4, &a, 0) == false);

    //bool oxo(char line[MAX], int sz)
    a = bd;
    strcpy(str2, "1.1.");
    assert(oxo(str2, 4, &a, 0) == true);
    assert(strcmp(str2, "101.") == 0);
    a = bd;
    strcpy(str2, ".0.0");
    assert(oxo(str2, 4, &a, 0) == true);
    assert(strcmp(str2, ".010") == 0);
    a = bd;
    assert(oxo("0000", 4, &a, 0) == false);
    assert(oxo("....", 4, &a, 0) == false);
    assert(oxo(".0.1", 4, &a, 0) == false);
    assert(oxo(".11.", 4, &a, 0) == false);
    
    //bool counting(char line[MAX], int sz)
    a = (line_details){
            {2},
            {1},
            {1},
            {0} 
    };
    strcpy(str2, "110.");
    assert(counting(str2, 4, &a, 0) == true);
    assert(strcmp(str2, "1100") == 0);
    a = (line_details){
            {2},
            {1},
            {1},
            {0} 
    };
    strcpy(str2, "101.");
    assert(counting(str2, 4, &a, 0) == true);
    assert(strcmp(str2, "1010") == 0);
    a = (line_details){
            {2},
            {0},
            {2},
            {0} 
    };
    strcpy(str2, "1.1.");
    assert(counting(str2, 4, &a, 0) == true);
    assert(strcmp(str2, "1010") == 0);
    a = (line_details){
            {0},
            {2},
            {2},
            {0} 
    };
    strcpy(str2, "..00");
    assert(counting(str2, 4, &a, 0) == true);
    assert(strcmp(str2, "1100") == 0);
    a = (line_details){
            {2},
            {2},
            {0},
            {1} 
    };
    assert(counting("0110", 4, &a, 0) == false);
    a = (line_details){
            {0},
            {0},
            {4},
            {0} 
    };
    assert(counting("....", 4, &a, 0) == false);
    a = (line_details){
            {1},
            {1},
            {2},
            {0} 
    };
    assert(counting(".01.", 4, &a, 0) == false); 
    
    // Solvable 14x14 Board
    printf("This is my test board:\n");
    assert(str2board(&b, "0....0...0.0.0.0.1.......1.01..1..0............1.1.01........1...0..1.0.0...0...00...01.1................1...0..0.......0.0..00..1.0.1...........0.11..1.....0....1.1.1...1.0.0........0.......0...."));
    assert(solve_board(&b)==true);
}

