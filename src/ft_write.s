global ft_write

extern __errno_location

section .text
ft_write:
	mov eax, 1
	syscall
	cmp eax, 0
	js _error
	ret

_error:
	mov ebx, eax
	neg ebx
	call __errno_location wrt ..plt
	mov [rax], ebx
	mov rax, -1
	ret

section .note.GNU-stack noalloc noexec nowrite