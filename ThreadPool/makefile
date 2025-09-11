threadPool = threadPool
threadUnit = threadUnit
taskQueue = taskQueue

base:
	g++ -c ${threadPool}.cpp -o ${threadPool}.o
	g++ -c ${threadUnit}.h -o ${threadUnit}.o
	g++ -c ${taskQueue}.h -o ${taskQueue}.o
	g++ ${threadPool}.o ${threadUnit}.o ${taskQueue}.o
