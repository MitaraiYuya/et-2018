/* スタックのデータ構造と関数プロトタイプ */
#ifndef _stack
#define _stack

/*--- スタックを実現する構造体 ---*/
typedef struct {
	int  max;       /* スタックのサイズ */
	int  ptr;       /* スタックポインタ */
	int  *stk;      /* スタック（の先頭要素へのポインタ） */
} Stack;


int StackAlloc(Stack *s, int max);     /*--- スタックの初期化 ---*/
void StackFree(Stack *s);              /*--- スタックの後始末 ---*/
int StackPush(Stack *s, int x);        /*--- スタックにデータをプッシュ ---*/
int StackPop(Stack *s, int *x);        /*--- スタックからデータをポップ ---*/
int StackPeek(const Stack *s, int *x); /*--- スタックからデータをピーク ---*/
int StackSize(const Stack *s);         /*--- スタックの大きさ ---*/
int StackNo(const Stack *s);           /*--- スタックに積まれているデータ数 ---*/
int StackIsEmpty(const Stack *s);      /*--- スタックは空か ---*/
int StackIsFull(const Stack *s);       /*--- スタックは満杯か ---*/
void StackClear(Stack *s);             /*--- スタックを空にする ---*/

#endif //_stack