#include "20141583.h"


opcd *temp, *del, *hashtable;
ptr *head = NULL, *prev;
char com[100], com_backup[100], *tok;

int memory [65536][16] = {{0}}, address = 0;
int quit = 0; // quit함수가 호출될 때 1로 변경됨
int len; // 커맨드 길이 저장

///////////////////////////////////////////////////////
char *filename; // type, assemble 함수에서 filename을 포인팅
FILE *fp, *lst, *obj; // assemble대상파일, lst파일, obj파일의 포인터
asmb *asmhead = NULL, *asmtemp, *asmprev; // asm 원본 코드를 저장 및 코드 정보에 접근할 때 사용
int LOCCTR=0, printSYMTAB=0, flagSTART=0; // assemble 과정에서 사용
char oneline[500]; // asm코드에서 현재 작업중인 줄을 저장하는데 쓰임
symtab SYMTAB[55], *symprev, *symtemp; // SYMTAB 관련 작업 시 사용
int startloc, endloc; // obj파일 작성 시 사용
// SYMTAB[0]은 ASCII 33~64, SYMTAB[1~26]은 알파벳 대문자
// SYMTAB[27]은 ASCII 91~96, SYMTAB[28~53]은 알파벳 소문자, SYMTAB[54]는 ASCII 123~126
/////////////////////////////////////////////////////// prj2에서 추가한 global 변수

int main()
{
	int count = 58, f_opcode, f_sum, i; char f_mnemonic[7], f_type[4];
	fp = fopen("opcode.txt", "r");
	if(!fp){
		print_error("File open Error!\n"); return 0;
	}
	hashtable = (opcd*)malloc(sizeof(opcd)*20);
	if(!hashtable){
		print_error("Memory Allocation Error!\n"); return 0;
	}
	for(i=0 ; i<20 ; i++){
		// 해쉬테이블 초기화 (opcode가 -1이면 '비어있다' 라고 간주)
		hashtable[i].opcode = -1;
		hashtable[i].next = NULL;
	}
	while(count--){
		// mnemonic의 각 문자의 아스키코드값을 더한 것을 20으로 나눈
		// 나머지의 값에 해당하는 hashtable칸에 mnemonic 정보를 저장
		f_sum=0;
		fscanf(fp, "%X %s %[1234/]", &f_opcode, f_mnemonic, f_type);
		for(i=0 ; f_mnemonic[i] != 0 ; i++){
			f_sum += f_mnemonic[i];
		}
		f_sum %= 20;

		if(hashtable[f_sum].opcode == -1){
			// hashtable의 f_sum번째 칸이 비어있으면 그대로 정보를 입력
			hashtable[f_sum].opcode = f_opcode;
			strcpy((hashtable+f_sum)->mnemonic, f_mnemonic);
			strcpy((hashtable+f_sum)->type, f_type);
		}
		else{
			// hashtable의 f_sum번째 칸이 비어있지 않으면
			// 정보를 저장할 공간을 만들고 hashtable과 이어줌
			opcd *hash = (opcd*)malloc(sizeof(opcd));
			if(!hash){
				print_error("Memory Allocation Error!\n"); return 0;
			}
			hash[0].opcode = f_opcode;
			strcpy(hash->mnemonic, f_mnemonic);
			strcpy(hash->type, f_type);
			hash->next = NULL;

			temp = hashtable + f_sum;
			while(temp->next != NULL){
				temp = temp->next;
			}
			// 이어주는 작업
			temp->next = hash;
		}
	}
	fclose(fp);

	// quit함수 호출 전까지 반복
	while(!quit)
	{
		printf("sicsim> ");
		fgets(com, 99, stdin);
		*(com + strlen(com) -1) = 0; // fgets는 \n까지 받아들이므로 이를 \0으로 치환
		
		// 중복되는 공백과 탭을 제거하는 함수.
		// 문자입력 없이 공백, 탭만 입력되거나 아무 입력도 없었을 경우 0을 반환하여 if문 내부로 진입한다.
		if( !remove_space() ) 
			continue;
		len = strlen(com);

		switch( com[0] ){
			// 입력 커맨드의 맨 앞 문자를 대조
		case 'h' : help_or_history(); break;
		case 'd' : dir_or_dump(); break;
		case 'q' : shell_quit(); break; 
		case 'e' : memory_edit(); break;
		case 'f' : memory_fill(); break;
		case 'r' : memory_reset(); break;
		case 'o' : opcode_or_opcodelist(); break;
		case 'a' : assemble(); break; ///////////////////////////////////////////////////////////
		case 't' : shell_type(); break; ///////////////////////////////////////////////////////////
		case 's' : symbol(); break; ///////////////////////////////////////////////////////////
		default : print_error("Input Command Error! Please type 'help' to see valid commands.\n");
		}
		
	}

	// quit함수가 호출되어 sicsim이 종료될 때
	printf("Freeing allocated memory...\n");
	while(head){
		// 명령어 히스토리 제거
		prev = head;
		head = head->next;
		free(prev);
	}

	for(i=0 ; i<20 ; i++){
		// 해쉬테이블 제거
		if( hashtable[i].opcode == -1) continue;
		temp = (hashtable +i)->next;
		while(temp){
			del = temp;
			temp = temp->next;
			free(del);
		}
	}
	free(hashtable);

	clearSYMTAB(); // 마지막 assemble 호출 이후 남아있는 SYMTAB을 sicsim 종료 시 clear시킴
				   // assemble 과정 중에 allocate된 다른 메모리들은 assemble 작업 종료 이전에 assemble 함수 내에서 free

	return 0;
}

void shell_help (void) // 사용 가능한 명령어를 출력
{
	switch(len){
		// 명령어 입력이 정상적인지 판단하는 작업
	case 1 : break;
	case 4 : if( strncmp(com, "help", 4) == 0) break;
	default : print_error("Input Command Error! (Suggested Command : h[elp], hi[story])\n"); return;
	}

	add_history();
	printf("\nh[elp]\nd[ir]\nq[uit]\nhi[story]\ndu[mp] [start, end]\ne[dit] address, value\nf[ill] start, end, value\nreset\nopcode mnemonic\nopcodelist\nassemble filename\ntype filename\nsymbol\n\n");
	return ;
}

void shell_dir (void) // 디렉토리 출력
{
	switch(len){
	case 1 : break;
	case 3 : if( strncmp(com, "dir", 3) == 0) break;
	default : print_error("Input Command Error! (Suggested Command : d[ir])\n"); return;
	}
	
	int count=0;
	if(!(dp = opendir("."))){
			printf("Opendir Error!\n");
			return; // 디렉토리 포인터를 받지 못할 경우 종료
	}
	add_history();

	while((dirp = readdir(dp))){
		if(!strncmp(".", dirp->d_name, 1) || !strncmp("..", dirp->d_name, 2))
			continue;
		printf("%20s", dirp->d_name);
		if(lstat(dirp->d_name, &buf) < 0){
			printf("lstat error!\n");
			continue; // lstat 예외처리
		}
		if(S_ISREG(buf.st_mode))
			printf("*"); // 일반 실행파일일 경우 이름 옆에 * 출력
		else if(S_ISDIR(buf.st_mode))
			printf("/"); // 디렉토리인 경우 / 출력
		else
			printf("(not REG nor DIR)"); // 실행파일도, 디렉토리도 아닐 경우
		if(++count==3){
			printf("\n");
			count -=3; // count 변수가 3일 때마다 개행
		}
	}
	printf("\n");
	closedir(dp);
	
	return ;
}

void shell_quit (void) // sicsim 종료
{
	switch(len){
	case 1 : break;
	case 4 : if( strncmp(com, "quit", 4) == 0) break;
	default : print_error("Input Command Error! (Suggested Command : q[uit])\n"); return;
	}
	quit = 1; // main의 while루프를 종료하기 위해 quit값을 1로 설정

	return ;
}

void shell_history (void) // 명령어 히스토리 출력
{
	ptr *temp; int i;
	switch(len){
	case 2 : break;
	case 7 : if( strncmp(com, "history", 7) == 0) break;
	default : print_error("Input Command Error! (Suggested Command : h[elp], hi[story])\n"); return;
	}
	add_history();
	
	temp = head; i = 1;
	while(temp){
		printf("\t%4d %s\n", i++, temp->command);
		temp = temp->next;
	}
	return ;
}

void memory_dump (void) // 메모리 출력
{
	int mode = 0; // mode 1 : dump만 입력, mode 2 : dump start까지 입력, mode 3 : dump, start, end까지 입력된 경우
	int a, b, c = 0; // 콤마개수 체크 작업에 쓰이는 변수
	int tok_len, start = -1, end = -1, dump1_len, dump2_len, i, j;
	char *dump1, *dump2; // 각각 start와 end인자로 간주되는 문자열의 주소를 저장하기 위한 변수
	
	strcpy(com_backup, com); // 원본 커맨드 보존
	tok = strtok(com, " "); // 입력된 커맨드 중에서 '명령어'로 간주되는 문자열을 분리
	tok_len = strlen(tok);
	switch(tok_len){
	case 2 : if( strncmp(tok, "du", 2) == 0) break;
	case 4 : if( strncmp(tok, "dump", 4) == 0) break;
	default : print_error("Input Command Error! (Suggested Command : d[ir], du[mp] [start, end])\n"); return;
	}

	dump1 = strtok(NULL, " ,"); // 입력된 커맨드 중에서 'start 인자'로 간주되는 문자열을 분리
	if(dump1 == NULL){
		//start 인자가 주어지지 않은 경우. 즉 "du" 또는 "dump"가 입력되었을 경우
		mode = 1;
	}
	else{
		// start 인자가 주어졌다고 간주된 경우

		// 명령어와 start 인자 사이에(원칙적으로 콤마가 없었어야 할 자리에) 콤마가 있었는지 체크하는 작업
		for (i=0 ; i< dump1 - tok - tok_len ; i++) {
			if (com_backup[tok_len + i] == ',') {
				c++;
			}
			if (c > 0) {
				//콤마가 1개 이상 있었다면
				print_error("Input Command Error! (Check your comma)\n"); return;
			}
		}

		dump1_len = strlen(dump1);
		while (dump1_len >= 6) {
			// 인자 앞에 0이 나열되어 있을 때 이를 무시하는 작업. 가령 00000FF가 입력되면, FF가 입력된 것으로 간주한다.
			if (*(dump1) != '0') {
				print_error("Start Memory Address Error! (Must be in 0~FFFFF)\n"); return;
			}
			else {
				dump1 = (dump1 + 1);
				dump1_len--;
			}
		}

		for(i=0 ; i<dump1_len ; i++){
			if( ((dump1[i]>='0'&&dump1[i]<='9') || (dump1[i]>='A'&&dump1[i]<='F') || (dump1[i]>='a'&&dump1[i]<='f')) != 1 ){
				// start 인자가 1~5자리의 16진수 값이 아닌 경우
				print_error("Start Memory Address Error! (Must be in 0~FFFFF)\n"); return;
			}
		}
		sscanf(dump1, "%X", &start);

		dump2 = strtok(NULL, " ,"); // 입력된 커맨드 중에서 'end 인자'로 간주되는 문자열을 분리

		if(dump2 == NULL){
			// end 인자가 주어지지 않은 경우. 즉 "du[mp] start" 타입의 입력이 들어온 경우
			mode = 2;

			// 이 때(end가 없을 때), start 인자 뒤에 콤마가 1개 이상 있었는지 확인하는 작업이 필요함.
			c = 0;
			for (i = 0; com_backup[dump1 - tok + dump1_len + i] != 0; i++) {
				if (com_backup[dump1 - tok + dump1_len + i] == ',') {
					c++;
				}
				if (c > 0) {
					//콤마가 1개 이상 있었다면
					print_error("Input Command Error! (Check your comma)\n"); return;
				}
			}
		}
		else{
			// end 인자가 주어졌다고 간주된 경우
			
			dump2_len = strlen(dump2);
			while (dump2_len >= 6) {
				// 인자 앞에 0이 나열되어 있을 때 이를 무시하는 작업. 가령 00000FF가 입력되면, FF가 입력된 것으로 간주한다.
				if (*(dump2) != '0') {
					print_error("End Memory Address Error! (Must be in 0~FFFFF)\n"); return;
				}
				else {
					dump2 = (dump2 + 1);
					dump2_len--;
				}
			}

			for(i=0 ; i<dump2_len ; i++){
				if( ((dump2[i]>='0'&&dump2[i]<='9') || (dump2[i]>='A'&&dump2[i]<='F') || (dump2[i]>='a'&&dump2[i]<='f')) != 1 ){
					// end 인자가 1~5자리의 16진수 값이 아닌 경우
					print_error("End Memory Address Error! (Must be in 0~FFFFF)\n"); return;
				}
			}
			sscanf(dump2, "%X", &end);
			mode = 3;

			// 두 인자 start, end 사이에 콤마가 있었는지 짚고 넘어가는 작업
			c = 0;
			a = dump1 - tok; b = dump2 - dump1 - dump1_len;
			for(i=0 ; i<b ; i++){
				if( com_backup[a + dump1_len + i] == ',' ){
					c++;
				}
			}
			if(c!=1){
				//두 인자 사이에 콤마가 1개가 아니었다면 (없거나, 2개 이상이었을 경우)
				print_error("Input Command Error! (Comma is missing or more than 2 Commas)\n"); return;
			}

			//dump 4, 37, 와 같이 end인자 바로 뒤에 콤마가 입력되었는지 체크하는 작업
			if (com_backup[dump2 - tok + dump2_len] == ',') {
				print_error("Input Command Error! (Check your comma)\n"); return;
			}

			tok = strtok(NULL, " ");
			if(tok != NULL){
				//end 인자까지 받았는데 뒤에 문자가 더 남아있는 경우
				print_error("Input Command Error! (Too much parameters)\n"); return;
			}
		}
	}
	
	switch(mode){
		// 입력된 커맨드 유형에 따라 start번지, end번지 값을 결정
	case 1 : start = address; end = start + 159; break; // start, end인자가 모두 주어지지 않은 경우
	case 2 : end = start + 159; break; // start인자만 주어진 경우
	default : if(start > end) {print_error("Memory Address Error! Check if your Start is bigger than End value.\n"); return; }
	}
	if(end > 0xFFFFF) { end = 0xFFFFF; }
	
	address = end + 1;
	if (address > 0xFFFFF) {
		address = 0;
	}
	strcpy(com, com_backup);
	add_history(); // 원본 커맨드를 불러와 명령어 히스토리에 저장

	// 출력부
	for(i=0 ; i<=(end/16 - start/16) ; i++){
		printf("%04X0", start/16 +i);
		for(j=0 ; j<16 ; j++){
			if( ((start/16 +i)*16 +j < start) || ((start/16 +i)*16 +j > end) ){
				// 주어진 범위를 벗어난 주소에 대하여 공백을 출력
				printf("   "); continue;
			}
			printf(" %02X", memory[start/16 +i][j]);
		}
		printf(" ; ");
		for(j=0 ; j<16 ; j++){
			// 아스키코드 문자 출력
			if( ((start/16 +i)*16 +j < start) || ((start/16 +i)*16 +j > end) ){
				printf("."); continue;
			}
			if( memory[start/16 +i][j] < 0x20 || memory[start/16 +i][j] > 0x7E ){
				printf("."); continue;
			}
			printf("%c", memory[start/16 +i][j]);
		}
		printf("\n");
	}
}

