#include "bmp180.h"
#include "i2c.h"
#include "math.h"

#define BMP180_COM_PORT 	hi2c2	/*通讯端口*/

#define	BMP180_ADDR 		0xEE 	/*定义器件在IIC总线中的从地址*/
#define BMP180_CALIB_REG	0xAA	/*校准寄存器开始地址*/
#define BMP180_MADDR_W 		0xF4 	/*写入地址*/
#define BMP180_MADDR_R 		0xF6 	/*读取地址*/

#define SEA_PRESSURE		101325.0f	/*海平面气压，pa*/

BMP180Data_T g_tBMP180;
 

uint16_t BMP180_CMD_WD = 0X2E;	/*温度控制寄存器，设置后，读取温度需要延迟4.5ms*/

//uint16_t BMP180_CMD_QY = 0x34;  //气压，需要延迟4.5ms
//uint16_t BMP180_CMD_QY = 0x74;  //气压，需要延迟7.5ms
//uint16_t BMP180_CMD_QY = 0xB4;  //气压，需要延迟13.5ms
uint16_t BMP180_CMD_QY = 0xF4;  /*气压控制寄存器值，设置后，读取气压需要延迟25.5ms*/
int osrs;

//定义BMP初始化变量
typedef struct {
	int16_t AC1;
	int16_t AC2;
	int16_t AC3;
	uint16_t AC4;
	uint16_t AC5;
	uint16_t AC6;
	int16_t B1;
	int16_t B2;
	int16_t MB;
	int16_t MC;
	int16_t MD;
	int32_t B5;
} BMP180_Calibration_TypeDef;
BMP180_Calibration_TypeDef BMP180_Calibration;

/** I2C气压读取 **/
//初始化所有默认值
void BMP180_Init()
{
	uint8_t buffer[22];
	if(BMP180_CMD_QY == 0x34)
	{
		osrs = 0;
	}
	else if(BMP180_CMD_QY == 0x74)
	{
		osrs = 1;
	}
	else if(BMP180_CMD_QY == 0xb4)
	{
		osrs = 2;
	}
	else if(BMP180_CMD_QY == 0xF4)
	{
		osrs = 3;
	}	
	HAL_I2C_Mem_Read(&BMP180_COM_PORT, BMP180_ADDR, BMP180_CALIB_REG, I2C_MEMADD_SIZE_8BIT, (uint8_t *)buffer, 22, 3000);
	BMP180_Calibration.AC1 = (buffer[0]  << 8) | buffer[1];
	BMP180_Calibration.AC2 = (buffer[2]  << 8) | buffer[3];
	BMP180_Calibration.AC3 = (buffer[4]  << 8) | buffer[5];
	BMP180_Calibration.AC4 = (buffer[6]  << 8) | buffer[7];
	BMP180_Calibration.AC5 = (buffer[8]  << 8) | buffer[9];
	BMP180_Calibration.AC6 = (buffer[10]  << 8) | buffer[11];
	BMP180_Calibration.B1 = (buffer[12]  << 8) | buffer[13];
	BMP180_Calibration.B2 = (buffer[14]  << 8) | buffer[15];
	BMP180_Calibration.MB = (buffer[16]  << 8) | buffer[17];
	BMP180_Calibration.MC = (buffer[18]  << 8) | buffer[19];
	BMP180_Calibration.MD = (buffer[20]  << 8) | buffer[21];
}
//温度计算
int16_t BMP180_Calc_WD(uint16_t UT)
{
	BMP180_Calibration.B5  = (((int32_t)UT - (int32_t)BMP180_Calibration.AC6) * (int32_t)BMP180_Calibration.AC5) >> 15;
	BMP180_Calibration.B5 += ((int32_t)BMP180_Calibration.MC << 11) / (BMP180_Calibration.B5 + BMP180_Calibration.MD);
	return (BMP180_Calibration.B5 + 8) >> 4;
}
//气压计算
int32_t BMP180_Calc_QY(uint32_t UP, uint8_t oss)
{
	int32_t B3,B6,X3,p;
	uint32_t B4,B7;
 
	B6 = BMP180_Calibration.B5 - 4000;
	X3 = ((BMP180_Calibration.B2 * ((B6 * B6) >> 12)) >> 11) + ((BMP180_Calibration.AC2 * B6) >> 11);
	B3 = (((((int32_t)BMP180_Calibration.AC1) * 4 + X3) << oss) + 2) >> 2;
	X3 = (((BMP180_Calibration.AC3 * B6) >> 13) + ((BMP180_Calibration.B1 * ((B6 * B6) >> 12)) >> 16) + 2) >> 2;
	B4 = (BMP180_Calibration.AC4 * (uint32_t)(X3 + 32768)) >> 15;
	B7 = ((uint32_t)UP - B3) * (50000 >> oss);
	if (B7 < 0x80000000) p = (B7 << 1) / B4; else p = (B7 / B4) << 1;
	p = p + (((((p >> 8) * (p >> 8) * 3038) >> 16) + ((-7357 * p) >> 16) + 3791) >> 4);
	return p;
}
//海拔计算
float BMP180_Calc_HB(int32_t Pa)
{
//	return 44330 * (1 - pow( Pa/ SEA_PRESSURE, 1.0f / 5.255f));
	return 44330 * (1 - pow( Pa/ SEA_PRESSURE, 0.190295F));
}

