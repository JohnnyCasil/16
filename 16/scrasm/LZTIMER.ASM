;
; *** Listing 2-5 ***
;
; The long-period Zen timer. (LZTIMER.ASM)
; Uses the 8253 timer and the BIOS time-of-day count to time the
; performance of code that takes less than an hour to execute.
; Because interrupts are left on (in order to allow the timer
; interrupt to be recognized), this is less accurate than the
; precision Zen timer, so it is best used only to time code that takes
; more than about 54 milliseconds to execute (code that the precision
; Zen timer reports overflow on). Resolution is limited by the
; occurrence of timer interrupts.
;
; By Michael Abrash 4/26/89
;
; Externally callable routines:
;
;  ZTimerOn: Saves the BIOS time of day count and starts the
;       long-period Zen timer.
;
;  ZTimerOff: Stops the long-period Zen timer and saves the timer
;       count and the BIOS time-of-day count.
;
;  ZTimerReport: Prints the time that passed between starting and
;       stopping the timer.
;
; Note: If either more than an hour passes or midnight falls between
;       calls to ZTimerOn and ZTimerOff, an error is reported. For
;       timing code that takes more than a few minutes to execute,
;       either the DOS TIME command in a batch file before and after
;       execution of the code to time or the use of the DOS
;       time-of-day function in place of the long-period Zen timer is
;       more than adequate.
;
; Note: The PS/2 version is assembled by setting the symbol PS2 to 1.
;       PS2 must be set to 1 on PS/2 computers because the PS/2's
;       timers are not compatible with an undocumented timer-stopping
;       feature of the 8253; the alternative timing approach that
;       must be used on PS/2 computers leaves a short window
;       during which the timer 0 count and the BIOS timer count may
;       not be synchronized. You should also set the PS2 symbol to
;       1 if you're getting erratic or obviously incorrect results.
;
; Note: When PS2 is 0, the code relies on an undocumented 8253
;       feature to get more reliable readings. It is possible that
;       the 8253 (or whatever chip is emulating the 8253) may be put
;       into an undefined or incorrect state when this feature is
;       used.
;
;     ***************************************************************
;     * If your computer displays any hint of erratic behavior      *
;     * after the long-period Zen timer is used, such as the floppy *
;     * drive failing to operate properly, reboot the system, set   *
;     * PS2 to 1 and leave it that way!                             *
;     ***************************************************************
;
; Note: Each block of code being timed should ideally be run several
;       times, with at least two similar readings required to
;       establish a true measurement, in order to eliminate any
;       variability caused by interrupts.
;
; Note: Interrupts must not be disabled for more than 54 ms at a
;       stretch during the timing interval. Because interrupts
;       are enabled, keys, mice, and other devices that generate
;       interrupts should not be used during the timing interval.
;
; Note: Any extra code running off the timer interrupt (such as
;       some memory-resident utilities) will increase the time
;       measured by the Zen timer.
;
; Note: These routines can introduce inaccuracies of up to a few
;       tenths of a second into the system clock count for each
;       code section timed. Consequently, it's a good idea to
;       reboot at the conclusion of timing sessions. (The
;       battery-backed clock, if any, is not affected by the Zen
;       timer.)
;
; All registers and all flags are preserved by all routines.
;
                DOSSEG
                .model  small
                .code
        public  ZTimerOn, ZTimerOff, ZTimerReport

;
; Set PS2 to 0 to assemble for use on a fully 8253-compatible
; system; when PS2 is 0, the readings are more reliable if the
; computer supports the undocumented timer-stopping feature,
; but may be badly off if that feature is not supported. In
; fact, timer-stopping may interfere with your computer's
; overall operation by putting the 8253 into an undefined or
; incorrect state.  Use with caution!!!
;
; Set PS2 to 1 to assemble for use on non-8253-compatible
; systems, including PS/2 computers; when PS2 is 1, readings
; may occasionally be off by 54 ms, but the code will work
; properly on all systems.
;
; A setting of 1 is safer and will work on more systems,
; while a setting of 0 produces more reliable results in systems
; which support the undocumented timer-stopping feature of the
; 8253. The choice is yours.
;
PS2     equ     1
;
; Base address of the 8253 timer chip.
;
BASE_8253               equ     40h
;
; The address of the timer 0 count registers in the 8253.
;
TIMER_0_8253            equ     BASE_8253 + 0
;
; The address of the mode register in the 8253.
;
MODE_8253               equ     BASE_8253 + 3
;
; The address of the BIOS timer count variable in the BIOS
; data segment.
;
TIMER_COUNT             equ     46ch
;
; Macro to emulate a POPF instruction in order to fix the bug in some
; 80286 chips which allows interrupts to occur during a POPF even when
; interrupts remain disabled.
;
MPOPF macro
        local   p1, p2
        jmp short p2
