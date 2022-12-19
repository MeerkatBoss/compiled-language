%def .x1.var_0x0 0
		push rsp
		inc
		pop rsp
		push 0
		pop [.x1.var_0x0]
%def .x2.var_0x1 1
		push rsp
		inc
		pop rsp
		push 0
		pop [.x2.var_0x1]
		call main	; Program entry point
		pop rdx		; Ignore return status
		halt		; Stop program execution


solv_leenear:
		push rbp
		pop [rsp]
		push rsp
		inc
		dup
		pop rsp
		pop rbp
%def .k.var_0x2 0
		push rsp
		inc
		pop rsp
%def .b.var_0x3 1
		push rsp
		inc
		pop rsp
		pop [rbp+.b.var_0x3]
		pop [rbp+.k.var_0x2]
; Start of .if_solv_leenear_0x0
		push [rbp+.k.var_0x2]
		push 0
		call __op.eq
		push 0
		je .if_solv_leenear_0x0_false

; Start of .if_solv_leenear_0x1
		push [rbp+.b.var_0x3]
		push 0
		call __op.eq
		push 0
		je .if_solv_leenear_0x1_false

		push 3000
		jmp .solv_leenear.end
		jmp .if_solv_leenear_0x1_end

.if_solv_leenear_0x1_false:
		push 0
		jmp .solv_leenear.end
.if_solv_leenear_0x1_end:

.if_solv_leenear_0x0_false:
.if_solv_leenear_0x0_end:

		push [rbp+.b.var_0x3]
		push -1
		mul
		push 1000
		mul
		push [rbp+.k.var_0x2]
		div
		jmp .solv_leenear.end
.solv_leenear.end:
		push rbp
		dec
		dup
		pop rbp
		pop rsp
		push [rbp]
		pop rbp
		ret

solv_qwadratik:
		push rbp
		pop [rsp]
		push rsp
		inc
		dup
		pop rsp
		pop rbp
%def .a.var_0x4 0
		push rsp
		inc
		pop rsp
%def .b.var_0x5 1
		push rsp
		inc
		pop rsp
%def .c.var_0x6 2
		push rsp
		inc
		pop rsp
		pop [rbp+.c.var_0x6]
		pop [rbp+.b.var_0x5]
		pop [rbp+.a.var_0x4]
; Start of .if_solv_qwadratik_0x2
		push [rbp+.a.var_0x4]
		push 0
		call __op.eq
		push 0
		je .if_solv_qwadratik_0x2_false

		push [rbp+.b.var_0x5]
		push [rbp+.c.var_0x6]
		call solv_leenear
		jmp .solv_qwadratik.end
.if_solv_qwadratik_0x2_false:
.if_solv_qwadratik_0x2_end:

%def .discrmant.var_0x7 3
		push rsp
		inc
		pop rsp
		push [rbp+.b.var_0x5]
		push [rbp+.b.var_0x5]
		mul
		push 1000
		div
		push 4000
		push [rbp+.a.var_0x4]
		mul
		push 1000
		div
		push [rbp+.c.var_0x6]
		mul
		push 1000
		div
		sub
		pop [rbp+.discrmant.var_0x7]
; Start of .if_solv_qwadratik_0x3
		push [rbp+.discrmant.var_0x7]
		push 0
		call __op.eq
		push 0
		je .if_solv_qwadratik_0x3_false

		push [rbp+.b.var_0x5]
		push -1
		mul
		push 1000
		mul
		push 2000
		push [rbp+.a.var_0x4]
		mul
		push 1000
		div
		div
		pop [.x1.var_0x0]
		push 1000
		jmp .solv_qwadratik.end
.if_solv_qwadratik_0x3_false:
.if_solv_qwadratik_0x3_end:

; Start of .if_solv_qwadratik_0x4
		push [rbp+.discrmant.var_0x7]
		push 0
		call __op.lt
		push 0
		je .if_solv_qwadratik_0x4_false

		push 0
		jmp .solv_qwadratik.end
.if_solv_qwadratik_0x4_false:
.if_solv_qwadratik_0x4_end:

		push [rbp+.discrmant.var_0x7]
		call sqrt
		pop [rbp+.discrmant.var_0x7]
		push [rbp+.b.var_0x5]
		push -1
		mul
		push [rbp+.discrmant.var_0x7]
		sub
		push 1000
		mul
		push 2000
		push [rbp+.a.var_0x4]
		mul
		push 1000
		div
		div
		pop [.x1.var_0x0]
		push [rbp+.b.var_0x5]
		push -1
		mul
		push [rbp+.discrmant.var_0x7]
		add
		push 1000
		mul
		push 2000
		push [rbp+.a.var_0x4]
		mul
		push 1000
		div
		div
		pop [.x2.var_0x1]
		push 2000
		jmp .solv_qwadratik.end
.solv_qwadratik.end:
		push rbp
		dec
		dup
		pop rbp
		pop rsp
		push [rbp]
		pop rbp
		ret

preent_rots:
		push rbp
		pop [rsp]
		push rsp
		inc
		dup
		pop rsp
		pop rbp
%def .cnt.var_0x8 0
		push rsp
		inc
		pop rsp
		pop [rbp+.cnt.var_0x8]
; Start of .if_preent_rots_0x5
		push [rbp+.cnt.var_0x8]
		push 0
		call __op.eq
		push 0
		je .if_preent_rots_0x5_false

		push 0
		call print
		jmp .if_preent_rots_0x5_end

.if_preent_rots_0x5_false:
; Start of .if_preent_rots_0x6
		push [rbp+.cnt.var_0x8]
		push 1000
		call __op.eq
		push 0
		je .if_preent_rots_0x6_false

		push 1
		call print
		pop rdx		; Discard function call result.
		push [.x1.var_0x0]
		call print
		pop rdx		; Discard function call result.
		jmp .if_preent_rots_0x6_end

