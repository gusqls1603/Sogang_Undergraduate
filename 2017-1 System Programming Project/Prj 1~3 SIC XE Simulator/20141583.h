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

typedef struct _estab{ // ESTAB 저장을 위한 구조체 (prj3에서 추가)
	char name[10];
	int addr;
	int length;
	struct _estab *next;
} estab;

typedef struct _bp{ // breakpoint 저장을 위한 구조체 (prj3에서 추가)
	int checkpoint;
	struct _bp *next;
} bp;

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


/* 아래는 prj3에 추가한 함수 */
void progaddr (void); // 프로그램의 시작주소를 지정
void loader (void); // 지정된 프로그램을 메모리에 로드한다.
void run (void); // 로드된 프로그램을 실행
void debug (void); // breakpoint를 설정하거나 초기화하는 함수

void reset_or_run (void);  // 입력 커맨드가 r로 시작할 때 reset과 run 중 무엇을 호출할지 결정하는 함수

int search_ESTAB (char *s); // 찾는 문자열이 ESTAB에 이미 있으면 그 address를, 없으면 0을 리턴

// 메모리에 적재된 프로그램을 실행하는 데 사용되는 명령어 수행 함수
void ADD (void); // Target Value를 A에 가산
void ADDF (void); // 미구현
void ADDR (void); // r1, r2를 더하여 r2에 저장
void AND (void); // Target Value와 A의 &연산
void CLEAR (void); // 해당 레지스터 초기화
void COMP (void); // Target Value와 A를 비교
void COMPF (void); // 미구현
void COMPR (void); // r1, r2를 비교
void DIV (void); // Target Value와 A 사이의 / 연산
void DIVF (void); // 미구현
void DIVR (void); // r1, r2 사이의 / 연산 값을 r2에 저장
void FIX (void); // 미구현
void FLOATT (void); // 미구현
void HIO (void); // 미구현
void J (void); // Target Address로 Jump
void JEQ (void); // SW가 =이면 Target Address로 Jump
void JGT (void); // SW가 >이면 Target Address로 Jump
void JLT (void); // SW가 < 이면 Target Address로 Jump
void JSUB (void); // Target Address로 Jump
void LDA (void); // Target Value를 A에 보관
void LDB (void); // Target Value를 B에 보관
void LDCH (void); // Target Character를 A에 보관
void LDF (void); // 미구현
void LDL (void); // Target Value를 L에 보관
void LDS (void); // Target Value를 S에 보관
void LDT (void); // Target Value를 T에 보관
void LDX (void); // Target Value를 X에 보관
void LPS (void); // 미구현
void MUL (void); // Target Value를 A와 곱하여 A에 저장
void MULF (void); // 미구현
void MULR (void); // r1, r2를 곱셉하여 r2에 저장
void NORM (void); // 미구현
void OR (void); // Target Value와 A 사이의 | 연산 값을 A에 저장
void RD (void); // 테스트가 가능하도록 간단하게 구현
void RMO (void); // r1의 값을 r2에 저장
void RSUB (void); // L 레지스터에 보관중인 Address로 Jump
void SHIFTL (void); // 해당 레지스터를 n bit만큼 left로 circular shift
void SHIFTR (void); // 해당 레지스터를 n bit만큼 right로 shift
void SIO (void); // 미구현
void SSK (void); // 미구현
void STA (void); // Target Address에 A 값 저장
void STB (void); // Target Address에 B 값 저장
void STCH (void); // Target Address에 A의 rightmost 값 저장
void STF (void); // 미구현
void STI (void); // 미구현
void STL (void); // Target Address에 L 값 저장
void STS (void); // Target Address에 S 값 저장
void STSW (void); // Target Address에 SW 값 저장
void STT (void);  // Target Address에 T 값 저장
void STX (void); // Target Address에 X 값 저장
void SUB (void); // Target Value를 A에 감산
void SUBF (void); // 미구현
void SUBR (void); // r1, r2를 감산하여 r2에 저장
void SVC (void); // 미구현
void TD (void); // 테스트가 가능하도록 간단하게 구현
void TIO (void); // 미구현
void TIX (void); // X에 1을 더한 뒤 X를 Target Value와 비교
void TIXR (void); // X에 1을 더한 뒤 x를 Target 레지스터와 비교
void WD (void); // 테스트가 가능하도록 간단하게 구현