void memory_edit (void) // 메모리 저장 값 변경 함수
{
	int a, b, c=0; // 콤마 체크 작업에 쓰이는 변수
	int tok_len, addr_len, edit_addr, val_len, edit_val, i;
	char *addr, *val; // 각각 address와 value 인자로 간주되는 문자열의 주소를 저장하기 위한 변수
	
	strcpy(com_backup, com); // 원본 커맨드 보존
	tok = strtok(com, " "); // 입력된 커맨드 중에서 '명령어'로 간주되는 문자열을 분리
	tok_len = strlen(tok); 
	switch(tok_len){
	case 1 : if( strncmp(tok, "e", 1) == 0) break;
	case 4 : if( strncmp(tok, "edit", 4) == 0) break;
	default : print_error("Input Command Error! (Suggested Command : 'e[dit] address, value')\n"); return;
	}
	
	addr = strtok(NULL, " ,"); // 입력된 커맨드 중에서 'address 인자'로 간주되는 문자열을 분리
	if(addr == NULL){
		// address 인자가 주어지지 않은 경우
		print_error("Command Format Error! See 'help' to check format.\n"); return;
	}

	addr_len = strlen(addr);
	while (addr_len >= 6) {
		// 인자 앞에 0이 나열되어 있을 때 이를 무시하는 작업. 가령 00000FF가 입력되면, FF가 입력된 것으로 간주한다.
		if (*(addr) != '0') {
			print_error("Target Memory Address Error! (Must be in 0~FFFFF)\n"); return;
		}
		else {
			addr = (addr + 1);
			addr_len--;
		}
	}

	for(i=0 ; i<addr_len ; i++){
		if( ((addr[i]>='0'&&addr[i]<='9') || (addr[i]>='A'&&addr[i]<='F') || (addr[i]>='a'&&addr[i]<='f')) != 1 ){
			// address 인자가 1~5자리의 16진수 값이 아닌 경우
			print_error("Target Memory Address Error! (Must be in 0~FFFFF)\n"); return;
		}
	}
	sscanf(addr, "%X", &edit_addr);

	val = strtok(NULL, " ,"); // 입력된 커맨드 중에서 'value 인자'로 간주되는 문자열을 분리
	if(val == NULL){
		// value 인자가 주어지지 않은 경우
		print_error("Command Format Error! See 'help' to check format.\n"); return;
	}

	val_len = strlen(val);
	while (val_len >= 3) {
		// 인자 앞에 0이 나열되어 있을 때 이를 무시하는 작업. 가령 00000FF가 입력되면, FF가 입력된 것으로 간주한다.
		if (*(val) != '0') {
			print_error("Value Range Error! (Must be in 0~FF)\n"); return;
		}
		else {
			val = (val + 1);
			val_len--;
		}
	}

	for(i=0 ; i<val_len ; i++){
		if( ((val[i]>='0'&&val[i]<='9') || (val[i]>='A'&&val[i]<='F') || (val[i]>='a'&&val[i]<='f')) != 1 ){
			// value 인자가 1~2자리의 16진수 값이 아닌 경우
			print_error("Value Range Error! (Must be in 0~FF)\n"); return;
		}
	}
	sscanf(val, "%X", &edit_val);

	// 두 인자 address, value 사이에 콤마가 있었는지 짚고 넘어가는 작업
	a = addr - tok; b = val - addr - addr_len;
	for(i=0 ; i<b ; i++){
		if( com_backup[a + addr_len + i] == ',' ){
			c++;
		}
	}
	if(c!=1){
		// 두 인자 사이에 콤마가 1개가 아니었다면 (없거나, 2개 이상이었을 경우)
		print_error("Input Command Error! (Comma is missing or more than 2 Commas)\n"); return;
	}

	tok = strtok(NULL, " ");
	if(tok != NULL){
		// value 인자까지 받았는데 뒤에 문자가 더 남아있는 경우
		print_error("Input Command Error! (Too much parameters)\n"); return;
	}

	strcpy(com, com_backup); // 원본 커맨드를 명령어 히스토리에 저장
	add_history();

	// 메모리 저장 값 변경
	memory[edit_addr/16][edit_addr%16] = edit_val;
}

void memory_fill (void)
{
	int a, b, c=0; // 콤마 체크 작업 관련 변수
	int tok_len, start_len, start_addr, end_len, end_addr, val_len, fill_val, i, j;
	char *start, *end, *val; // 각각 start, end, value 인자로 간주되는 문자열의 주소를 저장하기 위한 변수
	
	strcpy(com_backup, com); // 원본 커맨드 보존
	tok = strtok(com, " "); // 입력된 커맨드 중에서 '명령어'로 간주되는 문자열을 분리
	tok_len = strlen(tok);
	switch(tok_len){
	case 1 : if( strncmp(tok, "f", 1) == 0) break;
	case 4 : if( strncmp(tok, "fill", 4) == 0) break;
	default : print_error("Input Command Error! (Suggested Command : 'f[ill] start, end, value')\n"); return;
	}

	start = strtok(NULL, " ,"); // 입력된 커맨드 중에서 'start 인자'로 간주되는 문자열을 분리
	if(start == NULL){
		// start 인자가 주어지지 않은 경우
		print_error("Command Format Error! See 'help' to check format.\n"); return;
	}

	start_len = strlen(start);
	while (start_len >= 6) {
		// 인자 앞에 0이 나열되어 있을 때 이를 무시하는 작업. 가령 00000FF가 입력되면, FF가 입력된 것으로 간주한다.
		if (*(start) != '0') {
			print_error("Start Memory Address Error! (Must be in 0~FFFFF)\n"); return;
		}
		else {
			start = (start + 1);
			start_len--;
		}
	}

	for(i=0 ; i<start_len ; i++){
		if( ((start[i]>='0'&&start[i]<='9') || (start[i]>='A'&&start[i]<='F') || (start[i]>='a'&&start[i]<='f')) != 1 ){
			// start 인자가 1~5자리의 16진수 값이 아닌 경우
			print_error("Start Memory Address Error! (Must be in 0~FFFFF)\n"); return;
		}
	}
	sscanf(start, "%X", &start_addr);

	end = strtok(NULL, " ,"); // 입력된 커맨드 중에서 'end 인자'로 간주되는 문자열을 분리
	if(end == NULL){
		// end 인자가 주어지지 않은 경우
		print_error("Command Format Error! See 'help' to check format.\n"); return;
	}

	end_len = strlen(end);
	while (end_len >= 6) {
		// 인자 앞에 0이 나열되어 있을 때 이를 무시하는 작업. 가령 00000FF가 입력되면, FF가 입력된 것으로 간주한다.
		if (*(end) != '0') {
			print_error("End Memory Address Error! (Must be in 0~FFFFF)\n"); return;
		}
		else {
			end = (end + 1);
			end_len--;
		}
	}

	for(i=0 ; i<end_len ; i++){
		if( ((end[i]>='0'&&end[i]<='9') || (end[i]>='A'&&end[i]<='F') || (end[i]>='a'&&end[i]<='f')) != 1 ){
			// end 인자가 1~5자리의 16진수 값이 아닌 경우
			print_error("End Memory Address Error! (Must be in 0~FFFFF)\n"); return;
		}
	}
	sscanf(end, "%X", &end_addr);

	// 두 인자 start, end 사이에 콤마가 있었는지 짚고 넘어가는 작업
	a = start - tok; b = end - start - start_len;
	for(i=0 ; i<b ; i++){
		if( com_backup[a + start_len + i] == ',' ){
			c++;
		}
	}
	if(c!=1){
		// 두 인자 사이에 콤마가 1개가 아니었다면 (없거나, 2개 이상이었을 경우)
		print_error("Input Command Error! (Comma is missing or more than 2 Commas)\n"); return;
	}

	val = strtok(NULL, " ,"); // 입력된 커맨드 중에서 'value 인자'로 간주되는 문자열을 분리
	if(val == NULL){
		// value 인자가 주어지지 않은 경우
		print_error("Command Format Error! See 'help' to check format.\n"); return;
	}

	val_len = strlen(val);
	while (val_len >= 3) {
		// 인자 앞에 0이 나열되어 있을 때 이를 무시하는 작업. 가령 00000FF가 입력되면, FF가 입력된 것으로 간주한다.
		if (*(val) != '0') {
			print_error("Value Range Error! (Must be in 0~FF)\n"); return;
		}
		else {
			val = (val + 1);
			val_len--;
		}
	}

	for(i=0 ; i<val_len ; i++){
		if( ((val[i]>='0'&&val[i]<='9') || (val[i]>='A'&&val[i]<='F') || (val[i]>='a'&&val[i]<='f')) != 1 ){
			// value 인자가 1~2자리의 16진수 값이 아닌 경우
			print_error("Value Range Error! (Must be in 0~FF)\n"); return;
		}
	}
	sscanf(val, "%X", &fill_val);

	// 두 인자 end, value 사이에 콤마가 있었는지 짚고 넘어가는 작업
	c = 0;
	a = end - tok; b = val - end - end_len;
	for(i=0 ; i<b ; i++){
		if( com_backup[a + end_len + i] == ',' ){
			c++;
		}
	}
	if(c!=1){
		// 두 인자 사이에 콤마가 1개가 아니었다면 (없거나, 2개 이상이었을 경우)
		print_error("Input Command Error! (Comma is missing or more than 2 Commas)\n"); return;
	}

	tok = strtok(NULL, " ");
	if(tok != NULL){
		// value 인자까지 받았는데 뒤에 문자가 더 남아있는 경우
		print_error("Input Command Error! (Too much parameters)\n"); return;
	}

	if (start_addr > end_addr) {
		print_error("Memory Address Error! Check if your Start is bigger than End value.\n");
	}

	strcpy(com, com_backup); // 원본 커맨드를 불러와 명령어 히스토리에 저장
	add_history();

	// 주어진 범위에 포함되는 경우에 대하여 메모리에 value 값 저장
	for(i=0 ; i<=(end_addr/16 - start_addr/16) ; i++){
		for(j=0 ; j<16 ; j++){
			if( ((start_addr/16 +i)*16 +j >= start_addr) && ((start_addr/16 +i)*16 +j <= end_addr) ){
				memory[start_addr/16 +i][j] = fill_val;
			}
		}
	}
}

