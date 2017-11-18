;
; install wrangler rule set
;

(deftemplate parent-window-data
   (slot handle               (type STRING))
   (slot process-id           (type STRING))
   (slot style                (type STRING))
   (slot disabled             (type SYMBOL))
   (slot visible              (type SYMBOL))
   (slot class-name           (type STRING))
   (slot type                 (type SYMBOL))
   (slot height               (type INTEGER))
   (slot width                (type INTEGER))
   (slot x-upper-left-corner  (type INTEGER))
   (slot y-upper-left-corner  (type INTEGER))
   (slot action               (type SYMBOL)))


(deftemplate child-window-data
   (slot handle               (type STRING))
   (slot parent-handle        (type STRING))
   (slot process-id           (type STRING))
   (slot style                (type STRING))
   (slot disabled             (type SYMBOL))
   (slot visible              (type SYMBOL))
   (slot class-name           (type STRING))
   (slot type                 (type SYMBOL))
   (slot height               (type INTEGER))
   (slot width                (type INTEGER))
   (slot x-upper-left-corner  (type INTEGER))
   (slot y-upper-left-corner  (type INTEGER))
   (slot action               (type SYMBOL)))


(deftemplate top-level-child-window
   (slot handle         (type STRING))
   (slot parent-handle  (type STRING)))


(deftemplate ended-install-process
   (slot id (type INTEGER)))


(deftemplate install-process-count
   (slot value (type INTEGER)))


(deftemplate install-process
   (slot id    (type INTEGER))
   (slot found (type SYMBOL) (default FALSE)))


(deftemplate parent-window-handle
   (slot value             (type STRING))
   (slot enumeration-id    (type STRING))
   (slot processed         (type SYMBOL) (default FALSE)))


(deftemplate child-window-handle
   (slot value                (type STRING))
   (slot parent-window-handle (type STRING))
   (slot enumeration-id       (type STRING))
   (slot processed            (type SYMBOL) (default FALSE)))


(deftemplate current-ui-focus
   (slot associated-handle (type STRING)))


(deftemplate button-to-click
   (slot handle             (type STRING))
   (slot install-process-id (type INTEGER)))


(deftemplate parent-window-enumeration-sequence
   (slot id                                           (type STRING))
   (slot starting-time                                (type INTEGER))
   (slot install-process-id                           (type INTEGER))
   (slot enumeration-count                            (type INTEGER) (default 0))
   (slot previous-enumeration-window-count            (type INTEGER) (default 0))
   (slot current-enumeration-window-count             (type INTEGER) (default 0))
   (slot homogenous-consecutive-enumeration-count     (type INTEGER) (default 0))
   (slot homogenous-consecutive-enumeration-threshold (type INTEGER) (default 0))
   (slot finished                                     (type SYMBOL)  (default FALSE)))


(deftemplate parent-window-enumeration
   (slot id                            (type STRING))
   (slot sequence-id                   (type STRING))
   (slot starting-time                 (type INTEGER))
   (slot install-process-id            (type INTEGER))
   (slot enumeration-index             (type INTEGER) (default 0))
   (slot previous-window-count         (type INTEGER) (default 0))
   (slot current-window-count          (type INTEGER) (default 0))
   (slot pulse-count                   (type INTEGER) (default 0))
   (slot one-off-count                 (type INTEGER) (default 0))
   (slot homogenous-series-count       (type INTEGER) (default 0))
   (slot homogenous-series-magnitude   (type INTEGER) (default 0))
   (slot homogenous-series-threshold   (type INTEGER) (default 0))
   (slot finished                      (type SYMBOL)  (default FALSE)))


(deftemplate child-window-enumeration-sequence
   (slot id                                           (type STRING))
   (slot parent-handle                                (type STRING))
   (slot starting-time                                (type INTEGER))
   (slot enumeration-count                            (type INTEGER) (default 0))
   (slot previous-enumeration-window-count            (type INTEGER) (default 0))
   (slot current-enumeration-window-count             (type INTEGER) (default 0))
   (slot homogenous-consecutive-enumeration-count     (type INTEGER) (default 0))
   (slot homogenous-consecutive-enumeration-threshold (type INTEGER) (default 0))
   (slot no-child-windows-enumeration-limit-base      (type INTEGER) (default 2))
   (slot finished                                     (type SYMBOL)  (default FALSE)))


(deftemplate child-window-enumeration
   (slot id                            (type STRING))
   (slot sequence-id                   (type STRING))
   (slot parent-handle                 (type STRING))
   (slot starting-time                 (type INTEGER))
   (slot enumeration-index             (type INTEGER) (default 0))
   (slot previous-window-count         (type INTEGER) (default 0))
   (slot current-window-count          (type INTEGER) (default 0))
   (slot pulse-count                   (type INTEGER) (default 0))
   (slot one-off-count                 (type INTEGER) (default 0))
   (slot homogenous-series-count       (type INTEGER) (default 0))
   (slot homogenous-series-magnitude   (type INTEGER) (default 0))
   (slot homogenous-series-threshold   (type INTEGER) (default 0))
   (slot finished                      (type SYMBOL)  (default FALSE)))


(deftemplate re-enumerate-parent-windows-for-continue
   (slot sequence-id          (type STRING))
   (slot install-process-id   (type INTEGER)))


(deftemplate re-enumerate-child-windows-for-continue
   (slot sequence-id    (type STRING))
   (slot parent-handle  (type STRING)))


