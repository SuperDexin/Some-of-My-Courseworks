#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include <assert.h>
#include <unistd.h>

// Maximum carpark size is 20x20
#define MAX         20

// Maximum cars quantity in a line
/* Assume every car is in 2 blocks size  
      (MAX - 2) / 2
    = 9                         */
#define CAR_MAX_LINE 9

// Maxinum cars quantity in a grid
/*    (MAX - 2) * CAR_MAX_LINE
    = 18 * 9
    = 162                       */
#define CAR_MAX     162

// The limitation of the park list
#define LIST_MAX    1500

#define GAP         '.'
#define BOL         '#'

// Horizontal
#define HORI        0
// Vertical
#define VERT        1

#define HEAD        0
#define TAIL        1

// Negative
#define NEGA        0 
// Positive
#define POSI        1 

/* ERROR codes */
// No error
#define ERR_NO_ERR      0
// Other errors
#define ERR_OTHER       1
// Open file error   
#define ERR_OPEN_FILE   2
// Wrong shape
#define ERR_SHAPE       3
// Empty file error
#define ERR_EMPTY       4
// Invalid contents in the file
#define ERR_INVALID     5
// Invalid grid
#define ERR_INV_GRID    6
// No answer of the grid
#define ERR_NO_ANS      7
// No gate in the carpark
#define ERR_NO_GATE     8
// No car in the carpark
#define ERR_NO_CAR      9


typedef struct car {
    // The coordinate when first time the car is scanned.
    // It can be used to figure out 'loc' and 'dir'.
    int first_loc[2];

    //  It is which row/col the car is in.
    int loc;

    // The length of the car.
    int len;

    // The location of the head of the car.
    int head;

    // The location of the tail of the car. 
    // It must larger than 'head'.
    int tail;

    char name;

    /*  The direction of the car.
        0 or HORI means the car is horizontal.
        1 or VERT means the car is vertical. */
    bool dir;

    bool exist;

} car;

typedef struct carpark {
    /*  The 1st index is the direction.
            It must be HORI or VERT.
            It is related to the 'dir' in 'car'.
        The 2nd index is which row/col the car is.
            It is related to the 'loc' in 'car'.
        The 3rd index is the cars exist in a line
        at the same time. 
            It is related to the 'car_num_array'.
        'cars[][][]' should be used as:
            cars[c -> dir][c -> loc][num]
            where num is car_cum_array[c -> dir][c -> loc]*/
    car *cars[2][MAX][CAR_MAX_LINE];
    car *cars_list[CAR_MAX];

    unsigned long long int feature_num;

    /*  The array 'car_num_array' gives the quantity of
        cars in each line.
        The 1st index is the direction,
            It must be HORI or VERT.
            It is related to the 'dir' in 'car'.
        The 2nd index is which row/col the car is.
            It is related to the 'loc' in 'car'. 
        'car_num_array[][]' should be used as:
            car_num_array[c -> dir][c -> loc]*/
    int car_num_array[2][MAX];
    int row;
    int col;

    // How many cars in total.
    int car_num;

    // How many cars left in the park.
    int car_exist;
    int gate_num;

    /*  The array 'gates' stores if there is a gate at
        a certain location.
        The 1st index is the direction,
            It must be HORI or VERT.
        The 2nd index is which row/col the gate is.
        The 3rd index is whether the gate is at the head
        or the tail of a line. */
    bool gates[2][MAX][2];

} carpark;

/*  Because the rule says no link list can be used, the
    struct 'carpark_node' will be used in an array.*/
typedef struct carpark_node {
    carpark* cp;

    // The indexes of the frontier carparks in the carpark list
    int frontiers[LIST_MAX];

    int order;
} carpark_node;


void test();

void print_error(const int a, const int line, const char *s);
FILE *check_file (char *file_name);

car *new_car (const int first_loc[2], const char name);
carpark *new_carpark (const int row, const int col);
carpark_node *new_carpark_node (carpark *cp, int *frontiers, int order);

int move(car *c, bool mv_dir);

