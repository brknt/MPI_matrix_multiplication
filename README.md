# MPI_matrix_multiplication-
Performing series and parallel matrix multiplication and calculation of their total duration

# The Problem:
Write an MPI application that performs serial and parallel matrix multiplication and checks the
results for equality.

# Details of Solutions:

An MPI application that performs a serial and parallel matrix multiplication and checks the results for
equality.
N value is asked in the command line.The NX16 matrix a, the 16XN matrix b, are created with random
numbers between 0 and 100. And a.txt is saved in b.txt files.These matrices are multiplied serially
and parallelly by separate methods. It is saved in CSerial.txt and CParallel.txt files. Then, the serial
and parallel multiplication results are compared with the isMatrixEqual method. Meanwhile, in the
running application, serialTime, ParallelTime, EqualityTime and TotalTime are calculated using the
MPI library and functions.
