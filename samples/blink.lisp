(defun blink (num millis)
  (progn
    (pinmode num t)
    (digitalwrite num t)
    (delay millis)
    (digitalwrite num nil)
    (delay millis)))

(defun blink-repeat (num)
  (loop
   (blink num 150)))

(defun repeat (fn millis)
  (loop
   (fn 150)))

(defun myblink ()
  (repeat (lambda (millis)
	    (blink 25 millis))
	  150))



(defun fact (n)
  (let ((fact-int (lambda (n carry)
		    (if (= n 1)
			carry
		      (fact-int
		       (- n 1)
		       (* n carry))))))
    (fact-int n 1)))

(defun long-list (n xs)
  (if (< n 0)
      xs
    (long-list (- n 1) (cons n xs))))

(loop
 (print (for-millis ()
  (long-list 2000 '()))))
