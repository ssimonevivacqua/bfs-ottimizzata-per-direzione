#!/bin/bash
#SBATCH --nodes=1
#SBATCH --time=0:01:00
#SBATCH --partition=atesting
#SBATCH --constraint=ib
#SBATCH --ntasks=8
#SBATCH --job-name=Multiple Examples Searcj for time calculation
#SBATCH --job-name2=Parallelized Multiple Examples Searcj for time calculation
#SBATCH --output=MES
#SBATCH --output2=MESp



OMP_NUM_THREADS=8;
export OMP_NUM_THREADS

# choices:
    # Graph folder path : ../../../grafi/generati/
        # Grandezza iniziale del numero dei nodi : 100, 10000, 10000000
            # Numero di grafi generati con grandezza crescente : 5, 10, 20 grafi
                # 0 : BFS classica, 1 : Direction Optimizing BFS
                    # 0 : Mod solo media su grafi crescenti ripetuti e più sorgenti, 1 : Mod per Max,Min,Avg su grafi crescenti (ma unici) e più sorgenti : 2 single : 3 single ma con multipli crescenti simil 0
                        # 0 : VMemory unlimited (swap on), 1 : VMemory Limited (13,5GB, swap off se limito uso swap)
                            # 0 : Non generare nuove sorgenti e grafi, 1 : Generali
# ./MES ../../../grafi/generati/ 10 5 1 0 0 1
# ./MESP ../../../grafi/generati/ 10 5 1 0 0 1
# ./MES ../../../grafi/generati/ 100000 22 0 0 0 1
# ./MES ../../../grafi/generati/ 100 22 1 1 0 1

./MES ../../../grafi/generati/ 100 11 0 3 0 1
# valgrind --leak-check=full ./MES ../../../grafi/generati/ 100 4 0 3 0 0  #CHECK MEMORY LEAK
read -p "Press enter to continue"