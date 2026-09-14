global ft_strlen

section .text
ft_strlen:
	xor rdx, rdx
	jmp _loop

_end:
	mov rax, rdx
	ret
	
_loop:
	cmp byte [rdi], 0
	je _end
	inc rdi
	add rdx, 1
	jmp _loop

section .note.GNU-stack noalloc noexec nowrite