p1:     iret                    ;jump to pushed address & pop flags
p2:     push    cs              ;construct far return address to
        call    p1              ; the next instruction
        endm

;
; Macro to delay briefly to ensure that enough time has elapsed
; between successive I/O accesses so that the device being accessed
; can respond to both accesses even on a very fast PC.
;
DELAY   macro
        jmp     $+2
        jmp     $+2
        jmp     $+2
        endm

StartBIOSCountLow       dw      ?       ;BIOS count low word at the
                                        ; start of the timing period
StartBIOSCountHigh      dw      ?       ;BIOS count high word at the
                                        ; start of the timing period
EndBIOSCountLow         dw      ?       ;BIOS count low word at the
                                        ; end of the timing period
EndBIOSCountHigh        dw      ?       ;BIOS count high word at the
                                        ; end of the timing period
EndTimedCount           dw      ?       ;timer 0 count at the end of
                                        ; the timing period
ReferenceCount          dw      ?       ;number of counts required to
                                        ; execute timer overhead code
;
; String printed to report results.
;
OutputStr       label   byte
                db      0dh, 0ah, 'Timed count: '
TimedCountStr   db      10 dup (?)
                db      ' microseconds', 0dh, 0ah
                db      '$'
;
; Temporary storage for timed count as it's divided down by powers
; of ten when converting from doubleword binary to ASCII.
;
CurrentCountLow         dw      ?
CurrentCountHigh        dw      ?
;
; Powers of ten table used to perform division by 10 when doing
; doubleword conversion from binary to ASCII.
;
PowersOfTen     label   word
        dd      1
        dd      10
        dd      100
        dd      1000
        dd      10000
        dd      100000
        dd      1000000
        dd      10000000
        dd      100000000
        dd      1000000000
PowersOfTenEnd  label   word
;
; String printed to report that the high word of the BIOS count
; changed while timing (an hour elapsed or midnight was crossed),
; and so the count is invalid and the test needs to be rerun.
;
TurnOverStr     label   byte
        db      0dh, 0ah
        db      '****************************************************'
        db      0dh, 0ah
        db      '* Either midnight passed or an hour or more passed *'
        db      0dh, 0ah
        db      '* while timing was in progress. If the former was  *'
        db      0dh, 0ah
        db      '* the case, please rerun the test; if the latter   *'
        db      0dh, 0ah
        db      '* was the case, the test code takes too long to    *'
        db      0dh, 0ah
        db      '* run to be timed by the long-period Zen timer.    *'
        db      0dh, 0ah
        db      '* Suggestions: use the DOS TIME command, the DOS   *'
        db      0dh, 0ah
        db      '* time function, or a watch.                       *'
        db      0dh, 0ah
        db      '****************************************************'
        db      0dh, 0ah
        db      '$'

;********************************************************************
;* Routine called to start timing.                                  *
;********************************************************************

ZTimerOn        proc    near

;
; Save the context of the program being timed.
;
        push    ax
        pushf
;
; Set timer 0 of the 8253 to mode 2 (divide-by-N), to cause
; linear counting rather than count-by-two counting. Also stops
; timer 0 until the timer count is loaded, except on PS/2
; computers.
;
        mov     al,00110100b    ;mode 2
        out     MODE_8253,al
;
; Set the timer count to 0, so we know we won't get another
; timer interrupt right away.
; Note: this introduces an inaccuracy of up to 54 ms in the system
; clock count each time it is executed.
;
        DELAY
        sub     al,al
        out     TIMER_0_8253,al         ;lsb
        DELAY
        out     TIMER_0_8253,al         ;msb
;
; In case interrupts are disabled, enable interrupts briefly to allow
; the interrupt generated when switching from mode 3 to mode 2 to be
; recognized. Interrupts must be enabled for at least 210 ns to allow
; time for that interrupt to occur. Here, 10 jumps are used for the
; delay to ensure that the delay time will be more than long enough
; even on a very fast PC.
;
        pushf
        sti
        rept 10
        jmp     $+2
        endm
        MPOPF
;
; Store the timing start BIOS count.
; (Since the timer count was just set to 0, the BIOS count will
; stay the same for the next 54 ms, so we don't need to disable
; interrupts in order to avoid getting a half-changed count.)
;
        push    ds
        sub     ax,ax
        mov     ds,ax
        mov     ax,ds:[TIMER_COUNT+2]
        mov     cs:[StartBIOSCountHigh],ax
        mov     ax,ds:[TIMER_COUNT]
        mov     cs:[StartBIOSCountLow],ax
        pop     ds
