# vet -> $a0/$s2
# size -> $a1/$s3
# i -> $s0
# trocado -> $s1

.data
		vec: .word 5, 4, 3, 2, 1
	
.text
		la   $a0, vec  				# vector of integers
		li   $a1, 5    				# size of vector
		jal  bubble
		j    exit 

bubble:	addi $sp, $sp, -20
		sw   $ra, 16($sp)
		sw   $s0, 12($sp)
		sw   $s1, 8($sp)
		sw   $s2, 4($sp)
		sw   $s3, 0($sp)
		move $s2, $a0
		move $s3, $a1
		addi $s1, $zero, 0			# trocado = 0 (FALSE)
do1:				# do {
		addi $s1, $zero, 0			# trocado = 0
		addi $s0, $zero, 0			# i = 0	
for:	
		addi $t0, $s3, -1			# $t0 <- size - 1
		slt  $t0, $s0, $t0			# if ( i < size-1 )
		beq  $t0, $zero, endfor 	# exit if condition is false ( go to endfor )
		sll  $t0, $s0, 2			# index i
		add  $t0, $s2, $t0			# address of vet[i]
		lw   $t1, 0($t0)			# $t1 <- vet[i]
		lw   $t2, 4($t0)			# $t2 <- vet[i+1]
		slt  $t0, $t2, $t1			# $t0 <- 1, if ( vet[i+1] < vet[i] )
		beq  $t0, $zero, cond1		# skip if in case the condition was false
		move $a0, $s2
		move $a1, $s0
		jal  troca					# troca( vet, i )
		addi $s1, $zero, 1			# trocado = 1
cond1:	addi $s0, $s0, 1
		j    for
endfor:	
		addi $t0, $zero, 1
		beq  $s1, $t0, do1			# } while(trocado)
		lw   $s3, 0($sp)			# end of the code
		lw   $s2, 4($sp)
		lw   $s1, 8($sp)
		lw   $s0, 12($sp)
		lw   $ra, 16($sp)
		addi $sp, $sp, 20
		jr   $ra

troca:	sll  $t1, $a1, 2
		add  $t1, $a0, $t1
		lw   $t0, 0($t1)
		lw   $t2, 4($t1)
		sw   $t2, 0($t1)
		sw   $t0, 4($t1)
		jr   $ra
	
exit:	
