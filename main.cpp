/* mbed Example Program
 * Copyright (c) 2006-2014 ARM Limited
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/******************************************************************* Includes */
#include "mbed.h"
#include "rtos.h"

/******************************************************************** Typedef */
typedef struct {
    uint32_t    idx;        /* number of received character */
}
mail_t;

/******************************************************************** Globals */
DigitalOut led_flash(LED1);
DigitalOut led_serial(LED2);
DigitalIn  mypin1(SW2);
Serial device(P1_5, P1_4); // tx, rx

Thread thread1;
Mail<mail_t, 16> mail_box;

/****************************************************************** Callbacks */

/**
 * Callback for UART RX
 */
void callback_rx() {

    static int _idx = 0;
    mail_t *mail = mail_box.alloc();

    /* Fill mailbox with content */
    mail->idx = _idx++;
    /* Put mail into mailbox */
    mail_box.put(mail);
}

/**
 * Communication Thread
 */
void com_thread() {

    /* Configure UART */
    device.baud(9600);
    device.attach(&callback_rx);

    /* print welcome message over UART */
    device.printf("Serial-Example!!\n");

    /* Wait for message */
    while(1) {
        osEvent evt = mail_box.get();
        if (evt.status == osEventMail) {
            mail_t *mail = (mail_t*)evt.value.p;
            device.printf("%d-%c\n", mail->idx, device.getc());
            led_serial = !led_serial;
            mail_box.free(mail);
        }
    }
}

/**
 * Main Function
 */
int main() {

    /* Start Com Thread */
    thread1.start(com_thread);

    /* Toggle LED1  */
    while (1) {
        led_flash = !led_flash;
        wait(1);
    }
}

/*EOF*/
