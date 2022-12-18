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
		