(deftemplate child-window-purge
   (slot parent-handle  (type STRING)))


(defrule purge-parent-window-handle-without-data

                              (parent-window-purge)
   ?parent-window-handle   <- (parent-window-handle
                                    (value               ?value))
                              (not
                                 (parent-window-data
                                    (handle              ?value)))
   =>
   (retract ?parent-window-handle))


(defrule purge-parent-window-handle-with-data

                              (parent-window-purge)
   ?parent-window-handle   <- (parent-window-handle
                                 (value               ?value))
   ?parent-window-data     <- (parent-window-data
                                 (handle              ?value))
   =>
   (retract ?parent-window-handle)
   (retract ?parent-window-data))


(defrule stop-parent-window-purge

   ?parent-window-purge <- (parent-window-purge)
                           (not
                              (parent-window-handle))
   =>
   (retract ?parent-window-purge))


(defrule purge-child-window-handle-without-data

                           (child-window-purge
                              (parent-handle          ?parent-handle))

   ?child-window-handle <- (child-window-handle
                              (value                  ?value)
                              (parent-window-handle   ?parent-handle))

                           (not
                              (child-window-data
                                 (handle              ?value)
                                 (parent-handle       ?parent-handle)))
   =>
   (retract ?child-window-handle))


(defrule purge-child-window-handle-with-data

                           (child-window-purge
                              (parent-handle          ?parent-handle))

   ?child-window-handle <- (child-window-handle
                              (value                  ?value)
                              (parent-window-handle   ?parent-handle))

   ?child-window-data   <- (child-window-data
                              (handle                 ?value)
                              (parent-handle          ?parent-handle))
   =>
   (retract ?child-window-handle)
   (retract ?child-window-data))


(defrule stop-child-window-purge

   ?child-window-purge  <- (child-window-purge
                              (parent-handle             ?parent-handle))

                           (not
                              (child-window-handle
                                 (parent-window-handle   ?parent-handle)))
   =>
   (retract ?child-window-purge))


(defrule execute-button-click

   ?button-to-click <-  (button-to-click
                           (handle             ?handle)
                           (install-process-id ?install-process-id))
                        (not (parent-window-handle))
                        (not (child-window-handle))
   =>
   (retract ?button-to-click)
   (click-button ?handle)
   (bind ?parent-window-enumeration-sequence-id  (get-uuid))
   (bind ?parent-window-enumeration-id           (get-uuid))
   (bind ?starting-time (seconds-since-epoch))
   (assert
      (parent-window-enumeration-sequence
         (id                     ?parent-window-enumeration-sequence-id)
         (starting-time          ?starting-time)
         (install-process-id     ?install-process-id)))
   (assert
      (parent-window-enumeration
         (id                     ?parent-window-enumeration-id)
         (sequence-id            ?parent-window-enumeration-sequence-id)
         (starting-time          ?starting-time)
         (install-process-id     ?install-process-id)))
   (enumerate-parent-windows ?install-process-id ?parent-window-enumeration-id ?starting-time)
   (start-pulsar))


; !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
; NB:          since *all* access to the 
; knowledge base is serialized (yes, even 
; the hooked event notification and the window 
; enumeration handling from the operating system) the
; rules that are activated by the pulsar should
; not conflict with the rules that are activated
; by window enumeration with regard to the homogenous
; series, enumeration start, and enumeration finished
; rules. If access wasn't serialized, it would be
; necessary to add more fact slots for bookkeeping
; and/or move different rules into different modules
; !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
(defrule received-pulse-during-parent-window-enumeration

   ?pulse                     <- (pulse)
   ?parent-window-enumeration <- (parent-window-enumeration
                                    (finished               FALSE)
                                    (current-window-count   ?current-window-count)
                                    (pulse-count            ?pulse-count))
   =>
   (retract ?pulse)
   (modify  ?parent-window-enumeration
               (previous-window-count  ?current-window-count)
               (pulse-count            (+ 1 ?pulse-count)))
   (assert (update-parent-window-enumeration-homogenous-series)))


(defrule received-pulse-during-child-window-enumeration

   ?pulse                     <- (pulse)
   ?child-window-enumeration  <- (child-window-enumeration
                                    (finished               FALSE)
                                    (current-window-count   ?current-window-count)
                                    (pulse-count            ?pulse-count))
   =>
   (retract ?pulse)
   (modify  ?child-window-enumeration
               (previous-window-count  ?current-window-count)
               (pulse-count            (+ 1 ?pulse-count)))
   (assert (update-child-window-enumeration-homogenous-series)))


(defrule found-finished-parent-window-enumeration

   ?parent-window-enumeration-sequence <- (parent-window-enumeration-sequence
                                             (id                                       ?sequence-id)
                                             (enumeration-count                        ?enumeration-count)
                                             (previous-enumeration-window-count        ?previous-enumeration-window-count)
                                             (current-enumeration-window-count         ?current-enumeration-window-count)
                                             (homogenous-consecutive-enumeration-count ?homogenous-consecutive-enumeration-count)
                                             (starting-time                            ?starting-time))

   ?parent-window-enumeration          <- (parent-window-enumeration
                                             (sequence-id                        ?sequence-id)
                                             (finished                           TRUE)
                                             (current-window-count               ?current-window-count)
                                             (enumeration-index                  ?index-0))

                                          (not
                                             (parent-window-enumeration
                                                (sequence-id                     ?sequence-id)
                                                (finished                        TRUE)
                                                (current-window-count            ?current-window-count)
                                                (enumeration-index               ?index-1&:(< ?index-1 ?index-0))))
   =>
   (retract ?parent-window-enumeration)
   (bind    ?current-time (seconds-since-epoch))
   (bind    ?time-delta (- ?current-time ?starting-time))
   (modify  ?parent-window-enumeration-sequence
               (enumeration-count                              (+ 1 ?enumeration-count))
               (previous-enumeration-window-count              ?current-enumeration-window-count)
               (current-enumeration-window-count               ?current-window-count)
               (homogenous-consecutive-enumeration-count       (+ 1 ?homogenous-consecutive-enumeration-count))
               (homogenous-consecutive-enumeration-threshold   (+ 1 (ceiling
                                                                        (/ (log (+  ?current-enumeration-window-count
                                                                                    3))
                                                                           (* 2 (log (+   ?time-delta
                                                                                          3)))))))))


