	## -------------------------------------------------------------
	## Name : Heejin Jeong
	## Student ID : 20170616
	## HW # : 4
	## File name : mydc.s
	## -------------------------------------------------------------
### --------------------------------------------------------------------

	.equ   ARRAYSIZE, 20
	.equ   EOF, -1
	.equ   P, 112	    ## ascii code of 'p'
	.equ   Q, 113
	.equ   C, 99
	.equ   D, 100
	.equ   R, 114
	.equ   F, 102
	.equ   PLUS, 43
	.equ   MINUS, 45
	.equ   MINUSSIGN, 95
	.equ   DIVIDE, 47
	.equ   MULTIPLY, 42
	.equ   EXPO, 94
	.equ   REMAIN, 37
	.equ   UICARRY, 4

### --------------------------------------------------------------------

        .section ".rodata"

scanfFormat:
	.asciz "%s"

printfFormat:
	.asciz "%d\n"

emptyErrorStr:
	.asciz "dc: stack empty\n"

### --------------------------------------------------------------------

        .section ".data"

### --------------------------------------------------------------------

        .section ".bss"

buffer:				## input string
        .skip  ARRAYSIZE

### --------------------------------------------------------------------

	.section ".text"

	.globl  main
	.type   main,@function

### --------------------------------------------------------------------
	## -------------------------------------------------------------
	## int main(void)
	## Runs desk calculator program.  Returns 0.
	## -------------------------------------------------------------

main:

	pushl   %ebp
	movl    %esp, %ebp
	subl	$UICARRY, %esp
	movl	$0, (%esp)   ## result variable
	subl	$UICARRY, %esp
	movl	$0, (%esp)   ## num variable(count number of stack)

### --------------------------------------------------------------------

input:
	
	## scanf("%s", buffer)
	pushl	$buffer
	pushl	$scanfFormat
	call    scanf
	addl    $8, %esp

### p-operation---------------------------------------------------------

	## if ((buffer[0] != 'p') || (buffer[1] != 0))
	movl 	$0, %eax
	movb	buffer(,%eax,1), %cl
	cmpb	$P, %cl
	jne 	pendif
	incl	%eax
	movb	buffer(,%eax,1), %cl
	cmpb	$0, %cl
	jne	pendif

	## if (num < 1)
	movl	-8(%ebp), %ebx
	cmpl	$1, %ebx
	jl	pelse
	
	## printf("%d\n",stack[num-1])	
	pushl	(%esp)
	pushl	$printfFormat
	call	printf
	addl	$8, %esp
	jmp	input

pelse:

	## printf("dc: stack empty\n")
	pushl	$emptyErrorStr
	call	printf
	addl	$4, %esp
	jmp	input

### q-operation---------------------------------------------------------

pendif:

	## if ((buffer[0] != 'q') || (buffer[1] != 0))
	movl 	$0, %eax
	movb	buffer(,%eax,1), %cl
	cmpb	$Q, %cl
	jne 	qendif
	incl	%eax
	movb	buffer(,%eax,1), %cl
	cmpb	$0, %cl
	jne	qendif
	jmp	quit

### EOF---------------------------------------------------------

qendif:

	## if ((buffer[0] != EOF) || (buffer[1] != 0))
	movl 	$0, %eax
	movb	buffer(,%eax,1), %cl
	cmpb	$EOF, %cl
	jne 	EOFendif
	incl	%eax
	movb	buffer(,%eax,1), %cl
	cmpb	$0, %cl
	jne	EOFendif
	jmp	quit

### c-operation---------------------------------------------------------

EOFendif:

	## if ((buffer[0] != 'c') || (buffer[1] != 0))
	movl 	$0, %eax
	movb	buffer(,%eax,1), %cl
	cmpb	$C, %cl
	jne 	cendif
	incl	%eax
	movb	buffer(,%eax,1), %cl
	cmpb	$0, %cl
	jne	cendif

	## if (num < 1)
	movl	-8(%ebp), %ebx
	cmpl	$1, %ebx
	jl	celse
	
	## for (i=0;i<num;i++) stack[i]=0 , num=0
        movl	-8(%ebp), %ebx
	movl	$4, %eax
	imull	%ebx
	addl	%eax, %esp
	movl	$0, -8(%ebp)
	jmp	input	

celse:

	## printf("dc: stack empty\n")
	pushl	$emptyErrorStr
	call	printf
	addl	$4, %esp
	jmp	input

### d-operation---------------------------------------------------------

cendif:

	## if ((buffer[0] != 'd') || (buffer[1] != 0))
	movl 	$0, %eax
	movb	buffer(,%eax,1), %cl
	cmpb	$D, %cl
	jne 	dendif
	incl	%eax
	movb	buffer(,%eax,1), %cl
	cmpb	$0, %cl
	jne	dendif

	## if (num < 1)
	movl	-8(%ebp), %ebx
	cmpl	$1, %ebx
	jl	delse
	
	## stack[num]=stack[num-1] , num+=1
        movl	(%esp), %ecx
	pushl	%ecx
	movl	-8(%ebp), %ebx
	incl	%ebx
	movl	%ebx, -8(%ebp)
	jmp	input	