;
; Set the timer count to 0 again to start the timing interval.
;
        mov     al,00110100b            ;set up to load initial
        out     MODE_8253,al            ; timer count
        DELAY
        sub     al,al
        out     TIMER_0_8253,al         ;load count lsb
        DELAY
        out     TIMER_0_8253,al         ;load count msb
;
; Restore the context of the program being timed and return to it.
;
        MPOPF
        pop     ax
        ret

ZTimerOn        endp

;********************************************************************
;* Routine called to stop timing and get count.                     *
;********************************************************************

ZTimerOff proc  near

;
; Save the context of the program being timed.
;
        pushf
        push    ax
        push    cx
;
; In case interrupts are disabled, enable interrupts briefly to allow
; any pending timer interrupt to be handled. Interrupts must be
; enabled for at least 210 ns to allow time for that interrupt to
; occur. Here, 10 jumps are used for the delay to ensure that the
; delay time will be more than long enough even on a very fast PC.
;
        sti
        rept    10
        jmp     $+2
        endm

;
; Latch the timer count.
;

if PS2

        mov     al,00000000b
        out     MODE_8253,al            ;latch timer 0 count
;
; This is where a one-instruction-long window exists on the PS/2.
; The timer count and the BIOS count can lose synchronization;
; since the timer keeps counting after it's latched, it can turn
; over right after it's latched and cause the BIOS count to turn
; over before interrupts are disabled, leaving us with the timer
; count from before the timer turned over coupled with the BIOS
; count from after the timer turned over. The result is a count
; that's 54 ms too long.
;

else

;
; Set timer 0 to mode 2 (divide-by-N), waiting for a 2-byte count
; load, which stops timer 0 until the count is loaded. (Only works
; on fully 8253-compatible chips.)
;
        mov     al,00110100b            ;mode 2
        out     MODE_8253,al
        DELAY
        mov     al,00000000b            ;latch timer 0 count
        out     MODE_8253,al

endif

        cli                             ;stop the BIOS count
