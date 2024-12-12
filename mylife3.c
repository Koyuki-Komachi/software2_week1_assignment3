#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> // sleep()関数を使う
//#include "gol.h"

#define height 40
#define width 70
#define cell_number height * width

//initialize alive or dead patterns of cells when given .lif file
void my_init_cells_lif(int cell[height][width], FILE *fp) {
    int height_point, width_point;
    while (fscanf(fp, "%d %d", &width_point, &height_point) != EOF) {
        cell[height_point][width_point] = 1;
    }
}

//initialize alive or dead patterns of cells when given .rle file
int my_init_cells_rle(int cell[height][width], FILE *fp) {
    char s1[50];
    int run_count = 0;
    int run_count_small = 0;
    char tag;
    int count = 0;
    int y = 0;
    //2行目を読み取る
    fgets(s1, sizeof(s1), fp);
    printf("the second line is %s", s1);
    //3行目以降を読み取る
    for (int num = 0;; num++){
        if (fscanf(fp, "%d", &run_count) == 1) {
            printf("%d", run_count);                                                    //
            if (fscanf(fp, "%c", &tag) == 1) {
                printf("%c", tag);                                                      //
                for (int i = count; i < count + run_count; ++i) {
                    if (tag == 'o') {
                        cell[y][i] = 1;
                    }else if (tag == 'b') {
                        cell[y][i] = 0;
                    }else {
                        printf("this file includes character except b and o1\n");
                        exit(EXIT_FAILURE);
                    }
                }
            }else {
                printf("this file is invalid1\n");
                exit(EXIT_FAILURE);
            }
            count += run_count;
        }else if ((fscanf(fp, "%c", &tag) == 1) && (tag == 'o' || tag == 'b')) {
            printf("%c", tag);                                                          //
            run_count = 1;
            if (tag == 'o') {
                cell[y][count] = 1;
            }else if (tag == 'b') {
                cell[y][count] = 0;
            }else {
                printf("this file includes character except b and o2\n");
                exit(EXIT_FAILURE);
            }
            count += run_count;
        }else if (tag == '$') {
            printf("%c", tag);                                                          //
            for (int i = count; i < width; ++i) {
                cell[y][i] = 0;
            }
            count = 0;
            y++;
        }else if (tag == '!'){
            printf("%c", tag);                                                          //
            for (int i = count; i < width; ++i) {
                cell[y][i] = 0;
            }
            if (y == height - 1) {
                printf("program successfully done\n");
                return 0;
            }else if (y < height - 1) {
                printf("the file may be smaller than field of 40 * 70\n");
                exit(EXIT_FAILURE);
            }else if (y >= height) {
                printf("the file may be bigger than filed of 40 * 70\n");
                exit(EXIT_FAILURE);
            }
        }else {
            printf("%d\n", tag);
            printf("this file is invalid2\n");
            exit(EXIT_FAILURE);
        }
    }
}
    


//initialize alive or dead patterns of cells
void my_init_cells(int cell[height][width], FILE* fp) {
    //ファイルがない場合の初期配置
    if (fp == NULL){
        for (int i = 0; i < cell_number / 10; ++i) {
            int height_point = rand() % height;
            int width_point = rand() % width;
            cell[height_point][width_point] = 1;
        }
    }//ファイルがある場合の初期配置
    else {
        char s[50];
        fgets(s, sizeof(s), fp);
        if (s[1] == 'L') { // ファイルが.lifの場合
            my_init_cells_lif(cell, fp);
        }else if (s[1] == 'N') { // ファイルが.rleの場合
            my_init_cells_rle(cell, fp);
        }
    }
}

//calculate the ratio of living cells
double cell_ratio(int cell[height][width]) {
    int num = 0;
    double ratio;
    for (int i = 0; i < height; ++i) {
        for (int j = 0; j < width; ++j) {
            if (cell[i][j] == 1) {
                num++;
            }
        }
    }
    ratio = (double)num / (height * width);
    return ratio;
}