(defrule re-enumerate-parent-windows-for-enumeration-sequence

   (not (parent-window-purge))
   ?re-enumerate-parent-windows-for-continue <- (re-enumerate-parent-windows-for-continue
                                                   (sequence-id                           ?sequence-id)
                                                   (install-process-id                    ?install-process-id))
   =>
   (retract ?re-enumerate-parent-windows-for-continue)
   (bind                         ?parent-window-enumeration-id (get-uuid))
   (bind                         ?starting-time                (seconds-since-epoch))
   (assert
      (parent-window-enumeration
         (id                     ?parent-window-enumeration-id)
         (sequence-id            ?sequence-id)
         (starting-time          ?starting-time)
         (install-process-id     ?install-process-id)))
   (enumerate-parent-windows ?install-process-id ?parent-window-enumeration-id ?starting-time))


(defrule continue-parent-window-enumeration-sequence

   (parent-window-enumeration-sequence
      (id                                             ?id)
      (install-process-id                             ?install-process-id)
      (homogenous-consecutive-enumeration-count       ?homogenous-consecutive-enumeration-count)
      (homogenous-consecutive-enumeration-threshold   ?homogenous-consecutive-enumeration-threshold))

   (not
      (parent-window-enumeration
         (sequence-id                                 ?id)
         (finished                                    FALSE)))

   (test (or   (=    ?homogenous-consecutive-enumeration-threshold 0)
               (<=   ?homogenous-consecutive-enumeration-count
                     ?homogenous-consecutive-enumeration-threshold)))
   =>
   (assert (parent-window-purge))
   (assert (re-enumerate-parent-windows-for-continue
               (sequence-id         ?id)
               (install-process-id  ?install-process-id))))


(defrule homogenous-consecutive-enumeration-count-exceeds-threshold

   ?parent-window-enumeration-sequence <- (parent-window-enumeration-sequence
                                             (id                                             ?id)
                                             (finished                                       FALSE)
                                             (homogenous-consecutive-enumeration-count       ?homogenous-consecutive-enumeration-count)
                                             (homogenous-consecutive-enumeration-threshold   ?homogenous-consecutive-enumeration-threshold))

                                          (not
                                             (parent-window-enumeration
                                                (sequence-id                                 ?id)
                                                (finished                                    FALSE)))

                                          (test (> ?homogenous-consecutive-enumeration-count
                                                   ?homogenous-consecutive-enumeration-threshold))
   =>
   (modify  ?parent-window-enumeration-sequence
               (finished TRUE)))


(defrule zero-parent-window-enumeration-finished

   ?parent-window-enumeration          <- (parent-window-enumeration
                                             (id                     ?id)
                                             (sequence-id            ?sequence-id)
                                             (finished               FALSE)
                                             (current-window-count   0)
                                             (pulse-count            3)
                                             (install-process-id     ?install-process-id)
                                             (enumeration-index 		?enumeration-index))

   ?parent-window-enumeration-sequence <- (parent-window-enumeration-sequence
                                             (id                     ?sequence-id)
                                             (enumeration-count      ?enumeration-count))

                                          (not
                                             (parent-window-handle
                                                (enumeration-id      ?id)))
   =>
   (modify  ?parent-window-enumeration
               (finished TRUE))
   (modify  ?parent-window-enumeration-sequence
               (enumeration-count (+ 1 ?enumeration-count)))
   (bind                         ?parent-window-enumeration-id (get-uuid))
   (bind                         ?starting-time                (seconds-since-epoch))
   (assert
      (parent-window-enumeration
         (id                     ?parent-window-enumeration-id)
         (sequence-id            ?sequence-id)
         (starting-time          ?starting-time)
         (install-process-id     ?install-process-id)
         (enumeration-index      (+ 1 ?enumeration-index))))
   (enumerate-parent-windows ?install-process-id ?parent-window-enumeration-id ?starting-time))


(defrule non-zero-parent-window-enumeration-finished

   ?parent-window-enumeration <- (parent-window-enumeration
                                    (finished                     FALSE)
                                    (current-window-count         ~0)
                                    (homogenous-series-magnitude  ?homogenous-series-magnitude)
                                    (homogenous-series-threshold  ?homogenous-series-threshold&~0))
                                 (test (> ?homogenous-series-magnitude ?homogenous-series-threshold))
   =>
   (modify  ?parent-window-enumeration
               (finished TRUE)))


