all: SP_HW3
#%.o: %.c 這樣會編譯當前目錄下所有的.c檔成.o檔
	#gcc -c -o $@ $<
#.o檔makefile根據隱式規則會自動生成，像是 cc    -c -o SP_hw1-1.o SP_hw1-1.c
										# .h 標頭檔不用編譯
SP_HW3: parse.o shell.o builtin.o run_command.o is_background.o
	gcc -o SP_HW3 parse.c shell.c builtin.c run_command.c is_background.c
clean:
	rm -f *.o SP_HW3
