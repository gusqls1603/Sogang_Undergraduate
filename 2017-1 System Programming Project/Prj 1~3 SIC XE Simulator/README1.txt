﻿시스템프로그래밍 프로젝트1

 20141583 정현빈

본 프로젝트는 SIC/XE 머신을 구현하기 위한 전 단계로, 가상의 메모리공간을 구현하고 기본적인 셸, 메모리, opcode 명령어를 구현하는 것이 목적입니다.

make를 입력하면 Makefile을 통해 gcc -W -Wall 20141583.c 명령이 실행되어 대상 c파일이 컴파일됩니다.

(정상적인 컴파일을 위해 Makefile, 20141583.h, 20141583.c, opcode.txt 파일은 반드시 같은 디렉토리에 있어야 합니다)

이후 ./20141583.out 입력을 통해 20141583.out가 실행되면 sicsim> 문구가 출력되어 정해진 명령어 수행이 가능하게 됩니다.

기본 명령어 리스트는 help 명령어 입력을 통해 출력이 가능하고, quit 명령어 입력 시 sicsim이 종료됩니다.

프로그램 종료 후 make clean을 입력 시 20141583.o와 20141583.out 파일이 삭제됩니다.