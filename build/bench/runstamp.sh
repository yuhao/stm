#algs="CGL Ticket TML RingSW OrecALA OrecELA TMLLazy NOrecPrio OrecFair CToken CTokenTurbo Pipeline BitLazy LLT TLI ByteEager MCS Serial BitEager ByteLazy ByEAR OrecEagerRedo ByteEagerRedo BitEagerRedo RingALA Nano Swiss ByEAU ByEAUFCM ByEAUHA ByEAUHour OrEAU OrEAUFCM OrEAUHA OrEAUHour OrecEager OrecEagerHour OrecEagerBackoff OrecEagerHB OrecLazy  OrecLazyHour  OrecLazyBackoff OrecLazyHB NOrec NOrecHour NOrecBackoff NOrecHB"
algs="LLT"

for alg in $algs
do
	export STM_CONFIG=$alg; time ../stamp-0.9.10/bayes/bayesSTM32 -v32 -r4096 -n10 -p40 -i2 -e8 -s1 -t16 > bayes_adp
	export STM_CONFIG=$alg; time ../stamp-0.9.10/bayes/bayesSTM32_original -v32 -r4096 -n10 -p40 -i2 -e8 -t16 > bayes_ori
	export STM_CONFIG=CGL; time ../stamp-0.9.10/bayes/bayesSTM32_original -v32 -r4096 -n10 -p40 -i2 -e8 -t16 > bayes_cgl
	#gprof ../stamp-0.9.10/bayes/bayesSTM32 > profiling/gp_stamp_bayes_${alg}

	export STM_CONFIG=$alg; time ../stamp-0.9.10/genome/genomeSTM32 -g16384 -s64 -n16777216 -t16 > genome_adp
	export STM_CONFIG=$alg; time ../stamp-0.9.10/genome/genomeSTM32_original -g16384 -s64 -n16777216 -t16 > genome_ori
	export STM_CONFIG=CGL; time ../stamp-0.9.10/genome/genomeSTM32_original -g16384 -s64 -n16777216 -t16 > genome_cgl
	#gprof ../stamp-0.9.10/genome/genomeSTM32 > profiling/gp_stamp_genome_${alg}

	export STM_CONFIG=$alg; time ../stamp-0.9.10/intruder/intruderSTM32 -a10 -l128 -n262144 -s1 -t16 > intruder_adp
	export STM_CONFIG=$alg; time ../stamp-0.9.10/intruder/intruderSTM32_original -a10 -l128 -n262144 -s1 -t16 > intruder_ori
	export STM_CONFIG=CGL; time ../stamp-0.9.10/intruder/intruderSTM32_original -a10 -l128 -n262144 -s1 -t16 > intruder_cgl
	#gprof ../stamp-0.9.10/intruder/intruderSTM32 > profiling/gp_stamp_intruder_${alg}

	export STM_CONFIG=$alg; time ../stamp-0.9.10/kmeans/kmeansSTM32 -m40 -n40 -t0.00001 -i ~/Documents/stamp-0.9.10/kmeans/inputs/random-n65536-d32-c16.txt -p16 > kmeans_adp1
	export STM_CONFIG=$alg; time ../stamp-0.9.10/kmeans/kmeansSTM32_original -m40 -n40 -t0.00001 -i ~/Documents/stamp-0.9.10/kmeans/inputs/random-n65536-d32-c16.txt -p16 > kmeans_ori1
	export STM_CONFIG=CGL; time ../stamp-0.9.10/kmeans/kmeansSTM32_original -m40 -n40 -t0.00001 -i ~/Documents/stamp-0.9.10/kmeans/inputs/random-n65536-d32-c16.txt -p16 > kmeans_cgl1
	#gprof ../stamp-0.9.10/kmeans/kmeansSTM32 > profiling/gp_stamp_kmeans_${alg}_low
	export STM_CONFIG=$alg; time ../stamp-0.9.10/kmeans/kmeansSTM32 -m15 -n15 -t0.00001 -i ~/Documents/stamp-0.9.10/kmeans/inputs/random-n65536-d32-c16.txt -p16 > kmeans_adp2
	export STM_CONFIG=$alg; time ../stamp-0.9.10/kmeans/kmeansSTM32_original -m15 -n15 -t0.00001 -i ~/Documents/stamp-0.9.10/kmeans/inputs/random-n65536-d32-c16.txt -p16 > kmeans_ori2
	export STM_CONFIG=CGL; time ../stamp-0.9.10/kmeans/kmeansSTM32_original -m15 -n15 -t0.00001 -i ~/Documents/stamp-0.9.10/kmeans/inputs/random-n65536-d32-c16.txt -p16 > kmeans_cgl2
	#gprof ../stamp-0.9.10/kmeans/kmeansSTM32 > profiling/gp_stamp_kmeans_${alg}_high

	export STM_CONFIG=$alg; time ../stamp-0.9.10/labyrinth/labyrinthSTM32 -i ~/Documents/stamp-0.9.10/labyrinth/inputs/random-x512-y512-z7-n512.txt -t16 > labryinth_adp
	export STM_CONFIG=$alg; time ../stamp-0.9.10/labyrinth/labyrinthSTM32_original -i ~/Documents/stamp-0.9.10/labyrinth/inputs/random-x512-y512-z7-n512.txt -t16 > labryinth_ori
	export STM_CONFIG=CGL; time ../stamp-0.9.10/labyrinth/labyrinthSTM32_original -i ~/Documents/stamp-0.9.10/labyrinth/inputs/random-x512-y512-z7-n512.txt -t16 > labryinth_cgl
	#gprof ../stamp-0.9.10/labyrinth/labyrinthSTM32 > profiling/gp_stamp_labyrinth_${alg}

	export STM_CONFIG=$alg; time ../stamp-0.9.10/ssca2/ssca2STM32 -s20 -i1.0 -u1.0 -l3 -p3 -t16 > ssca2_adp
	export STM_CONFIG=$alg; time ../stamp-0.9.10/ssca2/ssca2STM32_original -s20 -i1.0 -u1.0 -l3 -p3 -t16 > ssca2_ori
	export STM_CONFIG=CGL; time ../stamp-0.9.10/ssca2/ssca2STM32_original -s20 -i1.0 -u1.0 -l3 -p3 -t16 > ssca2_cgl
	#gprof ../stamp-0.9.10/ssca2/ssca2STM32 > profiling/gp_stamp_ssca2_${alg}

	export STM_CONFIG=$alg; time ../stamp-0.9.10/vacation/vacationSTM32 -n2 -q90 -u98 -r1048576 -t4194304 -c16 > vacation_adp1
	export STM_CONFIG=$alg; time ../stamp-0.9.10/vacation/vacationSTM32_original -n2 -q90 -u98 -r1048576 -t4194304 -c16 > vacation_ori1
	export STM_CONFIG=CGL; time ../stamp-0.9.10/vacation/vacationSTM32_original -n2 -q90 -u98 -r1048576 -t4194304 -c16 > vacation_cgl1
	#gprof ../stamp-0.9.10/vacation/vacationSTM32 > profiling/gp_stamp_vacation_${alg}_low
	export STM_CONFIG=$alg; time ../stamp-0.9.10/vacation/vacationSTM32 -n4 -q60 -u90 -r16384 -t4096 -c16 > vacation_adp2
	export STM_CONFIG=$alg; time ../stamp-0.9.10/vacation/vacationSTM32_original -n4 -q60 -u90 -r16384 -t4096 -c16 > vacation_ori2
	export STM_CONFIG=CGL; time ../stamp-0.9.10/vacation/vacationSTM32_original -n4 -q60 -u90 -r16384 -t4096 -c16 > vacation_cgl2
	#gprof ../stamp-0.9.10/vacation/vacationSTM32 > profiling/gp_stamp_vacation_${alg}_high

	export STM_CONFIG=$alg; time ../stamp-0.9.10/yada/yadaSTM32 -a15 -i ~/Documents/stamp-0.9.10/yada/inputs/ttimeu1000000.2 -t16 > yada_apt
	export STM_CONFIG=$alg; time ../stamp-0.9.10/yada/yadaSTM32_original -a15 -i ~/Documents/stamp-0.9.10/yada/inputs/ttimeu1000000.2 -t16 > yada_ori
	export STM_CONFIG=CGL; time ../stamp-0.9.10/yada/yadaSTM32_original -a15 -i ~/Documents/stamp-0.9.10/yada/inputs/ttimeu1000000.2 -t16 > yada_cgl
	#gprof ../stamp-0.9.10/yada/yadaSTM32 > profiling/gp_stamp_yada_${alg}
done

