#include<stdio.h>
#include<stdlib.h>
#include<mpi.h>
#include<time.h>
 
int main(int argc,char *argv[])
{
    double start, stop;
    int i, j, k, l;
    int *a, *b, *c, *buffer, *ans;
    int size = 3000;
    int rank, numprocs, line;
 
    MPI_Init(NULL,NULL);//MPI Initialize
    MPI_Comm_rank(MPI_COMM_WORLD,&rank);// Получить текущий номер процесса
    MPI_Comm_size(MPI_COMM_WORLD,&numprocs);// Получить количество процессов
 
    line = size/numprocs;// Делим данные на блоки (количество процессов), и основной процесс также должен обрабатывать данные
    a = (int*)malloc(sizeof(int)*size*size);
    b = (int*)malloc(sizeof(int)*size*size);
    c = (int*)malloc(sizeof(int)*size*size);
    // Размер кеша больше или равен размеру обрабатываемых данных, когда он больше, чем фактическая часть данных
    buffer = (int*)malloc(sizeof(int)*size*line);// Размер пакета данных
    ans = (int*)malloc(sizeof(int)*size*line);// Сохраняем результат расчета блока данных
 
    // Основной процесс присваивает матрице начальное значение и передает матрицу N каждому процессу, а матрицу M передает каждому процессу в группах.
    if (rank==0)
    {
        // Читаем матрицу из файла
        FILE *fp;
 
        fp=fopen("a.txt","r");// открыть файл
        start = MPI_Wtime();
        for(i=0;i<3000;i++) // Чтение данных
            for(j=0;j<3000;j++)
                fscanf(fp,"%d",&a[i*size+j]);
        fclose(fp);// Закрыть файл
 
        fp=fopen("b.txt","r");
 
        for(i=0;i<3000;i++)
            for(j=0;j<3000;j++)
                fscanf(fp,"%d",&b[i*size+j]);
        fclose(fp);
        // Отправить матрицу N другим подчиненным процессам
        for (i=1;i<numprocs;i++)
        {
                MPI_Send(b,size*size,MPI_INT,i,0,MPI_COMM_WORLD);
        }
        // Отправляем каждую строку a каждому подчиненному процессу по очереди
        for (l=1; l<numprocs; l++)
        {
            MPI_Send(a+(l-1)*line*size,size*line,MPI_INT,l,1,MPI_COMM_WORLD);
        }
        // Получаем результат, рассчитанный по процессу
        for (k=1;k<numprocs;k++)
        {
            MPI_Recv(ans,line*size,MPI_INT,k,3,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
            // Передаем результат в массив c
            for (i=0;i<line;i++)
            {
                for (j=0;j<size;j++)
                {
                    c[((k-1)*line+i)*size+j] = ans[i*size+j];
                }
 
            }
        }
        // Рассчитать оставшиеся данные
        for (i=(numprocs-1)*line;i<size;i++)
        {
            for (j=0;j<size;j++)
            {
                int temp=0;
                for (k=0;k<size;k++)
                    temp += a[i*size+k]*b[k*size+j];
                c[i*size+j] = temp;
            }
        }
 
        fp=fopen("c.txt","w");
        for(i=0; i<size; i++){
            for(j=0; j<size; j++)
                fprintf(fp,"%d ",c[i*size+j]);
            fputc('\n',fp);
        }
        fclose(fp);
        // Результат теста
        // Статистика по времени
        stop = MPI_Wtime();
 
        printf("rank:%d time:%lfs\n",rank,stop-start);
 
        free(a);
        free(b);
        free(c);
        free(buffer);
        free(ans);
    }
 
    // Другие процессы получают данные и после вычисления результата отправляют их в основной процесс
    else
    {
        // Получаем широковещательные данные (матрица b)
        MPI_Recv(b,size*size,MPI_INT,0,0,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
        printf("Hello from %d", rank);
        MPI_Recv(buffer,size*line,MPI_INT,0,1,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
        // Рассчитать результат продукта и отправить результат в основной процесс
        for (i=0;i<line;i++)
        {
            for (j=0;j<size;j++)
            {
                int temp=0;
                for(k=0;k<size;k++)
                    temp += buffer[i*size+k]*b[k*size+j];
                ans[i*size+j]=temp;
            }
        }
        // Отправить результат расчета в основной процесс
        MPI_Send(ans,line*size,MPI_INT,0,3,MPI_COMM_WORLD);
    }
 
    MPI_Finalize();//Конец
 
    return 0;
}
