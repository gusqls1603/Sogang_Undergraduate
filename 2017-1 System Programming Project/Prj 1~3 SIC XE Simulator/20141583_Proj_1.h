#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <dirent.h>

// 명령어 히스토리를 저장하는 Linked List
typedef struct _ptr{
	char command[100];
	struct _ptr *next;
} ptr;

// opcodelist 저장을 위한 구조체
typedef struct _opcode{
	int opcode;
	char mnemonic[7];
	struct _opcode *next;
	char type[4];
} opcd;

// 디렉토리 관련 변수 및 구조체
DIR *dp;
struct dirent *dirp;
struct stat buf;

// 셸 함수
void shell_help (void);
void shell_dir (void);
void shell_quit (void);
void shell_history (void);

// 메모리 함수
void memory_dump (void);
void memory_edit (void);
void memory_fill (void);
void memory_reset (void);

// opcode 함수
void opcode_opcode (void);
void opcode_opcodelist (void);

// h, d, o로 시작하는 명령어 입력에 대하여
// 적절한 함수를 호출하거나 에러를 뿜는 함수
void help_or_history (void);
void dir_or_dump (void);
void opcode_or_opcodelist (void);

// 에러메시지 출력 함수
void print_error (char *s);

// 공백이나 수평 탭이 뭉쳐서 입력될 경우 이를 공백 하나로 줄여서 저장하는 함수
// EX)  "    h    el    p    "  ->  "h el p"
int remove_space (void);

// 입력된 명령어가 정상으로 간주될 경우 히스토리에 추가하는 함수
void add_history (void);


/* 아래는 sp2에 추가한 함수 */
void shell_type (void);

void assemble (void);
void symbol (void);

void pass1 (void);
void pass2 (void);

int filenamecheck (void);