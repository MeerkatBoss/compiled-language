		call main	; Program entry point
		pop rdx		; Ignore return status
		halt		; Stop program execution


main:
		push rbp
		pop [rsp]
		push rsp
		inc
		dup
		pop rsp
		pop rbp
%def .x.var_0x0 0
		push rsp
		inc
		pop rsp
		push 1000
		pop [rbp+.x.var_0x0]
%def .y.var_0x1 1
		push rsp
		inc
		pop rsp
		push [rbp+.x.var_0x0]
		push 8000
		mul
		push 1000
		div
		pop [rbp+.y.var_0x1]
		push [rbp+.y.var_0x1]
		push 2000
		sub
		pop [rbp+.x.var_0x0]
		push [rbp+.x.var_0x0]
		push [rbp+.y.var_0x1]
		add
		call print
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

