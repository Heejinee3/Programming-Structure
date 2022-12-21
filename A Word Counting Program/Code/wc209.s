	.file	"wc209.c"
	.text
	.globl	CountLineNum
	.type	CountLineNum, @function
CountLineNum:
.LFB0:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$32, %rsp
	movq	%rdi, -24(%rbp)
	movl	$1, -8(%rbp)
	movq	-24(%rbp), %rax
	movzbl	(%rax), %eax
	testb	%al, %al
	jne	.L2
	movl	$0, %eax
	jmp	.L3
.L2:
	movl	$0, -4(%rbp)
	jmp	.L4
.L6:
	movl	-4(%rbp), %eax
	movslq	%eax, %rdx
	movq	-24(%rbp), %rax
	addq	%rdx, %rax
	movzbl	(%rax), %eax
	cmpb	$10, %al
	jne	.L5
	addl	$1, -8(%rbp)
.L5:
	addl	$1, -4(%rbp)
.L4:
	movq	-24(%rbp), %rax
	movq	%rax, %rdi
	call	strlen
	cmpl	-4(%rbp), %eax
	jge	.L6
	movl	-8(%rbp), %eax
.L3:
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE0:
	.size	CountLineNum, .-CountLineNum
	.globl	CountWordNum
	.type	CountWordNum, @function
CountWordNum:
.LFB1:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$32, %rsp
	movq	%rdi, -24(%rbp)
	movl	$0, -8(%rbp)
	movl	$0, -4(%rbp)
	jmp	.L8
.L13:
	call	__ctype_b_loc
	movq	(%rax), %rax
	movl	-4(%rbp), %edx
	movslq	%edx, %rcx
	movq	-24(%rbp), %rdx
	addq	%rcx, %rdx
	movzbl	(%rdx), %edx
	movsbq	%dl, %rdx
	addq	%rdx, %rdx
	addq	%rdx, %rax
	movzwl	(%rax), %eax
	movzwl	%ax, %eax
	andl	$8192, %eax
	testl	%eax, %eax
	je	.L9
	addl	$1, -4(%rbp)
	jmp	.L8
.L9:
	addl	$1, -8(%rbp)
	addl	$1, -4(%rbp)
	jmp	.L10
.L12:
	call	__ctype_b_loc
	movq	(%rax), %rax
	movl	-4(%rbp), %edx
	movslq	%edx, %rcx
	movq	-24(%rbp), %rdx
	addq	%rcx, %rdx
	movzbl	(%rdx), %edx
	movsbq	%dl, %rdx
	addq	%rdx, %rdx
	addq	%rdx, %rax
	movzwl	(%rax), %eax
	movzwl	%ax, %eax
	andl	$8192, %eax
	testl	%eax, %eax
	je	.L11
	addl	$1, -4(%rbp)
	jmp	.L8
.L11:
	addl	$1, -4(%rbp)
.L10:
	movl	-4(%rbp), %eax
	movslq	%eax, %rdx
	movq	-24(%rbp), %rax
	addq	%rdx, %rax
	movzbl	(%rax), %eax
	testb	%al, %al
	jne	.L12
.L8:
	movl	-4(%rbp), %eax
	movslq	%eax, %rdx
	movq	-24(%rbp), %rax
	addq	%rdx, %rax
	movzbl	(%rax), %eax
	testb	%al, %al
	jne	.L13
	movl	-8(%rbp), %eax
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE1:
	.size	CountWordNum, .-CountWordNum
	.section	.rodata
.LC0:
	.string	"%[^NULL]s"
	.align 8
.LC1:
	.string	"Error: line %d: unterminates comment\n"
.LC2:
	.string	"%d %d %d\n"
	.text
	.globl	main
	.type	main, @function
main:
.LFB2:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	pushq	%r12
	pushq	%rbx
	subq	$4000032, %rsp
	.cfi_offset 12, -24
	.cfi_offset 3, -32
	movq	%fs:40, %rax
	movq	%rax, -24(%rbp)
	xorl	%eax, %eax
	movq	stdin(%rip), %rax
	leaq	-4000032(%rbp), %rdx
	movl	$.LC0, %esi
	movq	%rax, %rdi
	movl	$0, %eax
	call	__isoc99_fscanf
	movq	$0, -2000032(%rbp)
	leaq	-2000024(%rbp), %rax
	movl	$1999992, %edx
	movl	$0, %esi
	movq	%rax, %rdi
	call	memset
	movl	$0, -4000044(%rbp)
	movl	$0, -4000040(%rbp)
	movl	$1, -4000036(%rbp)
	jmp	.L16
