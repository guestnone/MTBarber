all: MainMutexed MainCondVar
MainMutexed:
	gcc -pthread -O3 Source/Main_Mutexed.c Source/Common.c -o MainMutexed
MainCondVar:
	gcc -pthread -O3 Source/Main_CondVar.c Source/Common.c -o MainCondVar
.PHONY: clean
clean:
	rm -f *.o ./Source/*.o MainMutexed MainCondVar
