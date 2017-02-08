( define (fibo n) (if (<= n 2) 1 (+ (fibo (- n 1) )) (fibo(- n 2))))\
(for-each (lambda (i) (executer (string-append "mkdir "\
  (number->string (fibo i))))) '(10 20 30))