#!/bin/bash
#SBATCH --nodes=1
#SBATCH --time=0:01:00
#SBATCH --partition=atesting
#SBATCH --constraint=ib
#SBATCH --ntasks=8
#SBATCH --job-name=Adj Matrix Breadth First Search
#SBATCH --job-name2=Parallelized Adj Matrix Breadth First Search 
#SBATCH --output=aBFS
#SBATCH --output2=aBFSp

#------------------- Graphs locations
# ../grafi/graph_test.txt
# ../grafi/bug.txt
# ../grafi/facebook_combined.txt
# ../grafi/soc-LiveJournal1.txt

#    //search type:
#        //case 0 -> ricarca con array based list
#        //case 1 -> ricerca con lista di adiacena
#        //case 2 -> ricerca con matrice di adiacenza

#        //graph_type:
#            //case 0 -> genera un grafo randomico non orientato
#            //case 1 -> genera un grafo randomico orientato
#            //case 2 -> importa un grafo non orientato
#            //case 3 -> importa un grafo orientato

#            //hybrid search:
#                //case 0 -> off
#                //case 1 -> on


OMP_NUM_THREADS=16;
export OMP_NUM_THREADS

# aBFS --> 1 thread
# aBFSp ---> 8 threads 
# ./executable/aBFS ../grafi/facebook_combined.txt 0 2 > ./outputs/output_A.txt
# ./executable/aBFS ../grafi/facebook_combined.txt 1 2 > ./outputs/output_L.txt
# ./executable/aBFS ../grafi/facebook_combined.txt 2 2 > ./outputs/output_M.txt
# ./executable/aBFSp ../grafi/facebook_combined.txt 0 2 1 > ./outputs/output_FB_pA.txt
# ./executable/aBFSp ../grafi/soc-LiveJournal1.txt 0 2 1 > ./outputs/output_LJ_pA.txt
./executable/aBFS ../grafi/facebook_combined.txt 0 2 1 #> ./outputs/output_LJ_list_research.txt

read -p "Press enter to continue"