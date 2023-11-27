# vet -> $a0/$s3
# tam -> $a1/$s4
# i -> $s0
# j -> $s1
# idMenor -> $s2

.data
		vec: .word 5, 4, 3, 2, 1
	
.text
		la   $a0, vec  				# vector of integers
		li   $a1, 5   				# size of vector
		jal  selec
		j    exit 

selec:	addi $sp, $sp, -24
		sw   $ra, 20($sp)
		sw   $s0, 16($sp)
		sw   $s1, 12($sp)
		sw   $s2, 8($sp)
		sw   $s3, 4($sp)
		sw   $s4, 0($sp)
		move $s3, $a0
		move $s4, $a1
		addi $s0, $zero, 0			# i = 0
for1:	
		addi $t0, $s4, -1			# $t0 <- tam - 1
		slt  $t0, $s0, $t0			# if ( i < tam - 1 )
		beq  $t0, $zero, endfor1 	# exit if condition is false ( go to endfor1 )
		addi $s2, $s0, 0			# idMenor = i
		addi $s1, $s0, 1			# j = i + 1
for2:
		slt  $t0, $s1, $s4			# if ( j < tam )
		beq  $t0, $zero, endfor2 	# exit if condition is false ( go to endfor2 )
		sll  $t0, $s1, 2			# index j
		sll  $t1, $s2, 2			# index idMenor
		add  $t0, $s3, $t0			# address of vet[j]
		add  $t1, $s3, $t1			# address of vet[idMenor]
		lw   $t0, 0($t0)			# $t0 <- vet[j]
		lw   $t1, 0($t1)			# $t1 <- vet[idMenor]
		slt  $t0, $t0, $t1			# $t0 <- 1, if ( vet[j] < vet[idMenor] )
		beq  $t0, $zero, cond1		# skip if in case the condition was false
		addi $s2, $s1, 0			# idMenor = j
cond1:	
		addi $s1, $s1, 1
		j    for2
endfor2:
		beq $s0, $s2, cond2			# if ( i == idMenor ) skip ( go to cond2 )
		move $a0, $s3
		move $a1, $s0
		move $a2, $s2
		jal  troca					# troca( vet, i, idMenor )
cond2:	
		addi $s0, $s0, 1
		j    for1
endfor1:	
		lw   $s4, 0($sp)			# end of the code
		lw   $s3, 4($sp)
		lw   $s2, 8($sp)
		lw   $s1, 12($sp)
		lw   $s0, 16($sp)
		lw   $ra, 20($sp)
		addi $sp, $sp, 24
		jr   $ra

troca:	sll  $t1, $a1, 2
		sll  $t2, $a2, 2
		add  $t1, $a0, $t1
		add  $t2, $a0, $t2
		lw   $t0, 0($t1)			# temp = vet[i]
		lw   $t3, 0($t2)
		sw   $t3, 0($t1)			# vet[i] = vet[j]
		sw   $t0, 0($t2)			# vet[j] = temp
		jr   $ra
	
exit:	
