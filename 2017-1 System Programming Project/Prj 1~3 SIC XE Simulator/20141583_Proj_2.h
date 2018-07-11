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

// asm 코드 저장을 위한 구조체
typedef struct _assemble {
	int comment; // 코멘트인 경우 1로 세팅
	int line;
	int loc;
	int format;
	char *label_field; // 코멘트가 아닐 경우 라벨을, 코멘트일 경우 그 라인 전체를 저장
	char *mnemonic_field;
	char *operand1;
	char *operand2;
	int *objcode; // 1칸에 1byte (00~FF) 값 저장
	int objcodelen;
	int opcode;
	int nextloc;
	int emptyline; // 공백이나 탭으로만 구성된 라인에 대하여 '아무것도 없는 줄'이라는 표식을 세움
	struct _assemble *next;
} asmb;

typedef struct _symtab{ // SYMTAB 저장을 위한 구조체
	int loc;
	char label[40];
	struct _symtab *next;
} symtab;

typedef struct _textrecord{ // Text Record 저장을 위한 구조체
	int startaddr;
	int length;
	char objcode[65];
} textrecord ;

// 디렉토리 관련 변수 및 구조체
DIR *dp;
struct dirent *dirp;
struct stat buf;

// 셸 함수
void shell_help (void);
//void shell_dir (void);
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


/* 아래는 prj2에 추가한 함수 */
void shell_type (void);

void assemble (void);
void symbol (void);

int pass1 (void); // label을 SYMTAB에 저장하고 코드의 자잘한 문법 등을 체크
int pass2 (void); // objcode를 생성

int filenamecheck (void); // 파일이 디렉토리에 존재하는지 체크하여, 존재하면 파일을 오픈
int check_if_opcode (char* p); // 대상 문자열이 valid한 입력인지 체크
int check_if_register (char* p); // 대상 문자열이 레지스터인지 체크

void clearSYMTAB (void); // SYMTAB 정리
int addSYMTAB (void); // label을 SYMTAB에 추가
int find_opcode (char *); // opcodelist에서 opcode값을 찾아주는 함수
void freeasm (void); // asmb 타입으로 할당한 메모리 공간을 할당해제
int find_symbol (char *); // 대상 label의 address를 반환
int find_regnum (char *); // 레지스터 번호를 반환
