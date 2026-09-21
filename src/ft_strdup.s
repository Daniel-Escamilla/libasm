global ft_strdup

extern ft_strlen
extern ft_strcpy
extern malloc


ft_strdup:
	push rdi
	call ft_strlen
	
	mov rdi, rax
	add rdi, 1
	call malloc

	cmp rax, 0
	jz _error
	
	mov rdx, rax
	pop rsi
	call ft_strcpy
	ret

_error:
	pop rdi
	ret


section .note.GNU-stack noalloc noexec nowrite