void memory_reset (void) // 메모리의 모든 값을 0으로 초기화
{
	int i, j;
	if(strlen(com)!=5 || strncmp(com, "reset", 5) != 0){
		// 잘못된 명령어 입력에 대한 에러처리
		print_error("Input Command Error! (Suggested Command : reset)\n"); return;
	}

	add_history();
	for(i=0 ; i<65536 ; i++){
		for(j=0 ; j<16 ; j++){
			memory[i][j] = 0;
		}
	}
}

void opcode_opcode (void) // 입력받은 mnemonic의 opcode를 출력
{
	int sum=0, i;
	char *mnemonic; 

	mnemonic = strtok(NULL, " "); // 입력된 커맨드 중에서 mnemonic 으로 간주되는 문자열을 분리
	if(!mnemonic){
		print_error("Input Command Error! (opcode is missing)\n"); return;
	}
	tok = strtok(NULL, " "); // mnemonic 뒤에 문자가 더 있었는지 체크
	if(tok != NULL){
		print_error("Input Command Error! (Too much parameters)\n"); return;
	}
	
	// main함수에서 opcodelist를 생성할 때 mnemonic의 각 문자의 아스키코드값을 더한 것을
	// 20으로 나눈 나머지의 값에 해당하는 hashtable칸에 mnemonic 정보를 저장하였으므로
	// 입력받은 mnemonic에 해당하는 opcode를 찾아갈 때에도 마찬가지로
	// mnemonic의 각 문자의 아스키코드값을 더한 것을 20으로 나눈 나머지를 단서로 활용한다
	for(i=0 ; *(mnemonic +i) != 0 ; i++){
		sum += *(mnemonic +i);
	}
	sum %= 20;

	temp = (hashtable +sum);
	if(temp->opcode == -1){
		print_error("Input mnemonic is not in the table\n"); return;
	}
	while(temp){
		// 단서를 가지고 opcode를 찾아가는 작업
		if( strcmp(mnemonic, temp->mnemonic) == 0){
			strcpy(com, com_backup); // opcode_or_opcodelist 함수에서 보존한 원본커맨드를 불러와 히스토리에 저장
			add_history();
			printf("opcode is %02X\n", temp->opcode); return;
		}
		temp = temp->next;
	}
	print_error("Input mnemonic is not in the table\n"); return;
}

void opcode_opcodelist (void) // main함수에서 생성한 hashtable을 출력하는 작업
{
	int i;
	tok = strtok(NULL, " ");
	if (tok != NULL) {
		// 명령어 문자열 뒤에 문자가 더 있었을 경우. EX) "opcodelist pp"
		print_error("Input Command Error! (Too much parameters)\n"); return;
	}

	//출력부
	for(i=0 ; i<20 ; i++){
		printf("%2d : ", i);
		temp = hashtable + i;
		while(temp){
			if(temp->opcode == -1) break;
			printf("[%s,%02X]", temp->mnemonic, temp->opcode);
			if(temp->next != NULL){
				printf(" -> ");
			}
			temp = temp->next;
		}
		printf("\n");
	}
	strcpy(com, com_backup); // opcode_or_opcodelist 함수에서 보존한 원본커맨드를 불러와 히스토리에 저장
	add_history();
}

void help_or_history (void) // 명령어가 h로 시작할 경우
{
	switch( com[1] ){
	case 'i' : shell_history(); break;
	default : shell_help(); break;
	}
}

void dir_or_dump (void) // 명령어가 d로 시작할 경우
{
	switch( com[1] ){
	case 'u' : memory_dump(); break;
	default : shell_dir(); break;
	}
}

void opcode_or_opcodelist (void) // 명령어가 o로 시작할 경우
{
	strcpy(com_backup, com); // 원본 커맨드 보존
	tok = strtok(com, " ");
	switch(strlen(tok)){
	case 6 : if( strncmp(com, "opcode", 6) == 0) opcode_opcode(); break;
	case 10 : if( strncmp(com, "opcodelist", 10) == 0) opcode_opcodelist(); break;
	default : print_error("Input Command Error! (Suggested Command : opcode, opcodelist)\n"); return;
	}
}

void print_error (char *s) // 에러메시지 출력
{
	printf("%s", s);
	return ;
}

int remove_space (void) // 입력받은 커맨드에서 공백 및 탭의 중복을 제거하는 함수.
{
	char new_com[100]; // 중복이 제거된 문자열을 저장하는 배열
	int new_com_len=0;

	if( (tok = strtok(com, " \t")) == NULL){ // 공백과 탭으로 문자열을 분리
		print_error("Typing only ENTER or SPACE or TAB is not allowed!\n");
		return 0;
	}

	strcpy(new_com, tok);
	new_com_len += strlen(tok);

	while( (tok = strtok(NULL, " \t")) != NULL){
		// 공백과 탭을 제외한 문자의 입력이 이어질 경우 while문이 수행됨
		new_com[new_com_len] = ' ';
		new_com[new_com_len +1] = 0;
		strcat(new_com, tok); // 공백 한 칸을 사이에 두고, 공백&탭 중복으로 분리된 2개의 문자열을 잇는 작업

		new_com_len += strlen(tok);
		new_com_len++;
	}

	strcpy(com, new_com); // (구) 커맨드를 (신) 커맨드로 갱신
	return 1;
}

void add_history (void) // 정상 커맨드로 간주된 커맨드를 명령어 히스토리에 저장
{
	ptr *node = (ptr*)malloc(sizeof(ptr));
	node->next = NULL;
	if(!node){
		print_error("Memory allocation failed\n");
		exit(-1);
	}

	strcpy(node->command, com);
	if(head == NULL){
		head = node;
	}
	else{
		prev->next = node;
	}
	prev = node;

	return ;
}

void shell_type (void)
{
	int t;
	strcpy(com_backup, com); // 원본 커맨드 보존

	tok = strtok(com, " ");
	if(strlen(tok)!=4 || strncmp(tok, "type", 4)!=0){
		// 잘못된 명령어 입력에 대한 에러처리
		print_error("Input Command Error! (Suggested Command : type)\n"); return;
	}

	filename = strtok(NULL, " "); // 명령어 type의 operand(filename)가 입력되지 않은 경우
	if(!filename){
		print_error("Input Command Error! (filename is missing)\n"); return;
	}

	tok = strtok(NULL, " "); // filename 뒤에 문자가 더 있었는지 체크
	if(tok != NULL){
		print_error("Input Command Error! (Too much parameters)\n"); return;
	}
	if( !filenamecheck() ){ // 디렉토리 내에 filename이 존재하는지 체크하는 함수
		print_error("File is not in the directory!\n"); return;
	}
	if(!fp){
		print_error("File open Error!\n"); return ;
	}

	while(1){ // 파일 내용을 출력
		t = fgetc(fp);
		if(t==EOF)
			break;
		putchar(t);
	}

	strcpy(com, com_backup);
	add_history();
	fclose(fp);
}

void assemble (void)
{
	int line_num=0, ls, name=0, i, current=0;
	char *lstname, *objname, proname[10];
	textrecord tr; // Text Record 한 줄의 정보를 임시로 저장하는 구조체
	printSYMTAB = 0;

	strcpy(com_backup, com); // 원본 커맨드 보존

	tok = strtok(com, " ");
	if(strlen(tok)!=8 || strncmp(tok, "assemble", 8)!=0){
		// 잘못된 명령어 입력에 대한 에러처리
		print_error("Input Command Error! (Suggested Command : assemble)\n"); return;
	}

	filename = strtok(NULL, " "); // filename이 입력되지 않은 경우
	if(!filename){
		print_error("Input Command Error! (filename is missing)\n"); return;
	}

	tok = strtok(NULL, " "); // filename 뒤에 문자가 더 있었는지 체크
	if(tok != NULL){
		print_error("Input Command Error! (Too much parameters)\n"); return;
	}

	if( !filenamecheck() ){ // 디렉토리에 파일이 있는지 체크하여 파일오픈
		print_error("File is not in the directory!\n"); return;
	}
	if(!fp){
		print_error("File open Error!\n"); return ;
	}

	// 여기부터 본격적인 어셈블 작업 시작
	LOCCTR = 0; flagSTART = 0; startloc=0;

	// SYMTAB 초기화
	clearSYMTAB();

	while ( fgets(oneline, 499, fp) ) { // 파일 끝에 다다를 때까지 한 라인씩 읽음
		*(oneline + strlen(oneline) - 1) = 0; // fgets는 \n까지 받아들이므로 이를 \0으로 치환

		asmtemp = (asmb*)malloc(sizeof(asmb)); // 읽은 라인의 정보 초기화
		line_num += 5;
		asmtemp->comment = 0;
		asmtemp->line = line_num;
		asmtemp->loc = -1;
		asmtemp->format = 0;
		asmtemp->label_field = NULL;
		asmtemp->mnemonic_field = NULL;
		asmtemp->operand1 = NULL;
		asmtemp->operand2 = NULL;
		asmtemp->objcode = NULL;
		asmtemp->opcode = -1;
		asmtemp->objcodelen = 0;
		asmtemp->nextloc = -1;
		asmtemp->emptyline = 0;
		asmtemp->next = NULL;

		// 코드의 모든 라인에 대하여 pass1 (문법체크 및 SYMTAB에 label 삽입) 진행. pass1에서 에러 검출 시 0이 반환되어 if문이 실행됨
		// 주석이 아니라면 코드 요소가 분리되어 저장되지만, 주석일 경우 그 라인 전체를 label_field에 저장
		if(pass1() == 0){
			printSYMTAB = 0;
			// clearSYMTAB은 assemble 수행 초기에 한번씩 수행되므로 굳이 여기에서 수행할 필요는 없음
			freeasm();
			return;
		}

		if (line_num == 5) { // asmb 타입 node들을 할당순으로 연결
			asmhead = asmtemp;
		}
		else {
			asmprev->next = asmtemp;
		}
		asmprev = asmtemp;	
	}

	// 정리된 코드와 SYMTAB을 가지고 object code를 생성
	if(pass2() == 0){
		printSYMTAB = 0;
		freeasm();
		return;
	}


	// pass1 pass2가 무사히 끝났다면 filename.lst 파일과 filename.obj 파일을 생성하고 작업을 마친다

	lstname = (char*)malloc(sizeof(char)*strlen(filename));
	strcpy(lstname, filename);
	*(lstname + strlen(filename) -3) = 'l';
	*(lstname + strlen(filename) -2) = 's';
	*(lstname + strlen(filename) -1) = 't';
	lst = fopen(lstname, "w");
	
	// filename.lst 작업
	asmtemp = asmhead;
	while(asmtemp){ // lst파일을 작성하는 루프
		fprintf(lst, "%d\t", asmtemp->line); // line 작성

		if(asmtemp->loc == -1) fprintf(lst, "\t");
		else fprintf(lst, "%04X\t", asmtemp->loc); // loc 작성

		if(asmtemp->comment == 1){ // 주석일 경우 내용을 쓰고 다음 줄로
			fprintf(lst, "%s\n", asmtemp->label_field);
			asmtemp = asmtemp->next;
			continue;
		}
		if(asmtemp->label_field) fprintf(lst, "%s\t", asmtemp->label_field); // label이 존재하는 경우 작성
		else fprintf(lst, "\t");

		if(asmtemp->mnemonic_field) fprintf(lst, "%s\t", asmtemp->mnemonic_field); // mnemonic 작성
		else fprintf(lst, "\t");

		if(asmtemp->operand1) fprintf(lst, "%s", asmtemp->operand1); // operand1, 2 작성
		if(asmtemp->operand2){ fprintf(lst, ",%s\t", asmtemp->operand2);
			if(strlen(asmtemp->operand1) + strlen(asmtemp->operand2) < 7) fprintf(lst, "\t");
		}
		else fprintf(lst, "\t\t");

		if(asmtemp->objcode){ // object code 기록
			for(ls=0 ; ls<asmtemp->objcodelen ; ls++){
				fprintf(lst, "%02X", *(asmtemp->objcode +ls));
			}
			fprintf(lst, "\n");
		}
		else fprintf(lst, "\n");
		
		asmtemp = asmtemp->next;
	}

	// filename.obj 작업
	objname = (char*)malloc(sizeof(char)*strlen(filename));
	strcpy(objname, filename);
	*(objname + strlen(filename) -3) = 'o';
	*(objname + strlen(filename) -2) = 'b';
	*(objname + strlen(filename) -1) = 'j';
	obj = fopen(objname, "w");

	asmtemp = asmhead;
	while(asmtemp){ // endloc 값을 찾는 루프. startloc은 pass1 과정 중에 저장되었음
		if(asmtemp->loc != -1){
			endloc = asmtemp->loc;

			if(name==0){ // program name 저장
				strncpy(proname, asmtemp->label_field, 8);
				name = 1;
			}
		}
		asmtemp = asmtemp->next;
	}
	
	fprintf(obj, "H%-6s%06X%06X\n", proname, startloc, endloc-startloc+1); // Header Record 기록
	
	asmtemp = asmhead; // Text Record 기록
	tr.startaddr = -1; tr.length = 0;
	while(asmtemp){
		if(asmtemp->objcode){ // objcode가 있는 라인일 때
			if(tr.startaddr == -1){ // 현재 기록중인 TextRecord의 start address가 비어있다면
				tr.startaddr = asmtemp->loc;
				tr.length += asmtemp->objcodelen;
				sprintf(tr.objcode, "%02X", *(asmtemp->objcode) ); // 기록중인 Record의 첫번째 objcode 삽입
				for(i=1 ; i < asmtemp->objcodelen ; i++){
					sprintf(tr.objcode, "%s%02X", tr.objcode, *(asmtemp->objcode +i) );
				}
			}
			else{
				if(tr.length + asmtemp->objcodelen <= 30){
					// 기록중인 TextRecord에 현재 탐색중인 objcode를 추가할 만큼 공간이 남아있다면
					tr.length += asmtemp->objcodelen;
	
					sprintf(tr.objcode, "%s%02X", tr.objcode, *(asmtemp->objcode) ); // 삽입
					for(i=1 ; i < asmtemp->objcodelen ; i++){
						sprintf(tr.objcode, "%s%02X", tr.objcode, *(asmtemp->objcode +i) );
					}
				}
				else{ // 공간이 부족하다면 작성중이던 Record를 파일에 쓰고 레코드 초기화
					fprintf(obj, "T%06X%02X", tr.startaddr, tr.length);
					for(i=0 ; i < tr.length *2 ; i++){
						fprintf(obj, "%c", *(tr.objcode +i) );
					}
					fprintf(obj, "\n");
					tr.startaddr = -1; tr.length = 0;
					continue;
				}
			}
			asmtemp = asmtemp->next;
		}
		else{ // objcode가 없는 라인일 때
			if(tr.startaddr!=-1 && asmtemp->mnemonic_field && (strcmp(asmtemp->mnemonic_field, "RESB")==0 || strcmp(asmtemp->mnemonic_field, "RESW")==0)){
				// objcode는 없지만, mnemonic이 RESW 또는 RESB라면 작성중이던 임시레코드를 파일에 쓰고, 새 레코드를 작성한다
				fprintf(obj, "T%06X%02X", tr.startaddr, tr.length);
				for(i=0 ; i < tr.length *2 ; i++){
					fprintf(obj, "%c", *(tr.objcode +i) );
				}
				fprintf(obj, "\n");
				tr.startaddr = -1; tr.length = 0;
			}
			asmtemp = asmtemp->next;
		}
	}
	// 레코드 작성 도중에 코드가 끝나서 루프가 종료된 경우, 그 시점까지 작성중이던 레코드를 파일에 기록
	fprintf(obj, "T%06X%02X", tr.startaddr, tr.length);
	for(i=0 ; i < tr.length *2 ; i++){
		fprintf(obj, "%c", *(tr.objcode +i) );
	}
	fprintf(obj, "\n");
	
	asmtemp = asmhead; // Modification Record 기록 ( 필요한 경우 )
	while (asmtemp) {
		if (asmtemp->objcodelen != 0) { // Modification Record 기록 시, 수정해야 할 위치를 제시하기 위한 연산
			current += asmtemp->objcodelen;
		}
		if (asmtemp->format == 3 && *(asmtemp->mnemonic_field) == '+') { // 4형식인 경우 (direct addressing 후보)
			// operand가 상수라면 패스, 그 외의 경우라면 Modification Record 작성 필요
			if (*(asmtemp->operand1) == '#' || *(asmtemp->operand1) == '@') {
				if (strlen(asmtemp->operand1 + 1) == strspn(asmtemp->operand1 + 1, "0123456789")) { // 상수라면
					asmtemp = asmtemp->next;
					continue;
				}
				else { // 상수가 아니라면 Modification Record 작성
					fprintf(obj, "M%06X05\n", current - 3);
				}
			}
			else { // operand의 첫 문자가 #나 @이 아니었을 경우
				if (strlen(asmtemp->operand1) == strspn(asmtemp->operand1, "0123456789")) { // 상수라면
					asmtemp = asmtemp->next;
					continue;
				}
				else { // 상수가 아니라면 Modification Record 작성
					fprintf(obj, "M%06X05\n", current - 3);
				}
			}

		}
		asmtemp = asmtemp->next;
	}

	fprintf(obj, "E%06X\n", startloc); // 마지막으로 End Record 작성
	

	printf("\toutput file : [%s], [%s]\n", lstname, objname);

	printSYMTAB = 1; // printSYMTAB을 1로 설정해야 SYMTAB이 출력됨
	strcpy(com, com_backup);
	add_history();
	fclose(fp); fclose(lst); fclose(obj);
	freeasm();
}

