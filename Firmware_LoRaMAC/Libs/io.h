/**@file io.h
 *
 * @author Craig Hesling <craig@hesling.com>
 * @date Apr 21, 2017
 */

#ifndef IO_H_
#define IO_H_

#include <stdint.h>

/* TI-RTOS Header files */
#include <ti/drivers/PIN.h>

#ifdef __cplusplus
extern "C" {
#endif

void setuppins();
void setupuart();

void uartwrite(const char *str, size_t size);
void uartputs(const char *str);
void uartprintf(const char *format, ...)
    __attribute__ ((format (printf, 1, 2)));
char *uartreadline();

void setPin(PIN_Id pin, uint_t value);
void togglePin(PIN_Id pin);

void setLed(PIN_Id pin, uint_t value);
void toggleLed(PIN_Id pin);

void hexdump(uint8_t *data, size_t size);
void uarthexdump(uint8_t *data, size_t size);

#ifdef __cplusplus
}
#endif

#endif /* IO_H_ */
