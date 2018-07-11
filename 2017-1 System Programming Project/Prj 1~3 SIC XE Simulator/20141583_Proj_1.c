#include "20141583.h"


opcd *temp, *del, *hashtable;
ptr *head = NULL, *prev;
char com[100], com_backup[100], *tok;

int memory [65536][16] = {{0}}, address = 0;
int quit = 0; // quit함수가 호출될 때 1로 변경됨
int len; // 커맨드 길이 저장

///////////////////////////////////////////////////////
char *filename; // type, assemble 함수에서 filename을 포인팅
FILE *fp;
///////////////////////////////////////////////////////

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


// 함수 실행이 정상적으로 이뤄질 경우 add_history() 실행
void shell_type (void)
{
	int t;
	strcpy(com_backup, com); // 원본 커맨드 보존

	tok = strtok(com, " ");
	if(strlen(tok)!=4 || strncmp(tok, "type", 4)!=0){
		// 잘못된 명령어 입력에 대한 에러처리
		print_error("Input Command Error! (Suggested Command : type)\n"); return;
	}

	filename = strtok(NULL, " ");
	if(!filename){
		print_error("Input Command Error! (filename is missing)\n"); return;
	}

	tok = strtok(NULL, " "); // filename 뒤에 문자가 더 있었는지 체크
	if(tok != NULL){
		print_error("Input Command Error! (Too much parameters)\n"); return;
	}
	if( !filenamecheck() ){
		print_error("File is not in the directory!\n"); return;
	}
	if(!fp){
		print_error("File open Error!\n"); return ;
	}

	while(1){
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
	strcpy(com_backup, com); // 원본 커맨드 보존

	tok = strtok(com, " ");
	if(strlen(tok)!=8 || strncmp(tok, "assemble", 8)!=0){
		// 잘못된 명령어 입력에 대한 에러처리
		print_error("Input Command Error! (Suggested Command : assemble)\n"); return;
	}

	filename = strtok(NULL, " ");
	if(!filename){
		print_error("Input Command Error! (filename is missing)\n"); return;
	}

	tok = strtok(NULL, " "); // filename 뒤에 문자가 더 있었는지 체크
	if(tok != NULL){
		print_error("Input Command Error! (Too much parameters)\n"); return;
	}

	/*
		디렉토리에 filename이 있는지 체크하여 있으면 어셈블 수행, 없으면 에러출력 후 종료
	*/
	if( !filenamecheck() ){
		print_error("File is not in the directory!\n"); return;
	}
	if(!fp){
		print_error("File open Error!\n"); return ;
	}

	/*
		pass1 및 pass2 진행
	*/

	strcpy(com, com_backup);
	add_history();
	fclose(fp);
}

void symbol (void)
{
	if(strlen(com)!=6 || strncmp(com, "symbol", 6) != 0){
		// 잘못된 명령어 입력에 대한 에러처리
		print_error("Input Command Error! (Suggested Command : symbol)\n"); return;
	}

	/*
		SYMTAB 출력 진행
	*/
}

void pass1 (void)
{

}

void pass2 (void)
{

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