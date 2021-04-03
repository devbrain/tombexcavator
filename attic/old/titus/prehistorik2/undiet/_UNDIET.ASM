ideal
model	tiny
codeseg
;	컴컴컴컴컴컴
macro	@lodsb
	local	@@u1
	lahf
	cmp	si,[bp-4]
	jb	@@u1
	call	_lods1
@@u1:	sahf
	lodsb
endm
;	컴컴컴컴컴컴
macro	@stosb
	local	@@u1
	stosb
	cmp	di,0fff0h
	jb	@@u1
	push	di
	shr	di,1
	shr	di,1
	shr	di,1
	shr	di,1
	mov	ax,es
	add	ax,di
	sub	ah,2
	mov	es,ax
	pop	di
	and	di,0fh
	add	di,2000h
@@u1:
endm
;;	컴컴컴컴컴컴
macro	@getbit
	local	u1
	shr	[word bp-2],1
	dec	dl
	jnz	u1
	@lodsb
	mov	dl,al
	@lodsb
	mov	ah,al
	mov	al,dl
	mov	[bp-2],ax
	mov	dl,10h
u1:
endm
;	컴컴컴컴컴컴
jumps
proc	_undiet
	push	bp
	mov	bp,sp
	push	bp bp ax bx cx dx si di
	cld
	mov	ds,[bp+14]
	mov	es,[bp+10]
	mov	di,[bp+8]
	mov	si,[bp+12]
	add	si,[bp+16]
	mov	[bp-4],si
	mov	dl,1
	@getbit
	xor	cx,cx
@@u11:	@getbit
	jnc	@@u12
	@lodsb
	@stosb
	jmp	@@u11
@@u12:	@getbit
	@lodsb
	mov	bh,0ffh
	mov	bl,al
	jc	@@u_1
	@getbit
	jc	@@u8
	cmp	bh,bl
	jnz	@@u13
	@getbit
	jc	@@u11
	jmp	@@up
@@u7:	@lodsb
	mov	cl,al
	add	cx,11h
	jmp	@@u10
@@u8:	mov	cl,3
@@u9:	@getbit
	rcl	bh,1
	loop	@@u9
	dec	bh
@@u13:	mov	cl,2
@@u10:	mov	al,[es:bx+di]
	@stosb
	loop	@@u10
	jmp	@@u11
@@u_1:	@getbit
	rcl	bh,1
	@getbit
	jc	@@u2
	mov	dh,2
	mov	cl,3
@@u0:	@getbit
	jc	@@u1
	@getbit
	rcl	bh,1
	shl	dh,1
	loop	@@u0
@@u1:	sub	bh,dh
@@u2:	mov	dh,2
	mov	cl,4
@@u3:	inc	dh
	@getbit
	jc	@@u4
	loop	@@u3
	@getbit
	jnc	@@u5
	inc	dh
	@getbit
	jnc	@@u4
	inc	dh
@@u4:	mov	cl,dh
	jmp	@@u10
@@u5:	@getbit
	jc	@@u7
	mov	cl,3
	mov	dh,0
@@u6:	@getbit
	rcl	dh,1
	loop	@@u6
	add	dh,9
	jmp	@@u4
@@up:	pop	bp bp di si dx cx bx ax bp
	retf
;	컴컴컴컴컴컴
_lods1:	push	ax bx cx dx
	mov	cx,[bp+16]
	mov	dx,[bp+12]
	mov	bx,[bp+6]
	mov	ah,3fh
	mov	si,dx
	int	21h
	pop	dx cx bx ax
	ret
endp
;컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴
public	_undiet
end