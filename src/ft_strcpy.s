global ft_strcpy

ft_strcpy:
	mov rax, rdi
	jmp _loop

_end:
	ret

_loop:
	mov cl, [rsi]
	mov [rdi], cl
	cmp byte [rsi], 0
	je _end
	add rdi, 1
	add rsi, 1
	jmp _loop

section .note.GNU-stack noalloc noexec nowrite
