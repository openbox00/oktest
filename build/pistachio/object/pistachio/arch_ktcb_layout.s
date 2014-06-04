	.file	"arch_ktcb_layout.cc"
	.global	foo
	.bss
	.align	2
	.type	foo, %object
	.size	foo, 92
foo:
	.space	92
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
	DEFINE OFS_ARCH_KTCB_CONTEXT 0
	DEFINE OFS_ARCH_KTCB_MISC 72
	DEFINE OFS_ARCH_KTCB_EXC_NUM 88
	ldmfd	sp, {fp, sp, pc}
	.size	_Z12make_offsetsv, .-_Z12make_offsetsv
	.ident	"GCC: (GNU) 3.4.4"
