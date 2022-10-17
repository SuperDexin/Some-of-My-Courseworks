#include "bingrid.h"
#define BOARDSTR (MAX*MAX+1)
bool is_squared(int a);
bool pairs(char line[MAX], int sz);
bool oxo(char line[MAX], int sz);
bool counting(char line[MAX], int sz);
bool operate(char line[MAX], int sz);
bool check_full(board *brd);

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
    printf("The question is in %dx%d:\n", sz, sz);
    printboard(brd);
    do{
        int i, j;
        op = false;
        for (i = 0; i< sz; i++){
            op = op || operate(b2d[i], sz);
        }
        for (i = 0; i< sz; i++){
            for (j = 0; j< sz; j++){
                col[j] = b2d[j][i];
            }
            op = op || operate(col, sz);
            for (j = 0; j< sz; j++){
                 b2d[j][i] = col[j];
            }
        }    
    } while (op); // When op is false, it means no operation can be done.
    a = check_full(brd);
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

bool pairs(char line[MAX], int sz){
    char last = line[0], cur;
    bool result = false;
    for (int i = 1; i < sz; i++){
        cur = line[i];
        if (cur == last && cur != '.'){
            if (i > 1 && line[i-2] == '.'){
                line[i-2] = (cur == '0') ? '1' : '0';
                result = true;
            }
            if (i < sz - 1 && line[i+1] == '.'){
                line[i+1] = (cur == '0') ? '1' : '0';
                result = true;
            }
        }
        last = cur;
    }
    return result;
}

bool oxo(char line[MAX], int sz){
    bool result = false;
    for (int i = 1; i < sz - 1; i++){
        if (line[i-1] == line[i+1] && line[i] == '.' && line[i-1] != '.'){
             line[i] = (line[i+1] == '0') ? '1' : '0';
             result = true;
        }
    }
    return result;
}

bool counting(char line[MAX], int sz){
    int count_1 = 0, count_0 = 0;
    int full = sz / 2;
    for (int i = 0; i < sz; i++){
        switch (line[i]){
            case '0': count_0 ++; break;
            case '1': count_1 ++; break;
        }
    }
    if ((count_1 != full && count_0 != full) || (count_1 == full && count_0 == full)){
        return false;
    }
    char c = (count_1 == full) ? '0' : '1';
    for (int i = 0; i < sz; i++){
        line[i] = (line[i] == '.') ? c : line[i];
    }
    return true;
}

bool operate(char line[MAX], int sz){
    return pairs(line, sz) || oxo(line, sz) || counting(line, sz);
}

bool check_full(board *brd){
    int sz = brd -> sz;
    for (int i = 0; i < sz; i++){
        for (int j = 0; j < sz; j++){
            if (brd -> b2d[i][j] == '.'){
                return false;
            }
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
    
    //void board2str(char* str, board* brd)
    board b;
    char str[BOARDSTR];
    assert(str2board(&b, "..10") == true);
    assert(b.sz == 2);
    board2str(str, &b);
    assert(strlen(str) == 4);
    assert(strcmp(str, "..10") == 0);

    //bool pairs(char line[MAX], int sz)
    char str2[] = "..00";
    assert(pairs(str2, 4) == true);
    assert(strcmp(str2, ".100") == 0);
    strcpy(str2, "11..");
    assert(pairs(str2, 4) == true);
    assert(strcmp(str2, "110.") == 0);
    strcpy(str2, "011.");
    assert(pairs(str2, 4) == true);
    assert(strcmp(str2, "0110") == 0);
    assert(pairs("0011", 4) == false);
    assert(pairs("....", 4) == false);
    assert(pairs("1..0", 4) == false);
    assert(pairs(".01.", 4) == false);
    
    //bool oxo(char line[MAX], int sz)
    strcpy(str2, "1.1.");
    assert(oxo(str2, 4) == true);
    assert(strcmp(str2, "101.") == 0);
    strcpy(str2, ".0.0");
    assert(oxo(str2, 4) == true);
    assert(strcmp(str2, ".010") == 0);
    assert(oxo("0000", 4) == false);
    assert(oxo("....", 4) == false);
    assert(oxo(".0.1", 4) == false);
    assert(oxo(".11.", 4) == false);
    
    //bool counting(char line[MAX], int sz)
    strcpy(str2, "110.");
    assert(counting(str2, 4) == true);
    assert(strcmp(str2, "1100") == 0);
    strcpy(str2, "101.");
    assert(counting(str2, 4) == true);
    assert(strcmp(str2, "1010") == 0);
    strcpy(str2, "1.1.");
    assert(counting(str2, 4) == true);
    assert(strcmp(str2, "1010") == 0);
    strcpy(str2, "..00");
    assert(counting(str2, 4) == true);
    assert(strcmp(str2, "1100") == 0);
    strcpy(str2, "0.");
    assert(counting(str2, 2) == true);
    assert(strcmp(str2, "01") == 0);
    strcpy(str2, ".0");
    assert(counting(str2, 2) == true);
    assert(strcmp(str2, "10") == 0);
    assert(counting("0110", 4) == false);
    assert(counting("....", 4) == false);
    assert(counting(".01.", 4) == false); 
    
    // Solvable 14x14 Board
   assert(str2board(&b, "0....0...0.0.0.0.1.......1.01..1..0............1.1.01........1...0..1.0.0...0...00...01.1................1...0..0.......0.0..00..1.0.1...........0.11..1.....0....1.1.1...1.0.0........0.......0...."));
   assert(solve_board(&b)==true);
}