(defrule start-parent-window-enumeration-homogenous-series

   ?update-parent-window-enumeration-homogenous-series <- (update-parent-window-enumeration-homogenous-series)

   ?parent-window-enumeration             <- (parent-window-enumeration
                                                (finished                     FALSE)
                                                (previous-window-count        ?previous-window-count&~0)
                                                (current-window-count         ?previous-window-count)
                                                (homogenous-series-count      ?homogenous-series-count)
                                                (homogenous-series-magnitude  0))
   =>
   (retract ?update-parent-window-enumeration-homogenous-series)
   (modify  ?parent-window-enumeration
               (homogenous-series-count      (+ 1 ?homogenous-series-count))
               (homogenous-series-magnitude  1)))


(defrule continue-parent-window-enumeration-homogenous-series

   ?update-parent-window-enumeration-homogenous-series <- (update-parent-window-enumeration-homogenous-series)

   ?parent-window-enumeration             <- (parent-window-enumeration
                                                (finished                     FALSE)
                                                (previous-window-count        ?previous-window-count)
                                                (current-window-count         ?current-window-count&?previous-window-count)
                                                (one-off-count                ?one-off-count)
                                                (homogenous-series-count      ?homogenous-series-count)
                                                (homogenous-series-magnitude  ?homogenous-series-magnitude&~0))
   =>
   (retract ?update-parent-window-enumeration-homogenous-series)
   (modify  ?parent-window-enumeration
               (homogenous-series-magnitude (+ 1 ?homogenous-series-magnitude))
               (homogenous-series-threshold (ceiling
                                              (log
                                                (+ (expt ?one-off-count 2)
                                                   (* ?homogenous-series-count ?current-window-count)
                                                   3))))))


(defrule stop-parent-window-enumeration-homogenous-series

   ?update-parent-window-enumeration-homogenous-series <- (update-parent-window-enumeration-homogenous-series)

   ?parent-window-enumeration             <- (parent-window-enumeration
                                                (finished                     FALSE)

                                                ;probably filter 0 here since those are handled separately
                                                ;from a homogenous series
                                                (previous-window-count        ?previous-window-count)
                                                (current-window-count         ~?previous-window-count)
                                                (one-off-count                ?one-off-count)
                                                (homogenous-series-count      ?homogenous-series-count)
                                                (homogenous-series-magnitude  ?homogenous-series-magnitude))
   =>
   (retract ?update-parent-window-enumeration-homogenous-series)
   (modify  ?parent-window-enumeration
               (one-off-count (+ 1 ?one-off-count))
               (homogenous-series-magnitude 0)))


(defrule start-child-window-enumeration-sequence

   (parent-window-handle
      (value ?value))

   (parent-window-enumeration-sequence
      (finished TRUE))
   =>
   (bind                         ?child-window-enumeration-id           (get-uuid))
   (bind                         ?child-window-enumeration-sequence-id  (get-uuid))
   (bind                         ?starting-time                         (seconds-since-epoch))
   (assert
      (child-window-enumeration-sequence
         (id                     ?child-window-enumeration-sequence-id)
         (starting-time          ?starting-time)
         (parent-handle          ?value)))
   (assert
      (child-window-enumeration
         (id                     ?child-window-enumeration-id)
         (sequence-id            ?child-window-enumeration-sequence-id)
         (starting-time          ?starting-time)
         (parent-handle          ?value)))
   (enumerate-child-windows ?value ?child-window-enumeration-id ?starting-time))


(defrule found-non-consecutive-child-window-enumeration-series

   ?child-window-enumeration-sequence <-  (child-window-enumeration-sequence
                                             (id                                 ?sequence-id)
                                             (enumeration-count                  ?enumeration-count)
                                             (previous-enumeration-window-count  ?previous-enumeration-window-count)
                                             (current-enumeration-window-count   ?current-enumeration-window-count&~?previous-enumeration-window-count))

   ?child-window-enumeration          <-  (child-window-enumeration
                                             (sequence-id                        ?sequence-id)
                                             (finished                           TRUE)
                                             (current-window-count               ?current-window-count))
   =>
   (retract ?child-window-enumeration)
   (modify  ?child-window-enumeration-sequence
               (enumeration-count                        (+ 1 ?enumeration-count))
               (previous-enumeration-window-count        ?current-enumeration-window-count)
               (current-enumeration-window-count         ?current-window-count)
               (homogenous-consecutive-enumeration-count 0)))


(defrule found-consecutive-child-window-enumeration-series

   ?child-window-enumeration-sequence <-  (child-window-enumeration-sequence
                                             (id                                       ?sequence-id)
                                             (enumeration-count                        ?enumeration-count)
                                             (previous-enumeration-window-count        ?previous-enumeration-window-count)
                                             (current-enumeration-window-count         ?previous-enumeration-window-count&~0)
                                             (homogenous-consecutive-enumeration-count ?homogenous-consecutive-enumeration-count)
                                             (starting-time                            ?starting-time))

   ?child-window-enumeration          <-  (child-window-enumeration
                                             (sequence-id                              ?sequence-id)
                                             (finished                                 TRUE)
                                             (current-window-count                     ?current-window-count&~0))
   =>
   (retract ?child-window-enumeration)
   (bind    ?current-time (seconds-since-epoch))
   (bind    ?time-delta (- ?current-time ?starting-time))
   (modify  ?child-window-enumeration-sequence
               (enumeration-count                              (+ 1 ?enumeration-count))
               (previous-enumeration-window-count              ?previous-enumeration-window-count)
               (current-enumeration-window-count               ?current-window-count)
               (homogenous-consecutive-enumeration-count       (+ 1 ?homogenous-consecutive-enumeration-count))
               (homogenous-consecutive-enumeration-threshold   (ceiling
                                                                  (- (log ?time-delta)
                                                                     (log ?current-window-count))))))