//display the condition of all cells
void my_print_cells(FILE* fp, int gen, int cell[height][width]) {
        fprintf(fp, "geneneration = %d\n", gen);
        fprintf(fp, "living cell ratio = %f\n", cell_ratio(cell));
        //the top wall
        fprintf(fp, "+");
        for (int i = 0; i < width; ++i) {
            fprintf(fp, "-");
        }
        fprintf(fp, "+\n");
        //the content
        for (int i = 0; i < height; ++i) {
            fprintf(fp, "|");
            for (int j = 0; j < width; ++j) {
                if (cell[i][j]) {
                    fprintf(fp, "\e[31m#\e[0m");
                }else {
                    fprintf(fp, " ");
                }
            }
            fprintf(fp, "|\n");
        }
        //the bottom wall
        fprintf(fp, "+");
        for (int i = 0; i < width; ++i) {
            fprintf(fp, "-");
        }
        fprintf(fp, "+\n");
}

//個別のセルの次状態を別の配列に書いておく
void my_update_individual(int k, int l, int cell[height][width], int copy_cell[height][width]) {
    //隣接するセルのうち生きているものの個数を数える
    int count = 0;
    for (int m = k - 1; m <= k + 1; ++m) {
        for (int n = l - 1; n <= l + 1; ++n) {
            if (m >= 0 && m < height && n >= 0 && n < width) { // 隣接するセルがフィールド内にあるか確認する
                if (cell[m][n] == 1) {
                    count++;
                }
            }
        }
    }
    //いま対象のセルが生きている場合
    if (cell[k][l] == 1) {
        if (count == 3 || count == 4) {
            copy_cell[k][l] = 1;
        }else {
            copy_cell[k][l] = 0;
        }
    }
    //いま対象のセルが死んでいる場合
    if (cell[k][l] == 0) {
        if (count == 3) {
            copy_cell[k][l] = 1;
        }else {
            copy_cell[k][l] = 0;
        }
    }
}

//すべてのセルの次状態がそろった時点で、もとのセルに次状態を書き込む
void my_update_cells(int cell[height][width]) {
    int copy_cell[height][width] = {0};
    for (int k = 0; k < height; ++k) {
        for (int l = 0; l < width; ++l) {
            my_update_individual(k, l, cell, copy_cell);
        }
    }
    for (int k = 0; k < height; ++k) {
        for (int l = 0; l < width; ++l) {
            cell[k][l] = copy_cell[k][l];
        }
    }
}


int main(int argc, char **argv) {
    FILE *fp = stdout;
    
    int cell[height][width];
    for(int y = 0 ; y < height ; y++){
        for(int x = 0 ; x < width ; x++){
            cell[y][x] = 0;
        }
    }
    
    /* ファイルを引数にとるか、ない場合はデフォルトの初期値を使う */
    if ( argc > 2 ) {
        fprintf(stderr, "usage: %s [filename for init]\n", argv[0]);
        return EXIT_FAILURE;
    }
    else if (argc == 2) {
        FILE *lgfile;
        if ( (lgfile = fopen(argv[1],"r")) != NULL ) {
            my_init_cells(cell,lgfile); // ファイルによる初期化
        }
        else{
            fprintf(stderr,"cannot open file %s\n",argv[1]);
            return EXIT_FAILURE;
        }
        fclose(lgfile);
    }
    else{
        my_init_cells(cell, NULL); // デフォルトの初期値を使う
    }
    
    //my_print_cells(fp, 0, cell); // 表示する
    //sleep(1); // 1秒休止
    //fprintf(fp,"\e[%dA",height+3);//height+3 の分、カーソルを上に戻す(壁2、表示部1)
    
    /* 世代を進める*/
    for (int gen = 1 ;gen < 10000; gen++) {
        my_update_cells(cell); // セルを更新
        FILE *fp;
        if (gen % 100 == 0) {
            char filename[20];
            sprintf(filename, "gen%04d.lif", gen);
            fp = fopen(filename, "w");
            my_print_cells(fp, gen, cell);// 表示する
        }
        //sleep(1); //1秒休止する
        //fprintf(fp,"\e[%dA",height+3);//height+3 の分、カーソルを上に戻す(壁2、表示部1)
    }
    fclose(fp);
    
    return EXIT_SUCCESS;
}
