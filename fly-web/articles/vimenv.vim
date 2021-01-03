
command! -nargs=? Smake call ArticlesMake(<q-args>)
command! -nargs=? SMake call ArticlesMake(<q-args>)
func! ArticlesMake(parm)
	update
	botright new
	setlocal buftype=nofile bufhidden=wipe nobuflisted noswapfile wrap
	silent execute "read !winmake.bat" a:parm
	silent! %s/\r//g
	setlocal nomodifiable
	nnoremap <buffer> <CR> :q<CR>
endfunc
