thread: thread.cpp
	g++ -o out thread.cpp

mutex: mutex.cpp
	g++ -o out mutex.cpp

lock_guard: lock_guard.cpp
	g++ -o out lock_guard.cpdp

unique_lock: unique_lock.cpp
	g++ -o out unique_lock.cpp

recursive_mutex: recursive_mutex.cpp
	g++ -o out recursive_mutex.cpp

shared_mutex: shared_mutex.cpp 
	g++ -o out shared_mutex.cpp

atomic: atomic.cpp
	g++ -o out atomic.cpp

test: test.cpp
	g++ -o out test.cpp

# other interesting

consumer: consumer.cpp
	g++ -o out consumer.cpp

macros: macros.cpp
	g++ -o out macros.cpp

taskQueueOrigin: taskQueueOrigin.cpp
	g++ -o out taskQueueOrigin.cpp

# invalid
# threadPool: threadPool.cpp
# 	g++ -o out threadPool.cpp

# threadPool: taskQueue.h ThreadPool/threadUnit.h ThreadPool/taskQueue.cpp
# FOLDER = ThreadPool
# threadPool = ${FOLDER}/threadPool
# threadUnit = ${FOLDER}/threadUnit
# taskQueue = ${FOLDER}/taskQueue
# threadPool:
# 	g++ -c ${threadPool}.cpp -o ${threadPool}.o
# 	g++ -c ${threadUnit}.h -o ${threadUnit}.o
# 	g++ -c ${taskQueue}.h -o ${taskQueue}.o
# 	g++ ${threadPool}.o ${threadUnit}.o ${taskQueue}.o

run: out
	./out
