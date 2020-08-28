openmp:
	g++ -fopenmp gaussian_openMP.cpp -o gaussianOMP -lpthread
	./gaussianOMP
	
pthreads:
	g++ gaussian_pthreads.cpp -o gaussianPthreads -lpthread
	./gaussianPthreads
seq:
	g++ gaussian.cpp -o gaussian 
	./gaussian


