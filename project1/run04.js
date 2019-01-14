#PBS -N Work04P
#PBS -l nodes=1:ppn=4
#PBS -l walltime=0:20:00
#PBS -q q64p48h@raptor
#PBS -o /dev/null
#PBS -e /dev/null
#PBS -r n
#PBS -V
cd $PBS_O_WORKDIR
#ulimit -c 0
lamboot
for i in 1 2 4 8 16 32 60 80 100
do
	mpirun -np 4 project1 hard_sample.dat sol_hard.04 ${i} 1 >> results.04p 2>&1
	mpirun -np 4 project1 hard_sample.dat sol_hard.04 ${i} 0 >> results.04p2 2>&1

done
lamhalt