.if_preent_rots_0x6_false:
; Start of .if_preent_rots_0x7
		push [rbp+.cnt.var_0x8]
		push 2000
		call __op.eq
		push 0
		je .if_preent_rots_0x7_false

		push 2
		call print
		pop rdx		; Discard function call result.
		push [.x1.var_0x0]
		call print
		pop rdx		; Discard function call result.
		push [.x2.var_0x1]
		call print
		pop rdx		; Discard function call result.
		jmp .if_preent_rots_0x7_end

.if_preent_rots_0x7_false:
		push 999999
		call print
		call print
.if_preent_rots_0x7_end:

.if_preent_rots_0x6_end:

.if_preent_rots_0x5_end:

		push [rbp+.cnt.var_0x8]
		jmp .preent_rots.end
.preent_rots.end:
		push rbp
		dec
		dup
		pop rbp
		pop rsp
		push [rbp]
		pop rbp
		ret

main:
		push rbp
		pop [rsp]
		push rsp
		inc
		dup
		pop rsp
		pop rbp
		call read
		push 1000000
		mul
		push 1000
		div
		call read
		push 1000000
		mul
		push 1000
		div
		call read
		push 1000000
		mul
		push 1000
		div
		call solv_qwadratik
		call preent_rots
		pop rdx		; Discard function call result.
		push 0
		jmp .main.end
.main.end:
		push rbp
		dec
		dup
		pop rbp
		pop rsp
		push [rbp]
		pop rbp
		ret

; __cmp_op.asm

__op.lt:	jl __op.lt.true
		push 0
		ret
__op.lt.true:	push 1000
		ret

__op.gt:	jg __op.gt.true
		push 0
		ret
__op.gt.true:	push 1000
		ret

__op.leq:	jle __op.leq.true
		push 0
		ret
__op.leq.true:	push 1000
		ret

__op.geq:	jge __op.geq.true
		push 0
		ret
__op.geq.true:	push 1000
		ret

__op.eq:	je __op.eq.true
		push 0
		ret
__op.eq.true:	push 1000
		ret

__op.neq:	jne __op.neq.true
		push 0
		ret
__op.neq.true:	push 1000
		ret
		
; __logic_op.asm

__op.and:	mul
		push 0
		jne __op.and.true
		push 0
		ret
__op.and.true:	push 1000
		ret

__op.or:	push 0
		je __op.or.second
		pop rdx
		push 1000
		ret
__op.or.second:	push 0
		je __op.or.false
		push 1000
		ret
__op.or.false:	push 0
		ret

__op.not:	push 0
		je __op.not.true
		push 0
		ret
__op.not.true:	push 1000
		ret
		
; stdlib.asm


; print(x)
print:		dup
		out
		ret

; read()
read:		get
		ret

; abs(x)
abs:		dup
		push 0
		jle abs.neg
		ret
abs.neg:	push -1
		mul
		ret


%def vbuf.y0 20
%def vbuf.x0 50
%def vbuf.height 41
%def vbuf.width 101
%def vbuf.start 61395

; set_pixel(x, y, ch)
set_pixel:	push rax
		pop [rsp]		; store rax in [rsp]
		push rbx
		pop [rsp+1]		; store rbx in [rsp+1]
		push rdx
		pop [rsp+2]		; store rdx in [rsp+2]

		push 1000
		div
		pop rdx			; rdx = ch / 1000

		push -1
		mul
		push vbuf.height
		mul
		push 2000
		div
		push vbuf.y0
		add
		pop rbx			; rbx = (-y * h)/2000 + y0

		push vbuf.width
		mul
		push 2000
		div
		push vbuf.x0
		add
		pop rax			; rax = (x * w)/2000 + x0

		push vbuf.width
		push rbx
		mul
		push rax
		add
		pop rax			; rax = rbx*2 + rax

		push rdx
		pop [vbuf.start+rax]	; set pixel in memory

		push rdx

		push [rsp]
		pop rax			; restore rax from [rsp]
		push [rsp+1]
		pop rbx			; restore rbx from [rsp+1]
		push [rsp+2]
		pop rdx			; restore rdx from [rsp+2]

		ret

; flush()
flush:		push vbuf.start
		pop vbp
		push 0
		ret


%def sq.min 0
%def sq.max 100000000

; sqrt(x)
sqrt:		push rax		; save rax in [rsp]	
		pop [rsp]

		push 10
		mul
		pop rax			; rax = 10*x

		push rbx		; save register values
		push rcx
		push rdx

		push sq.min
		pop rbx			; rbx = sq.min

		push sq.max
		pop rdx			; rdx = sq.max

sq.loop.start:	push rdx
		push rbx
		sub
		push 1
		jle sq.loop.end		; if (rdx - rbx <= 1) break;

		push rbx
		push rdx
		add
		push 2
		div			; m = (rbx + rdx)/2

		pop rcx			; rcx = m

		push rcx		; m

		push rax
		push rcx
		div			; rax / m

		jg sq.greater

		push rcx		; if (m <= rax/m) rbx = m;
		pop rbx
		jmp sq.loop.start

sq.greater:	push rcx
		pop rdx
		jmp sq.loop.start	; else rdx = m;

sq.loop.end:	push rbx
		push 10
		mul
		pop rax			; rax = 10*rbx (normalized with fixed precision)

		pop rdx
		pop rcx
		pop rbx			; restore register values

		push rax		; return rax

		push [rsp]
		pop rax			; restore rax from [rsp]

		ret

