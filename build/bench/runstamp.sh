algs="LLT"

for alg in $algs
do
	for thd in 16 64
	do
		export STM_CONFIG=$alg; ../stamp-0.9.10/bayes/bayesSTM32 -v32 -r4096 -n10 -p40 -i2 -e8 -s1 -t$thd > bayes_adp_$thd
		export STM_CONFIG=$alg; ../stamp-0.9.10/bayes/bayesSTM32_original -v32 -r4096 -n10 -p40 -i2 -e8 -s1 -t$thd > bayes_ori_$thd
		export STM_CONFIG=CGL; ../stamp-0.9.10/bayes/bayesSTM32_original -v32 -r4096 -n10 -p40 -i2 -e8 -s1 -t$thd > bayes_cgl_$thd

		export STM_CONFIG=$alg; ../stamp-0.9.10/genome/genomeSTM32 -g16384 -s64 -n16777216 -t$thd > genome_adp_$thd
		export STM_CONFIG=$alg; ../stamp-0.9.10/genome/genomeSTM32_original -g16384 -s64 -n16777216 -t$thd > genome_ori_$thd
		export STM_CONFIG=CGL; ../stamp-0.9.10/genome/genomeSTM32_original -g16384 -s64 -n16777216 -t$thd > genome_cgl_$thd

		export STM_CONFIG=$alg; ../stamp-0.9.10/intruder/intruderSTM32 -a10 -l128 -n262144 -s1 -t$thd > intruder_adp_$thd
		export STM_CONFIG=$alg; ../stamp-0.9.10/intruder/intruderSTM32_original -a10 -l128 -n262144 -s1 -t$thd > intruder_ori_$thd
		export STM_CONFIG=CGL; ../stamp-0.9.10/intruder/intruderSTM32_original -a10 -l128 -n262144 -s1 -t$thd > intruder_cgl_$thd

		export STM_CONFIG=$alg; ../stamp-0.9.10/kmeans/kmeansSTM32 -m40 -n40 -t0.00001 -i ~/Documents/stamp-0.9.10/kmeans/inputs/random-n65536-d32-c16.txt -p$thd > kmeans_adp1_$thd
		export STM_CONFIG=$alg; ../stamp-0.9.10/kmeans/kmeansSTM32_original -m40 -n40 -t0.00001 -i ~/Documents/stamp-0.9.10/kmeans/inputs/random-n65536-d32-c16.txt -p$thd > kmeans_ori1_$thd
		export STM_CONFIG=CGL; ../stamp-0.9.10/kmeans/kmeansSTM32_original -m40 -n40 -t0.00001 -i ~/Documents/stamp-0.9.10/kmeans/inputs/random-n65536-d32-c16.txt -p$thd > kmeans_cgl1_$thd

		export STM_CONFIG=$alg; ../stamp-0.9.10/kmeans/kmeansSTM32 -m15 -n15 -t0.00001 -i ~/Documents/stamp-0.9.10/kmeans/inputs/random-n65536-d32-c16.txt -p$thd > kmeans_adp2_$thd
		export STM_CONFIG=$alg; ../stamp-0.9.10/kmeans/kmeansSTM32_original -m15 -n15 -t0.00001 -i ~/Documents/stamp-0.9.10/kmeans/inputs/random-n65536-d32-c16.txt -p$thd > kmeans_ori2_$thd
		export STM_CONFIG=CGL; ../stamp-0.9.10/kmeans/kmeansSTM32_original -m15 -n15 -t0.00001 -i ~/Documents/stamp-0.9.10/kmeans/inputs/random-n65536-d32-c16.txt -p$thd > kmeans_cgl2_$thd

		######export STM_CONFIG=$alg; ../stamp-0.9.10/labyrinth/labyrinthSTM32 -i ~/Documents/stamp-0.9.10/labyrinth/inputs/random-x512-y512-z7-n512.txt -t$thd > labryinth_adp_$thd
		######export STM_CONFIG=$alg; ../stamp-0.9.10/labyrinth/labyrinthSTM32_original -i ~/Documents/stamp-0.9.10/labyrinth/inputs/random-x512-y512-z7-n512.txt -t$thd > labryinth_ori_$thd
		######export STM_CONFIG=CGL; ../stamp-0.9.10/labyrinth/labyrinthSTM32_original -i ~/Documents/stamp-0.9.10/labyrinth/inputs/random-x512-y512-z7-n512.txt -t$thd > labryinth_cgl_$thd

		export STM_CONFIG=$alg; ../stamp-0.9.10/ssca2/ssca2STM32 -s20 -i1.0 -u1.0 -l3 -p3 -t$thd > ssca2_adp_$thd
		export STM_CONFIG=$alg; ../stamp-0.9.10/ssca2/ssca2STM32_original -s20 -i1.0 -u1.0 -l3 -p3 -t$thd > ssca2_ori_$thd
		export STM_CONFIG=CGL; ../stamp-0.9.10/ssca2/ssca2STM32_original -s20 -i1.0 -u1.0 -l3 -p3 -t$thd > ssca2_cgl_$thd

		export STM_CONFIG=$alg; ../stamp-0.9.10/vacation/vacationSTM32 -n2 -q90 -u98 -r1048576 -t4194304 -c$thd > vacation_adp1_$thd
		export STM_CONFIG=$alg; ../stamp-0.9.10/vacation/vacationSTM32_original -n2 -q90 -u98 -r1048576 -t4194304 -c$thd > vacation_ori1_$thd
		export STM_CONFIG=CGL; ../stamp-0.9.10/vacation/vacationSTM32_original -n2 -q90 -u98 -r1048576 -t4194304 -c$thd > vacation_cgl1_$thd

		export STM_CONFIG=$alg; ../stamp-0.9.10/vacation/vacationSTM32 -n4 -q60 -u90 -r16384 -t4096 -c$thd > vacation_adp2_$thd
		export STM_CONFIG=$alg; ../stamp-0.9.10/vacation/vacationSTM32_original -n4 -q60 -u90 -r16384 -t4096 -c$thd > vacation_ori2_$thd
		export STM_CONFIG=CGL; ../stamp-0.9.10/vacation/vacationSTM32_original -n4 -q60 -u90 -r16384 -t4096 -c$thd > vacation_cgl2_$thd

		######export STM_CONFIG=$alg; ../stamp-0.9.10/yada/yadaSTM32 -a15 -i ~/Documents/stamp-0.9.10/yada/inputs/ttimeu1000000.2 -t$thd > yada_adp_$thd
		######export STM_CONFIG=$alg; ../stamp-0.9.10/yada/yadaSTM32_original -a15 -i ~/Documents/stamp-0.9.10/yada/inputs/ttimeu1000000.2 -t$thd > yada_ori_$thd
		######export STM_CONFIG=CGL; ../stamp-0.9.10/yada/yadaSTM32_original -a15 -i ~/Documents/stamp-0.9.10/yada/inputs/ttimeu1000000.2 -t$thd > yada_cgl_$thd
	done
done