(defrule found-finished-child-window-enumeration

   ?child-window-enumeration-sequence <- (child-window-enumeration-sequence
                                             (id                                       ?sequence-id)
                                             (enumeration-count                        ?enumeration-count)
                                             (previous-enumeration-window-count        ?previous-enumeration-window-count)
                                             (current-enumeration-window-count         ?current-enumeration-window-count)
                                             (homogenous-consecutive-enumeration-count ?homogenous-consecutive-enumeration-count)
                                             (starting-time                            ?starting-time))

   ?child-window-enumeration          <- (child-window-enumeration
                                             (sequence-id                        ?sequence-id)
                                             (finished                           TRUE)
                                             (current-window-count               ?current-window-count)
                                             (enumeration-index                  ?index-0))

                                          (not
                                             (child-window-enumeration
                                                (sequence-id                     ?sequence-id)
                                                (finished                        TRUE)
                                                (current-window-count            ?current-window-count)
                                                (enumeration-index               ?index-1&:(< ?index-1 ?index-0))))
   =>
   (retract ?child-window-enumeration)
   (bind    ?current-time (seconds-since-epoch))
   (bind    ?time-delta (- ?current-time ?starting-time))
   (modify  ?child-window-enumeration-sequence
               (enumeration-count                              (+ 1 ?enumeration-count))
               (previous-enumeration-window-count              ?current-enumeration-window-count)
               (current-enumeration-window-count               ?current-window-count)
               (homogenous-consecutive-enumeration-count       (+ 1 ?homogenous-consecutive-enumeration-count))
               (homogenous-consecutive-enumeration-threshold   (+ 1 (ceiling
                                                                        (/ (log (+  ?current-enumeration-window-count
                                                                                    3))
                                                                           (* 2 (log (+   ?time-delta
                                                                                          3)))))))))


(defrule re-enumerate-child-windows-for-enumeration-sequence

   (not (child-window-purge))
   ?re-enumerate-child-windows-for-continue  <- (re-enumerate-child-windows-for-continue
                                                   (sequence-id                           ?sequence-id)
                                                   (parent-handle                         ?parent-handle))
   =>
   (retract ?re-enumerate-child-windows-for-continue)
   (bind                         ?child-window-enumeration-id  (get-uuid))
   (bind                         ?starting-time                (seconds-since-epoch))
   (assert
      (child-window-enumeration
         (id                     ?child-window-enumeration-id)
         (sequence-id            ?sequence-id)
         (starting-time          ?starting-time)
         (parent-handle          ?parent-handle)))
   (enumerate-child-windows ?parent-handle ?child-window-enumeration-id ?starting-time))


(defrule continue-child-window-enumeration-sequence

   (child-window-enumeration-sequence
      (id                                             ?id)
      (parent-handle                                  ?parent-handle)
      (homogenous-consecutive-enumeration-count       ?homogenous-consecutive-enumeration-count)
      (homogenous-consecutive-enumeration-threshold   ?homogenous-consecutive-enumeration-threshold))

   (not
      (child-window-enumeration
         (sequence-id                                 ?id)
         (finished                                    FALSE)))

   (test (or   (=    ?homogenous-consecutive-enumeration-threshold 0)
               (<=   ?homogenous-consecutive-enumeration-count
                     ?homogenous-consecutive-enumeration-threshold)))
   =>
   (assert (child-window-purge
               (parent-handle ?parent-handle)))
   (assert (re-enumerate-child-windows-for-continue
               (sequence-id   ?id)
               (parent-handle ?parent-handle))))


(defrule finish-child-window-enumeration-sequence

   ?child-window-enumeration-sequence <-  (child-window-enumeration-sequence
                                             (id                                             ?id)
                                             (homogenous-consecutive-enumeration-count       ?homogenous-consecutive-enumeration-count)
                                             (homogenous-consecutive-enumeration-threshold   ?homogenous-consecutive-enumeration-threshold)
                                             (finished                                       FALSE))

                                          (not
                                             (child-window-enumeration
                                                (sequence-id                                 ?id)
                                                (finished                                    FALSE)))

                                          (test (> ?homogenous-consecutive-enumeration-count
                                                   ?homogenous-consecutive-enumeration-threshold))
   =>
   (modify  ?child-window-enumeration-sequence
               (finished TRUE)))


(defrule finish-no-child-windows-enumeration-sequence

   ?child-window-enumeration-sequence <-  (child-window-enumeration-sequence
                                             (id                                       ?id)
                                             (starting-time                            ?starting-time)
                                             (enumeration-count                        ?enumeration-count)
                                             (previous-enumeration-window-count        0)
                                             (current-enumeration-window-count         0)
                                             (no-child-windows-enumeration-limit-base  ?no-child-windows-enumeration-limit-base)
                                             (finished                                 FALSE))

   ?child-window-enumeration           <- (child-window-enumeration
                                             (previous-window-count                    0)
                                             (current-window-count                     0)
                                             (sequence-id                              ?id)
                                             (finished                                 FALSE))

                                          (test (> (- ?enumeration-count
                                                      (log (+  (- (seconds-since-epoch) ?starting-time)
                                                               1)))
                                                   ?no-child-windows-enumeration-limit-base))
   =>
   (modify  ?child-window-enumeration
               (finished TRUE)))