void symbol (void) // printSYMTAB이 1이면 SYMTAB 출력, 0이면 add_history만 수행하고 종료
{
	int i;
	if(strlen(com)!=6 || strncmp(com, "symbol", 6) != 0){
		// 잘못된 명령어 입력에 대한 에러처리
		print_error("Input Command Error! (Suggested Command : symbol)\n"); return;
	}
	add_history();
	if(!printSYMTAB){ // assemble 도중 문제가 생겨서 printSYMTAB이 0으로 설정된 경우
		return ;
	}
	else{ // assemble이 정상적으로 수행되어 printSYMTAB이 1로 유지된 경우
		for(i=0 ; i<55 ; i++){
			symtemp = (SYMTAB +i)->next;
			while(symtemp){
				printf("\t%s\t%04X\n", symtemp->label, symtemp->loc);
				symtemp = symtemp->next;
			}
		}
	}
}

int pass1 (void)
{
	char *f1=NULL, *f2=NULL, *f3=NULL, *f4=NULL, *f5=NULL; // 작업하는 라인에서 tokenize하는 각 요소의 주소를 포인팅
	int n; // format2에서 인자 n 검사 역할과 이후 16진수 체크부에 쓰임
	if (oneline[0] == '.') { // 읽은 라인이 주석일 경우 그 라인 전체를 label_field에 저장
		asmtemp->comment = 1;
		asmtemp->label_field = (char*)malloc(sizeof(char)*(strlen(oneline) +1));
		strcpy(asmtemp->label_field, oneline);
		return 1;
	}

	f1 = strtok(oneline, " ,\r\t\n");
	if(f1){ // 코드가 빈 줄이 아닐 때
		flagSTART++; // 코드 한 줄(주석 제외) 읽을 때마다 1씩 추가
		if(check_if_opcode(f1) == 0){ // 첫 인자가 label이라고 간주될 경우, mnemonic이 반드시 이어서 나와야 한다
			f2 = strtok(NULL, " ,\r\t\n");
			if(!f2){ // label 뒤에 (mnemonic이 있었어야 할 자리에) 코드가 비어있는 경우
				printf("Error detected on line %d. May be opcode is missing.\n", asmtemp->line);
				return 0;
			}
			else{ // label 뒤에 mnemonic으로 간주되는 코드가 입력된 경우
				if(check_if_opcode(f2) == 0){ // (mnemonic이었어야 할) 코드가 mnemonic이 아니었다면
					printf("Error detected on line %d. Check if your opcode is correct.\n", asmtemp->line);
					return 0;
				}
				else{ // label 뒤에 mnemonic이 정상적으로 입력된 경우
					switch(check_if_opcode(f2)){
					
					// mnemonic이 RSUB이거나 format이 1형식인 경우 mnemonic 뒤에 아무 입력도 없었어야 한다
					case 1 : f3 = strtok(NULL, " ,\r\t\n");
						if(f3){ // mnemonic이 RSUB이거나 1형식인데 mnemonic 뒤에 무언가 입력이 이어졌을 경우
							printf("Error detected on line %d. %s cannot have operands\n", asmtemp->line, f2);
							return 0;
						}
						else{ // 정상적인 1형식(혹은 RSUB) 입력이 주어진 경우 LOCCTR, format, label, mnemonic 정보 저장
							  asmtemp->loc = LOCCTR; LOCCTR += 1;
							  asmtemp->format = 1;
							  asmtemp->label_field = (char*)malloc(sizeof(char)*(strlen(f1) +1));
							  strcpy(asmtemp->label_field, f1);
							  asmtemp->mnemonic_field = (char*)malloc(sizeof(char)*(strlen(f2) +1));
							  strcpy(asmtemp->mnemonic_field, f2);
							  
							  if( (strcmp(f2, "RSUB") && strcmp(f2, "+RSUB")) == 0){
								  asmtemp->format = 3; LOCCTR += 2; // 3형식인 경우 LOCCTR 총 3 증가
								  if(f2[0]=='+') LOCCTR += 1; // 3형식인 경우 LOCCTR 총 4 증가
							  }
							  asmtemp->nextloc = LOCCTR;
						}
						break;
					
					// opcode format이 2형식이고 operand로 r1 또는 n(SVC)이 오는 경우
					case 21 : f3 = strtok(NULL, " ,\r\t\n");
						if(!f3){ // operand1(r1 or n)이 입력되지 않았다면
							printf("Error detected on line %d. May be operand(r1 or n) is missing.\n", asmtemp->line);
							return 0;
						}
						else{ // (r1 or n으로 간주되는) operand1이 정상적으로 입력된 경우
							if( strcmp(f2, "SVC") == 0 ){ // operand1이 n이어야 하는 경우

								sscanf(f3, "%x", &n); n--;
								if(strlen(f3) != strspn(f3, "0123456789ABCDEFabcdef")){ // n이 16진수 이외의 문자를 가질 경우
									printf("Error detected on line %d. Check if your operand(n) is correct.\n", asmtemp->line);
									return 0;
								}
								else if(n<0 || n>15){ // n이 16진수 값을 가지지만 범위가 0~F 밖일 경우
									printf("Error detected on line %d. Check the range of n (0~F).\n", asmtemp->line);
									return 0;
								}
								else{ // 정상적으로 n이 입력된 경우
									f4 = strtok(NULL, " ,\r\t\n");
									if(f4){ // n 뒤에 무언가 입력이 이어졌을 경우
										printf("Error detected on line %d. %s can have only one operand(n).\n", asmtemp->line, f2);
										return 0;
									}
									else{ // 정상적인 2-1형식 입력이 주어진 경우 LOCCTR, format, label, mnemonic, operand1 정보 저장
										asmtemp->loc = LOCCTR; LOCCTR += 2;
										asmtemp->format = 21;
										asmtemp->label_field = (char*)malloc(sizeof(char)*(strlen(f1) +1));
										strcpy(asmtemp->label_field, f1);
										asmtemp->mnemonic_field = (char*)malloc(sizeof(char)*(strlen(f2) +1));
										strcpy(asmtemp->mnemonic_field, f2);
										asmtemp->operand1 = (char*)malloc(sizeof(char)*(strlen(f3) +1));
										strcpy(asmtemp->operand1, f3);
										asmtemp->nextloc = LOCCTR;
									}
								}
							}
							else{ // operand1이 r1이어야 하는 경우
								if(check_if_register(f3) == 0){ // operand1이 레지스터가 아니라면
									printf("Error detected on line %d. Check if your operand(r1) is correct.\n", asmtemp->line);
									return 0;																
								}
								else{ // operand1이 레지스터가 맞다면
									f4 = strtok(NULL, " ,\r\t\n");
									if(f4){ // r1 뒤에 무언가 입력이 이어졌을 경우
										printf("Error detected on line %d. %s can have only one operand(r1).\n", asmtemp->line, f2);
										return 0;
									}
									else{ // 정상적인 2-1형식 입력이 주어진 경우 LOCCTR, format, label, mnemonic, operand1 정보 저장
										asmtemp->loc = LOCCTR; LOCCTR += 2;
										asmtemp->format = 21;
										asmtemp->label_field = (char*)malloc(sizeof(char)*(strlen(f1) +1));
										strcpy(asmtemp->label_field, f1);
										asmtemp->mnemonic_field = (char*)malloc(sizeof(char)*(strlen(f2) +1));
										strcpy(asmtemp->mnemonic_field, f2);
										asmtemp->operand1 = (char*)malloc(sizeof(char)*(strlen(f3) +1));
										strcpy(asmtemp->operand1, f3);
										asmtemp->nextloc = LOCCTR;
									}
								}
							}
						}
						break;

					// opcode format이 2형식이고 operand로 r1, r2가 오는 경우
					case 22 : f3 = strtok(NULL, " ,\r\t\n");
						if(!f3){ // operand1(r1)이 입력되지 않았다면
							printf("Error detected on line %d. May be operand(r1) is missing.\n", asmtemp->line);
							return 0;
						}
						else{ // (r1으로 간주되는) operand1이 정상적으로 입력된 경우
							if(check_if_register(f3) == 0){ // operand1이 레지스터가 아니라면
								printf("Error detected on line %d. Check if your operand(r1) is correct.\n", asmtemp->line);
								return 0;
							}
							else{ // operand1이 레지스터가 맞다면 operand2가 이어서 입력되어야 함
								f4 = strtok(NULL, " ,\r\t\n");
								if(!f4){ // operand2(r2)가 입력되지 않았다면
									printf("Error detected on line %d. May be operand(r2) is missing.\n", asmtemp->line);
									return 0;
								}
								else{ // (r2로 간주되는) operand2가 정상적으로 입력된 경우
									if(check_if_register(f4) == 0){ // operand2가 레지스터가 아니라면
										printf("Error detected on line %d. Check if your operand(r2) is correct.\n", asmtemp->line);
										return 0;
									}
									/*
										r1, r2가 달라야 한다면 여기에 else if문을 추가
									*/
									else{ // operand2가 레지스터가 맞다면
										f5 = strtok(NULL, " ,\r\t\n");
										if(f5){ // r2 뒤에 무언가 입력이 이어졌을 경우
											printf("Error detected on line %d. %s cannot have three operands or more.\n", asmtemp->line, f2);
											return 0;
										}
										else{ // 정상적인 2-2형식 입력이 주어진 경우 LOCCTR, format, label, mnemonic, operand1, operand2 정보 저장
											asmtemp->loc = LOCCTR; LOCCTR += 2;
											asmtemp->format = 22;
											asmtemp->label_field = (char*)malloc(sizeof(char)*(strlen(f1) +1));
											strcpy(asmtemp->label_field, f1);
											asmtemp->mnemonic_field = (char*)malloc(sizeof(char)*(strlen(f2) +1));
											strcpy(asmtemp->mnemonic_field, f2);
											asmtemp->operand1 = (char*)malloc(sizeof(char)*(strlen(f3) +1));
											strcpy(asmtemp->operand1, f3);
											asmtemp->operand2 = (char*)malloc(sizeof(char)*(strlen(f4) +1));
											strcpy(asmtemp->operand2, f4);
											asmtemp->nextloc = LOCCTR;
										}
									}
								}
							}
						}
						break;

					// opcode format이 2형식이고 operand로 r1, n이 오는 경우
					case 23 : f3 = strtok(NULL, " ,\r\t\n");
						if(!f3){ // operand1(r1)이 입력되지 않았다면
							printf("Error detected on line %d. May be operand(r1) is missing.\n", asmtemp->line);
							return 0;
						}
						else{ // (r1으로 간주되는) operand1이 정상적으로 입력된 경우
							if(check_if_register(f3) == 0){ // operand1이 레지스터가 아니라면
								printf("Error detected on line %d. Check if your operand(r1) is correct.\n", asmtemp->line);
								return 0;
							}
							else{ // operand1이 레지스터가 맞다면 operand2(n)가 이어서 입력되어야 함
								f4 = strtok(NULL, " ,\r\t\n");
								if(!f4){ // operand2(n)가 입력되지 않았다면
									printf("Error detected on line %d. May be operand(n) is missing.\n", asmtemp->line);
									return 0;
								}
								else{ // (n으로 간주되는) operand2가 정상적으로 입력된 경우
									sscanf(f4, "%x", &n); n--;
									if(strlen(f4) != strspn(f4, "0123456789ABCDEFabcdef")){ // n이 16진수 이외의 문자를 가질 경우
										printf("Error detected on line %d. Check if your operand(n) is correct.\n", asmtemp->line);
										return 0;
									}
									else if(n<0 || n>15){ // n이 16진수 값을 가지지만 범위가 0~F 밖일 경우
										printf("Error detected on line %d. Check the range of n (0~F).\n", asmtemp->line);
										return 0;
									}
									else{ // n이 정상적으로 입력되었다고 간주될 경우
										f5 = strtok(NULL, " ,\r\t\n");
										if(f5){ // n 뒤에 무언가 입력이 이어졌을 경우
											printf("Error detected on line %d. %s cannot have three operands or more.\n", asmtemp->line, f2);
											return 0;
										}
										else{ // 정상적인 2-3형식 입력이 주어진 경우 LOCCTR, format, label, mnemonic, operand1, operand2 정보 저장
											asmtemp->loc = LOCCTR; LOCCTR += 2;
											asmtemp->format = 23;
											asmtemp->label_field = (char*)malloc(sizeof(char)*(strlen(f1) +1));
											strcpy(asmtemp->label_field, f1);
											asmtemp->mnemonic_field = (char*)malloc(sizeof(char)*(strlen(f2) +1));
											strcpy(asmtemp->mnemonic_field, f2);
											asmtemp->operand1 = (char*)malloc(sizeof(char)*(strlen(f3) +1));
											strcpy(asmtemp->operand1, f3);
											asmtemp->operand2 = (char*)malloc(sizeof(char)*(strlen(f4) +1));
											strcpy(asmtemp->operand2, f4);
											asmtemp->nextloc = LOCCTR;
										}
									}
								}
							}
						}
						break;

					// opcode format이 3/4형식인 경우 (RSUB 제외) operand(m)가 이어져야 한다
					case 3 : f3 = strtok(NULL, " ,\r\t\n");
						if(!f3){ // operand1(m)이 입력되지 않았다면
							printf("Error detected on line %d. May be operand(r1) is missing.\n", asmtemp->line);
							return 0;
						}
						else{ // (m으로 간주되는) operand1이 정상적으로 입력된 경우
							f4 = strtok(NULL, " ,\r\t\n");
							if(f4){ // m 뒤에 무언가 입력이 이어졌을 경우 (operand2)
								if(strcmp(f4, "X") != 0){ // operand2가 X가 아니라면
									printf("Error detected on line %d. Check if your operand(maybe X) is correct.\n", asmtemp->line);
									return 0;
								}
								else{ // operand2가 X였다면 저장
									asmtemp->operand2 = (char*)malloc(sizeof(char)*(strlen(f4) +1));
									strcpy(asmtemp->operand2, f4);
								}
							}
							// 정상적인 3/4형식 입력이 주어진 경우 (RSUB 제외)
							// LOCCTR, format, label, mnemonic, operand1 정보 저장
							// extend mode일 경우 LOCCTR 1 더 증가
							asmtemp->loc = LOCCTR;  if(f2[0]!='+') LOCCTR += 3; else LOCCTR += 4;		
							asmtemp->format = 3;
							asmtemp->label_field = (char*)malloc(sizeof(char)*(strlen(f1) +1));
							strcpy(asmtemp->label_field, f1);
							asmtemp->mnemonic_field = (char*)malloc(sizeof(char)*(strlen(f2) +1));
							strcpy(asmtemp->mnemonic_field, f2);
							asmtemp->operand1 = (char*)malloc(sizeof(char)*(strlen(f3) +1));
							strcpy(asmtemp->operand1, f3);	
							asmtemp->nextloc = LOCCTR;
						}
						break;
					/*
						case 3에서 m 값으로 들어온 symbol이 valid한 것인지는 pass1에서 확인할 수 없다.
						pass2에서, 즉 SYMTAB이 완성된 시점에서 symbol의 valid 유무의 판별이 가능.
					*/

					// opcode가 START, BYTE, WORD, RESB, RESW 중 하나일 경우 (label 있음)
					case 5 : 
						switch(f2[3]){ // mnemonic의 4번째 알파벳으로 START, BYTE, WORD, RESB, RESW를 구분
						
						// START일 경우 operand(start addr)가 이어져야 한다.
						case 'R' : f3 = strtok(NULL, " ,\r\t\n");
							if(flagSTART != 1){ // START가 코드의 첫 줄이 아닌 위치에 등장한 경우
								printf("Error detected on line %d. START must be on top of the code.\n", asmtemp->line);
								return 0;
							}
							if(!f3){ // start address가 입력되지 않았다면
								printf("Error detected on line %d. May be Starting Address is missing.\n", asmtemp->line);
								return 0;
							}
							else{ // start address로 간주되는 인자가 입력되었다면
								sscanf(f3, "%x", &n);
								if(strlen(f3) != strspn(f3, "0123456789ABCDEFabcdef")){ // start addr가 16진수 이외의 문자를 가질 경우
									printf("Error detected on line %d. Check if your Starting Address is correct.\n", asmtemp->line);
									return 0;
								}
								else if(n > 0xFFFF){ // n이 FFFF보다 큰 값을 갖는 경우 (메모리 범위 초과)
									printf("Error detected on line %d. Check the range of Starting Address (0~FFFF).\n", asmtemp->line);
									return 0;
								}
								else{ // 정상적인 16진수 address가 operand로 입력된 경우
									f4 = strtok(NULL, " ,\r\t\n");
									if(f4){ // start addr 뒤에 무언가 입력이 이어졌을 경우
										printf("Error detected on line %d. %s can have only one operand(Starting Address).\n", asmtemp->line, f2);
										return 0;
									}
									else{ // 정상적인 START 형식 입력이 들어온 경우 LOCCTR, format, label, mnemonic, operand1 정보 저장
										startloc = n;
										asmtemp->loc = n; LOCCTR = n;
										asmtemp->format = 5;
										asmtemp->label_field = (char*)malloc(sizeof(char)*(strlen(f1) +1));
										strcpy(asmtemp->label_field, f1);
										asmtemp->mnemonic_field = (char*)malloc(sizeof(char)*(strlen(f2) +1));
										strcpy(asmtemp->mnemonic_field, f2);
										asmtemp->operand1 = (char*)malloc(sizeof(char)*(strlen(f3) +1));
										strcpy(asmtemp->operand1, f3);
										asmtemp->nextloc = LOCCTR;
									}
								}
							}
							break;

						// BYTE일 경우 operand(C'string' 또는 X'16진수')가 이어져야 한다
						case 'E' : f3 = strtok(NULL, " ,\r\t\n");
							if(!f3){ // operand(C'string' 또는 X'16진수')가 입력되지 않았다면
								printf("Error detected on line %d. May be operand(C or X) is missing.\n", asmtemp->line);
								return 0;
							}
							else{ // operand(C'string' 또는 X'16진수')로 간주되는 인자가 입력되었다면
								if( (f3[0]!='C')&&(f3[0]!='X') ){ // 그 인자의 첫 문자가 C도 X도 아니라면
									printf("Error detected on line %d. Check if your operand(C or X) is correct.\n", asmtemp->line);
									return 0;
								}
								else if(strlen(f3) <= 3){ // 첫 문자가 C 또는 X이지만 operand의 길이가 3(C'' or X'') 이하인 경우
									printf("Error detected on line %d. Check your operand's format (C'string' or X'Hex').\n", asmtemp->line);
									return 0;
								}
								else{ // operand format (C'string' or X'Hex')이 정상적인 경우
									if( (f3[1]!=39) || *(f3 + strlen(f3) -1) != 39 ){ // '가 있어야 할 자리에 '가 없는 경우
										printf("Error detected on line %d. May be ' is missing.\n", asmtemp->line);
										return 0;
									}
									// C'string'의 경우 string의 형식제약사항이 특별히 없기 때문에 여기까지만 검사해도 되지만
									// X'Hex'의 경우 Hex 부분이 16진수가 맞는지 확인할 필요가 있다.
									if( (f3[0]=='X') && ( (strlen(f3 +2) -1) != strspn(f3 +2, "0123456789ABCDEFabcdef") )){ // Hex가 16진수 이외의 문자를 가질 경우
										printf("Error detected on line %d. Check if your operand(Hex) is correct.\n", asmtemp->line);
										return 0;
									}
									// 여기까지 체크가 끝났다면 일단 입력받은 양식 (C'String' or X'Hex') 그대로 asmtemp->operand1에 저장,
									// 나중에 pass2에서 object code를 생성할 때 byte 단위로 문자열을 쪼개서 주소연산에 쓰게 된다.
									
									f4 = strtok(NULL, " ,\r\t\n");
									if(f4){ // operand(C'String' or X'Hex') 뒤에 무언가 입력이 이어졌을 경우
										printf("Error detected on line %d. %s can have only one operand(C'String' or X'Hex').\n", asmtemp->line, f2);
										return 0;
									}
									else{ // 정상적인 BYTE 형식 입력이 들어온 경우 LOCCTR, format, label, mnemonic, operand1 정보 저장
										asmtemp->loc = LOCCTR;
										asmtemp->format = 5;
										asmtemp->label_field = (char*)malloc(sizeof(char)*(strlen(f1) +1));
										strcpy(asmtemp->label_field, f1);
										asmtemp->mnemonic_field = (char*)malloc(sizeof(char)*(strlen(f2) +1));
										strcpy(asmtemp->mnemonic_field, f2);
										asmtemp->operand1 = (char*)malloc(sizeof(char)*(strlen(f3) +1));
										strcpy(asmtemp->operand1, f3);

										//BYTE C형식의 경우 String 길이만큼, X형식의 경우 byte 수만큼 LOCCTR 추가
										if(f3[0]=='C'){
											LOCCTR = LOCCTR + strlen(f3) - 3;
										}
										else{
											sscanf(f3 +2, "%X", &n);

											LOCCTR++;
											while(n){ // 16진수 n을 2byte씩 끊어서 1칸씩 LOCCTR 증가											
												n = n / 256; // 0x100
												if(n) LOCCTR++;
											}
										}
										asmtemp->nextloc = LOCCTR;
									}
								}
							}
							break;
							
						// WORD, RESW, RESB일 경우 뒤에 10진수 상수가 이어진다 (아마 0~FFFF)
						default : f3 = strtok(NULL, " ,\r\t\n");
							if(!f3){ // operand(constant)가 입력되지 않았다면
								printf("Error detected on line %d. May be operand(constant) is missing.\n", asmtemp->line);
								return 0;
							}
							else{ // constant로 간주되는 operand 입력이 들어왔을 경우								
								if(strlen(f3) != strspn(f3, "0123456789")){ // operand가 constant가 아닐 경우
									printf("Error detected on line %d. Check if your operand(constant) is correct.\n", asmtemp->line);
									return 0;
								}
								else{ // constant만 제대로 입력된 경우
									sscanf(f3, "%d", &n);
									f4 = strtok(NULL, " ,\r\t\n");
									if(f4){ // operand(constant) 뒤에 무언가 입력이 이어졌을 경우
										printf("Error detected on line %d. %s can have only one operand(constant).\n", asmtemp->line, f2);
										return 0;
									}
									else if(n > 0xFFFF){ // constant가 정상 범위(0~FFFF) 밖에 있을 경우
										printf("Error detected on line %d. Check the range of the operand (0~FFFF).\n", asmtemp->line);
										return 0;
									}
									else{ // 정상적인 WORD, RESW, RESB 형식 입력이 들어온 경우 LOCCTR, format, label, mnemonic, operand1 정보 저장
										asmtemp->loc = LOCCTR;
										asmtemp->format = 5;
										asmtemp->label_field = (char*)malloc(sizeof(char)*(strlen(f1) +1));
										strcpy(asmtemp->label_field, f1);
										asmtemp->mnemonic_field = (char*)malloc(sizeof(char)*(strlen(f2) +1));
										strcpy(asmtemp->mnemonic_field, f2);
										asmtemp->operand1 = (char*)malloc(sizeof(char)*(strlen(f3) +1));
										strcpy(asmtemp->operand1, f3);
										
										//WORD의 경우  LOC+3, RESW의 경우 operand 상수값에 3배만큼, RESB의 경우 operand(10진수->Hex)만큼 LOC 증가
										if( f2[3]=='D'){
											LOCCTR += 3;
										}
										else if ( f2[3]=='W' ){
											LOCCTR += n*3;
										}
										else{
											LOCCTR += n;
										}
										asmtemp->nextloc = LOCCTR;
									}
								}
							}
						}
					} // label 뒤에 mnemonic이 정상적으로 입력된 경우의 switch문 종료
				}
			}
			// 여기까지 왔다는 것은 입력받은 코드(label이 포함되어있는)가 문법상 오류가 없었음을 의미
			// 따라서 label이 SYMTAB에 없는지 체크한 뒤, SYMTAB에 추가하는 작업이 필요함

			if( !addSYMTAB() ){ // SYMTAB에 이미 label이 존재할 경우
				printf("Error detected on line %d. Label is already in SYMTAB.\n", asmtemp->line);
				return 0;
			}
			else{ // SYMTAB에 label을 추가하는 작업까지 끝났을 경우, 즉 무사히 pass1을 마친 경우
				return 1;
			}
		}

//   아래부터는 label이 입력되지 않은 경우에 대한 작업
		else{ // 첫 인자가 opcode였을 경우, 즉 label 필드가 비어있었을 경우
			f2 = f1; // mnemonic을 포인팅하고 있는 f1의 주소를 f2에서 받아서 사용
					 // 그리고 f1은 사용하지 않음
			switch(check_if_opcode(f2)){
			
			// mnemonic이 RSUB이거나 format이 1형식인 경우 mnemonic 뒤에 아무 입력도 없었어야 한다
			case 1 : f3 = strtok(NULL, " ,\r\t\n");
				if(f3){ // mnemonic이 RSUB이거나 1형식인데 mnemonic 뒤에 무언가 입력이 이어졌을 경우
					printf("Error detected on line %d. %s cannot have operands\n", asmtemp->line, f2);
					return 0;
				}
				else{ // 정상적인 1형식(혹은 RSUB) 입력이 주어진 경우 LOCCTR, format, mnemonic 정보 저장
					  asmtemp->loc = LOCCTR; LOCCTR += 1;
					  asmtemp->format = 1;
					  asmtemp->mnemonic_field = (char*)malloc(sizeof(char)*(strlen(f2) +1));
					  strcpy(asmtemp->mnemonic_field, f2);
					  
					  if( (strcmp(f2, "RSUB") && strcmp(f2, "+RSUB")) == 0){
						  asmtemp->format = 3; LOCCTR += 2; // 3형식인 경우 LOCCTR 총 3 증가
						  if(f2[0]=='+') LOCCTR += 1; // 3형식인 경우 LOCCTR 총 4 증가
					  }
					  asmtemp->nextloc = LOCCTR;
				}
				break;
			
			// opcode format이 2형식이고 operand로 r1 또는 n(SVC)이 오는 경우
			case 21 : f3 = strtok(NULL, " ,\r\t\n");
				if(!f3){ // operand1(r1 or n)이 입력되지 않았다면
					printf("Error detected on line %d. May be operand(r1 or n) is missing.\n", asmtemp->line);
					return 0;
				}
				else{ // (r1 or n으로 간주되는) operand1이 정상적으로 입력된 경우
					if( strcmp(f2, "SVC") == 0 ){ // operand1이 n이어야 하는 경우

						sscanf(f3, "%x", &n); n--;
						if(strlen(f3) != strspn(f3, "0123456789ABCDEFabcdef")){ // n이 16진수 이외의 문자를 가질 경우
							printf("Error detected on line %d. Check if your operand(n) is correct.\n", asmtemp->line);
							return 0;
						}
						else if(n<0 || n>15){ // n이 16진수 값을 가지지만 범위가 0~F 밖일 경우
							printf("Error detected on line %d. Check the range of n (0~F).\n", asmtemp->line);
							return 0;
						}
						else{ // 정상적으로 n이 입력된 경우
							f4 = strtok(NULL, " ,\r\t\n");
							if(f4){ // n 뒤에 무언가 입력이 이어졌을 경우
								printf("Error detected on line %d. %s can have only one operand(n).\n", asmtemp->line, f2);
								return 0;
							}
							else{ // 정상적인 2-1형식 입력이 주어진 경우 LOCCTR, format, mnemonic, operand1 정보 저장
								asmtemp->loc = LOCCTR; LOCCTR += 2;
								asmtemp->format = 21;
								asmtemp->mnemonic_field = (char*)malloc(sizeof(char)*(strlen(f2) +1));
								strcpy(asmtemp->mnemonic_field, f2);
								asmtemp->operand1 = (char*)malloc(sizeof(char)*(strlen(f3) +1));
								strcpy(asmtemp->operand1, f3);
								asmtemp->nextloc = LOCCTR;
							}
						}
					}
					else{ // operand1이 r1이어야 하는 경우
						if(check_if_register(f3) == 0){ // operand1이 레지스터가 아니라면
							printf("Error detected on line %d. Check if your operand(r1) is correct.\n", asmtemp->line);
							return 0;																
						}
						else{ // operand1이 레지스터가 맞다면
							f4 = strtok(NULL, " ,\r\t\n");
							if(f4){ // r1 뒤에 무언가 입력이 이어졌을 경우
								printf("Error detected on line %d. %s can have only one operand(r1).\n", asmtemp->line, f2);
								return 0;
							}
							else{ // 정상적인 2-1형식 입력이 주어진 경우 LOCCTR, format, mnemonic, operand1 정보 저장
								asmtemp->loc = LOCCTR; LOCCTR += 2;
								asmtemp->format = 21;
								asmtemp->mnemonic_field = (char*)malloc(sizeof(char)*(strlen(f2) +1));
								strcpy(asmtemp->mnemonic_field, f2);
								asmtemp->operand1 = (char*)malloc(sizeof(char)*(strlen(f3) +1));
								strcpy(asmtemp->operand1, f3);
								asmtemp->nextloc = LOCCTR;
							}
						}
					}
				}
				break;

				// opcode format이 2형식이고 operand로 r1, r2가 오는 경우
			case 22 : f3 = strtok(NULL, " ,\r\t\n");
				if(!f3){ // operand1(r1)이 입력되지 않았다면
					printf("Error detected on line %d. May be operand(r1) is missing.\n", asmtemp->line);
					return 0;
				}
				else{ // (r1으로 간주되는) operand1이 정상적으로 입력된 경우
					if(check_if_register(f3) == 0){ // operand1이 레지스터가 아니라면
						printf("Error detected on line %d. Check if your operand(r1) is correct.\n", asmtemp->line);
						return 0;
					}
					else{ // operand1이 레지스터가 맞다면 operand2가 이어서 입력되어야 함
						f4 = strtok(NULL, " ,\r\t\n");
						if(!f4){ // operand2(r2)가 입력되지 않았다면
							printf("Error detected on line %d. May be operand(r2) is missing.\n", asmtemp->line);
							return 0;
						}
						else{ // (r2로 간주되는) operand2가 정상적으로 입력된 경우
							if(check_if_register(f4) == 0){ // operand2가 레지스터가 아니라면
								printf("Error detected on line %d. Check if your operand(r2) is correct.\n", asmtemp->line);
								return 0;
							}
							/*
								r1, r2가 달라야 한다면 여기에 else if문을 추가
							*/
							else{ // operand2가 레지스터가 맞다면
								f5 = strtok(NULL, " ,\r\t\n");
								if(f5){ // r2 뒤에 무언가 입력이 이어졌을 경우
									printf("Error detected on line %d. %s cannot have three operands or more.\n", asmtemp->line, f2);
									return 0;
								}
								else{ // 정상적인 2-2형식 입력이 주어진 경우 LOCCTR, format, mnemonic, operand1, operand2 정보 저장
									asmtemp->loc = LOCCTR; LOCCTR += 2;
									asmtemp->format = 22;
									asmtemp->mnemonic_field = (char*)malloc(sizeof(char)*(strlen(f2) +1));
									strcpy(asmtemp->mnemonic_field, f2);
									asmtemp->operand1 = (char*)malloc(sizeof(char)*(strlen(f3) +1));
									strcpy(asmtemp->operand1, f3);
									asmtemp->operand2 = (char*)malloc(sizeof(char)*(strlen(f4) +1));
									strcpy(asmtemp->operand2, f4);
									asmtemp->nextloc = LOCCTR;
								}
							}
						}
					}
				}
				break;

			// opcode format이 2형식이고 operand로 r1, n이 오는 경우
			case 23 : f3 = strtok(NULL, " ,\r\t\n");
				if(!f3){ // operand1(r1)이 입력되지 않았다면
					printf("Error detected on line %d. May be operand(r1) is missing.\n", asmtemp->line);
					return 0;
				}
				else{ // (r1으로 간주되는) operand1이 정상적으로 입력된 경우
					if(check_if_register(f3) == 0){ // operand1이 레지스터가 아니라면
						printf("Error detected on line %d. Check if your operand(r1) is correct.\n", asmtemp->line);
						return 0;
					}
					else{ // operand1이 레지스터가 맞다면 operand2(n)가 이어서 입력되어야 함
						f4 = strtok(NULL, " ,\r\t\n");
						if(!f4){ // operand2(n)가 입력되지 않았다면
							printf("Error detected on line %d. May be operand(n) is missing.\n", asmtemp->line);
							return 0;
						}
						else{ // (n으로 간주되는) operand2가 정상적으로 입력된 경우
							sscanf(f4, "%x", &n); n--;
							if(strlen(f4) != strspn(f4, "0123456789ABCDEFabcdef")){ // n이 16진수 이외의 문자를 가질 경우
								printf("Error detected on line %d. Check if your operand(n) is correct.\n", asmtemp->line);
								return 0;
							}
							else if(n<0 || n>15){ // n이 16진수 값을 가지지만 범위가 0~F 밖일 경우
								printf("Error detected on line %d. Check the range of n (0~F).\n", asmtemp->line);
								return 0;
							}
							else{ // n이 정상적으로 입력되었다고 간주될 경우
								f5 = strtok(NULL, " ,\r\t\n");
								if(f5){ // n 뒤에 무언가 입력이 이어졌을 경우
									printf("Error detected on line %d. %s cannot have three operands or more.\n", asmtemp->line, f2);
									return 0;
								}
								else{ // 정상적인 2-3형식 입력이 주어진 경우 LOCCTR, format, mnemonic, operand1, operand2 정보 저장
									asmtemp->loc = LOCCTR; LOCCTR += 2;
									asmtemp->format = 23;
									asmtemp->mnemonic_field = (char*)malloc(sizeof(char)*(strlen(f2) +1));
									strcpy(asmtemp->mnemonic_field, f2);
									asmtemp->operand1 = (char*)malloc(sizeof(char)*(strlen(f3) +1));
									strcpy(asmtemp->operand1, f3);
									asmtemp->operand2 = (char*)malloc(sizeof(char)*(strlen(f4) +1));
									strcpy(asmtemp->operand2, f4);
									asmtemp->nextloc = LOCCTR;
								}
							}
						}
					}
				}
				break;

			// opcode format이 3/4형식인 경우 (RSUB 제외) operand(m)가 이어져야 한다
			case 3 : f3 = strtok(NULL, " ,\r\t\n");
				if(!f3){ // operand1(m)이 입력되지 않았다면
					printf("Error detected on line %d. May be operand(r1) is missing.\n", asmtemp->line);
					return 0;
				}
				else{ // (m으로 간주되는) operand1이 정상적으로 입력된 경우
					f4 = strtok(NULL, " ,\r\t\n");
					if(f4){ // m 뒤에 무언가 입력이 이어졌을 경우 (operand2)
						if(strcmp(f4, "X") != 0){ // operand2가 X가 아니라면
							printf("Error detected on line %d. Check if your operand(maybe X) is correct.\n", asmtemp->line);
							return 0;
						}
						else{ // operand2가 X였다면 저장
							asmtemp->operand2 = (char*)malloc(sizeof(char)*(strlen(f4) +1));
							strcpy(asmtemp->operand2, f4);
						}
					}
					// 정상적인 3/4형식 입력이 주어진 경우 (RSUB 제외)
					// LOCCTR, format, mnemonic, operand1 정보 저장
					// extend mode일 경우 LOCCTR 1 더 증가
					asmtemp->loc = LOCCTR;  if(f2[0]!='+') LOCCTR += 3; else LOCCTR += 4;
					asmtemp->format = 3;
					asmtemp->mnemonic_field = (char*)malloc(sizeof(char)*(strlen(f2) +1));
					strcpy(asmtemp->mnemonic_field, f2);
					asmtemp->operand1 = (char*)malloc(sizeof(char)*(strlen(f3) +1));
					strcpy(asmtemp->operand1, f3);	
					asmtemp->nextloc = LOCCTR;
				}
				break;
			// opcode가 BASE 또는 END일 경우 뒤에 operand(symbol)이 와야 하고, LOC값을 저장하지 않는다
			case 5 : f3 = strtok(NULL, " ,\r\t\n"); 
				if(!f3){ // operand1(symbol)이 입력되지 않았다면
					printf("Error detected on line %d. May be operand(symbol) is missing.\n", asmtemp->line);
					return 0;
				}
				else{ // (symbol로 간주되는) operand1이 정상적으로 입력된 경우
					f4 = strtok(NULL, " ,\r\t\n");
					if(f4){ // symbol 뒤에 무언가 입력이 이어졌을 경우
						printf("Error detected on line %d. %s cannot have two operands or more.\n", asmtemp->line, f2);
						return 0;
					}
					else{ // 정상적인 BASE, END형식 입력이 들어왔을 경우 format, mnemonic, operand1 정보 저장
						asmtemp->format = 5;
						asmtemp->mnemonic_field = (char*)malloc(sizeof(char)*(strlen(f2) +1));
						strcpy(asmtemp->mnemonic_field, f2);
						asmtemp->operand1 = (char*)malloc(sizeof(char)*(strlen(f3) +1));
						strcpy(asmtemp->operand1, f3);
					}
				}
			}		
		}
	}
	else{ // 코드가 빈 줄이었다면 (빈 줄이 있는 경우가 있는지 확인 요망)
		asmtemp->emptyline = 1;
	}

	return 1;
}

