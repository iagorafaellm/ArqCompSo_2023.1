# vet -> $a0/$s3
# inicio -> $a1/$s4
# fim -> $a2/$s5
# i -> $s0
# j -> $s1
# pivo -> $s2

.data 
		vec: .word 5, 4, 3, 2, 1
	
.text
		la   $a0, vec  				# vector of integers
		li   $a1, 0   				# index of first element of vector
		li   $a2, 4					# index of last element of vector
		jal  quick
		j    exit 

quick:	addi $sp, $sp, -28
		sw   $ra, 24($sp)
		sw   $s0, 20($sp)
		sw   $s1, 16($sp)
		sw   $s2, 12($sp)
		sw   $s3, 8($sp)
		sw   $s4, 4($sp)
		sw   $s5, 0($sp)
		move $s3, $a0
		move $s4, $a1
		move $s5, $a2
		addi $s0, $s4, 0			# i = inicio
		addi $s1, $s5, 0			# j = fim
		add  $t0, $s0, $s1
		srl  $t0, $t0, 1			# (i+j)/2
		sll  $t0, $t0, 2
		add  $t0, $s3, $t0			# $t0 <- vet[(i+j)/2]
		lw   $s2, 0($t0)			# pivo = vet[(i+j)/2]
while1:	slt  $t0, $s1, $s0			# $t0 <- 1, if ( j < i )
		bne  $t0, $zero, endw1
while2: sll  $t0, $s0, 2
		add  $t0, $s3, $t0			# address of vet[i]
		lw   $t0, 0($t0)			# $t0 <- vet[i]
		slt  $t0, $t0, $s2			# $t0 <- 1, if ( vet[i] < pivo )
		beq  $t0, $zero, endw2
		addi $s0, $s0, 1			# i += 1
		j    while2
endw2: 
while3: sll  $t0, $s1, 2
		add  $t0, $s3, $t0			# address of vet[j]
		lw   $t0, 0($t0)			# $t0 <- vet[j]
		slt  $t0, $s2, $t0			# $t0 <- 1, if ( pivo < vet[j] )
		beq  $t0, $zero, endw3
		addi $s1, $s1, -1			# j -= 1
		j    while3
endw3:
		slt  $t0, $s1, $s0			# $t0 <- 1, if ( j < i )
		bne  $t0, $zero, cond1
		move $a0, $s3
		move $a1, $s0
		move $a2, $s1
		jal  troca					# troca( vet, i, j )
		addi $s0, $s0, 1			# i += 1
		addi $s1, $s1, -1			# j -= 1
cond1:	
		j    while1
endw1:	
		slt  $t0, $s4, $s1			# $t0 <- 1, if ( inicio < j )
		beq  $t0, $zero, cond2
		move $a0, $s3
		move $a1, $s4
		move $a2, $s1
		jal  quick					# quick( vet, inicio, j )
cond2:
		slt  $t0, $s0, $s5			# $t0 <- 1, if ( i < fim )
		beq  $t0, $zero, cond3
		move $a0, $s3
		move $a1, $s0
		move $a2, $s5
		jal  quick					# quick( vet, i, fim )
cond3:
		lw   $s5, 0($sp)			# end of the code
		lw   $s4, 4($sp)
		lw   $s3, 8($sp)
		lw   $s2, 12($sp)
		lw   $s1, 16($sp)
		lw   $s0, 20($sp)
		lw   $ra, 24($sp)
		addi $sp, $sp, 28
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