;
; Read the BIOS count. (Since interrupts are disabled, the BIOS
; count won't change.)
;
        push    ds
        sub     ax,ax
        mov     ds,ax
        mov     ax,ds:[TIMER_COUNT+2]
        mov     cs:[EndBIOSCountHigh],ax
        mov     ax,ds:[TIMER_COUNT]
        mov     cs:[EndBIOSCountLow],ax
        pop     ds
;
; Read the timer count and save it.
;
        in      al,TIMER_0_8253         ;lsb
        DELAY
        mov     ah,al
        in      al,TIMER_0_8253         ;msb
        xchg    ah,al
        neg     ax                      ;convert from countdown
                                        ; remaining to elapsed
                                        ; count
        mov     cs:[EndTimedCount],ax
;
; Restart timer 0, which is still waiting for an initial count
; to be loaded.
;

ife PS2

        DELAY
        mov     al,00110100b            ;mode 2, waiting to load a
                                        ; 2-byte count
        out     MODE_8253,al
        DELAY
        sub     al,al
        out     TIMER_0_8253,al         ;lsb
        DELAY
        mov     al,ah
        out     TIMER_0_8253,al         ;msb
        DELAY

endif

        sti             ;let the BIOS count continue
;
; Time a zero-length code fragment, to get a reference for how
; much overhead this routine has. Time it 16 times and average it,
; for accuracy, rounding the result.
;
        mov     cs:[ReferenceCount],0
        mov     cx,16
        cli                             ;interrupts off to allow a
                                        ; precise reference count
RefLoop:
        call    ReferenceZTimerOn
        call    ReferenceZTimerOff
        loop    RefLoop
        sti
        add     cs:[ReferenceCount],8   ;total + (0.5 * 16)
        mov     cl,4
        shr     cs:[ReferenceCount],cl  ;(total) / 16 + 0.5
;
; Restore the context of the program being timed and return to it.
;
        pop     cx
        pop     ax
        MPOPF
        ret

ZTimerOff endp

;
; Called by ZTimerOff to start the timer for overhead measurements.
;

ReferenceZTimerOn       proc    near
;
; Save the context of the program being timed.
;
        push    ax
        pushf
;
; Set timer 0 of the 8253 to mode 2 (divide-by-N), to cause
; linear counting rather than count-by-two counting.
;
        mov     al,00110100b    ;mode 2
        out     MODE_8253,al
;
; Set the timer count to 0.
;
        DELAY
        sub     al,al
        out     TIMER_0_8253,al         ;lsb
        DELAY
        out     TIMER_0_8253,al         ;msb
;
; Restore the context of the program being timed and return to it.
;
        MPOPF
        pop     ax
        ret

ReferenceZTimerOn       endp

;
; Called by ZTimerOff to stop the timer and add the result to
; ReferenceCount for overhead measurements. Doesn't need to look
; at the BIOS count because timing a zero-length code fragment
; isn't going to take anywhere near 54 ms.
;

ReferenceZTimerOff proc near
;
; Save the context of the program being timed.
;
        pushf
        push    ax
        push    cx

;
; Match the interrupt-window delay in ZTimerOff.
;
        sti
        rept    10
        jmp     $+2
        endm

        mov     al,00000000b
        out     MODE_8253,al            ;latch timer
;
; Read the count and save it.
;
        DELAY
        in      al,TIMER_0_8253         ;lsb
        DELAY
        mov     ah,al
        in      al,TIMER_0_8253         ;msb
        xchg    ah,al
        neg     ax                      ;convert from countdown
                                        ; remaining to elapsed
                                        ; count
        add     cs:[ReferenceCount],ax
;
; Restore the context and return.
;
        pop     cx
        pop     ax
        MPOPF
        ret

ReferenceZTimerOff endp

;********************************************************************
;* Routine called to report timing results.                         *
;********************************************************************

ZTimerReport    proc    near

        pushf
        push    ax
        push    bx
        push    cx
        push    dx
        push    si
        push    di
        push    ds
;
        push    cs      ;DOS functions require that DS point
        pop     ds      ; to text to be displayed on the screen
        assume  ds:_TEXT
;
; See if midnight or more than an hour passed during timing. If so,
; notify the user.
;
        mov     ax,[StartBIOSCountHigh]
        cmp     ax,[EndBIOSCountHigh]
        jz      CalcBIOSTime            ;hour count didn't change,
                                        ; so everything's fine
        inc     ax
        cmp     ax,[EndBIOSCountHigh]
        jnz     TestTooLong             ;midnight or two hour
                                        ; boundaries passed, so the
                                        ; results are no good
        mov     ax,[EndBIOSCountLow]
        cmp     ax,[StartBIOSCountLow]
        jb      CalcBIOSTime            ;a single hour boundary
                                        ; passed-that's OK, so long as
                                        ; the total time wasn't more
                                        ; than an hour

;
; Over an hour elapsed or midnight passed during timing, which
; renders the results invalid. Notify the user. This misses the
; case where a multiple of 24 hours has passed, but we'll rely
; on the perspicacity of the user to detect that case.
;
TestTooLong:
        mov     ah,9
        mov     dx,offset TurnOverStr
        int     21h
        jmp     short ZTimerReportDone
;
; Convert the BIOS time to microseconds.
;
CalcBIOSTime:
        mov     ax,[EndBIOSCountLow]
        sub     ax,[StartBIOSCountLow]
        mov     dx,54925                ;number of microseconds each
                                        ; BIOS count represents
        mul     dx
        mov     bx,ax                   ;set aside BIOS count in
        mov     cx,dx                   ; microseconds
;
; Convert timer count to microseconds.
;
        mov     ax,[EndTimedCount]
        mov     si,8381
        mul     si
        mov     si,10000
        div     si              ;* .8381 = * 8381 / 10000
;
; Add timer and BIOS counts together to get an overall time in
; microseconds.
;
        add     bx,ax
        adc     cx,0
;
; Subtract the timer overhead and save the result.
;
        mov     ax,[ReferenceCount]
        mov     si,8381         ;convert the reference count
        mul     si              ; to microseconds
        mov     si,10000
        div     si              ;* .8381 = * 8381 / 10000
        sub     bx,ax
        sbb     cx,0
        mov     [CurrentCountLow],bx
        mov     [CurrentCountHigh],cx
;
; Convert the result to an ASCII string by trial subtractions of
; powers of 10.
;
        mov     di,offset PowersOfTenEnd - offset PowersOfTen - 4
        mov     si,offset TimedCountStr
CTSNextDigit:
        mov     bl,'0'
CTSLoop:
        mov     ax,[CurrentCountLow]
        mov     dx,[CurrentCountHigh]
        sub     ax,PowersOfTen[di]
        sbb     dx,PowersOfTen[di+2]
        jc      CTSNextPowerDown
        inc     bl
        mov     [CurrentCountLow],ax
        mov     [CurrentCountHigh],dx
        jmp     CTSLoop
CTSNextPowerDown:
        mov     [si],bl
        inc     si
        sub     di,4
        jns     CTSNextDigit
;
;
; Print the results.
;
        mov     ah,9
        mov     dx,offset OutputStr
        int     21h
;
ZTimerReportDone:
        pop     ds
        pop     di
        pop     si
        pop     dx
        pop     cx
        pop     bx
        pop     ax
        MPOPF
        ret

ZTimerReport    endp

        end