/*
****************************************************************************************
* 说明：开始温度采集
* 输入：无
* 输出：无
****************************************************************************************
*/
void BMP180_StartTempSample(void)
{
	HAL_I2C_Mem_Write(&BMP180_COM_PORT, BMP180_ADDR, BMP180_MADDR_W, I2C_MEMADD_SIZE_8BIT, (uint8_t *)&BMP180_CMD_WD, 1, 100);
}

/*
****************************************************************************************
* 说明：获取温度
* 输入：无
* 输出：浮点温度值，摄氏度，
****************************************************************************************
*/
void BMP180_GetTemp(BMP180Data_T *tpBMP180)
{
	uint8_t ucTemp[2];
	uint16_t usTemp;
	float fTemp;
	HAL_I2C_Mem_Read(&BMP180_COM_PORT, BMP180_ADDR, BMP180_MADDR_R, I2C_MEMADD_SIZE_8BIT, ucTemp, 2, 100);
	usTemp = (ucTemp[0]<<8) | ucTemp[1];
	fTemp = BMP180_Calc_WD(usTemp) / 10.0f;
	tpBMP180->fTemp = fTemp;
}

/*
****************************************************************************************
* 说明：开始压力采集
* 输入：无
* 输出：无
****************************************************************************************
*/
void BMP180_StartPressureSample(void)
{
	HAL_I2C_Mem_Write(&BMP180_COM_PORT, BMP180_ADDR, BMP180_MADDR_W, I2C_MEMADD_SIZE_8BIT, (uint8_t *)&BMP180_CMD_QY, 1, 100);
}

/*
****************************************************************************************
* 说明：获取压力和高度
* 输入：无
* 输出：浮点压力数据，高度数据
****************************************************************************************
*/
void BMP180_GetPressureAltitude(BMP180Data_T *tpBMP180)
{
	uint8_t ucTemp[3];
	uint32_t ulPressure;
	int32_t lPressure;
	HAL_I2C_Mem_Read(&BMP180_COM_PORT, BMP180_ADDR, BMP180_MADDR_R, I2C_MEMADD_SIZE_8BIT, ucTemp, 3, 1000);
	
	//做气压运算
	ulPressure = ((ucTemp[0] << 16) + (ucTemp[1] <<8) + ucTemp[2]) >> (8 - osrs);
	lPressure = BMP180_Calc_QY(ulPressure,osrs); //气压值
	tpBMP180->fPressure	= (float)lPressure;

	//计算海拔
	tpBMP180->fAltitude = BMP180_Calc_HB(lPressure);	
}