carpark *make_carpark (char *file_name);
void set_gate(FILE *fp, carpark *cp, const bool dir, const int loc, const bool part, const char *file_name);
void set_car (carpark *cp, const char name, const int row, const int col, const char *file_name);
void set_new_car (carpark *cp, const char name, const int row, const int col);
void set_halfnew_car (carpark *cp, car *c, const int row, const int col, const char *file_name);
void set_old_car (car *c, const int row, const int col, const char *file_name);

char **show (carpark *cp, const bool print_flag);
carpark *carpark_copy(carpark *cp);
car *car_copy(car *c);
void add_car(carpark *cp, car *c);
bool carpark_compare(carpark *cp1, carpark *cp2);
void get_feature_num (carpark *cp);

void solve_grid (carpark *cp, const char *file_name);
bool try_move (carpark *cp, car *c, const bool dir);
bool check_car_conflict (car *c1, car *c2, const int loc);
bool make_carpark_list (carpark_node **list, carpark_node *cpn, const int node_num, int *list_len);
bool repeated_carpark (carpark_node **list, carpark *cp, const int list_len);

void print_cars (carpark *cp);

int main(int argc, char const *argv[])
{
    test();
    return 0;
}

void test(){
    int loc[2] = {4, 5};
    car *c = new_car(loc, 'A');
    assert(c -> first_loc[0] == 4);
    assert(c -> name == 'A');
    assert(c -> len == 0);
    c -> head = 3;
    c -> tail = 4;
    c -> dir = VERT;
    c -> loc = 2;
    assert(move(c, POSI) == 5);
    assert(c -> tail == 5);
    assert(c -> head == 4);
    assert(move(c, NEGA) == 3);
    assert(c -> tail == 4);
    assert(c -> head == 3);

    carpark *cp = new_carpark(6, 7);

    assert(cp -> row == 6);
    assert(cp -> col == 7);
    assert(cp -> car_num_array[HORI][2] == 0);
    assert(cp -> car_num_array[VERT][3] == 0);
    assert(cp -> car_num == 0);
    assert(cp -> car_num_array[c -> dir][c -> loc] == 0);

    add_car(cp, c);
    assert(cp -> car_num == 1);
    assert(cp -> car_num_array[c -> dir][c -> loc] == 1);
    assert(cp -> cars[c -> dir][c -> loc][0] == c);
    assert(cp -> cars_list[0] == c);

    get_feature_num(cp);
    carpark *cp2 = carpark_copy(cp);
    assert(carpark_compare(cp, cp2) == 1);
    assert(cp2 -> car_num_array[VERT][3] == 0);

    move(cp2 -> cars_list[0], NEGA);
    get_feature_num(cp2);
    assert(carpark_compare(cp, cp2) == 0);

    move(cp2 -> cars_list[0], POSI);
    get_feature_num(cp2);
    assert(carpark_compare(cp, cp2) == 1);

    char *file_name = "./6x6_2c_3t.prk";
    // cp = make_carpark(file_name);
    // //show(cp, 1);
    // solve_grid(cp, file_name);
    file_name = "./11x9_10c_none.prk";
    cp = make_carpark(file_name);
    //show(cp, 1);
    solve_grid(cp, file_name);
}

void print_error(const int err_num, const int line, const char *s){
    // ERROR messages
    char* errmsg[] = {
    /* err_num */   /* message */
    /*    0    */   "No error",
    /*    1    */   "Other errors",
    /*    2    */   "Open file error",
    /*    3    */   "Wrong shape error",
    /*    4    */   "Empty file error",
    /*    5    */   "Invalid contents",
    /*    6    */   "Invalid Grid",
    /*    7    */   "No answer of the grid",
    /*    8    */   "No gate in the carpark",
    /*    9    */   "No car in the carpark"
    };
    printf("ERROR in line %d: ", line);
    printf("%s in the file %s\n", errmsg[err_num], s);
    printf("exit(%d)\n", err_num);
    exit(err_num);
}

FILE *check_file (char *file_name){
    FILE *fp = fopen(file_name, "r");
    if(fp == NULL){
        print_error(ERR_OPEN_FILE, __LINE__, file_name);
    }
    if(fgetc(fp) == -1){
        print_error(ERR_EMPTY, __LINE__, file_name);
    }

    fp = fopen(file_name, "r");
    if(fp == NULL){
        print_error(ERR_OPEN_FILE, __LINE__, file_name);
    }
    else{
        return fp;
    }
}

