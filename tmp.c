//２行目以降にコメントが入っていないファイルを仮定する
    char s[30];
    fgets(s, sizeof(s), fp);
    char run_count;
    char tag;
    char first_character = fgetc(fp);
    if (first_character == 'o' || first_character == 'b') { // 連続文字数の１が省略されている場合
        run_count = 1;
        tag = first_character;
        if (tag == 'o') {
            cell[0][0] = 1;
        }else if (tag == 'b') {
            cell[0][0] = 0;
        }else {
            printf("this file contains character except 'b' and 'o'");
            exit(EXIT_FAILURE);
        }
    }else if (first_character >= '0' && first_character <= '9') { // 連続文字数が書いてある場合
        char second_character = fgetc(fp);
        if (second_character == 'o' || second_character == 'b') { //連続文字数が１桁の場合
            run_count = first_character;
            tag = second_character;
            if (tag == 'o') {
                for (int i = 0; i < run_count; ++i) {
                    cell[0][i] = 1;
                }
            }else if (tag == 'b') {
                for (int i = 0; i < run_count; ++i) {
                    cell[0][i] = 0;
                }
            }else {
                printf("this file contains character except 'b' and 'o'");
                exit(EXIT_FAILURE);
            }
        }
        else if (second_character >= '0' && second_character <= '9') { //連続文字数が２桁の場合
            run_count = atoi("first_character, second_character");
            printf("run count is %d\n", run_count);
            tag = fgetc(fp);
            if (tag == 'o') {
                for (int i = 0; i < run_count; ++i) {
                    cell[0][i] = 1;
                }
            }else if (tag == 'b') {
                for (int i = 0; i < run_count; ++i) {
                    cell[0][i] = 0;
                }
            }else {
                printf("this file contains character except 'b' and 'o");
                exit(EXIT_FAILURE);
            }
        }
    }