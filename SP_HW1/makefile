all: SP_hw1
#%.o: %.c 這樣會編譯當前目錄下所有的.c檔成.o檔
	#gcc -c -o $@ $<	
SP_hw1: SP_hw1-1 SP_hw1-2

SP_hw1-1: SP_hw1-1.o	#.o檔makefile根據隱式規則會自動生成，像是 cc    -c -o SP_hw1-1.o SP_hw1-1.c
	gcc -o SP_hw1-1 SP_hw1-1.c
SP_hw1-2: SP_hw1-2.o create_hole_file.o
	gcc -o SP_hw1-2 SP_hw1-2.c
	gcc -o create_hole_file create_hole_file.c

clean:
	rm -f *.o SP_hw1-1 SP_hw1-2 create_hole_file hole_file copy_hole.txt