(defrule zero-child-window-enumeration-finished

   ?child-window-enumeration          <-  (child-window-enumeration
                                             (id                     ?id)
                                             (sequence-id            ?sequence-id)
                                             (finished               FALSE)
                                             (current-window-count   0)
                                             (pulse-count            3)
                                             (enumeration-index 		?enumeration-index))

   ?child-window-enumeration-sequence <-  (child-window-enumeration-sequence
                                             (id                     ?sequence-id)
                                             (parent-handle          ?parent-handle)
                                             (enumeration-count      ?enumeration-count))
                                          (not
                                             (child-window-handle
                                                (enumeration-id      ?id)))
   =>
   (modify  ?child-window-enumeration
               (finished TRUE))
   (modify  ?child-window-enumeration-sequence
               (enumeration-count (+ 1 ?enumeration-count)))
   (bind                         ?child-window-enumeration-id  (get-uuid))
   (bind                         ?starting-time                (seconds-since-epoch))
   (assert
      (child-window-enumeration
         (id                     ?child-window-enumeration-id)
         (sequence-id            ?sequence-id)
         (starting-time          ?starting-time)
         (parent-handle          ?parent-handle)
         (enumeration-index      (+ 1 ?enumeration-index))))
   (enumerate-child-windows ?parent-handle ?child-window-enumeration-id ?starting-time))


(defrule non-zero-child-window-enumeration-finished

   ?child-window-enumeration <-  (child-window-enumeration
                                    (finished                     FALSE)
                                    (current-window-count         ~0)
                                    (homogenous-series-magnitude  ?homogenous-series-magnitude)
                                    (homogenous-series-threshold  ?homogenous-series-threshold&~0))
                                 (test (> ?homogenous-series-magnitude ?homogenous-series-threshold))
   =>
   (modify  ?child-window-enumeration
               (finished TRUE)))


(defrule start-child-window-enumeration-homogenous-series

   ?update-child-window-enumeration-homogenous-series <- (update-child-window-enumeration-homogenous-series)

   ?child-window-enumeration             <-  (child-window-enumeration
                                                (finished                     FALSE)
                                                (previous-window-count        ?previous-window-count&~0)
                                                (current-window-count         ?previous-window-count)
                                                (homogenous-series-count      ?homogenous-series-count)
                                                (homogenous-series-magnitude  0))
   =>
   (retract ?update-child-window-enumeration-homogenous-series)
   (modify  ?child-window-enumeration
               (homogenous-series-count      (+ 1 ?homogenous-series-count))
               (homogenous-series-magnitude  1)))


(defrule continue-child-window-enumeration-homogenous-series

   ?update-child-window-enumeration-homogenous-series <- (update-child-window-enumeration-homogenous-series)

   ?child-window-enumeration             <-  (child-window-enumeration
                                                (finished                     FALSE)
                                                (previous-window-count        ?previous-window-count)
                                                (current-window-count         ?current-window-count&?previous-window-count)
                                                (one-off-count                ?one-off-count)
                                                (homogenous-series-count      ?homogenous-series-count)
                                                (homogenous-series-magnitude  ?homogenous-series-magnitude&~0))
   =>
   (retract ?update-child-window-enumeration-homogenous-series)
   (modify  ?child-window-enumeration
               (homogenous-series-magnitude (+ 1 ?homogenous-series-magnitude))
               (homogenous-series-threshold (ceiling
                                              (log
                                                (+ (expt ?one-off-count 2)
                                                   (* ?homogenous-series-count ?current-window-count)
                                                   3))))))


(defrule stop-child-window-enumeration-homogenous-series

   ?update-child-window-enumeration-homogenous-series <- (update-child-window-enumeration-homogenous-series)

   ?child-window-enumeration             <- (child-window-enumeration
                                                (finished                     FALSE)
                                                (previous-window-count        ?previous-window-count)
                                                (current-window-count         ~?previous-window-count)
                                                (one-off-count                ?one-off-count)
                                                (homogenous-series-count      ?homogenous-series-count)
                                                (homogenous-series-magnitude  ?homogenous-series-magnitude))
   =>
   (retract ?update-child-window-enumeration-homogenous-series)
   (modify  ?child-window-enumeration
               (one-off-count (+ 1 ?one-off-count))
               (homogenous-series-magnitude 0)))


(defrule found-initial-install-process

   ?install-process <-  (install-process
                           (id ?id)
                           (found FALSE))
                        (not (install-process-count))
   =>
   (modify ?install-process (found TRUE))
   (assert (install-process-count (value 1)))
   (bind ?parent-window-enumeration-sequence-id  (get-uuid))
   (bind ?parent-window-enumeration-id          (get-uuid))
   (bind ?starting-time (seconds-since-epoch))
   (assert
      (parent-window-enumeration-sequence
         (id                     ?parent-window-enumeration-sequence-id)
         (starting-time          ?starting-time)
         (install-process-id     ?id)
         (enumeration-count      1)))
   (assert
      (parent-window-enumeration
         (id                     ?parent-window-enumeration-id)
         (sequence-id            ?parent-window-enumeration-sequence-id)
         (starting-time          ?starting-time)
         (install-process-id     ?id)))
   (enumerate-parent-windows ?id ?parent-window-enumeration-id ?starting-time)
   (start-pulsar))