// 정리된 코드와 SYMTAB을 가지고 object code를 생성
int pass2 (void)
{
	int OP, n, i, x, b, p, e, TA, PC, BASE=-1, TAsubPC; int oprd1, oprd2, a;
	char Hexa[40]; // BYTE인 경우 사용됨

	asmtemp = asmhead;
	while(asmtemp->next){
		if(asmtemp->emptyline == 1){ // 빈 줄일 경우 아무 작업도 하지 않고 다음 줄로 넘어감
			asmtemp = asmtemp->next;
			continue;
		}
		n = i = x = b = p = e = 0; oprd1 = oprd2 = 0;
		// 해당 라인에 대하여 obj code 생성 (asmtemp->objcode 위치에 동적할당)
		// 주석, START, BASE, END, RESW, RESB는 objcode 생성 안함
		// WORD, BYTE는 특수한 생성 방식을 따름
		if( (asmtemp->comment == 1)||(strcmp(asmtemp->mnemonic_field, "START")==0)||(strcmp(asmtemp->mnemonic_field, "RESW")==0)||(strcmp(asmtemp->mnemonic_field, "RESB")==0) ){
			// objcode를 생성하지 않는 경우
			asmtemp = asmtemp->next;
			continue;
		}
		else if( (strcmp(asmtemp->mnemonic_field, "WORD")==0) || (strcmp(asmtemp->mnemonic_field, "BYTE")==0) || (strcmp(asmtemp->mnemonic_field, "BASE")==0)||(strcmp(asmtemp->mnemonic_field, "END")==0)||(strcmp(asmtemp->mnemonic_field, "RSUB")==0) ){
			// WORD이거나 BYTE이거나 RSUB이거나 BASE이거나 END인 경우
			// BASE, END의 경우 objcode는 생성하지 않지만 symbol을 인자로 갖기 때문에 SYMTAB의 탐색 과정을 거쳐야 함
			// BASE의 경우 symbol에 해당하는 address 값을 int형 변수 base에 저장
			if( (strcmp(asmtemp->mnemonic_field, "WORD")==0) ){
				// WORD인 경우 3byte를 할당하여 그대로 저장
				asmtemp->objcode = (int*)malloc(sizeof(int)*3);
				asmtemp->objcodelen = 3;
				sscanf(asmtemp->operand1, "%d", &oprd1);
				
				*((asmtemp->objcode) +0) = (oprd1 / 0x100000 * 16) + (oprd1 / 0x10000) ;
				*((asmtemp->objcode) +1) = (oprd1 / 0x1000 * 16) + (oprd1 / 0x100) ;
				*((asmtemp->objcode) +2) = (oprd1 / 0x10 * 16) + (oprd1 / 0x1) ;
			}
			else if( (strcmp(asmtemp->mnemonic_field, "BASE")==0) ){
				// BASE인 경우 find_symbol 함수를 통해 BASE 값을 가져와 변수 BASE에 저장해 둔다
				BASE = find_symbol(asmtemp->operand1);
				if(BASE == -1){
					printf("Error detected on line %d. Symbol is not in the SYMTAB.\n", asmtemp->line);
					return 0;
				}
			}
			else if( (strcmp(asmtemp->mnemonic_field, "END")==0) ){
				// END인 경우 인자로 온 symbol이 SYMTAB에 있는지만 체크하면 된다
				oprd1 = find_symbol(asmtemp->operand1);
				if(oprd1 == -1){
					printf("Error detected on line %d. Symbol is not in the SYMTAB.\n", asmtemp->line);
					return 0;
				}
			}
			else if( (strcmp(asmtemp->mnemonic_field, "RSUB")==0) ){
				// RSUB인 경우 3형식이지만 인자를 갖지 않으므로 3byte를 할당하고 OPCODE 정보만 저장한다
				asmtemp->objcode = (int*)malloc(sizeof(int)*3);
				asmtemp->objcodelen = 3;
				*((asmtemp->objcode) +0) = asmtemp->opcode +3; // 3형식이므로 n, i가 1이라고 가정하고 더해줌
				*((asmtemp->objcode) +1) = 0;
				*((asmtemp->objcode) +2) = 0;
			}
			else{ // BYTE인 경우
				if( *(asmtemp->operand1) == 'C'){ // C'String' 형식인 경우 String 길이만큼 byte 할당
					asmtemp->objcode = (int*)malloc(sizeof(int)*( strlen(asmtemp->operand1) -3 ));
					for(oprd1 = 0 ; oprd1 < (int)strlen(asmtemp->operand1) -3 ; oprd1++){
						*((asmtemp->objcode) + oprd1) = *((asmtemp->operand1) + oprd1 + 2) ;
					}
					asmtemp->objcodelen = strlen(asmtemp->operand1) -3 ;
				}
				else{ // X'Hex' 형식인 경우 Hex 수를 2자리씩(1byte 단위) 끊어서 저장
					oprd1 = strlen(asmtemp->operand1) -3; oprd1 /= 2;
					asmtemp->objcode = (int*)malloc(sizeof(int)*oprd1); // oprd1 : Hex 자릿수
					
					strcpy(Hexa, asmtemp->operand1 +2);
					Hexa[oprd1 * 2] = 0;
					for(a=0 ; a < oprd1 *2 ; a+=2){
						sscanf(Hexa +a, "%02X", &oprd2);
						*((asmtemp->objcode) + a/2) = oprd2;
					}
					asmtemp->objcodelen = oprd1;
				}
			}
		}
		else if( asmtemp->format == 1 || asmtemp->format == 21 || asmtemp->format == 22 || asmtemp->format == 23 ){
			// 1형식이거나 2형식인 경우
			if( asmtemp->format == 1 ){ // 1형식인 경우 인자가 없으며 objcode 1byte 할당
				asmtemp->objcode = (int*)malloc(sizeof(int)*1);
				asmtemp->objcodelen = 1;
				*((asmtemp->objcode) +0) = asmtemp->opcode; // OPCODE 값 그대로 objcode가 된다
			}
			else{ // 2형식인 경우 objcode 2byte가 할당되고, 앞의 1byte에 OPCODE, 뒤의 1byte에 operand 정보가 저장된다
				asmtemp->objcode = (int*)malloc(sizeof(int)*2);
				asmtemp->objcodelen = 2;
				*((asmtemp->objcode) +0) = asmtemp->opcode;
				
				if( asmtemp->format == 21 ){ // operand가 r1 또는 n(SVC) 하나 오는 경우
					if( (strcmp(asmtemp->mnemonic_field, "SVC")==0) ){ // mnemonic이 SVC라면
						sscanf(asmtemp->operand1, "%X", &oprd1);
					}
					else{
						oprd1 = find_regnum(asmtemp->operand1);
					}
				}
				else if( asmtemp->format == 22 ){ // operand가 r1, r2가 오는 경우
					oprd1 = find_regnum(asmtemp->operand1);
					oprd2 = find_regnum(asmtemp->operand2);
				}
				else{ // operand가 r1, n이 오는 경우
					oprd1 = find_regnum(asmtemp->operand1);
					sscanf(asmtemp->operand2, "%X", &oprd2);
				}

				*((asmtemp->objcode) +1) = 16 * oprd1 + oprd2 ;
			}
		}
		else{ // 3형식이거나 4형식인 경우 (mnemonic 앞에 +가 있으면 4형식(extended))
			  // operand1 앞에 @이 붙으면 direct, #이 붙으면 immediate, 없으면 simple addressing

			TA = find_symbol(asmtemp->operand1); // Target Address 저장
			OP = asmtemp->opcode; // OPCODE 값 저장
			PC = asmtemp->nextloc; // PC값 저장. BASE의 경우 코드에 존재했다면 변수 BASE에 저장. 코드에 없다면 -1 값 유지.
			if(TA == -1){
				printf("Error detected on line %d. Symbol is not in the SYMTAB.\n", asmtemp->line);
				return 0;
			}
			if(TA == -2){ // operand가 범위를 넘는 경우 에러처리
				printf("Error detected on line %d. Check the constant's range.\n", asmtemp->line);
				return 0;
			}
			if(asmtemp->operand2){ // X가 저장되어 있다면
				x=1;
			}

			// operand가 상수일 경우 PC를 0으로 설정하여 결과적으로 TA값 그대로 가게 된다
			if( *(asmtemp->operand1) == '@'){ // indirect addressing인 경우 n=1, i=0
				n=1;
				if( strlen(asmtemp->operand1 +1) == strspn(asmtemp->operand1 +1, "0123456789")){ // operand가 10진수일 경우
					PC = 0;
				}
			}
			else if( *(asmtemp->operand1) == '#'){ // immediate addressing인 경우 n=0, i=1
				i=1;
				if( strlen(asmtemp->operand1 +1) == strspn(asmtemp->operand1 +1, "0123456789")){ // operand가 10진수일 경우
					PC = 0;
				}
			}
			else{ // simple addressing인 경우 n=1, i=1
				n=1; i=1;
				if( strlen(asmtemp->operand1) == strspn(asmtemp->operand1, "0123456789")){ // operand가 10진수일 경우
					PC = 0;
				}
			}

			if( *(asmtemp->mnemonic_field) != '+'){ // 3형식이라면 objcode 3byte 할당
				asmtemp->objcode = (int*)malloc(sizeof(int)*3);
				asmtemp->objcodelen = 3;

				if( (TA-PC) < 0 ){ // TA - PC(or BASE) 값을 계산하고 b, p 값 세팅
					if( (TA-PC) < -2048 ){ // PC값을 빼면 범위를 벗어날 경우
						if(BASE==-1){
							printf("Error detected on line %d.\n", asmtemp->line);
							return 0;
						}
						else{ // PC 대신 BASE를 써야 함
							if( (TA-BASE) < 0 ){
								if( (TA-BASE) < -2048 ){ // BASE값을 빼도 범위를 벗어날 경우
									printf("Error detected on line %d.\n", asmtemp->line);
									return 0;
								}
								else{
									TAsubPC = TA - BASE + 0x10000; b = 1;
								}
							}
							else{
								TAsubPC = TA - BASE; b = 1;
							}
						}
					}
					else{ // 범위 내의 음수인 경우
						TAsubPC = TA - PC + 0x10000; p = 1;
					}
				}
				else{
					TAsubPC = TA - PC; p = 1;
				}
				if(PC==0) p=0;
				// 필요한 요소 (OP, n, i, x, b, p, e, TAsubPC)의 값 세팅이 완료되었으므로 objcode 값 계산에 들어감
				*((asmtemp->objcode) +0) = OP + 2*n + i ;
				*((asmtemp->objcode) +1) = (8*x + 4*b + 2*p + e)*16 + (TAsubPC % 0x1000) / 0x100 ;
				*((asmtemp->objcode) +2) = TAsubPC % 0x100 ;
			}
			else{ // 4형식이라면 objcode 4byte 할당, direct addressing이므로 b, p가 0이고 TA값 그대로 obj code에 삽입된다
				asmtemp->objcode = (int*)malloc(sizeof(int)*4);
				asmtemp->objcodelen = 4;
				
				e = 1;
				// 필요한 요소 (OP, n, i, x, b, p, e, TA)의 값 세팅이 완료되었으므로 objcode 값 계산에 들어감
				*((asmtemp->objcode) +0) = OP + 2*n + i ;
				*((asmtemp->objcode) +1) = (8*x + 4*b + 2*p + e)*16 + TA / 0x10000 ;
				*((asmtemp->objcode) +2) = TA / 0x100 ;
				*((asmtemp->objcode) +3) = TA % 0x100 ;
			}
		}
		asmtemp = asmtemp->next;
	}

	return 1;
}

