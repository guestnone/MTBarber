OBJMUTEX = Source/Main_Mutexed.o Source/Common.o
OBJCONDVAR = Source/Main_CondVar.o Source/myqueue.o Source/cond_queue.o
all: MainMutexed MainCondVar
MainMutexed: $(OBJMUTEX)
	gcc -pthread -O3 $(OBJMUTEX) -o MainMutexed
MainCondVar: $(OBJCONDVAR)
	gcc -pthread -O3 $(OBJCONDVAR) -o MainCondVar
$(OBJCONDVAR): Source/myqueue.h Source/cond_queue.h
$(OBJMUTEX): Source/Common.h
.PHONY: clean
clean:
	rm -f *.o ./Source/*.o MainMutexed MainCondVar
