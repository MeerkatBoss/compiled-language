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

		pop rax			; store parameter in rax

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
		push 1000
		mul
		push 32
		div
		pop rax			; rax = rbx * 1000 / 32 (normalized with fixed precision)

		pop rdx
		pop rcx
		pop rbx			; restore register values

		push rax		; return rax

		push [rsp]
		pop rax			; restore rax from [rsp]

		ret

