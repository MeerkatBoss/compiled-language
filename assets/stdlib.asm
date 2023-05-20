section .text

print_num:	push		rbp
		mov		rbp,		rsp
		sub		rsp,		32

		lea		rdi,		[rbp - 2]	; End of buffer in rdi
		mov	BYTE	[rdi+1],	0x0A		; Terminate with '\n'

		mov		rax,		[rbp + 16]	; Converted number in rax
		mov		rsi,		10		; Divisor in rsi

		xor		rcx,		rcx		; Total char count in rcx
		inc		rcx
		
.fill_chars:	xor		rdx,		rdx
		idiv		rsi

		add		dl,		0x30		; '0'
		mov	BYTE	[rdi],		dl
		dec		rdi
		inc		rcx

		test		rax,		rax
		jnz		.fill_chars


		xor		rdi,		rdi
		inc		rdi				; rdi = 1 (stdout)
		mov		rsi,		rbp
		sub		rsi,		rcx		; buf addr in rsi
		mov		rdx,		rcx		; buf size in rdx
		
		xor		rax,		rax,
		inc		rax				; rax = 1 (write)

		syscall

		add		rsp,		32
		pop		rbp
.end:		ret

read_num:	push		rbp
		mov		rbp,		rsp
		sub		rsp,		32

		xor		rdi,		rdi		; rdi = 0 (stdin)
		mov		rsi,		rsp		; rsi = buf addr
		mov		rdx,		32		; rdx = buf size
		xor		rax,		rax		; read
		syscall

		dec		rax
		jz		.end

		mov		rcx,		rax
		xor		rax,		rax
		xor		rdx,		rdx
		mov		rsi,		rsp

.convert_num:	imul		rax,		rax,		10
		mov		dl,	BYTE	[rsi]
		sub		dl,		0x30
		add		rax,		rdx

		inc		rsi
		dec		rcx
		jnz		.convert_num

.end:		add		rsp,		32
		pop		rbp
		ret

sqrt:		push		rbp
		mov		rbp,		rsp

		mov		rax,		1000
		vpbroadcastq	xmm1,		rax
		vcvtqq2pd	xmm1,		xmm1

		movq		xmm0,	QWORD	[rbp + 16]
		vcvtqq2pd	xmm0,		xmm0

		vdivpd		xmm0,		xmm0,		xmm1
		sqrtsd		xmm0,		xmm0
		vmulpd		xmm0,		xmm0,		xmm1

		vcvtpd2qq	xmm0,		xmm0

		movq		rax,		xmm0

		pop		rbp
		ret

