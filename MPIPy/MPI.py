from mpi4py import MPI
from random import randint
import time

comm = MPI.COMM_WORLD
rank = comm.Get_rank()
workers = comm.Get_size() - 1

mtrx1 = []
mtrx2 = []
mtrx3 = []

N = 1000


def init_matrix():
    global mtrx1
    mtrx1 = [[randint(0, 9) for i in range(N)] for j in range(N)]

    global mtrx2
    mtrx2 = [[randint(0, 9) for i in range(N)] for j in range(N)]


def multiply_matrix(X, Y):

    Z = [[sum(a * b for a, b in zip(X_row, Y_col)) for Y_col in zip(*Y)]
            for X_row in X]

    return Z


def distribute_matrix_data():
    def split_matrix(seq, p):
        rows = []
        n = len(seq) / p
        r = len(seq) % p
        b = 0
        e = int(n + min(1, r))
        for i in range(p):
            print(b)
            print(e)
            rows.append(seq[b:e])
            r = max(0, r - 1)
            b = e
            e = int(e + n + min(1, r))

        return rows

    rows = split_matrix(mtrx1, workers)

    pid = 1
    for row in rows:
        comm.send(row, dest=pid, tag=1)
        comm.send(mtrx2, dest=pid, tag=2)
        pid = pid + 1


def assemble_matrix_data():
    global mtrx3

    pid = 1
    for n in range(workers):
        row = comm.recv(source=pid, tag=pid)
        mtrx3 = mtrx3 + row
        pid = pid + 1


def master_operation():
    distribute_matrix_data()
    assemble_matrix_data()


def slave_operation():
    # receive data from master node
    x = comm.recv(source=0, tag=1)
    y = comm.recv(source=0, tag=2)

    # multiply the received matrix and send the result back to master
    z = multiply_matrix(x, y)
    comm.send(z, dest=0, tag=rank)


if __name__ == '__main__':
    if rank == 0:
        init_matrix()

        # start time
        t1 = time.time()
        master_operation()

        # end time
        t2 = time.time()

        print('Секунд:', int(t2 - t1))
        print()
    else:
        slave_operation()
