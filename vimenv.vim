func! BasdonPluginMake(parm)
	update all
	vertical botright new
	setlocal buftype=nofile bufhidden=wipe nobuflisted noswapfile wrap
	execute "read !. " a:parm
	nnoremap <buffer> <CR> :q<CR>

	" go to error line, if any
	silent! /error
	silent! normal n
endfunc

nnoremap ,d :call BasdonPluginMake('devmake.sh')<CR>
nnoremap ,r :call BasdonPluginMake('nixmake.sh')<CR>
