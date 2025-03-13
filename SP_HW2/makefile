all: SP_hw2
#%.o: %.c 這樣會編譯當前目錄下所有的.c檔成.o檔
	#gcc -c -o $@ $<	
SP_hw2: SP_hw2_tail
	
SP_hw2_tail: SP_hw2_tail.o	#.o檔makefile根據隱式規則會自動生成，像是 cc    -c -o SP_hw1-1.o SP_hw1-1.c
	gcc -o SP_hw2_tail SP_hw2_tail.c
clean:
	rm -f *.o SP_hw2_tail
