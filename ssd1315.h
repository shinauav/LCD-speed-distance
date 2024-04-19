#ifndef SSD1315_H_
#define SSD1315_H_

#include <stdint.h>

extern void SSD1315_Init(uint32_t ui32_i2cBase);
extern void SSD1315_Clear();
extern void SSD1315_WriteString(uint8_t* ui8p_string, uint8_t stringLength);
extern void SSD1315_SetCursorPosition(uint8_t row, uint8_t col);

#endif /* SSD1315_H_ */
