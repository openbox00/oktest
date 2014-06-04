	.file	"tcb_layout.cc"
	.data
	.align	2
	.type	generic_execution_units, %object
	.size	generic_execution_units, 4
generic_execution_units:
	.word	1
	.global	foo
	.bss
	.align	2
	.type	foo, %object
	.size	foo, 308
foo:
	.space	308
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
	DEFINE OFS_TCB_UTCB_LOCATION 0
	DEFINE OFS_TCB_UTCB 4
	DEFINE OFS_TCB_SPACE 8
	DEFINE OFS_TCB_SPACE_ID 12
	DEFINE OFS_TCB_PAGE_DIRECTORY 16
	DEFINE OFS_TCB_PAGER 20
	DEFINE OFS_TCB_THREAD_LOCK 28
	DEFINE OFS_TCB_THREAD_STATE 32
	DEFINE OFS_TCB_PARTNER 36
	DEFINE OFS_TCB_END_POINT 40
	DEFINE OFS_TCB_WAITING_FOR 56
	DEFINE OFS_TCB_EXCEPTION_HANDLER 60
	DEFINE OFS_TCB_RESOURCE_BITS 68
	DEFINE OFS_TCB_CONT 72
	DEFINE OFS_TCB_PREEMPTION_CONTINUATION 76
	DEFINE OFS_TCB_ARCH 80
	DEFINE OFS_TCB_RUNTIME_FLAGS 172
	DEFINE OFS_TCB_POST_SYSCALL_CALLBACK 176
	DEFINE OFS_TCB_READY_LIST 180
	DEFINE OFS_TCB_BLOCKED_LIST 188
	DEFINE OFS_TCB_MUTEXES_HEAD 196
	DEFINE OFS_TCB_BASE_PRIO 200
	DEFINE OFS_TCB_EFFECTIVE_PRIO 204
	DEFINE OFS_TCB_TIMESLICE_LENGTH 208
	DEFINE OFS_TCB_CURRENT_TIMESLICE 212
	DEFINE OFS_TCB_SAVED_PARTNER 216
	DEFINE OFS_TCB_SAVED_STATE 220
	DEFINE OFS_TCB_RESOURCES 224
	DEFINE OFS_TCB_THREAD_LIST 228
	DEFINE OFS_TCB_SAVED_SENT_FROM 236
	DEFINE OFS_TCB_SYS_DATA 240
	DEFINE OFS_TCB_TCB_IDX 292
	DEFINE OFS_TCB_MASTER_CAP 296
	DEFINE OFS_TCB_SENT_FROM 300
	DEFINE OFS_TCB_IRQ_STACK 304
	ldmfd	sp, {fp, sp, pc}
	.size	_Z12make_offsetsv, .-_Z12make_offsetsv
	.align	2
	.type	_Z41__static_initialization_and_destruction_0ii, %function
