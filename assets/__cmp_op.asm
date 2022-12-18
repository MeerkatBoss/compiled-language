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
		
