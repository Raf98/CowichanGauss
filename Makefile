openmp1:
	g++ -fopenmp gaussian_openMP.cpp utils.hpp -o gaussianOMP -lpthread
	./gaussianOMP test1

exopenmp1:	
	./gaussianOMP test1

openmp2:
	g++ -fopenmp gaussian_openMP.cpp utils.hpp -o gaussianOMP -lpthread
	./gaussianOMP test2

exopenmp2:	
	./gaussianOMP test2

openmp3:
	g++ -fopenmp gaussian_openMP.cpp utils.hpp -o gaussianOMP -lpthread
	./gaussianOMP test3

exopenmp3:	
	./gaussianOMP test3




pthreads1:
	g++ gaussian_pthreads.cpp utils.hpp -o gaussianPthreads -lpthread
	./gaussianPthreads test1

expthreads1:
	./gaussianPthreads test1

pthreads2:
	g++ gaussian_pthreads.cpp utils.hpp -o gaussianPthreads -lpthread
	./gaussianPthreads test2

expthreads2:
	./gaussianPthreads test2

pthreads3:
	g++ gaussian_pthreads.cpp utils.hpp -o gaussianPthreads -lpthread
	./gaussianPthreads test3

expthreads3:
	./gaussianPthreads test3


seq1:
	g++ gaussian.cpp utils.hpp -o gaussian 
	./gaussian test1

exseq1:
	./gaussian test1

seq2:
	g++ gaussian.cpp utils.hpp -o gaussian 
	./gaussian test2

exseq2:
	./gaussian test2

seq3:
	g++ gaussian.cpp utils.hpp -o gaussian 
	./gaussian test3

exseq3:
	./gaussian test3

execAllTests:
				$(MAKE) seq1
				$(MAKE) seq2
				$(MAKE) seq3
				$(MAKE) pthreads1
				$(MAKE) pthreads2
				$(MAKE) pthreads3
				$(MAKE) openmp1
				$(MAKE) openmp2
				$(MAKE) openmp3

clean:
		rm output*





