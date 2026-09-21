global ft_read

extern __errno_location

section .text
ft_read:
	mov eax, 0
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