car *new_car (const int first_loc[2], const char name){
    car *c = (car *)malloc(sizeof(car));
    c -> len = 0;
    c -> exist = 1;
    c -> head = 0;
    c -> tail = 0;
    c -> first_loc[0] = first_loc[0];
    c -> first_loc[1] = first_loc[1];
    c -> name = name;
    return c;
}

carpark *new_carpark (const int row, const int col){
    carpark *cp = (carpark *)malloc(sizeof(carpark));
    cp -> row = row;
    cp -> col = col;
    cp -> car_num = 0;
    cp -> car_exist = 0;
    cp -> gate_num = 0;
    cp -> feature_num = 0;
    for (int i = 0; i < 2; i++){
        for (int j = 0; j < MAX; j++){
            for (int k = 0; k < 2; k++){
                cp -> gates[i][j][k] = 0;
            }
        }
    }
    for (int i = 0; i < MAX; i++){
        cp -> car_num_array[HORI][i] = 0;
        cp -> car_num_array[VERT][i] = 0;
    }
    return cp;
}

carpark_node *new_carpark_node (carpark *cp, int *frontiers, int order){
    carpark_node *cpn = (carpark_node *)malloc(sizeof(carpark_node));
    cpn -> cp = cp;
    for (int i = 0; i < order; i++){
        cpn -> frontiers[i] = frontiers[i];
    }
    cpn -> order = order;
    return cpn;
}

int move(car *c, bool mv_dir){
    /* 'mv_dir' is the direction of the movement
        mv_dir = 0 means moving in negative direction.
        mv_dir = 1 means moving in positive direction.*/
    if (mv_dir){
        c -> head ++;
        c -> tail ++;
        return c -> tail;
    }
    else {
        c -> head --;
        c -> tail --;
        return c -> head;
    }
}


carpark *make_carpark (char *file_name){
    FILE *fp = check_file(file_name);

    int row_num, col_num;
    if(fscanf(fp, "%dx%d", &row_num, &col_num) != 2){
        print_error(ERR_INVALID, __LINE__, file_name);
    }

    carpark *cp = new_carpark(row_num, col_num);
    // Scan the first row 
    for (int col = 0; col < col_num; col++){
        set_gate(fp, cp, VERT, col, HEAD, file_name);
    }
    // Scan the middle rows
    for (int row = 1; row < row_num - 1; row++){
        // Scan the first char
        set_gate(fp, cp, HORI, row, HEAD, file_name);
        // Scan the middle chars
        for (int col = 1; col < col_num - 1; col++){
            char c = fgetc(fp);
            if (c >= 'A' && c <= 'Z'){
                set_car(cp, c, row, col, file_name);
            }
            else if (c != GAP || feof(fp)){
                print_error(ERR_INV_GRID, __LINE__, file_name);
            }
        }
        // Scan the last char
        set_gate(fp, cp, HORI, row, TAIL, file_name);
    }
    // Scan the last row
    for (int col = 0; col < col_num; col++){
        set_gate(fp, cp, VERT, col, TAIL, file_name);
    }
    if (cp -> car_num == 0){
        print_error(ERR_NO_CAR, __LINE__, file_name);
    }
    if (cp -> gate_num == 0){
        print_error(ERR_NO_GATE, __LINE__, file_name);
    }
    cp -> car_exist = cp -> car_num;
    return cp;
}

void set_gate (FILE *fp, carpark *cp, const bool dir, const int loc, const bool part, const char *file_name){
    char c;
    do{
        if (feof(fp)){
            print_error(ERR_INV_GRID, __LINE__, file_name);
        }
        c = fgetc(fp);
    }while (c != GAP && c != BOL);
    if (c == GAP){
        cp -> gates[dir][loc][part] = 1;
        cp -> gate_num ++;
    }
}

