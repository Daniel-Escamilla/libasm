global ft_strdup

extern ft_strlen
extern ft_strcpy
extern malloc

section .text
ft_strdup:
	push rdi
	call ft_strlen
	
	mov rdi, rax
	add rdi, 1
	call malloc wrt ..plt

	cmp rax, 0
	jz _error
	
	mov rdi, rax
	pop rsi
	call ft_strcpy
	ret

_error:
	pop rdi
	ret


section .note.GNU-stack noalloc noexec nowrite
