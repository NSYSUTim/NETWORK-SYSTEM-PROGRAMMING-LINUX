all: SP_HW4
#%.o: %.c 這樣會編譯makefile裡所有的.c檔成.o檔		前幾次理解錯了
	#gcc -c -o $@ $<
#.o檔makefile根據隱式規則會自動生成，像是 cc    -c -o SP_hw1-1.o SP_hw1-1.c
										# .h 標頭檔不用編譯
SP_HW4: 12_1 12_2 12_3
	gcc -o 12_1 12_1.c
	gcc -o 12_2 12_2.c
	gcc -o 12_3 12_3.c
clean:
	rm -f *.o SP_HW4 12_1 12_2 12_3