int filenamecheck (void) // 찾는 파일이 디렉토리에 있으면 1, 없으면 0을 리턴
{
	if(!(dp = opendir("."))){
			printf("Opendir Error!\n");
			return 0; // 디렉토리 포인터를 받지 못할 경우 종료
	}

	while((dirp = readdir(dp))){
		// filename과 같은 이름을 찾으면 1을 리턴하며 종료
		if( strcmp(filename, dirp->d_name) == 0 ){
			fp = fopen(filename, "r");
			// fp open 에러처리는 type, assemble 함수 내에서 진행됨

			closedir(dp);
			return 1;
		}
	}

	closedir(dp);
	return 0;
}


/*
	opcodelist에 없고 특수한 mnemonic(RESW 등)이 아니면 0
	format1이거나 RSUB이면 1
	format2이고 operand가 r1 또는 n이면 21
	format2이고 operand가 r1, r2이면 22
	format2이고 operand가 r1, n이면 23
	format3/4이면 3
	특수한 mnemonic이면 5
	0을 리턴하지 않는 경우에 asmtemp->opcode에 opcode 값 저장
*/
int check_if_opcode (char* p)
{
	char test[10]; int find;
	if(p[0] == '+') strcpy(test, p+1);
	else strcpy(test, p);
	find = find_opcode(test); // mnemonic에 해당하는 opcode 값을 미리 가져와둠
	
	
	asmtemp->opcode = find; // mnemonic에 맞는 opcode 값을 저장
	if( !strcmp(test, "START") || !strcmp(test, "BYTE") || !strcmp(test, "WORD") || !strcmp(test, "RESB") || !strcmp(test, "RESW") || !strcmp(test, "BASE") || !strcmp(test, "END") ){
		return 5;
	}
	else if( find == -1 ){ // opcodelist에 없고 특수한 mnemonic이 아닌 경우, 즉 label로 인식해야 하는 경우
		return 0;
	}
	else if( !strcmp(test, "FIX") || !strcmp(test, "FLOAT") || !strcmp(test, "HIO") || !strcmp(test, "NORM") || !strcmp(test, "SIO") || !strcmp(test, "TIO") || !strcmp(test, "RSUB")){
		return 1;
	}
	else if( !strcmp(test, "CLEAR") || !strcmp(test, "TIXR") || !strcmp(test, "SVC") ){
		return 21;
	}
	else if( !strcmp(test, "ADDR") || !strcmp(test, "COMPR") || !strcmp(test, "DIVR") || !strcmp(test, "MULR") || !strcmp(test, "RMO") || !strcmp(test, "SUBR") ){
		return 22;
	}
	else if( !strcmp(test, "SHIFTL") || !strcmp(test, "SHIFTR") ){
		return 23;
	}
	else{
		return 3;
	}
}