delse:

	## printf("dc: stack empty\n")
	pushl	$emptyErrorStr
	call	printf
	addl	$4, %esp
	jmp	input

### r-operation---------------------------------------------------------

dendif:

	## if ((buffer[0] != 'r') || (buffer[1] != 0))
	movl 	$0, %eax
	movb	buffer(,%eax,1), %cl
	cmpb	$R, %cl
	jne 	rendif
	incl	%eax
	movb	buffer(,%eax,1), %cl
	cmpb	$0, %cl
	jne	rendif

	## if (num < 2)
	movl	-8(%ebp), %ebx
	cmpl	$1, %ebx
	jl	relse
	
	## i = stack[num-1], stack[num-1] = stack[num-2]
        ## stack[num-2]= i
        movl	(%esp), %ecx
	movl	4(%esp), %eax
	movl	%eax, (%esp)
	movl	%ecx, 4(%esp)
	jmp	input	

relse:

	## printf("dc: stack empty\n")
	pushl	$emptyErrorStr
	call	printf
	addl	$4, %esp
	jmp	input

### f-operation---------------------------------------------------------

rendif:

	## if ((buffer[0] != 'f') || (buffer[1] != 0))
	movl 	$0, %eax
	movb	buffer(,%eax,1), %cl
	cmpb	$F, %cl
	jne 	fendif
	incl	%eax
	movb	buffer(,%eax,1), %cl
	cmpb	$0, %cl
	jne	fendif
	pushl	-8(%ebp)
	movl	$0, %ebx

print:
	## printf("%d\n",stack[num-1])
	cmpl	$0, (%esp)
	je	arrange
	pushl	4(%esp,%ebx,4)
	pushl	$printfFormat
	call	printf
	addl	$8, %esp
	subl	$1, (%esp)
	addl	$1, %ebx
	jmp	print

arrange:
	
	addl	$4, %esp


### '+'-operation-------------------------------------------------------

fendif:

	## if (buffer[0] != '+')
	movl 	$0, %eax
	movb	buffer(,%eax,1), %cl
	cmpb	$PLUS, %cl
	jne 	plusendif

	## if (num < 2)
	movl	-8(%ebp), %ebx
	cmpl	$2, %ebx
	jl	pluselse

	movl	$0, %edx
	addl	(%esp), %edx
	addl	4(%esp), %edx
	movl	%edx, -4(%ebp)
	addl	$4, %esp
	movl	%edx, (%esp)
	movl	-8(%ebp), %ebx
	decl	%ebx
	movl	%ebx, -8(%ebp)
	jmp	input

pluselse:
	
	## printf("dc: stack empty\n")
	pushl	$emptyErrorStr
	call	printf
	add	$4, %esp
	jmp	input

### '-'-operation-------------------------------------------------------

plusendif:

	## if (buffer[0] != '-')
	movl 	$0, %eax
	movb	buffer(,%eax,1), %cl
	cmpb	$MINUS, %cl
	jne 	minusendif

	## if (num < 2)
	movl	-8(%ebp), %ebx
	cmpl	$2, %ebx
	jl	minuselse

	movl	$0, %edx
	subl	(%esp), %edx
	addl	4(%esp), %edx
	movl	%edx, -4(%ebp)
	addl	$4, %esp
	movl	%edx, (%esp)
	movl	-8(%ebp), %ebx
	decl	%ebx
	movl	%ebx, -8(%ebp)
	jmp	input

minuselse:
	
	## printf("dc: stack empty\n")
	pushl	$emptyErrorStr
	call	printf
	add	$4, %esp
	jmp	input

### '*'-operation-------------------------------------------------------

minusendif:

	## if (buffer[0] != '*')
	movl 	$0, %eax
	movb	buffer(,%eax,1), %cl
	cmpb	$MULTIPLY, %cl
	jne 	multiendif

	## if (num < 2)
	movl	-8(%ebp), %ebx
	cmpl	$2, %ebx
	jl	multielse

	movl	4(%esp), %eax
	movl	(%esp), %ebx
	imull	%ebx
	movl	%eax, -4(%ebp)
	addl	$4, %esp
	movl	%eax, (%esp)
	movl	-8(%ebp), %ebx
	decl	%ebx
	movl	%ebx, -8(%ebp)
	jmp	input

multielse:
	
	## printf("dc: stack empty\n")
	pushl	$emptyErrorStr
	call	printf
	add	$4, %esp
	jmp	input

### '/'-operation-------------------------------------------------------

