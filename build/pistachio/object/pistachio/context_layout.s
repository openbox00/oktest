	.file	"context_layout.cc"
	.global	foo
	.bss
	.align	2
	.type	foo, %object
	.size	foo, 72
foo:
	.space	72
	.text
	.align	2
	.global	_Z12make_offsetsv
	.type	_Z12make_offsetsv, %function
_Z12make_offsetsv:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 1, uses_anonymous_args = 0
	mov	ip, sp
	stmfd	sp!, {fp, ip, lr, pc}
	sub	fp, ip, #4
#APP
	DEFINE PT_KLR 0
	DEFINE PT_R0 4
	DEFINE PT_R1 8
	DEFINE PT_R2 12
	DEFINE PT_R3 16
	DEFINE PT_R4 20
	DEFINE PT_R5 24
	DEFINE PT_R6 28
	DEFINE PT_R7 32
	DEFINE PT_R8 36
	DEFINE PT_R9 40
	DEFINE PT_R10 44
	DEFINE PT_R11 48
	DEFINE PT_R12 52
	DEFINE PT_SP 56
	DEFINE PT_LR 60
	DEFINE PT_PC 64
	DEFINE PT_CPSR 68
	DEFINE PT_SIZE 72
	ldmfd	sp, {fp, sp, pc}
	.size	_Z12make_offsetsv, .-_Z12make_offsetsv
	.ident	"GCC: (GNU) 3.4.4"