int check_if_register (char* p) // 레지스터면 1, 아니면 0을 리턴
{
	if( strcmp(p,"A")&&strcmp(p,"X")&&strcmp(p,"L")&&strcmp(p,"PC")&&strcmp(p,"SW")&&strcmp(p,"B")&&strcmp(p,"S")&&strcmp(p,"T")&&strcmp(p,"F") ){
		return 0;
	}
	else{
		return 1;
	}
}

void clearSYMTAB (void) // SYMTAB 초기화
{
	int i;
	for(i=0 ; i<55 ; i++){
		if( (SYMTAB +i)->next ){ // i번째 칸이 비어있지 않다면
			symtemp = (SYMTAB +i)->next;
			while(symtemp){ // i번째 칸이 빌 때까지 free 수행
				symprev = symtemp;
				symtemp = symtemp->next;
				free(symprev);
			}
			(SYMTAB +i)->next = NULL;
		}
	}
}

// SYMTAB[0]은 ASCII 33~64, SYMTAB[1~26]은 알파벳 대문자
// SYMTAB[27]은 ASCII 91~96, SYMTAB[28~53]은 알파벳 소문자, SYMTAB[54]는 ASCII 123~126	
int addSYMTAB (void) // 성공적으로 추가하면 1, 이미 라벨이 존재하면 0을 리턴
{
	char tem[40]; int i;
	strcpy(tem, asmtemp->label_field);
	if(tem[0] < 'A') i = 0;
	else if(tem[0] <= 'Z') i = tem[0]-64;
	else if(tem[0] < 'a') i = 27;
	else if(tem[0] <= 'z') i = tem[0]-69;
	else i = 54;

	symprev = SYMTAB +i;
	while(symprev->next){
		if(strcmp(tem, symprev->next->label) < 0){ // 문자열 비교 결과에서 tem이 더 작은 ASCII 값을 갖는다면
			break;
		}
		else if(strcmp(tem, symprev->next->label) == 0){ // 같은 label일 경우
			return 0;
		}
		else{ // tem이 더 큰 ASCII 값을 갖는다면
			symprev = symprev->next;
		}
	}
	
	symtemp = (symtab*)malloc(sizeof(symtab)); // 노드 생성
	strcpy(symtemp->label, asmtemp->label_field);
	symtemp->loc = asmtemp->loc;
	
	symtemp->next = symprev->next; // 추가된 노드를 기존의 노드들과 이어주는 작업
	symprev->next = symtemp;

	return 1;
}