(defrule found-additional-install-process

   ?install-process        <- (install-process
                                 (id    ?id)
                                 (found FALSE))

   ?install-process-count  <- (install-process-count
                                 (value ?value))
   =>
   (modify ?install-process       (found TRUE))
   (modify ?install-process-count (value (+ ?value 1)))
   (bind ?parent-window-enumeration-sequence-id (get-uuid))
   (bind ?parent-window-enumeration-id          (get-uuid))
   (bind ?starting-time (seconds-since-epoch))
   (assert
      (parent-window-enumeration-sequence
         (id                     ?parent-window-enumeration-sequence-id)
         (starting-time          ?starting-time)
         (install-process-id     ?id)))
   (assert
      (parent-window-enumeration
         (id                     ?parent-window-enumeration-id)
         (sequence-id            ?parent-window-enumeration-sequence-id)
         (starting-time          ?starting-time)
         (install-process-id     ?id)))
   (enumerate-parent-windows ?id ?parent-window-enumeration-id ?starting-time)
   (start-pulsar))


(defrule found-ended-install-process
   
   ?ended-install-process  <- (ended-install-process
                                 (id ?id))
   ?install-process-count  <- (install-process-count
                                 (value ?value))
   =>
   (retract ?ended-install-process)
   (modify  ?install-process-count (value (- ?value 1))))


(defrule install-finished

   (install-process-count (value 0))
   (not
      (install-process
         (found FALSE)))
   =>
   (printout t "installation finished" crlf crlf)
   (shutdown-wrangler))


(defrule remove-extraneous-parent-window-handle

   ?parent-window-handle   <- (parent-window-handle
                                 (value               ?value)
                                 (processed           FALSE))

                              (parent-window-handle
                                 (value               ?value)
                                 (processed           TRUE))
   =>
   (retract ?parent-window-handle))


(defrule remove-extraneous-child-window-handle

   ?child-window-handle <- (child-window-handle
                              (value               ?value)
                              (processed           FALSE))

                           (child-window-handle
                              (value               ?value)
                              (processed           TRUE))
   =>
   (retract ?child-window-handle))


(defrule found-unprocessed-parent-window-handle

   ?parent-window-handle      <- (parent-window-handle
                                    (value                  ?value)
                                    (enumeration-id         ?enumeration-id)
                                    (processed              FALSE))

                                 (not
                                    (parent-window-handle
                                       (value               ?value)
                                       (processed           TRUE)))

   ?parent-window-enumeration <- (parent-window-enumeration
                                    (id                     ?enumeration-id)
                                    (finished               FALSE)
                                    (current-window-count   ?current-window-count))
   =>
   (get-parent-window-data ?value)

   (modify  ?parent-window-handle
               (processed TRUE))

   (modify  ?parent-window-enumeration
               (current-window-count (+ 1 ?current-window-count))))


(defrule found-unprocessed-child-window-handle

   ?child-window-handle       <- (child-window-handle
                                    (value                  ?value)
                                    (parent-window-handle   ?parent-window-handle)
                                    (enumeration-id         ?enumeration-id))

                                 (not
                                    (child-window-handle
                                       (value               ?value)
                                       (processed           TRUE)))

   ?child-window-enumeration  <- (child-window-enumeration
                                    (id                     ?enumeration-id)
                                    (finished               FALSE)
                                    (current-window-count   ?current-window-count))
   =>
   (get-child-window-data ?value ?parent-window-handle)

   (modify  ?child-window-handle
               (processed TRUE))

   (modify  ?child-window-enumeration
               (current-window-count (+ 1 ?current-window-count))))


(defrule found-button-with-*finish*-action
(disable)
   (child-window-handle
      (value                  ?handle)
      (parent-window-handle   ?parent-window-handle))
   (child-window-data
      (handle           ?handle)
      (disabled         FALSE)
      (visible          TRUE)
      (process-id       ?process-id)
      (action           FINISH))
   (not
      (child-window-data
         (disabled      FALSE)
         (visible       TRUE)
         (action        NEXT)))
   =>
   (assert (parent-window-purge))
   (assert (child-window-purge
               (parent-handle ?parent-window-handle)))
   (assert
      (button-to-click
         (handle             ?handle)
         (install-process-id ?process-id))))


(defrule found-button-with-*next*-action

   (child-window-handle
      (value                  ?handle)
      (parent-window-handle   ?parent-window-handle))
   (child-window-data
      (handle           ?handle)
      (disabled         FALSE)
      (visible          TRUE)
      (action           NEXT)
      (process-id       ?process-id))
   (not
      (parent-window-enumeration-sequence
         (finished                        FALSE)))
   (not
      (child-window-enumeration-sequence
         (finished                        FALSE)))
   =>
   (assert (parent-window-purge))
   (assert (child-window-purge
               (parent-handle ?parent-window-handle)))
   (assert
      (button-to-click
         (handle             ?handle)
         (install-process-id ?process-id))))


