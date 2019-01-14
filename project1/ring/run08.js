#PBS -N Work08P
#PBS -l nodes=2:ppn=4
#PBS -l walltime=0:20:00
#PBS -q q64p48h@raptor
#PBS -o /dev/null
#PBS -e /dev/null
#PBS -r n
#PBS -V
cd $PBS_O_WORKDIR
#ulimit -c 0
mpirun -np 8 ring >& ring.out
