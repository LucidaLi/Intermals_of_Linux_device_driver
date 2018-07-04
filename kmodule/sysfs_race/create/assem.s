	.file	"assem.c"
	.text
	.p2align 4,,15
	.globl	npfs_remove_dir
	.type	npfs_remove_dir, @function
npfs_remove_dir:
.LFB34:
	.cfi_startproc
	movq	$0, 8(%rdi)
	ret
	.cfi_endproc
.LFE34:
	.size	npfs_remove_dir, .-npfs_remove_dir
	.ident	"GCC: (Ubuntu/Linaro 4.7.2-2ubuntu1) 4.7.2"
	.section	.note.GNU-stack,"",@progbits
