; 代码中一些变量以 _ 开头, 认作是局部变量, 但实际是全局变量, 方便外部debug


(vl-load-com)

; 获取block的数量
(defun wzj_block_count ()
  (setq i 0)
  (setq tmp (tblnext "block" T))
  (while tmp
    (setq i (+ i 1))
    (setq tmp (tblnext "block"))
  )

  (print (strcat "The number of block is: " (itoa i)))
  (setq i nil)
  (setq tmp nil)
  (princ)
)



; 由界面上的INSERT, 获取所有块, 列出块名
(defun wzj_list_blocks (/ i x x_len block_name block_ref)
    (setq x (ssget "X" '((0 . "INSERT"))))
    (setq x_len (sslength x))
    (setq i 0)
    (repeat x_len
        (setq block_ref (ssname x i))
        (setq i (1+ i))
        
        (setq block_name (cdr (assoc 2 (entget block_ref))))
        (print block_name)
    )

    (princ)
)


; 返回块中对象的数量
(defun wzj_block_count (bname)
    (setq blocks (vla-get-Blocks (vla-get-ActiveDocument (vlax-get-acad-object))))

    (setq i 0)
    (setq count (vla-get-Count blocks))
    (while (< i count)
        (setq b (vla-Item blocks i))
        (setq i (1+ i))
        (if (= (vla-get-Name b) bname)
            (setq i (+ i count))
        )
    )

    (if (= i count) 
        (print (strcat "Not found blocks called: " bname)) 
        (progn
            (setq count (vla-get-Count b))
            (print (strcat "The count of entities in block is: " (itoa count)))
        )
    )

    (princ)
) 



; 炸开选中的块. 由于存在嵌套块, 所以会一直炸, 直到不能
(defun wzj_explode_block (bname / i b blocks count explode_ents next)
    (setq blocks (vla-get-Blocks (vla-get-ActiveDocument (vlax-get-acad-object))))

    (setq i 0)
    (setq count (vla-get-Count blocks))
    (while (< i count)
        (setq b (vla-Item blocks i))
        (setq i (1+ i))
        (if (= (vla-get-Name b) bname)
            (setq i (+ i count))
        )
    )
   
    (if (= i count) 
        (print (strcat "Not found blocks called: " bname)) 
        (progn
            (setq count (vla-get-Count b))
            (print (strcat "The count of entities in block is: " (itoa count)))

            (setq count_next 0)
            (while (/= count (length next))
                (setq next (list))
                ; 子对象炸开, 会直接影响块本身. 所以这边使用b就行
                (vlax-for i b
                    (if (vlax-method-applicable-p i 'Explode)
                        (setq explode_ents 
                            ; vla-Explode返回的是variant类型. 使用vlax-variant-value获取内置的safearray
                            (vlax-safearray->list (vlax-variant-value (vla-Explode i)))
                        )
                        ; else
                        (setq explode_ents (list i))
                    )
                   
                    (setq next (append next explode_ents))
                )

                (print (strcat "after explode, has sub entities: " (itoa (length next))))
            )
        )
    )
    (print done)
    (princ)
)


; 显示块中块, 仅一层
(defun wzj_sub_block_in_block (bname / blocks i count b)
    (setq blocks (vla-get-Blocks (vla-get-ActiveDocument (vlax-get-acad-object))))

    (setq i 0)
    (setq count (vla-get-Count blocks))
    (while (< i count)
        (setq b (vla-Item blocks i))
        (setq i (1+ i))
        (if (= (vla-get-Name b) bname)
            (setq i (+ i count))
        )
    )

    (if (= i count) 
        (print (strcat "Not found blocks called: " bname)) 
        (progn
            (setq count (vla-get-Count b))
            (print (strcat "The count of entities in block is: " (itoa count)))

            (vlax-for i b
                (if (= "AcDbBlockReference" (vla-get-ObjectName i))
                    (print (vla-get-Name i))
                )
            )
        )
    )

    (print "done")
    (print)
)


; 炸开所有块中的标注
(defun wzj_explode_dim_in_block ()
    (setq _blocks (vla-get-Blocks (vla-get-ActiveDocument (vlax-get-acad-object))))
    (vlax-for _b _blocks
        (setq _bname (vla-get-Name _b))
        (if (not (wcmatch _bname "`**"))  
            (progn
                (print (strcat "In block: " _bname))
                (command ".bedit" _bname)
                (setq _ents (ssget "_X" '((0 . "*DIM*"))))
                ; 画线, 然后删除. 确保bclose的时候一定需要参数
                (command ".line" "1,1,0" "0,0,0" "")
                (entdel (entlast))
                ; ssget没有选中对象, 则返回nil
                (if (not (null _ents))                  
                    (foreach _i (mapcar 'cadr (ssnamex _ents))
                        (command ".explode" _i)
                    )                  
                )
                (command "bclose" "S")
            )
        )
    )

    (print "done")
    (princ)
)