int find_opcode (char* m) // opcodelist에서 mnemonic을 찾아 그 opcode 값을 반환
{
	char mnemo[10]; int sum=0, i;
	strcpy(mnemo, m);
	
	for(i=0 ; *(mnemo +i) != 0 ; i++){
		sum += *(mnemo +i);
	}
	sum %= 20;

	temp = (hashtable +sum);
	if(temp->opcode == -1){
		return -1;
	}
	while(temp){
		// 단서를 가지고 opcode를 찾아가는 작업
		if( strcmp(mnemo, temp->mnemonic) == 0){
			return temp->opcode;
		}
		temp = temp->next;
	}
	return -1;
}

void freeasm (void) // assemble 함수에서 할당한 메모리들의 할당해제
{
	while(asmhead){
		asmtemp = asmhead;
		asmhead = asmhead->next;
		if(asmtemp->label_field)
			free(asmtemp->label_field);
		if(asmtemp->mnemonic_field)
			free(asmtemp->mnemonic_field);
		if(asmtemp->operand1)
			free(asmtemp->operand1);
		if(asmtemp->operand2)
			free(asmtemp->operand2);
		if(asmtemp->objcode)
			free(asmtemp->objcode);
	}
}

int find_symbol (char *s) // SYMTAB에 symbol이 있으면 그 loc을 반환하고, 없으면 -1을 반환, 범위를 넘는 상수일 경우 -2 반환
{
	// 문자열 앞에 #이나 @가 있다면 한칸 뒤부터 operand로 간주하고 탐색 시작
	char symb[40]; int i;
	if(s[0]=='#' || s[0]=='@') strcpy(symb, s +1);
	else strcpy(symb, s);

	if(strlen(symb) == strspn(symb, "0123456789")){ // symbol이 아니라 10진수 상수였을 경우
		sscanf(symb, "%d", &i);
		if(*(asmtemp->mnemonic_field)!='+' && i > 0xFFF){ // 3형식에서 표현할 수 있는 범위(~4095)를 넘어섰을 경우
			return -2;
		}
		if(*(asmtemp->mnemonic_field)=='+' && i > 0xFFFFF){ // 4형식에서 표현할 수 있는 범위를 넘어섰을 경우
			return -2;
		}
		return i; // 범위 내의 상수일 경우 상수 그대로 반환
	}

	// SYMTAB[0]은 ASCII 33~64, SYMTAB[1~26]은 알파벳 대문자
	// SYMTAB[27]은 ASCII 91~96, SYMTAB[28~53]은 알파벳 소문자, SYMTAB[54]는 ASCII 123~126	
	if(symb[0] < 'A') i = 0;
	else if(symb[0] <= 'Z') i = symb[0]-64;
	else if(symb[0] < 'a') i = 27;
	else if(symb[0] <= 'z') i = symb[0]-69;
	else i = 54;

	symtemp = (SYMTAB +i)->next;
	while(symtemp){
		if(strcmp(symtemp->label, symb) == 0){ // 일치하는 symbol을 찾으면 그 loc을 리턴
			return symtemp->loc;
		}
		symtemp = symtemp->next;
	}
	return -1;
}

int find_regnum (char *r) // 레지스터 번호를 반환
{
	if(!strcmp("A", r)) return 0;
	if(!strcmp("X", r)) return 1;
	if(!strcmp("L", r)) return 2;
	if(!strcmp("B", r)) return 3;
	if(!strcmp("S", r)) return 4;
	if(!strcmp("T", r)) return 5;
	if(!strcmp("F", r)) return 6;
	if(!strcmp("PC", r)) return 8;
	return 9;
}