;(defrule found-button-called-install
;
;   (child-window-handle
;      (value            ?handle))
;   (child-window-data
;      (handle           ?handle)
;      (disabled         FALSE)
;      (visible          TRUE)
;      (process-id       ?process-id))
;   (test (eq (get-window-text ?handle) "install"))
;   =>
;  (assert (parent-window-purge))
;   (assert (child-window-purge))
;   (assert
;      (button-to-click
;         (handle             ?handle)
;         (install-process-id ?process-id))))
;
;
;(defrule found-button-called-setup
;
;   (child-window-handle
;      (value            ?handle))
;   (child-window-data
;      (handle           ?handle)
;      (disabled         FALSE)
;      (visible          TRUE)
;      (process-id       ?process-id))
;   (test (eq (get-window-text ?handle) "setup"))
;   =>
;  (assert (parent-window-purge))
;   (assert (child-window-purge))
;   (assert
;      (button-to-click
;         (handle             ?handle)
;         (install-process-id ?process-id))))
;
;
;(defrule found-button-called-next
;
;   (child-window-handle
;      (value            ?handle))
;   (child-window-data
;      (handle           ?handle)
;      (disabled         FALSE)
;      (visible          TRUE)
;      (process-id       ?process-id))
;   (test (eq (get-window-text ?handle) "next"))
;   =>
;  (assert (parent-window-purge))
;   (assert (child-window-purge))
;   (assert
;      (button-to-click
;         (handle             ?handle)
;         (install-process-id ?process-id))))


(defrule found-dialog-ok-action
(disable)
   (child-window-handle
      (value                  ?handle)
      (parent-window-handle   ?parent-window-handle))
   (child-window-data
      (handle           ?handle)
      (disabled         FALSE)
      (visible          TRUE)
      (process-id       ?process-id)
      (parent-handle    ?parent-handle)
      (action           OK))
   (parent-window-data
      (handle           ?parent-handle)
      (class-name      "#32770"))
   =>
   (assert (parent-window-purge))
   (assert (child-window-purge
               (parent-handle ?parent-window-handle)))
   (assert
      (button-to-click
         (handle             ?handle)
         (install-process-id ?process-id))))


(defrule found-dialog-ok-action
(disable)
   (child-window-handle
      (value                  ?handle)
      (parent-window-handle   ?parent-window-handle))
   (child-window-data
      (handle           ?handle)
      (disabled         FALSE)
      (visible          TRUE)
      (process-id       ?process-id)
      (parent-handle    ?parent-handle)
      (action           YES))
   (parent-window-data
      (handle           ?parent-handle)
      (class-name      "#32770"))
   =>
   (assert (parent-window-purge))
   (assert (child-window-purge
               (parent-handle ?parent-window-handle)))
   (assert
      (button-to-click
         (handle             ?handle)
         (install-process-id ?process-id))))


(defrule found-isolated-close-button
(disable)
   (child-window-handle
      (value                  ?handle)
      (parent-window-handle   ?parent-window-handle))
   (child-window-data
      (handle           ?handle)
      (visible          TRUE)   
      (disabled         FALSE)
      (class-name       "button")
      (action           CLOSE)   
      (type             DEFPUSHBUTTON|PUSHBUTTON|OWNERDRAW)
      (process-id       ?process-id))
   (not
      (child-window-data
         (disabled         FALSE)
         (class-name       "button")
         (action           NEXT|OK)))
   =>
   (assert (parent-window-purge))
   (assert (child-window-purge
               (parent-handle ?parent-window-handle)))
   (assert
      (button-to-click
         (handle             ?handle)
         (install-process-id ?process-id))))


(defrule found-enabled-non-cancel-button

   (disable)

   (child-window-handle
      (value                  ?handle)
      (parent-window-handle   ?parent-window-handle))
   (child-window-data
      (handle           ?handle)
      (disabled         FALSE)
      (visible          TRUE)
      (class-name       "button")
      (action           ~CANCEL)   
      (type             DEFPUSHBUTTON|PUSHBUTTON|OWNERDRAW)
      (process-id       ?process-id))
   =>
   (assert (parent-window-purge))
   (assert (child-window-purge
               (parent-handle ?parent-window-handle)))
   (assert
      (button-to-click
         (handle             ?handle)
         (install-process-id ?process-id))))


(defrule found-disabled-non-cancel-button

   (disable)

   (child-window-handle
      (value                  ?handle)
      (parent-window-handle   ?parent-window-handle))
   (child-window-data
      (handle           ?handle)
      (disabled         TRUE)
      (class-name       "button")
      (type             DEFPUSHBUTTON|PUSHBUTTON|OWNERDRAW))
   =>
   (assert (parent-window-purge))
   (assert (child-window-purge
               (parent-handle ?parent-window-handle)))
   (void))


;(defrule found-pushbutton)
;(defrule found-radiobutton)
;(defrule found-checkbox)
;(defrule found-winzip-self-extractor)
;(defrule found-resume-exit-by-english-text)
;(defrule found-single-active-buttton-dialog)
;(defrule found-single-active-buttton-with-all-others-disabled)
;(defrule found-single-active-buttton-with-progress-control-dialog)
;(defrule found-single-buttton-messagebox)
;(defrule found-progress-control)
;(defrule found-candidate-eula-by-english-text)
;(defrule found-candidate-eula-by-layout)
;(defrule found-candidate-eula-by-large-text)
;(defrule found-candidate-yes-no-cancel-messagebox-by-layout)
;(defrule found-candidate-yes-no-messagebox-by-layout)
;(defrule found-ok-cancel-combobox-messagebox-by-layout)
;(defrule found-ok-cancel-combobox-messagebox-by-english-text)
;(defrule found-setup-cancel-messagebox-by-english-text)
;(defrule found-agree-cancel-messagebox-by-english-text)
;(defrule found-ok-cancel-messagebox-by-english-text)
;(defrule found-yes-no-messagebox-by-english-text)
;(defrule found-yes-no-cancel-messagebox-by-english-text)
;(defrule found-disabled-ok-button)
;(defrule found-disabled-next-button)
;(defrule accept-eula)
;(defrule find-target-button-by-position)

