# 终极目标
t:t.c
	gcc -o t t.c




# 来指定clean为伪目标，这样make就总会执行该目标的命令
.PHONY: clean
clean:
	rm -rf *.o t