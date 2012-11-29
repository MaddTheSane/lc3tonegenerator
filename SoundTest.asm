.ORIG x3000

	LD R0 R0Val
	LD R4 R4Val
		;the reserved opcode used changes the value of the NZP value.
		;If the NZP is Zero, that means there's no more notes
		;If Positive, the note was added successfully,
		;if negative, the note buffer is full
	.FILL xD100 ; Reserved R0, R4

	LD R0 R0Val2
	ld R4 R4Val2
	.FILL xD100 ; Reserved R0, R4
	LD R0 BlankVal
loop	.FILL xD000
	BRnp loop
	HALT	





BlankVal .FILL #0
R0Val .FILL #100
R4Val .FILL #1000
R0Val2 .FILL #120
R4Val2 .FILL #6000

.END