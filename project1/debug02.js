#PBS -N Debug02P
#PBS -l nodes=1:ppn=2
#PBS -l walltime=0:02:00
#PBS -q q64p48h@raptor
#PBS -o /dev/null
#PBS -e /dev/null
#PBS -r n
#PBS -V
cd $PBS_O_WORKDIR
#ulimit -c 0
for i in 1 2 4; do
	mpirun -np 2 project1 easy_sample.dat sol_easy.02 $i 1 >> out.02p 2>&1
done