void set_car (carpark *cp, const char name, const int row, const int col, const char *file_name){
    int index = -1;
    for (int i = 0; i < cp -> car_num; i++){
        if (cp -> cars_list[i] -> name == name){
            index = i;
        }
    }

    // A new car
    if (index == -1){ 
        set_new_car(cp, name, row, col);
    }
    else{
        car *c = cp -> cars_list[index];
        // The second time meet the car
        if (c -> len == 0){
            set_halfnew_car(cp, c, row, col, file_name);
        }
        // Set an old car
        else {
            set_old_car (c, row, col, file_name);
        }
    }
}

void set_new_car (carpark *cp, const char name, const int row, const int col){
    int loc[] = {row, col};
    car *c = new_car(loc, name);
    cp -> cars_list[cp -> car_num] = c;
    cp -> car_num ++;
}

void set_halfnew_car (carpark *cp, car *c, const int row, const int col, const char *file_name){
    int first_r = c -> first_loc[0];
    int first_c = c -> first_loc[1];
    if (first_r == row && first_c == col){
        printf("Repeated coordinate\n");
        print_error(ERR_OTHER, __LINE__, file_name);
    }
    else if (   (first_r != row && first_c != col)
            ||  (first_r == row && first_c != col - 1)
            ||  (first_r != row - 1 && first_c == col))
    {
        print_error(ERR_SHAPE, __LINE__, file_name);
    }
    else if (first_r == row){
        c -> loc = row;
        c -> head = first_c;
        c -> tail = col;
        c -> dir = HORI;
    }
    else if (first_c == col){
        c -> loc = col;
        c -> head = first_r;
        c -> tail = row;
        c -> dir = VERT;
    }
    c -> len ++;
    int *num = &(cp -> car_num_array[c -> dir][c -> loc]);
    cp -> cars[c -> dir][c -> loc][*num] = c;
    (*num) ++;
}

void set_old_car (car *c, const int row, const int col, const char *file_name){
    if (    (c -> dir == HORI 
                && (c -> loc != row || c -> tail != col - 1))
        ||  (c -> dir == VERT 
                && (c -> loc != col || c -> tail != row - 1)))
        {
            print_error(ERR_SHAPE, __LINE__, file_name);
        }
    c -> tail ++;
    c -> len ++;
}

/*  The function 'show()' is not only used to print the grid,
    but also return a 2D char map, which will be used to 
    compare two different carparks.
    Thus there is a var 'print_flag'. If it is 1, the function
    will print the grid to the console, else it will return
    a 2D map.
    To save memories, the function will return NULL and the
    memory space of the map will be freed if 'print_flag' is 1.*/
char** show (carpark *cp, const bool print_flag){
    int row_num = cp -> row;
    int col_num = cp -> col;
    char **map = (char **)malloc(sizeof(char *) * row_num);
    for (int i = 0; i < row_num; i++){
        map[i] = (char *)malloc(sizeof(char) * col_num);
        for (int j = 1; j < col_num; j ++){
            map[i][j] = GAP;
        }
    }
    for (int i = 0; i < col_num; i++){  // Top and bottom walls
        map[0][i] = (cp -> gates[VERT][i][HEAD]) ? GAP : BOL;
        map[row_num - 1][i] = (cp -> gates[VERT][i][TAIL]) ? GAP : BOL;
    }
    for (int i = 0; i < row_num; i++){  // Left and right walls
        map[i][0] = (cp -> gates[HORI][i][HEAD]) ? GAP : BOL;
        map[i][col_num - 1] = (cp -> gates[HORI][i][TAIL]) ? GAP : BOL;
    }
    for (int i = 0; i < cp -> car_num; i ++){ // Centre region
        car *c = cp -> cars_list[i];
        if (c -> exist){
            for (int j = c -> head; j <= c -> tail; j ++){
                char *temp = c -> dir ? &map[j][c -> loc] : &map[c -> loc][j];
                *temp = c -> name;
            }
        }
    }
    if (print_flag){ // Print
        for (int i = 0; i < row_num; i ++){
            for (int j = 0; j < col_num; j ++){
                printf("%c", map[i][j]);
            }
            printf("\n");
        }
        printf("\n");
        free(map);
        return NULL;
    }
    return map;
}