.L25:
	movl	-4000044(%rbp), %eax
	cltq
	movzbl	-4000032(%rbp,%rax), %eax
	cmpb	$47, %al
	jne	.L17
	movl	-4000044(%rbp), %eax
	addl	$1, %eax
	cltq
	movzbl	-4000032(%rbp,%rax), %eax
	cmpb	$42, %al
	je	.L18
.L17:
	movl	-4000044(%rbp), %eax
	cltq
	movzbl	-4000032(%rbp,%rax), %edx
	movl	-4000040(%rbp), %eax
	cltq
	movb	%dl, -2000032(%rbp,%rax)
	addl	$1, -4000044(%rbp)
	addl	$1, -4000040(%rbp)
	jmp	.L16
.L18:
	leaq	-2000032(%rbp), %rax
	movq	%rax, %rdi
	call	CountLineNum
	movl	%eax, -4000036(%rbp)
	addl	$2, -4000044(%rbp)
	movl	-4000040(%rbp), %eax
	cltq
	movb	$32, -2000032(%rbp,%rax)
	addl	$1, -4000040(%rbp)
.L24:
	movl	-4000044(%rbp), %eax
	cltq
	movzbl	-4000032(%rbp,%rax), %eax
	cmpb	$10, %al
	jne	.L19
	movl	-4000044(%rbp), %eax
	cltq
	movzbl	-4000032(%rbp,%rax), %edx
	movl	-4000040(%rbp), %eax
	cltq
	movb	%dl, -2000032(%rbp,%rax)
	addl	$1, -4000040(%rbp)
	addl	$1, -4000044(%rbp)
	jmp	.L24
.L19:
	movl	-4000044(%rbp), %eax
	cltq
	movzbl	-4000032(%rbp,%rax), %eax
	cmpb	$42, %al
	jne	.L21
	movl	-4000044(%rbp), %eax
	addl	$1, %eax
	cltq
	movzbl	-4000032(%rbp,%rax), %eax
	cmpb	$47, %al
	jne	.L21
	addl	$2, -4000044(%rbp)
	jmp	.L16
.L21:
	movl	-4000044(%rbp), %eax
	cltq
	movzbl	-4000032(%rbp,%rax), %eax
	testb	%al, %al
	jne	.L22
	movq	stderr(%rip), %rax
	movl	-4000036(%rbp), %edx
	movl	$.LC1, %esi
	movq	%rax, %rdi
	movl	$0, %eax
	call	fprintf
	movl	$1, %eax
	jmp	.L26
.L22:
	addl	$1, -4000044(%rbp)
	jmp	.L24
.L16:
	movl	-4000044(%rbp), %eax
	cltq
	movzbl	-4000032(%rbp,%rax), %eax
	testb	%al, %al
	jne	.L25
	leaq	-2000032(%rbp), %rax
	movq	%rax, %rdi
	call	strlen
	movl	%eax, %r12d
	leaq	-2000032(%rbp), %rax
	movq	%rax, %rdi
	call	CountWordNum
	movl	%eax, %ebx
	leaq	-2000032(%rbp), %rax
	movq	%rax, %rdi
	call	CountLineNum
	movl	%eax, %edx
	movq	stdout(%rip), %rax
	movl	%r12d, %r8d
	movl	%ebx, %ecx
	movl	$.LC2, %esi
	movq	%rax, %rdi
	movl	$0, %eax
	call	fprintf
	movl	$0, %eax
.L26:
	movq	-24(%rbp), %rcx
	xorq	%fs:40, %rcx
	je	.L27
	call	__stack_chk_fail
.L27:
	addq	$4000032, %rsp
	popq	%rbx
	popq	%r12
	popq	%rbp
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE2:
	.size	main, .-main
	.ident	"GCC: (Ubuntu 5.4.0-6ubuntu1~16.04.9) 5.4.0 20160609"
	.section	.note.GNU-stack,"",@progbits
