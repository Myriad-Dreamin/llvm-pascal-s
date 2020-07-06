//
// Created by kamiyoru on 2020/4/30.
//

#ifndef PASCAL_S_LEXER_GULP_H
#define PASCAL_S_LEXER_GULP_H

#define YY_USER_ACTION if (yy_start == 1) current_offset += yyleng;

#endif //PASCAL_S_GULP_H