carpark *carpark_copy(carpark *cp){
/*  struct carpark {
    car *cars[2][MAX][CAR_MAX_LINE];
    car *cars_list[CAR_MAX];
    int car_num_array[2][MAX];
    int row;
    int col;
    int car_num;
    int car_exist;
    int gate_num;
    bool gates[2][MAX][2];}*/

    carpark *copy = new_carpark(cp -> row, cp -> col);
    copy -> feature_num = cp -> feature_num;

    for (int i = 0; i < 2; i++){
        for (int j = 0; j < MAX; j++){
            for(int k = 0; k < 2; k++){
                copy -> gates[i][j][k] = cp -> gates[i][j][k];
            }
        }
    }
    for (int i = 0; i < MAX; i++){
        copy -> car_num_array[HORI][i] = 0;
        copy -> car_num_array[VERT][i] = 0;
    }
    for (int i = 0; i < cp -> car_num; i++){
        car *c = car_copy(cp -> cars_list[i]);
        if (c -> exist){
            copy -> car_exist ++;
        }
        add_car(copy, c);
    }
    return copy;
}

car *car_copy (car *c){
    car *copy = (car *)malloc(sizeof(car));
    *copy = *c;
    return copy;
}

void add_car (carpark *cp, car *c){
    int *i = &(cp -> car_num_array[c -> dir][c -> loc]);
    cp -> cars_list[cp -> car_num] = c;
    cp -> cars[c -> dir][c -> loc][*i] = c;
    (*i) ++;
    cp -> car_num ++;
}

bool carpark_compare(carpark *cp1, carpark *cp2){
    if( !(  cp1 -> row == cp2 -> row
        &&  cp1 -> col == cp2 -> col
        &&  cp1 -> car_num == cp2 -> car_num))
    {
        return 0;
    }
    if (cp1 -> feature_num == 0){
        get_feature_num(cp1);
    }
    if (cp2 -> feature_num == 0){
        get_feature_num(cp2);
    }
    if (cp1 -> feature_num != cp2 -> feature_num){
        int row_num = cp1 -> row;
        int col_num = cp1 -> col;
        char **map1 = show(cp1, 0);
        char **map2 = show(cp2, 0);
        for (int i = 0; i < row_num; i ++){
            for (int j = 0; j < col_num; j ++){
                if (map1[i][j] != map2[i][j]){
                    free(map1);
                    free(map2);
                    return 0;
                }
            }
        }
        free(map1);
        free(map2);
    }
    return 1;
}

/*  void get_feature_num (carpark *cp);
    It can convert the relative 2D char map of 'cp' into
    an integer and record it in 'cp -> feature_num'.

    The algorithm is that, each character XOR in bit with
    the 'result', then the 'result' left move a bit.
    That is:
        result = (result << 1) + char.

    For a maximum grid in 20*20, the feature number has
    at least 8 + 19 + 19 = 46 bits, so it needs a long long int.
    If the grid is small, an int is enough.
*/
void get_feature_num (carpark *cp){
    int row_num = cp -> row;
    int col_num = cp -> col;
    unsigned int fn_line[row_num];
    unsigned long long int result = 0;
    char **map = show(cp, 0);

    for (int i = 0; i < row_num; i ++){
        fn_line[i] = 0;
        for (int j = 0; j < col_num; j ++){
            fn_line[i] = fn_line[i] << 1;
            fn_line[i] = fn_line[i] + (int)map[i][j];
        }
        result = result << 1;
        result = result ^ (unsigned long long int)fn_line[i];
    }
    free(map);
    cp -> feature_num = result;
}


void solve_grid (carpark *cp, const char *file_name){
    carpark_node *list[LIST_MAX];
    int temp[LIST_MAX] = {};
    carpark_node *cpn_head = new_carpark_node(cp, temp, 0);
    int list_len = 1;
    list[0] = cpn_head;
    int i = 0;
    while (!make_carpark_list(list, list[i], i, &list_len)){
        i ++;
        if (list_len == i){
            print_error(ERR_NO_ANS, __LINE__, file_name);
            return;
        }
    }
    carpark_node *result = list[list_len - 1];
    for (int j = 0; j < result -> order; j ++){
        show(list[result -> frontiers[j]] -> cp, 1);
    }
    show(result -> cp, 1);

    printf("%d moves\n", result -> order);
}

