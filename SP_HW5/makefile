all: SP_HW5	# 3 題一起編譯

SP_HW5: part1 part2 part3

part1: hostinfo.o mydate.o mycat.o printdir.o pipe_ls.o
        gcc -o hostinfo hostinfo.c
        gcc -o mydate mydate.c
        gcc -o printdir printdir.c
        gcc -o mycat mycat.c
        gcc -o pipe_ls pipe_ls.c

part2: parse.o shell.o builtin.o is_background.o run_command.o redirect_in.o redirect_out.o pipe_command.o pipe_present.o
        gcc -o myshell parse.c shell.c builtin.c is_background.c run_command.c redirect_in.c redirect_out.c pipe_command.c pipe_present.c
part3: convert.o main.c lookup1.c
        gcc -o convert convert.c
        gcc -o file_lookup main.c lookup1.c
clean:
        rm -f *.o hostinfo mydate mycat printdir pipe_ls myshell