_Z41__static_initialization_and_destruction_0ii:
	@ args = 0, pretend = 0, frame = 8
	@ frame_needed = 1, uses_anonymous_args = 0
	mov	ip, sp
	stmfd	sp!, {fp, ip, lr, pc}
	sub	fp, ip, #4
	sub	sp, sp, #8
	str	r0, [fp, #-16]
	str	r1, [fp, #-20]
	ldr	r2, [fp, #-20]
	mov	r3, #65280
	add	r3, r3, #255
	cmp	r2, r3
	bne	.L2
	ldr	r3, [fp, #-16]
	cmp	r3, #1
	bne	.L2
	ldr	r0, .L4
	bl	_ZN5tcb_tC1Ev
.L2:
	sub	sp, fp, #12
	ldmfd	sp, {fp, sp, pc}
.L5:
	.align	2
.L4:
	.word	foo
	.size	_Z41__static_initialization_and_destruction_0ii, .-_Z41__static_initialization_and_destruction_0ii
	.section	.gnu.linkonce.t._ZN5tcb_tC1Ev,"ax",%progbits
	.align	2
	.weak	_ZN5tcb_tC1Ev
	.type	_ZN5tcb_tC1Ev, %function
_ZN5tcb_tC1Ev:
	@ args = 0, pretend = 0, frame = 4
	@ frame_needed = 1, uses_anonymous_args = 0
	mov	ip, sp
	stmfd	sp!, {fp, ip, lr, pc}
	sub	fp, ip, #4
	sub	sp, sp, #4
	str	r0, [fp, #-16]
	ldr	r3, [fp, #-16]
	add	r3, r3, #32
	mov	r0, r3
	ldr	r3, .L7
	mov	lr, pc
	mov	pc, r3
	ldr	r3, [fp, #-16]
	add	r3, r3, #68
	mov	r0, r3
	ldr	r3, .L7+4
	mov	lr, pc
	mov	pc, r3
	ldr	r3, [fp, #-16]
	add	r3, r3, #220
	mov	r0, r3
	ldr	r3, .L7
	mov	lr, pc
	mov	pc, r3
	ldmib	sp, {fp, sp, pc}
.L8:
	.align	2
.L7:
	.word	_ZN14thread_state_tC1Ev
	.word	_ZN15resource_bits_tC1Ev
	.size	_ZN5tcb_tC1Ev, .-_ZN5tcb_tC1Ev
	.section	.gnu.linkonce.t._ZN15resource_bits_tC1Ev,"ax",%progbits
	.align	2
	.weak	_ZN15resource_bits_tC1Ev
	.type	_ZN15resource_bits_tC1Ev, %function
_ZN15resource_bits_tC1Ev:
	@ args = 0, pretend = 0, frame = 4
	@ frame_needed = 1, uses_anonymous_args = 0
	mov	ip, sp
	stmfd	sp!, {fp, ip, lr, pc}
	sub	fp, ip, #4
	sub	sp, sp, #4
	str	r0, [fp, #-16]
	ldr	r0, [fp, #-16]
	ldr	r3, .L10
	mov	lr, pc
	mov	pc, r3
	ldmib	sp, {fp, sp, pc}
.L11:
	.align	2
.L10:
	.word	_ZN9bitmask_tC1Ev
	.size	_ZN15resource_bits_tC1Ev, .-_ZN15resource_bits_tC1Ev
	.section	.gnu.linkonce.t._ZN9bitmask_tC1Ev,"ax",%progbits
	.align	2
	.weak	_ZN9bitmask_tC1Ev
	.type	_ZN9bitmask_tC1Ev, %function
_ZN9bitmask_tC1Ev:
	@ args = 0, pretend = 0, frame = 4
	@ frame_needed = 1, uses_anonymous_args = 0
	mov	ip, sp
	stmfd	sp!, {fp, ip, lr, pc}
	sub	fp, ip, #4
	sub	sp, sp, #4
	str	r0, [fp, #-16]
	ldr	r2, [fp, #-16]
	mov	r3, #0
	str	r3, [r2, #0]
	ldmib	sp, {fp, sp, pc}
	.size	_ZN9bitmask_tC1Ev, .-_ZN9bitmask_tC1Ev
	.section	.gnu.linkonce.t._ZN14thread_state_tC1Ev,"ax",%progbits
	.align	2
	.weak	_ZN14thread_state_tC1Ev
	.type	_ZN14thread_state_tC1Ev, %function
_ZN14thread_state_tC1Ev:
	@ args = 0, pretend = 0, frame = 4
	@ frame_needed = 1, uses_anonymous_args = 0
	mov	ip, sp
	stmfd	sp!, {fp, ip, lr, pc}
	sub	fp, ip, #4
	sub	sp, sp, #4
	str	r0, [fp, #-16]
	ldr	r2, [fp, #-16]
	mov	r3, #15
	str	r3, [r2, #0]
	ldmib	sp, {fp, sp, pc}
	.size	_ZN14thread_state_tC1Ev, .-_ZN14thread_state_tC1Ev
	.text
	.align	2
	.type	_GLOBAL__I_foo, %function
_GLOBAL__I_foo:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 1, uses_anonymous_args = 0
	mov	ip, sp
	stmfd	sp!, {fp, ip, lr, pc}
	sub	fp, ip, #4
	mov	r0, #1
	mov	r1, #65280
	add	r1, r1, #255
	bl	_Z41__static_initialization_and_destruction_0ii
	ldmfd	sp, {fp, sp, pc}
	.size	_GLOBAL__I_foo, .-_GLOBAL__I_foo
	.section	.ctors,"aw",%progbits
	.align	2
	.word	_GLOBAL__I_foo
	.ident	"GCC: (GNU) 3.4.4"