bool try_move (carpark *cp, car *c, const bool dir){
    int temp;
    temp = move(c, dir);
    if (    temp == 0
        ||  (c -> dir == VERT && temp == cp -> row - 1)
        ||  (c -> dir == HORI && temp == cp -> col - 1)) // Reach the wall
    {
        if (cp -> gates[c -> dir][c -> loc][dir]){ //Reach a gate
            c -> exist = 0;
            cp -> car_exist --;
            get_feature_num(cp);
            return 1;
        }
        else{
            move(c, !dir);
            return 0;
        }
    }
    int car_num_orth = cp -> car_num_array[!(c -> dir)][temp];
    int car_num_same = cp -> car_num_array[c -> dir][c -> loc];
    if (car_num_orth > 0){ // Cars in orthogonal direction
        for (int i = 0; i < car_num_orth; i++){
            car *c_temp = cp -> cars[!(c -> dir)][temp][i];
            if (check_car_conflict(c, c_temp, temp)){
                move(c, !dir);
                return 0;
            }
        }
    }
    if (car_num_same > 1){ // Cars in same line
        for (int i = 0; i < car_num_same; i++){
            car *c_temp = cp -> cars[c -> dir][c -> loc][i];
            if (check_car_conflict(c, c_temp, temp)){
                move(c, !dir);
                return 0;
            }
        }
    }
    get_feature_num(cp);
    return 1;
}

bool check_car_conflict (car *c1, car *c2, const int loc){
    // c1 is the moving car
    // c2 is the car may block c1
    if (c2 != c1 && c2 -> exist){
        if (c2 -> dir == c1 -> dir){ // in a same line
            int head = c2 -> head;
            int tail = c2 -> tail;
            if (loc >= head && loc <= tail){ 
                return 1;
            }
        }
        else{ // in orthogonal lines
            int head = c2 -> head;
            int tail = c2 -> tail;
            if (c1 -> loc >= head && c1 -> loc <= tail){ 
                return 1;
            }
        }

        
    }
    return 0;
}

bool make_carpark_list (carpark_node **list, carpark_node *cpn, const int node_num, int *list_len){
    carpark *cp = cpn -> cp;
    int car_num = cp -> car_num;
    for (int i = 0; i < car_num; i ++){
        for (int dir = 0; dir < 2; dir ++){
            carpark *cp_temp = carpark_copy(cp);
            car *c = cp_temp -> cars_list[i];
            if (    c -> exist 
                &&  try_move(cp_temp, c, dir)
                &&  !repeated_carpark(list, cp_temp, *list_len))
            {
                int frontiers[LIST_MAX];
                for (int j = 0; j < cpn -> order; j++){
                    frontiers[j] = cpn -> frontiers[j];
                }

                frontiers[cpn -> order] = node_num;
                carpark_node *node = (carpark_node *)malloc(sizeof(carpark_node));
                node = new_carpark_node(cp_temp, frontiers, (cpn -> order) + 1);
                list[*list_len] = node;
                (*list_len) ++;
                printf("list: %d\n", *list_len - 1);
                show(cp_temp, 1);
                //sleep(1);

                if (cp_temp -> car_exist == 0){
                    return 1;
                }
            }
            else{
                free(cp_temp);
            }
        }
    }
    return 0;
}

bool repeated_carpark (carpark_node **list, carpark *cp, const int list_len){
    for (int i = 0; i < list_len; i ++){
        if (carpark_compare(list[i] -> cp, cp)){
            return 1;
        }
    }
    return 0;
}

void print_cars (carpark *cp){
    printf("----\ncar num: %d\n", cp -> car_num);
    for (int i = 0; i < cp -> car_num; i++){
        car *c = cp -> cars_list[i];
        printf("car %c\n", c -> name);
        printf("head %d\n", c -> head);
        printf("tail %d\n", c -> tail);
        printf("dir %s\n", c -> dir ? "posi" : "nega");
        printf("los %d\n", c -> loc);
        printf("exist %c\n", c -> exist ? '1':'0');
        printf("\n");
    }
}