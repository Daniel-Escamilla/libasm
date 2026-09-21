global ft_strcmp

section .text
ft_strcmp:
_loop:
	movzx eax, byte [rdi]
	movzx ecx, byte [rsi]
	sub eax, ecx
	jne _end
	cmp byte [rsi], 0
	je _end
	cmp byte [rdi], 0
	je _end
	add rdi, 1
	add rsi, 1
	jmp _loop
_end:
	ret

section .note.GNU-stack noaxloc noexec nowrite