multiendif:

	## if (buffer[0] != '/')
	movl 	$0, %eax
	movb	buffer(,%eax,1), %cl
	cmpb	$DIVIDE, %cl
	jne 	dividendif

	## if (num < 2)
	movl	-8(%ebp), %ebx
	cmpl	$2, %ebx
	jl	dividelse

	movl	4(%esp), %eax
	movl	(%esp), %ebx
	idivl	%ebx
	movl	%eax, -4(%ebp)
	addl	$4, %esp
	movl	%eax, (%esp)
	movl	-8(%ebp), %ebx
	decl	%ebx
	movl	%ebx, -8(%ebp)
	jmp	input

dividelse:
	
	## printf("dc: stack empty\n")
	pushl	$emptyErrorStr
	call	printf
	add	$4, %esp
	jmp	input

### '%'-operation-------------------------------------------------------

dividendif:

	## if (buffer[0] != '%')
	movl 	$0, %eax
	movb	buffer(,%eax,1), %cl
	cmpb	$REMAIN, %cl
	jne 	remainendif

	## if (num < 2)
	movl	-8(%ebp), %ebx
	cmpl	$2, %ebx
	jl	remainelse

	movl	4(%esp), %eax
	movl	(%esp), %ebx
	idivl	%ebx
	movl	%edx, -4(%ebp)
	addl	$4, %esp
	movl	%edx, (%esp)
	movl	-8(%ebp), %ebx
	decl	%ebx
	movl	%ebx, -8(%ebp)
	jmp	input

remainelse:
	
	## printf("dc: stack empty\n")
	pushl	$emptyErrorStr
	call	printf
	add	$4, %esp
	jmp	input

### '^'-operation-------------------------------------------------------

remainendif:

	## if (buffer[0] != '^')
	movl 	$0, %eax
	movb	buffer(,%eax,1), %cl
	cmpb	$EXPO, %cl
	jne 	expoendif

	## if (num < 2)
	movl	-8(%ebp), %ebx
	cmpl	$2, %ebx
	jl	expoelse

	movl	(%esp), %ecx
	pushl	%ecx
	movl	8(%esp), %ecx
	pushl	%ecx
	call	power
	add	$8, %esp
	movl	%eax, -4(%ebp)
	addl	$4, %esp
	movl	%eax, (%esp)
	movl	-8(%ebp), %ebx
	decl	%ebx
	movl	%ebx, -8(%ebp)
	jmp	input

expoelse:
	
	## printf("dc: stack empty\n")
	pushl	$emptyErrorStr
	call	printf
	add	$4, %esp
	jmp	input

### --------------------------------------------------------------------
	## -------------------------------------------------------------
	## Check the number buffer and stack typed integer
	## -------------------------------------------------------------

expoendif:

	## if (buffer[0] != '_')
	movl 	$0, %eax
	movb	buffer(,%eax,1), %cl
	cmpb	$MINUSSIGN, %cl
	jne 	checkdigit1
	jmp	checkdigit2

checkdigit2:

	## if (!isdigit(buffer[1]))
	movl 	$1, %eax
	movb	buffer(,%eax,1), %cl
	pushl	%ecx
	call isdigit
	add	$4, %esp
	cmpl	$0, %eax
	je	input
	jmp	stack

checkdigit1:

	## if (!isdigit(buffer[0]))
	pushl	%ecx
	call isdigit
	add	$4, %esp
	cmpl	$0, %eax
	je	input
	jmp	stack

stack:

	## if (buffer[0] != '_')
	movl 	$0, %eax
	movb	buffer(,%eax,1), %cl
	cmpb	$MINUSSIGN, %cl
	jne	positive
	jmp	negative

positive:
	
	pushl	$buffer
	call 	atoi
	add	$4, %esp
	pushl	%eax
	movl	-8(%ebp), %ebx
	incl	%ebx
	movl	%ebx, -8(%ebp)
	jmp input

negative:

	movl	$1, %eax
	movl	$buffer, %edx
	addl	%eax, %edx 
	pushl	%edx
	call 	atoi
	add	$4, %esp
	movl	$-1, %ebx
	imull	%ebx
	pushl	%eax
	movl	-8(%ebp), %ebx
	incl	%ebx
	movl	%ebx, -8(%ebp)
	jmp 	input

###--------------------------------------------------------------------	
	## -------------------------------------------------------------
	## int power(int base, int exponent)
	## Perform exponentiation.  Returns base^exponent.
	## -------------------------------------------------------------

power: 

	pushl	%ebp
	movl	%esp, %ebp
	subl	$4, %esp
	movl	$1, %eax
	movl	%eax, (%esp)
	movl	8(%ebp), %ebx
	movl	12(%ebp), %ecx
	jmp	retrn

multiply:
	# for (i=0;i<exponent;i++) result *= base
	imull	%ebx
	movl	%eax, (%esp)
	decl	%ecx
	jmp	retrn

retrn:
	cmpl	$0, %ecx
	jne	multiply
	movl	%ebp, %esp
	popl	%ebp
	ret
	
###--------------------------------------------------------------------	
	## -------------------------------------------------------------
	## main function return
	## -------------------------------------------------------------

quit:	
	## return 0
	movl    $0, %eax
	movl    %ebp, %esp
	popl    %ebp
	ret




