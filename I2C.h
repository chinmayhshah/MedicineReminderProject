#ifndef I2C_h
#define I2C_h

#define SCL P1_3//SCL Pin
#define SDA P1_4//SDA pin

/* prototype definitions */
void I2CStart();
void I2CStop();
unsigned char I2CRecieve (void);
unsigned char I2CSend (unsigned char data);

#endif // I2C_h
