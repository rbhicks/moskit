(module clams racket/base

  (provide
   *smutex*
   *env*
   *action-labels*
   *env*
   *action-labels*
   *window-texts*
   *continue*
   clips-assert-format-string
   normalize-control-text
   add-log-entry
   add-file-creation-entry
   register-process-id
   extract-parent-window-enumeration-data-values
   extract-child-window-enumeration-data-values
   handle-file-creation-data
   handle-registry-data
   window-visible?
   window-disabled?
   get-button-type
   get-window-type
   process-non-edit-control-text
   process-edit-control-text
   determine-window-action
   get-common-window-data
   handle-parent-window-enum
   handle-child-window-enum
   handle-process-data
   handle-install-finished-data
   handle-exit-process-data
   handle-error-data
   handle-debug-data
   handle-message-data
   handle-console-output-data
   handle-socket-shutdown
   extract-window-handle
   get-parent-window-data
   get-child-window-data
   enumerate-parent-windows
   enumerate-child-windows
   get-window-text
   start-pulsar
   stop-pulsar
   click-button
   process-exists?
   process-exited-process
   process-exited-within-two-minutes?
   clips-show-all-facts
   get-uuid
   modulo)
  
  (require "wrangler.rkt")
  (require (planet williams/uuid:1:1/uuid))
  
  
  (define *env*           (clips-create-environment))
  (define *action-labels* (make-hash))
  (define *window-texts*  (make-hash))  
  (define *smutex*        (make-semaphore 1))
  (define *continue*      #t)

  
  (define-struct (destroyed-window-exception exn:fail:user) ())
  
  
  ; English
  (hash-set! *action-labels* "Back"      'BACK)
  (hash-set! *action-labels* "Next"      'NEXT)
  (hash-set! *action-labels* "Cancel"    'CANCEL)
  (hash-set! *action-labels* "Setup"     'NEXT)
  (hash-set! *action-labels* "Install"   'NEXT)
  (hash-set! *action-labels* "back"      'BACK)
  (hash-set! *action-labels* "next"      'NEXT)
  (hash-set! *action-labels* "cancel"    'CANCEL)
  (hash-set! *action-labels* "setup"     'NEXT)
  (hash-set! *action-labels* "install"   'NEXT)
  (hash-set! *action-labels* "OK"        'OK)
  (hash-set! *action-labels* "Ok"        'OK)
  (hash-set! *action-labels* "ok"        'OK)
  (hash-set! *action-labels* "Yes"       'YES)
  (hash-set! *action-labels* "yes"       'YES)
  (hash-set! *action-labels* "Close"     'CLOSE)
  (hash-set! *action-labels* "close"     'CLOSE)
  (hash-set! *action-labels* "Finish"    'FINISH)
  (hash-set! *action-labels* "finish"    'FINISH)
  (hash-set! *action-labels* "Exit"      'FINISH)
  (hash-set! *action-labels* "exit"      'FINISH)

  
  ; Norwegian
  (hash-set! *action-labels* "Tilbake"   'BACK)
  (hash-set! *action-labels* "Neste"     'NEXT)
  (hash-set! *action-labels* "Avbryt"    'CANCEL)
  
  ; Spanish
  (hash-set! *action-labels* "Anterior"  'BACK)
  (hash-set! *action-labels* "Siguiente" 'NEXT)
  (hash-set! *action-labels* "Cancelar"  'CANCEL)
  
  ; Portuguese
  (hash-set! *action-labels* "Avançar"   'BACK)
  (hash-set! *action-labels* "Voltar"    'NEXT)
  (hash-set! *action-labels* "Cancelar"  'CANCEL)
  
  ; Danish
  (hash-set! *action-labels* "Tilbage"   'BACK)
  (hash-set! *action-labels* "Næste"     'NEXT)
  (hash-set! *action-labels* "Annuller"  'CANCEL)
  
  ; Finnish
  (hash-set! *action-labels* "Edellinen" 'BACK)
  (hash-set! *action-labels* "Seuraava"  'NEXT)
  (hash-set! *action-labels* "Peruuta"   'CANCEL)
  
  ; German
  (hash-set! *action-labels* "Zurück"    'BACK)
  (hash-set! *action-labels* "Weiter"    'NEXT)
  (hash-set! *action-labels* "Abbrechen" 'CANCEL)
  
  ; French
  (hash-set! *action-labels* "Précédent" 'BACK)
  (hash-set! *action-labels* "Suivant"   'NEXT)
  (hash-set! *action-labels* "Annuler"   'CANCEL)
  
  ; Dutch
  (hash-set! *action-labels* "Vorige"    'BACK)
  (hash-set! *action-labels* "Volgende"  'NEXT)
  (hash-set! *action-labels* "Annuleren" 'CANCEL)
  
  ; Italian
  (hash-set! *action-labels* "Indietro"  'BACK)
  (hash-set! *action-labels* "Avanti"    'NEXT)
  (hash-set! *action-labels* "Annulla"   'CANCEL)
  
  ; Swedish
  (hash-set! *action-labels* "Bakåt"     'BACK)
  (hash-set! *action-labels* "Nästa"     'NEXT)
  (hash-set! *action-labels* "Avbryt"    'CANCEL)
  
  ; Czech
  (hash-set! *action-labels* "Zpět"      'BACK)
  (hash-set! *action-labels* "Další"     'NEXT)
  (hash-set! *action-labels* "Storno"    'CANCEL)
  
  ; Catalan
  (hash-set! *action-labels* "Darrere"   'BACK)
  (hash-set! *action-labels* "Següent"   'NEXT)
  (hash-set! *action-labels* "Cancel·la" 'CANCEL)
  
  ; Estonian
  (hash-set! *action-labels* "Tagasi"    'BACK)
  (hash-set! *action-labels* "Edasi"     'NEXT)
  (hash-set! *action-labels* "Loobu"     'CANCEL)
  
  ; Hungarian
  (hash-set! *action-labels* "Vissza"    'BACK)
  (hash-set! *action-labels* "Tovább"    'NEXT)
  (hash-set! *action-labels* "Mégse"     'CANCEL)
  
  ; Polish
  (hash-set! *action-labels* "Wstecz"    'BACK)
  (hash-set! *action-labels* "Dalej"     'NEXT)
  (hash-set! *action-labels* "Anuluj"    'CANCEL)
  
  ; Latvian
  (hash-set! *action-labels* "Atpakaļ"   'BACK)
  (hash-set! *action-labels* "Tālāk"     'NEXT)
  (hash-set! *action-labels* "Atcelt"    'CANCEL)
  
  ; Lithuanian
  (hash-set! *action-labels* "Atgal"     'BACK)
  (hash-set! *action-labels* "Kitas"     'NEXT)
  (hash-set! *action-labels* "Atšaukti"  'CANCEL)
  
  ; Romanian
  (hash-set! *action-labels* "Înapoi"    'BACK)
  (hash-set! *action-labels* "Următor"   'NEXT)
  (hash-set! *action-labels* "Anulare"   'CANCEL)
  
  ; Slovakian
  (hash-set! *action-labels* "Späť"      'BACK)
  (hash-set! *action-labels* "Ďalšie"    'NEXT)
  (hash-set! *action-labels* "Storno"    'CANCEL)
  
  ; Slovenian
  (hash-set! *action-labels* "Nazaj"     'BACK)
  (hash-set! *action-labels* "Naprej"    'NEXT)
  (hash-set! *action-labels* "Prekliči"  'CANCEL)
  
  
  (define (clips-assert-format-string clips-format-string . args)
    
    (clips-assert-string
     *env*
     (apply format clips-format-string args))
    
    (clips-run *env* -1))
  
  
  (define (normalize-control-text control-text)
    
    ; (order listed from innermost to outermost)
    ;
    ; strip "control" chars
    ; strip leading white space
    ; strip trailing white space
    (regexp-replace*
     #px"[[:blank:]]*$"
     (regexp-replace*
      #px"^[[:blank:]]*"
      (regexp-replace*
       #px"[<>&]*"
       control-text
       "")
      "")
     ""))  
  
  
  (define *log-output*      (open-output-file "wrangler-log.txt"    #:exists 'truncate))
  (define *installed-files* (open-output-file "installed-files.txt" #:exists 'truncate))
  
  
  (define (add-log-entry log-entry-type log-entry)
    
    (fprintf *log-output* "~a: ~a~n" log-entry-type log-entry))
  
  
  (define (add-file-creation-entry file-path)
    
    (fprintf *installed-files* "~a~n" file-path))
  
  
  (define (register-process-id process-id)
    
    (let ([clips-format-string "(install-process (id ~a))"])
      
      (clips-assert-format-string clips-format-string process-id)))
  
  
  (define (extract-parent-window-enumeration-data-values parent-window-enumeration-data)
    
    (let* ([data-values             (regexp-split "\0" parent-window-enumeration-data)]
           [window-handle           (car   data-values)]
           [enumeration-id          (cadr  data-values)]
           [starting-time           (caddr data-values)])
      
      (values
       (extract-window-handle window-handle)
       enumeration-id
       (string->number starting-time))))
  
  
  (define (extract-child-window-enumeration-data-values child-window-enumeration-data)
    
    (let* ([data-values            (regexp-split "\0" child-window-enumeration-data)]
           [window-handle          (car    data-values)]
           [parent-window-handle   (cadr   data-values)]
           [enumeration-id         (caddr  data-values)]
           [starting-time          (cadddr data-values)])
      
      (values
       (extract-window-handle window-handle)
       (extract-window-handle parent-window-handle)
       enumeration-id
       (string->number starting-time))))
  
  
  (define (window-visible? style)
    
    (if style
        (if (= 0 (bitwise-and style #x10000000))
            'FALSE
            'TRUE)
        #f))
  
  
  (define (window-disabled? style)
    
    (if style
        (if (= 0 (bitwise-and style #x08000000))
            'FALSE
            'TRUE)
        #f))
  
  
  (define (get-button-type style)
    
    (if style
        (let ([push-button       #x00000000]
              [def-push-button   #x00000001]
              [radio-button      #x00000004]
              [auto-radio-button #x00000009]
              [check-box         #x00000002]
              [auto-check-box    #x00000003]
              [owner-draw-button #x0000000b]
              [masked-style (bitwise-and style #xf)])
          
          (cond
            [(= push-button       masked-style) 'PUSHBUTTON]
            [(= def-push-button   masked-style) 'DEFPUSHBUTTON]
            [(= radio-button      masked-style) 'RADIOBUTTON]
            [(= auto-radio-button masked-style) 'RADIOBUTTON]
            [(= check-box         masked-style) 'CHECKBOX]
            [(= auto-check-box    masked-style) 'CHECKBOX]
            [(= owner-draw-button masked-style) 'OWNERDRAW]
            [else                               'NON-SPECIFIC]))
        #f))
  
  
  (define (get-window-type class-name style)
    
    (if (and class-name style)
        (cond
          [(string-ci=? class-name "BUTTON")   (get-button-type style)]
          [(string-ci=? class-name "COMBOBOX") 'COMBOBOX]
          [(string-ci=? class-name "EDIT")     'EDIT]
          [else                                'NON-SPECIFIC])
        #f))
  
  
  (define (process-non-edit-control-text handle)
    
    (when (> (win32-get-window-text-length handle) 0)
      (let ([text (win32-retrieve-window-text handle)])
        
        (when text
          (hash-set! *window-texts* handle (normalize-control-text text))))))
  
  
  (define (process-edit-control-text handle)
    
    (if (> (win32-get-window-text-length handle) 0)
        (let ([text (win32-retrieve-window-text handle)])
          
          (when text
            (hash-set! *window-texts* handle (normalize-control-text text))))
        (cond
          
          [(win32-check-for-large-ascii-window-text handle)
           (let ([text (win32-retrieve-large-ascii-window-text handle)])
          
             (when text
               (hash-set! *window-texts* handle text)))]
          
          [(win32-check-for-large-unicode-window-text handle)
           (let ([text (win32-retrieve-large-unicode-window-text handle)])
             (when text
               (hash-set! *window-texts* handle text)))])))
  
  
  (define (determine-window-action handle)
    
    (if (hash-has-key? *window-texts* handle)
      (let ([text (hash-ref *window-texts* handle)])
        
        (if (hash-has-key? *action-labels* text)
            (hash-ref *action-labels* text)
            'NON-SPECIFIC))
      #f))
  
  
  (define (get-common-window-data handle)
    
    (let*-values ([(process-id)          (win32-get-window-thread-process-id handle)]
                  [(style)               (win32-get-window-long-ptr          handle)]
                  [(disabled)            (window-disabled?                   style)]
                  [(visible)             (window-visible?                    style)]
                  [(class-name)          (win32-get-class-name-a             handle)]
                  [(type)                (get-window-type                    class-name style)]
                  [(height
                    width
                    x-upper-left-corner
                    y-upper-left-corner) (win32-get-window-size-and-location handle)]
                  [(action)              'NON-SPECIFIC])
      
      ; the text for edit controls is handled separately than for
      ;'clickable' controls (i.e., checkboxes, radio buttons, push buttons)
      (if (eq? type 'EDIT)
          (process-edit-control-text     handle)
          (process-non-edit-control-text handle))
      
      ; The 'action' (forward, back, cancel) of a button is determined
      ; based on its window text; so text processing has to be done first
      (when (or
             (eq? type 'PUSHBUTTON)
             (eq? type 'DEFPUSHBUTTON))
        (set! action (determine-window-action handle)))
      
      ; if any value is false then the window has been destroyed. throw an
      ; exception so the assingments in the caller can be aborted. this is
      ; defensive handling for a normal occurence (with the OS enumerating
      ; windows asynchronously and clips clicking buttons, sometimes the
      ; "set" of windows will get out of sync. this is expected.
      (if (and
            process-id
            style
            disabled
            visible
            class-name
            type
            height
            width
            x-upper-left-corner
            y-upper-left-corner
            action)
          
          (values
           process-id
           style
           disabled
           visible
           class-name
           type
           height
           width
           x-upper-left-corner
           y-upper-left-corner
           action)
          
          (raise
            (make-destroyed-window-exception
              "target window no longer exists"
              (current-continuation-marks))))))
  
  
  (define (extract-window-handle window-handle-string)
    
    (string->number window-handle-string))
  

  ; an external "timer interrupt" generator so clips can ask to have itself
  ; run in polling loop fashion. clips can start and stop the "pulsar" via
  ; extension functions.
  (define (clips-pulsar)

    ; suspend the thread immediately so the thread can be started at require
    ; time but only activated upon request
    (thread-suspend (current-thread))
    
    (namespace-require "clams.rkt")
    (parameterize ([current-namespace (module->namespace "clams.rkt")])
      (let pulse ()
        
        (semaphore-wait *smutex*)
        (clips-assert-format-string "(pulse)")
        (semaphore-post *smutex*)
        (sleep .3)
        (pulse))))
  
  ; thread variable for the pulsar (see above)
  (define *pulsar* (thread clips-pulsar))
  
  
  (define (process-exited-process process-id)
    
    (let ([assert-format-string "(ended-install-process (id ~a))"])
      
      (clips-assert-format-string assert-format-string process-id)))
  
  
  (define (process-exited-within-two-minutes? process-id)
    
    (wrangler-wait-for-process-two-minutes process-id))
  
  ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  ;; start:  socket data handler procs
  ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

  
  (define (handle-file-creation-data file-creation-data)
    
    (add-file-creation-entry file-creation-data))
  
  
  (define (handle-registry-data registry-data)
    
    ; not implemented
    (void))
  
  
  (define (handle-parent-window-enum parent-window-enumeration-data)
        
    (let*-values ([( handle
                     enumeration-id
                     starting-time)                (extract-parent-window-enumeration-data-values
                                                      parent-window-enumeration-data)]
                  [(top-level-child-window-handle) (win32-get-top-window handle)])

      (clips-assert-format-string
        "(parent-window-handle
           (value            \"~a\")
           (enumeration-id   \"~a\"))"
        handle
        enumeration-id)

      (clips-assert-format-string
        "(top-level-child-window
           (handle \"~a\")
           (parent-handle \"~a\"))"
        top-level-child-window-handle
        handle)))
  
  
  (define (handle-child-window-enum child-window-enumeration-data)

    (let*-values ([( handle
                     parent-handle
                     enumeration-id
                     starting-time)   (extract-child-window-enumeration-data-values
                                         child-window-enumeration-data)])
      
      (clips-assert-format-string
       "(child-window-handle
          (value                \"~a\")
          (parent-window-handle \"~a\")
          (enumeration-id       \"~a\"))"
       handle
       parent-handle
       enumeration-id)))
  
  
  (define (handle-process-data process-data)
    
    (register-process-id process-data))
  
  
  (define (handle-install-finished-data install-finished-data)
    
    (stop-pulsar))
  
  
  (define (handle-exit-process-data exit-process-data)
    
    (process-exited-process exit-process-data))
  
  
  (define (handle-error-data error-data)
    
    (display (format "error: ~a\n" error-data) (current-output-port)))
  
  
  (define (handle-debug-data debug-data)
    
    (void)
    ;(add-log-entry 'DEBUG debug-data)
    )
  
  (define (handle-socket-shutdown)
    
    (void)
    )
  
  (define (handle-message-data message-data)
    
    ;(add-log-entry 'MESSAGE message-data)
    
    (display message-data (current-output-port))
    
    )
  
  
  (define (handle-console-output-data console-output-data)
    
    (display console-output-data (current-output-port)))
  
  
  ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  ;; start:  procs called directly by clips
  ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
 
  
  (define (start-pulsar) (thread-resume  *pulsar*))
  (define (stop-pulsar)  (thread-suspend *pulsar*))
    
    
  (define (get-parent-window-data handle-string)
    
    (with-handlers ([destroyed-window-exception? (lambda (x) (void))])
      (let*-values ([(handle)             (extract-window-handle handle-string)]
                    [(process-id
                      style
                      disabled
                      visible
                      class-name
                      type
                      height
                      width
                      x-upper-left-corner
                      y-upper-left-corner
                      action)             (get-common-window-data handle)])
        (clips-assert-format-string
          "(parent-window-data
          (handle              \"~a\")
          (process-id          \"~a\")
          (style               \"~a\")
          (disabled            ~a)
          (visible             ~a)
          (class-name          \"~a\")
          (type                ~a)
          (height              ~a)
          (width               ~a)
          (x-upper-left-corner ~a)
          (y-upper-left-corner ~a)
          (action              ~a))"
          handle
          process-id
          style
          disabled
          visible
          (string-downcase class-name)
          type
          height
          width
          x-upper-left-corner
          y-upper-left-corner
          action))))
  
  
  (define (get-child-window-data handle-string parent-handle-string)
    
    (with-handlers ([destroyed-window-exception? (lambda (x) (void))])
      (let*-values ([(handle)             (extract-window-handle handle-string)]
                    [(parent-handle)      (extract-window-handle parent-handle-string)]
                    [(process-id
                      style
                      disabled
                      visible
                      class-name
                      type
                      height
                      width
                      x-upper-left-corner
                      y-upper-left-corner
                      action)             (get-common-window-data handle)])    
        (clips-assert-format-string
          "(child-window-data
          (handle              \"~a\")
          (parent-handle       \"~a\")
          (process-id          \"~a\")
          (style               \"~a\")
          (disabled            ~a)
          (visible             ~a)
          (class-name          \"~a\")
          (type                ~a)
          (height              ~a)
          (width               ~a)
          (x-upper-left-corner ~a)
          (y-upper-left-corner ~a)
          (action              ~a))"
          handle
          parent-handle
          process-id
          style
          disabled
          visible
          (string-downcase class-name)
          type
          height
          width
          x-upper-left-corner
          y-upper-left-corner
          action))))
  

  (define (click-button handle-string)
    
    (let ([handle (string->number handle-string)])
      
      ; post WM_LBUTTONDOWN and WM_LBUTTONUP
      ; messages since BN_CLICK can fail
      (win32-post-message handle #x201)
      (win32-post-message handle #x202)))
  
  
  (define (enumerate-parent-windows process-id enumeration-id starting-time)
    
    (win32-enumerate-parent-windows
      process-id
      enumeration-id
      starting-time))
  
  
  (define (enumerate-child-windows parent-handle enumeration-id starting-time)
    
    (win32-enumerate-child-windows
      (extract-window-handle parent-handle)
      enumeration-id
      starting-time))
  
  
  (define (get-window-text handle-string)
    
    (let ([handle (string->number handle-string)])
      
      (if (hash-has-key? *window-texts* handle)
          (string-downcase (hash-ref *window-texts* handle))
          "")))
  
  
  (define (remove-window-text handle-string)

    (let ([handle (string->number handle-string)])
      
      (hash-remove! *window-texts* handle)))
  
  
  (define (process-exists? process-id)
    
    (wrangler-process-exists? process-id))
  
  
  (define (shutdown-wrangler)
  
    (kill-thread *pulsar*)
    (set! *continue* #f))
  
  
  (define (get-uuid)
    
    (uuid->string (make-uuid-1)))
    
  
  (display (format "wiring up: get-parent-window-data-> ~a\n"   (clips-add-scheme-function *env* "get-parent-window-data"))   (current-output-port))
  (display (format "wiring up: get-child-window-data-> ~a\n"    (clips-add-scheme-function *env* "get-child-window-data"))    (current-output-port))
  (display (format "wiring up: enumerate-parent-windows-> ~a\n" (clips-add-scheme-function *env* "enumerate-parent-windows")) (current-output-port))
  (display (format "wiring up: enumerate-child-windows-> ~a\n"  (clips-add-scheme-function *env* "enumerate-child-windows"))  (current-output-port))
  (display (format "wiring up: get-window-text-> ~a\n"          (clips-add-scheme-function *env* "get-window-text"))          (current-output-port))
  (display (format "wiring up: remove-window-text-> ~a\n"       (clips-add-scheme-function *env* "remove-window-text"))       (current-output-port))  
  (display (format "wiring up: start-pulsar-> ~a\n"             (clips-add-scheme-function *env* "start-pulsar"))             (current-output-port))
  (display (format "wiring up: stop-pulsar-> ~a\n"              (clips-add-scheme-function *env* "stop-pulsar"))              (current-output-port))
  (display (format "wiring up: void-> ~a\n"                     (clips-add-scheme-function *env* "void"))                     (current-output-port))
  (display (format "wiring up: sleep-> ~a\n"                    (clips-add-scheme-function *env* "sleep"))                    (current-output-port))
  (display (format "wiring up: click-button-> ~a\n"             (clips-add-scheme-function *env* "click-button"))             (current-output-port))
  (display (format "wiring up: process-exists?-> ~a\n"          (clips-add-scheme-function *env* "process-exists?"))          (current-output-port))
  (display (format "wiring up: shutdown-wrangler-> ~a\n"        (clips-add-scheme-function *env* "shutdown-wrangler"))        (current-output-port))
  (display (format "wiring up: get-uuid-> ~a\n"                 (clips-add-scheme-function *env* "get-uuid"))                 (current-output-port))
  (display (format "wiring up: modulo-> ~a\n"                   (clips-add-scheme-function *env* "modulo"))                   (current-output-port))
  (display (format "wiring up: log-> ~a\n"                      (clips-add-scheme-function *env* "log"))                      (current-output-port))
  (display (format "wiring up: expt-> ~a\n"                     (clips-add-scheme-function *env* "expt"))                     (current-output-port))
  (display (format "wiring up: ceiling-> ~a\n"                  (clips-add-scheme-function *env* "ceiling"))                  (current-output-port))
  
  (define (clips-show-all-facts)
      
      (clips-facts *env* "wdisplay" (void) -1 -1 -1)))